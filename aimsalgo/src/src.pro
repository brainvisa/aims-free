TEMPLATE=       subdirs

#!include ../config

SUBDIRS = aimsalgo

PSUBDIRS = \
    Aims3DPointCloud \
    AimsActivPixel2Volume          \
    AimsAddNoise2Data              \
    AimsApplyTransform        \
    AimsApplyVectorField      \
    AimsBuildDti              \
    AimsBuildIndex            \
    AimsBvalue                \
    AimsChamferDistanceMap         \
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
    AimsElevationMap \
    AimsFoldArgAtt            \
    AimsFrontDistanceMap      \
    AimsGaussianSmoothing          \
    AimsGaussianSmoothing1d        \
    AimsGraphMesh                  \
    AimsGraphTransferLabels        \
    AimsGTMDump                    \
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
    AimsLightenAtlas \
    AimsMask                  \
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
    AimsVol2Tex               \
    AimsVoronoi               \
    AimsZCat


module(aimsecat):PSUBDIRS     += AimsFovSplit \
#    AimsTestSino \
    AimsSumPETFrame

module(aimsdicom):PSUBDIRS += AimsSiemensEPI2Volume





