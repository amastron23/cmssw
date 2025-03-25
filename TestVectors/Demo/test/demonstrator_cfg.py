# this compares event by event the output of the C++ emulation with the ModelSim simulation of the firmware
import FWCore.ParameterSet.Config as cms

process = cms.Process( "Demo" )
process.load( 'FWCore.MessageService.MessageLogger_cfi' )
process.load( 'Configuration.EventContent.EventContent_cff' )
process.load( 'Configuration.Geometry.GeometryExtendedRun4D98Reco_cff' ) 
process.load( 'Configuration.Geometry.GeometryExtendedRun4D98_cff' )
process.load( 'Configuration.StandardSequences.MagneticField_cff' )
process.load( 'Configuration.StandardSequences.FrontierConditions_GlobalTag_cff' )
process.load( 'L1Trigger.TrackTrigger.TrackTrigger_cff' )

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '133X_mcRun4_realistic_v1', '')

# load code that produces DTCStubs
process.load( 'TestVectors.Demo.DTC_cff' )
# L1 tracking => hybrid emulation 
process.load("TestVectors.Demo.L1HybridEmulationTracks_cff")
# load code that fits hybrid tracks
process.load( 'TestVectors.Demo.Producer_cff' )
#--- Load code that compares s/w with f/w
process.load( 'TestVectors.Demo.Demonstrator_cff' )
from L1Trigger.TrackFindingTracklet.Customize_cff import *
#reducedConfig( process )
fwConfig( process )

# build schedule
process.tt = cms.Sequence (  process.ProducerDTC
                           + process.L1THybridTracks
                           + process.ProducerTM
                           + process.ProducerDR
                           + process.ProducerKF
                           + process.ProducerTQ
                          )
process.demo = cms.Path(process.tt + process.TrackerTFPDemonstrator)
process.schedule = cms.Schedule(process.demo)

# create options
import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing( 'analysis' )
# specify input MC
Samples = [
'/store/mc/Phase2Spring24DIGIRECOMiniAOD/TT_TuneCP5_14TeV-powheg-pythia8/GEN-SIM-DIGI-RAW-MINIAOD/PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/2560000/a1c0916c-3068-4935-90ad-f2f4da74ab36.root'
]

options.register( 'inputMC', Samples, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "Files to be processed" )
# specify number of events to process.
options.register( 'Events',1,VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, "Number of Events to analyze" )
options.parseArguments()

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.Events) )
process.source = cms.Source(
  "PoolSource",
  fileNames = cms.untracked.vstring( options.inputMC ),
  #skipEvents = cms.untracked.uint32( 301 ),
  secondaryFileNames = cms.untracked.vstring(),
  duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
)
process.Timing = cms.Service( "Timing", summaryOnly = cms.untracked.bool( True ) )
