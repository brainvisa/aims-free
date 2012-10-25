TEMPLATE=       subdirs

#!include ../config

SUBDIRS =       aimsdata

module(aimsvida) :SUBDIRS	+= aimsvida
module(aimsdicom):SUBDIRS	+= aimsdicom
module(aimsjpeg) :SUBDIRS	+= aimsjpeg
module(aimstiff) :SUBDIRS	+= aimstiff
module(aimsecat) :SUBDIRS	+= aimsecat
module(aimsminc) :SUBDIRS	+= aimsminc
module(aimsply)  :SUBDIRS	+= aimsply
module(aimsnifti):SUBDIRS	+= aimsnifti
module(aimsgifti):SUBDIRS      += aimsgifti
module(aimsfdf):SUBDIRS   += aimsfdf
#module(aimsndp):SUBDIRS   += aimsndp

darwin-module(aimsdicom):PSUBDIRS	+= bundle_aimsdicom
darwin-module(aimsecat):PSUBDIRS	+= bundle_aimsecat
darwin-module(aimsjpeg):PSUBDIRS	+= bundle_aimsjpeg
darwin-module(aimstiff):PSUBDIRS	+= bundle_aimstiff
darwin-module(aimsvida):PSUBDIRS	+= bundle_aimsvida
darwin-module(aimsminc):PSUBDIRS	+= bundle_aimsminc
darwin-module(aimsply):PSUBDIRS         += bundle_aimsply
darwin-module(aimsnifti):PSUBDIRS       += bundle_aimsnifti
# darwin-module(aimsgifti):PSUBDIRS       += bundle_aimsgifti
darwin-module(aimsfdf):PSUBDIRS       += bundle_aimsfdf
#darwin-module(aimsndp):PSUBDIRS       += bundle_aimsndp

PSUBDIRS += \
    AimsAverage	\
    AimsComposeTransformation \
    AimsConnectedComp \
    AimsChannelExtract \
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
