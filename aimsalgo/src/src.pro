TEMPLATE=       subdirs

#!include ../config

SUBDIRS = public        \
          aimsalgo

PSUBDIRS = \
#    Aims3DAnalysis \
    Aims3DPointCloud \
    AimsActivPixel2Volume          \
    AimsAddNoise2Data              \
    AimsApplyVectorField      \
#    AimsAtlasQualityAssessment    \
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
#    AimsExtractRegionMasks \
    AimsFoldArgAtt            \
    AimsFrontDistanceMap      \
    AimsGaussianSmoothing          \
    AimsGaussianSmoothing1d        \
    AimsGraphMesh                  \
    AimsGraphTransferLabels        \
#    AimsGTM                        \
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
    AimsLinearComb            \
#    AimsLmaGtm            \
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
    AimsMeshPlane             \
    AimsMeshSmoothing         \
    AimsMeshWhite             \
#    AimsMixtureOfPpcaForRats \
    AimsMIRegister \
    AimsMoment           \
    AimsMorphoMath            \
    AimsNormWithRegion             \
    AimsNormDiff              \
#    AimsNormalizeAtlasProbabilities \
    AimsOpening                    \
    AimsParamCorrectDti            \
    AimsPowerComb                       \
#    AimsRegisterStriataMainLines \
    AimsPCAfilter             \
    AimsPCAloc                \
    AimsPCAloc2                    \
#    AimsPETInputFunctionEstimation \
#    AimsPETMovieNormalise          \
#    AimsPETMovieInterpolate        \
#    AimsPETMaskAndCrop \
#    AimsPETMIPProjection  \
#    AimsRatClassif           \
    AimsRefineMesh            \
    AimsReplaceLevel               \
    AimsResample                   \
    AimsSimilarComponentAnalysisSegmentation \
    AimsSegmentsToMesh             \
    AimsSNR \
    AimsSlice2Bucket          \
    AimsSpaghettiModel        \
    AimsSphereDistribution         \
    AimsSplitArray    \
#    AimsSplitHemispheres \
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


module(jpeg):PSUBDIRS +=    AimsData2Jpeg

module(aimsecat):PSUBDIRS     += AimsFovSplit \
#    AimsTestSino \
    AimsSumPETFrame

module(aimsdicom):PSUBDIRS += AimsSiemensEPI2Volume





