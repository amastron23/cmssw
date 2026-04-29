#ifndef L1Trigger_TrackFindingTracklet_KalmanFilter_h
#define L1Trigger_TrackFindingTracklet_KalmanFilter_h

#include "L1Trigger/TrackTrigger/interface/Setup.h"
#include "L1Trigger/TrackFindingTracklet/interface/DataFormats.h"
#include "L1Trigger/TrackFindingTracklet/interface/KalmanFilterFormats.h"
#include "L1Trigger/TrackFindingTracklet/interface/State.h"
#include "DataFormats/L1TrackTrigger/interface/TTTypes.h"
#include "L1Trigger/TrackFindingTMTT/interface/Settings.h"
#include "L1Trigger/TrackFindingTMTT/interface/KFParamsComb.h"

#include <vector>
#include <deque>

namespace trklet {

  /*! \class  trklet::KalmanFilter
   *  \brief  Class to do helix fit to all tracks in a region.
   *          All variable names & equations come from Fruhwirth KF paper
   *          http://dx.doi.org/10.1016/0168-9002%2887%2990887-4
   *          Summary of variables:
   *          m = hit position (phi,z)
   *          V = hit position 2x2 covariance matrix in (phi,z).
   *          x = helix params
   *          C = helix params 4x4 covariance matrix
   *          r = residuals
   *          H = 2x4 derivative matrix (expected stub position w.r.t. helix params)
   *          K = KF gain 2x2 matrix
   *          x' & C': Updated values of x & C after KF iteration
   *          Boring: F = unit matrix; pxcov = C
   *          Summary of equations:
   *          S = H*C (2x4 matrix); St = Transpose S
   *          R = V + H*C*Ht (KF paper) = V + H*St (used here at simpler): 2x2 matrix
   *          Rinv = Inverse R
   *          K = St * Rinv : 2x2 Kalman gain matrix * det(R)
   *          r = m - H*x
   *          x' = x + K*r
   *          C' = C - K*H*C (KF paper) = C - K*S (used here as simpler)
   *          can run 4 parameter emulation with seeding stage or 5 parameter simulation
   *          without seeding stage or old KF. To run 5 parameter simulation set
   *          TrackTriggerSetup.KalmanFilter.Use5ParameterFit = True
   *          to run olfKF set
   *          TrackTriggerSetup.KalmanFilter.UseSimmulation = True
   *  \author Thomas Schuh
   *  \date   2024, Sep
   */
  class KalmanFilter {
  public:
    typedef State::Stub Stub;
    KalmanFilter(const tt::Setup*,
                const DataFormats*,
                KalmanFilterFormats*,
                tmtt::Settings*,
                tmtt::KFParamsComb*,
                int,
                tt::TTTracks&,
                const TTClusterAssociationMap<Ref_Phase2TrackerDigi_>&,
                const TTStubAssociationMap<Ref_Phase2TrackerDigi_>&);

    ~KalmanFilter() = default;
    // read in and organize input tracks and stubs
    void consume(const tt::StreamsTrack&, const tt::StreamsStub&);
    // fill output products
    void produce(tt::StreamsStub&, tt::StreamsTrack&);

  private:

    struct TrackAssociationResult {
        bool isGenuine;
        TrackingParticlePtr associatedTP;
        int nUnknownStubs;
        int nMatchingTPs;
    };

    //
    struct Track {
      Track() {}
      Track(int trackId,
            int numConsistent,
            int numConsistentPS,
            double d0,
            const TTBV& hitPattern,
            const TrackKF& trackKF,
            const std::vector<StubKF>& stubsKF)
          : trackId_(trackId),
            numConsistent_(numConsistent),
            numConsistentPS_(numConsistentPS),
            d0_(d0),
            hitPattern_(hitPattern),
            trackKF_(trackKF),
            stubsKF_(stubsKF) {}
      int trackId_;
      int numConsistent_;
      int numConsistentPS_;
      double d0_;
      TTBV hitPattern_;
      TrackKF trackKF_;
      std::vector<StubKF> stubsKF_;
    };
    // call old KF
    void simulate(tt::StreamsStub& streamsStub, tt::StreamsTrack& streamsTrack);
    // constraints double precision
    double digi(VariableKF var, double val) { return kalmanFilterFormats_->format(var).digi(val); }
    //
    int integer(VariableKF var, double val) { return kalmanFilterFormats_->format(var).integer(val); }
    //
    void updateRangeActual(VariableKF var, double val) {
      return kalmanFilterFormats_->format(var).updateRangeActual(val);
    }

    // In KalmanFilter.h, update the associateTrack method:

    TrackAssociationResult associateTrack(const Track& track) const {
        // Get all stubs of this track as TTStubRefs
        const std::vector<StubKF>& stubsKF = track.stubsKF_;
        std::vector<TTStubRef> theseStubs;
        theseStubs.reserve(stubsKF.size());
        for (const StubKF& skf : stubsKF) {
            const auto& frame = skf.frame();
            theseStubs.push_back(frame.first);
        }
        
        // Auxiliary map to relate TP addresses and TP edm::Ptr
        std::map<const TrackingParticle*, TrackingParticlePtr> auxMap;
        int mayCombinUnknown = 0;
        
        // Collect all TrackingParticles that contribute to this track's stubs
        for (const TTStubRef& stub : theseStubs) {
            for (unsigned int ic = 0; ic < 2; ic++) {
                const std::vector<TrackingParticlePtr>& tempTPs =
                    ttClusterAssociationMap_.findTrackingParticlePtrs(stub->clusterRef(ic));
                
                for (const TrackingParticlePtr& testTP : tempTPs) {
                    if (testTP.isNull())
                        continue;
                    
                    if (auxMap.find(testTP.get()) == auxMap.end()) {
                        auxMap.emplace(testTP.get(), testTP);
                    }
                }
            }
            
            if (ttStubAssociationMap_.isUnknown(stub))
                ++mayCombinUnknown;
        }
        
        // Reject tracks with ANY unknown stub (strict matching)
        if (mayCombinUnknown > 0) {
            return {false, TrackingParticlePtr(), mayCombinUnknown, 0};
        }
        
        // Find which TrackingParticles appear in ALL stubs (strict matching - no mismatches allowed)
        std::vector<const TrackingParticle*> tpInAllStubs;
        
        for (const auto& auxPair : auxMap) {
            const std::vector<TTStubRef>& tempStubs = 
                ttStubAssociationMap_.findTTStubRefs(auxPair.second);
            
            // Count stubs on track that are NOT related to this TP
            int nnotfound = 0;
            for (const TTStubRef& stub : theseStubs) {
                if (std::find(tempStubs.begin(), tempStubs.end(), stub) == tempStubs.end()) {
                    ++nnotfound;
                    break;  // Early exit - one mismatch is enough to fail strict matching
                }
            }
            
            // For strict matching, we require ALL stubs to be found
            if (nnotfound > 0)
                continue;
            
            // This TP generates hits in ALL stubs (strict matching)
            tpInAllStubs.push_back(auxPair.first);
        }
        
        // Count how many TPs were associated to all stubs on this track
        unsigned int nTPs = tpInAllStubs.size();
        
        // Strict matching logic:
        // - If exactly 1 TP appears in ALL stubs: GENUINE
        // - If 0 or >= 2 TP: FAKE
        if (nTPs != 1) {
            return {false, TrackingParticlePtr(), mayCombinUnknown, static_cast<int>(nTPs)};
        }
        
        // This track is genuine (strict matching) - return the associated TP
        const TrackingParticle* bestTPptr = tpInAllStubs.at(0);
        TrackingParticlePtr bestTP = auxMap.find(bestTPptr)->second;
        
        return {true, bestTP, mayCombinUnknown, 1};
    }

    bool isFake(const Track& track) const { 
        return !associateTrack(track).isGenuine; 
    }

    TrackingParticlePtr getAssociatedTP(const Track& track) const { 
        auto result = associateTrack(track);
        return result.isGenuine ? result.associatedTP : TrackingParticlePtr();
    }

    //
    double base(VariableKF var) { return kalmanFilterFormats_->format(var).base(); }
    //
    int width(VariableKF var) { return kalmanFilterFormats_->format(var).width(); }
    // remove and return first element of deque, returns nullptr if empty
    template <class T>
    T* pop_front(std::deque<T*>& ts) const;
    // calculates the helix params & their cov. matrix from a pair of stubs
    void calcSeeds();
    // Transform States into output products
    void conv(tt::StreamsStub& streamsStub, tt::StreamsTrack& streamsTrack);
    // adds a layer to states, bool indicating if in seeding process
    void addLayer(bool seed = false);
    // apply final cuts
    void finalize();
    // best state selection
    void accumulator();
    // updates state using 4 paramter fit
    void update4(State*& state);
    // updates state using 5 parameter fit
    void update5(State*& state);

    // provides run-time constants
    const tt::Setup* setup_;
    // provides dataformats
    const DataFormats* dataFormats_;
    // provides dataformats of Kalman filter internals
    KalmanFilterFormats* kalmanFilterFormats_;
    //
    tmtt::Settings* settings_;
    //
    tmtt::KFParamsComb* tmtt_;
    // processing region
    int region_;
    //
    tt::TTTracks& ttTracks_;
    // container of tracks
    std::vector<TrackDR> tracks_;
    // container of stubs
    std::vector<Stub> stubs_;
    // container of all Kalman Filter states
    std::deque<State> states_;
    // processing stream
    std::deque<State*> stream_;
    //
    std::vector<Track> finals_;
    // current layer used during state propagation
    int layer_;
    //
    std::vector<double> zTs_;
    
    const TTClusterAssociationMap<Ref_Phase2TrackerDigi_>& ttClusterAssociationMap_;
    const TTStubAssociationMap<Ref_Phase2TrackerDigi_>& ttStubAssociationMap_;

  };

}  // namespace trklet

#endif
