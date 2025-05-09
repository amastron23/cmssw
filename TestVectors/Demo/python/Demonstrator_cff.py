# ESProducer providing the algorithm to run input data through modelsim and to compares results with expected output data
# and EDAnalyzer running the ESProduct produced by above ESProducer

import FWCore.ParameterSet.Config as cms

from TestVectors.Demo.Demonstrator_cfi import TrackTriggerDemonstrator_params
from TestVectors.Demo.Producer_cfi import TrackFindingTrackletProducer_params

TrackTriggerDemonstrator = cms.ESProducer("trackerTFP::ProducerDemonstrator", TrackTriggerDemonstrator_params)

TrackerTFPDemonstrator = cms.EDAnalyzer("trklet::AnalyzerDemonstrator", TrackTriggerDemonstrator_params, TrackFindingTrackletProducer_params)
