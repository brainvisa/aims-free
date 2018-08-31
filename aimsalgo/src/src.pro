TEMPLATE=       subdirs

#!include ../config

SUBDIRS = aimsalgo

PSUBDIRS = \
    Aims3DPointCloud \
    AimsActivPixel2Volume          \
    AimsAddNoise2Data              \
    AimsApplyVectorField      \
    AimsBuildDti              \
    AimsBuildIndex            \
    AimsBvalue                \
    AimsChamferDistanceMap         \
    AimsClosing                    \
    AimsClusterArg            \
    AimsConvexHull            \
    AimsCorrectionEpi                   \
    AimsT2BasedCorrectionEpi \
    AimsCreateTexture              \
    AimsCurvature2d           \
    AimsCurvature3d           \
    AimsCylinder              \
    AimsDanielssonDistanceMap \
    AimsCompareSPMtToPrimalSketch \
    AimsDilation              \
    AimsDistordDti            \
    AimsDtiBck2Vol            \
    AimsElevationMap \
    AimsErosion              \
    AimsFoldArgAtt            \
    AimsFrontDistanceMap      \
    AimsGaussianSmoothing          \
    AimsGaussianSmoothing1d        \
    AimsGraphMesh                  \
    AimsGraphTransferLabels        \
    AimsGTMDump                    \
    AimsHJEvolution2d              \
    AimsHJEvolution3d              \
    AimsHippocampicReferential          \
    AimsIcmSpaghetti               \
    AimsIcmSpaghettiN              \
    AimsIcosaList             \
    AimsImageGLBlobStats \
    AimsImagePrimalSketch     \
    AimsImageScaleSpace  \
    AimsImageSmoothing   \
    AimsParameterize2DImageDomain \
    AimsInflate                    \
    AimsKMeans                \
    AimsKMeansImageSegmentation                \
    AimsLightenAtlas \
    AimsManualBiasCorrection       \
    AimsMask                  \
    AimsMaskPhantom           \
    AimsMeanCurvFlow          \
    AimsMedianSmoothing       \
    AimsSmoothing       \
    AimsMerge            \
    AimsMesh             \
    AimsMesh2Ascii            \
    AimsMesh2SSGraph          \
    AimsMesh2Sphere      \
    AimsMeshAverage      \
    AimsMeshBlobExtract       \
    AimsMeshBrain             \
    AimsMeshCC2Arg            \
    AimsMeshCheckClosed            \
    AimsMeshConnectedComp          \
    AimsMeshCurvature              \
    AimsMeshDecimation             \
    AimsMeshDistance               \
    AimsMeshGeodesicDepth          \
    AimsMeshLineMeasurement        \
    AimsMeshParcellation2VolumeParcellation \
    AimsMeshSmoothing         \
    AimsMeshWhite             \
    AimsMIRegister \
    AimsMoment           \
    AimsMorphoMath            \
    AimsNormWithRegion             \
    AimsOpening                    \
    AimsParamCorrectDti            \
    AimsPCAfilter             \
    AimsPCAloc                \
    AimsPCAloc2                    \
    AimsRefineMesh            \
    AimsRegisterBMEstimate         \
    AimsRegisterFFDFromCoordImages \
    AimsRegisterFFDApply           \
    AimsReplaceLevel               \
    AimsResample                   \
    AimsSimilarComponentAnalysisSegmentation \
    AimsSegmentsToMesh             \
    AimsSNR \
    AimsSphereDistribution         \
    AimsSplitArray    \
    AimsSubSampling                 \
    AimsSubTensor             \
    AimsSulcusSPAM              \
    AimsSumFrame              \
    AimsTMStarget             \
    AimsTalairachTransform         \
    AimsTensor2Bucket         \
    AimsTensorExtract         \
    AimsTensorSNR             \
    AimsTexture2Primal        \
    AimsTextureRegularize  \
    AimsTextureAverage  \
    AimsTextureDerivative           \
    AimsTextureDilation             \
    AimsTextureErosion              \
    AimsTexturePrimalSketch   \
    AimsTextureScaleSpace          \
    AimsTextureSmoothing      \
    AimsTextureThreshold      \
    AimsTextureVoronoi        \
    AimsTopologicalClassification  \
    AimsValidateFit           \
    AimsVeinExtraction             \
    AimsVFilter          \
    AimsVoiStat               \
    AimsVoronoi               \
    AimsZCat


module(aimsecat):PSUBDIRS     += AimsFovSplit \
#    AimsTestSino \
    AimsSumPETFrame

module(aimsdicom):PSUBDIRS += AimsSiemensEPI2Volume





