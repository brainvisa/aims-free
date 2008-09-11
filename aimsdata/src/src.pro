TEMPLATE=       subdirs

#!include ../config

SUBDIRS =       aimsdata

module(aimsgui)  :SUBDIRS	+= aimsgui
module(aimsvida) :SUBDIRS	+= aimsvida
module(aimsdicom):SUBDIRS	+= aimsdicom
module(aimsjpeg) :SUBDIRS	+= aimsjpeg
module(aimstiff) :SUBDIRS	+= aimstiff
module(aimsecat) :SUBDIRS	+= aimsecat
module(aimsminc) :SUBDIRS	+= aimsminc
module(aimsgui)  :SUBDIRS	+= aimsqtformats
module(aimsply)  :SUBDIRS	+= aimsply
module(aimsnifti):SUBDIRS	+= aimsnifti

darwin-module(aimsdicom):PSUBDIRS	+= bundle_aimsdicom
darwin-module(aimsecat):PSUBDIRS	+= bundle_aimsecat
darwin-module(aimsjpeg):PSUBDIRS	+= bundle_aimsjpeg
darwin-module(aimstiff):PSUBDIRS	+= bundle_aimstiff
darwin-module(aimsvida):PSUBDIRS	+= bundle_aimsvida
darwin-module(aimsminc):PSUBDIRS	+= bundle_aimsminc
darwin-module(aimsgui):PSUBDIRS	        += bundle_aimsqtformats
darwin-module(aimsply):PSUBDIRS         += bundle_aimsply
darwin-module(aimsnifti):PSUBDIRS       += bundle_aimsnifti

PSUBDIRS += \
    AimsAverage	\
    AimsComposeTransformation \
    AimsConnectedComp \
    AimsFileConvert \
    AimsFileInfo \
    AimsFlip \
    AimsGraphComplete \
    AimsGraphConvert \
    AimsGraphExtractTransformation \
    AimsGraphMerge \
    AimsInvertTransformation \
    AimsMassCenter \
    AimsMerge2Rgb \
    AimsMergeLabel \
    AimsMeshArea \
    AimsMeshCut \
    AimsMeshes2Graph \
    AimsMeshGenerate \
    AimsMeshTransform \
    AimsOverVolume \
    AimsRemoveNaN \
    AimsRoiFeatures \
    AimsSelectLabel \
    AimsSetMinf \
    AimsSplitRgb \
    AimsSubVolume \
    AimsTCat \
    AimsTex2Graph \
    AimsThreshold \
    AimsVectorFieldToMesh \
    examples \
    tests
