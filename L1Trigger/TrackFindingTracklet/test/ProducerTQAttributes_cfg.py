#!/cvmfs/cms.cern.ch/el9_amd64_gcc12/cms/cmssw/CMSSW_14_2_0_pre4/bin/el9_amd64_gcc12/cmsRun

import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import os
process = cms.Process("L1TrackNtuple")

############################################################
# edit options here
############################################################

# D88 was used for CMSSW_12_6 datasets, and D98 recommended for more recent ones.
# GEOMETRY = "D88"
GEOMETRY = "D110"
# GEOMETRY = "D110"

# Set L1 tracking algorithm:
# 'HYBRID' (baseline, 4par fit) or 'HYBRID_DISPLACED' (extended, 5par fit).
# 'HYBRID_NEWKF' (baseline, 4par fit, with bit-accurate KF emulation),
# 'HYBRID_REDUCED' to use the "L5L6" seeding only reduced configuration.
# (Or legacy algos 'TMTT' or 'TRACKLET').
L1TRKALGO = 'HYBRID_NEWKF'

WRITE_DATA = False

############################################################
# import standard configurations
############################################################

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.L1track = dict(limit = -1)
process.MessageLogger.Tracklet = dict(limit = -1)
process.MessageLogger.TrackTriggerHPH = dict(limit = -1)


print("using geometry " + GEOMETRY + " (tilted)")
process.load('Configuration.Geometry.GeometryExtendedRun4' + GEOMETRY + 'Reco_cff')
process.load('Configuration.Geometry.GeometryExtendedRun4' + GEOMETRY +'_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.GlobalTag import GlobalTag
# Change needed to run with D98 geometry in recent CMSSW versions.
if GEOMETRY == "D88" or GEOMETRY == 'D98':
    process.GlobalTag = GlobalTag(process.GlobalTag, '133X_mcRun4_realistic_v1', '')
elif GEOMETRY == 'D110':
    process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')
else:
    print("this is not a valid geometry!!!")

process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


############################################################
# input and output
############################################################

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))

#--- To use MCsamples scripts, defining functions get*data*() for easy MC access,
#--- follow instructions in https://github.com/cms-L1TK/MCsamples

#from MCsamples.Scripts.getCMSdata_cfi import *
#from MCsamples.Scripts.getCMSlocaldata_cfi import *

if GEOMETRY == "D98":

  inputMC = [
      "/store/relval/CMSSW_14_0_0_pre2/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/PU_133X_mcRun4_realistic_v1_STD_2026D98_PU200_RV229-v1/2580000/0b2b0b0b-f312-48a8-9d46-ccbadc69bbfd.root"
  ]

elif GEOMETRY == "D88":

  # Read specified .root file:
  inputMC = ["/store/mc/CMSSW_12_6_0/RelValTTbar_14TeV/GEN-SIM-DIGI-RAW/PU_125X_mcRun4_realistic_v5_2026D88PU200RV183v2-v1/30000/0959f326-3f52-48d8-9fcf-65fc41de4e27.root"]

elif GEOMETRY == "D110":

  # Read specified .root file:
  inputMC = [
    "/store/mc/Phase2Spring24DIGIRECOMiniAOD/TT_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/2560000/a1c0916c-3068-4935-90ad-f2f4da74ab36.root",
    # '/store/mc/Phase2Spring24DIGIRECOMiniAOD/TT_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/2560000/318c440c-316c-49c3-96d0-ede7a3a139e4.root',
    # '/store/mc/Phase2Spring24DIGIRECOMiniAOD/TT_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/2560000/c07df786-9969-4bfb-b656-b71c4a767fb1.root',
    # '/store/mc/Phase2Spring24DIGIRECOMiniAOD/TT_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/2560000/32d79276-4233-46ce-a6ae-4232f413b39d.root',
    # '/store/mc/Phase2Spring24DIGIRECOMiniAOD/TT_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/2560000/8fb04c2e-3ecf-4643-91bd-e39608cec0e9.root',
    # '/store/mc/Phase2Spring24DIGIRECOMiniAOD/TT_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/2560000/a0b09015-eae1-4296-8470-09a91fe15e1e.root',
    # '/store/mc/Phase2Spring24DIGIRECOMiniAOD/TT_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/2560000/dbdd8041-2336-4ec3-bb29-360f05d27303.root',
    # '/store/mc/Phase2Spring24DIGIRECOMiniAOD/TT_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/2560000/d79fe329-afbf-4fb9-bec3-74cd819a14dc.root'
    ]

else:

  print("this is not a valid geometry!!!")

process.source = cms.Source("PoolSource", fileNames = cms.untracked.vstring(*inputMC))

process.TFileService = cms.Service("TFileService", fileName = cms.string('L1TrkNtuple.root'), closeFileFast = cms.untracked.bool(True))
process.Timing = cms.Service("Timing", summaryOnly = cms.untracked.bool(True))


############################################################
# L1 tracking: stubs / DTC emulation
############################################################

process.load('L1Trigger.TrackTrigger.TrackTrigger_cff')

# remake stubs?
#from L1Trigger.TrackTrigger.TTStubAlgorithmRegister_cfi import *
#process.load("SimTracker.TrackTriggerAssociation.TrackTriggerAssociator_cff")

#from SimTracker.TrackTriggerAssociation.TTClusterAssociation_cfi import *
#TTClusterAssociatorFromPixelDigis.digiSimLinks = cms.InputTag("simSiPixelDigis","Tracker")

#process.TTClusterStub = cms.Path(process.TrackTriggerClustersStubs)
#process.TTClusterStubTruth = cms.Path(process.TrackTriggerAssociatorClustersStubs)


# load code that associates stubs with mctruth
process.load( 'SimTracker.TrackTriggerAssociation.StubAssociator_cff' )
# DTC emulation
process.load('L1Trigger.TrackerDTC.DTC_cff')

# load code that analyzes DTCStubs
process.load('L1Trigger.TrackerDTC.Analyzer_cff')

# modify default cuts
#process.TrackTriggerSetup.FrontEnd.BendCut = 5.0
#process.TrackTriggerSetup.Hybrid.MinPt = 1.0

process.dtc = cms.Path(process.StubAssociator + process.ProducerDTC + process.AnalyzerDTC)

############################################################
# L1 tracking
############################################################

process.load("L1Trigger.TrackFindingTracklet.L1HybridEmulationTracks_cff")

process.load( 'L1Trigger.TrackFindingTracklet.Producer_cff' )
process.load( 'L1Trigger.TrackFindingTracklet.Analyzer_cff' )
NHELIXPAR = 4
L1TRK_NAME  = process.TrackFindingTrackletAnalyzer_params.OutputLabelTFP.value()
L1TRK_LABEL = process.TrackFindingTrackletProducer_params.BranchTTTracks.value()
L1TRUTH_NAME = "TTTrackAssociatorFromPixelDigis"
process.TTTrackAssociatorFromPixelDigis.TTTracks = cms.VInputTag( cms.InputTag(L1TRK_NAME, L1TRK_LABEL) )
process.HybridNewKF = cms.Sequence(process.L1THybridTracks + process.ProducerTM + process.ProducerDR + process.ProducerKF + process.ProducerTQ + process.ProducerTFP)
process.TTTracksEmulation = cms.Path(process.StubAssociator + process.HybridNewKF)
#process.TTTracksEmulationWithTruth = cms.Path(process.HybridNewKF +  process.TrackTriggerAssociatorTracks)
# Optionally include code producing performance plots & end-of-job summary.
process.load( 'SimTracker.TrackTriggerAssociation.StubAssociator_cff' )
process.TTTracksEmulationWithTruth = cms.Path(process.StubAssociator + process.HybridNewKF + process.TrackTriggerAssociatorTracks + process.AnalyzerTracklet + process.AnalyzerTM + process.AnalyzerDR + process.AnalyzerKF + process.AnalyzerTQ + process.AnalyzerTFP )
from L1Trigger.TrackFindingTracklet.Customize_cff import *
fwConfig( process )
# Needed by L1TrackNtupleMaker
process.HitPatternHelperSetup.useNewKF = True