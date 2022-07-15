import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run3_dd4hep_cff import Run3_dd4hep
process = cms.Process('G4PrintGeometry',Run3_dd4hep)
process.load('Configuration.Geometry.GeometryDD4hepExtended2021Reco_cff')

#from Configuration.Eras.Era_Phase2C11_dd4hep_cff import Phase2C11_dd4hep
#process = cms.Process('G4PrintGeometry',Phase2C11_dd4hep)
#process.load('Configuration.Geometry.GeometryDD4hepExtended2026D77Reco_cff')
#process.load('Configuration.Geometry.GeometryDD4hepExtended2026D83Reco_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

from SimG4Core.PrintGeomInfo.g4PrintGeomInfo_cfi import *

process = printGeomInfo(process)

if hasattr(process,'MessageLogger'):
    process.MessageLogger.G4cerr=dict()
    process.MessageLogger.G4cout=dict()

process.g4SimHits.Watchers = cms.VPSet(cms.PSet(
    DumpSummary      = cms.untracked.bool(True),
    DumpLVTree       = cms.untracked.bool(False),
    DumpMaterial     = cms.untracked.bool(False),
    DumpLVList       = cms.untracked.bool(False),
    DumpLV           = cms.untracked.bool(False),
    DumpSolid        = cms.untracked.bool(False),
    DumpAttributes   = cms.untracked.bool(False),
    DumpPV           = cms.untracked.bool(False),
    DumpRotation     = cms.untracked.bool(False),
    DumpReplica      = cms.untracked.bool(False),
    DumpTouch        = cms.untracked.bool(False),
    DumpSense        = cms.untracked.bool(False),
    DumpRegion       = cms.untracked.bool(False),
    DD4hep           = cms.untracked.bool(True),
    Name             = cms.untracked.string(''),
    Names            = cms.untracked.vstring(''),
    MaterialFileName = cms.untracked.string('matfileDD4hep.txt'),
    SolidFileName    = cms.untracked.string('solidfileDD4hep.txt'),
    LVFileName       = cms.untracked.string('lvfileDD4hep.txt'),
    PVFileName       = cms.untracked.string('pvfileDD4hep.txt'),
    TouchFileName    = cms.untracked.string('touchfileDD4hep.txt'),
    RegionFileName   = cms.untracked.string('regionfileDD4hep.txt'),
    FileDetail       = cms.untracked.bool(True),
    type             = cms.string('PrintGeomInfoAction')
))
