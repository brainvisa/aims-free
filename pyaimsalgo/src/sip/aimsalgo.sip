%Import aims.sip

%Module soma.aimsalgo.aimsalgosip

%Include moment_DOUBLE.sip
%Include moment_FLOAT.sip
%Include moment_S16.sip
%Include moment_S32.sip
%Include moment_U8.sip
%Include moment_U16.sip
%Include moment_U32.sip
%Include moment.sip
%Include morphomath.sip
%Include morphomath_U8.sip
%Include morphomath_S16.sip
%Include morphomath_U16.sip
%Include morphomath_S32.sip
%Include morphomath_U32.sip
%Include morphomath_FLOAT.sip
%Include morphomath_DOUBLE.sip
%Include meshresampler.sip
%Include meshresampler_AimsData_U32.sip
%Include meshresampler_BucketMap_VOID.sip
%Include meshinterpoler.sip
%Include resampler_U8.sip
%Include resampler_S16.sip
%Include resampler_U16.sip
%Include resampler_S32.sip
%Include resampler_U32.sip
%Include resampler_FLOAT.sip
%Include resampler_DOUBLE.sip
%Include masklinresampler.sip
%Include sampler_FLOAT_3.sip
%Include samplable_objects.sip
%Include polynomial_FLOAT_3.sip
%Include distancemap_S16.sip
%Include distancemap_FLOAT.sip
%Include diffusionsmoother_S16.sip
%Include diffusionsmoother_FLOAT.sip
%Include smoothing_U8.sip
%Include smoothing_S16.sip
%Include smoothing_U16.sip
%Include smoothing_S32.sip
%Include smoothing_U32.sip
%Include smoothing_FLOAT.sip
%Include smoothing_DOUBLE.sip
%Include mesher.sip
%Include fastmarching.sip
%Include meshdistance.sip
%Include meshconnectivity.sip
%Include spam.sip
%Include foldgraphattributes.sip
%Include gradient.sip
%Include gradient_FLOAT.sip
%Include hypergeometric.sip
%Include information.sip
%Include knn.sip
%Include pdf.sip
%Include advection.sip
%Include histogram_S16.sip
%Include histogram_S32.sip
%Include histogram_U16.sip
%Include histogram_U32.sip
%Include histogram_U8.sip
%Include histogram_FLOAT.sip
%Include histogram_DOUBLE.sip
%Include geodesicPath.sip
%Include meshwatershed.sip
%Include points_distribution.sip
%Include ffd.sip
%Include ffd_U8.sip
%Include ffd_S16.sip
%Include ffd_U16.sip
%Include ffd_S32.sip
%Include ffd_U32.sip
%Include ffd_FLOAT.sip
%Include ffd_DOUBLE.sip
%Include ffd_RGB.sip
%Include ffd_RGBA.sip
%Include ffd_POINT3DF.sip
%Include ffd2_U8_U8.sip
%Include ffd2_S16_S16.sip
%Include ffd2_U16_U16.sip
%Include ffd2_S32_S32.sip
%Include ffd2_U32_U32.sip
%Include ffd2_FLOAT_FLOAT.sip
%Include ffd2_DOUBLE_DOUBLE.sip
%Include ffd2_RGB_U8.sip
%Include ffd2_RGBA_U8.sip
%Include ffd2_POINT3DF_FLOAT.sip
%Include rcptr_FfdTransformation.sip
%Include rcptr_SplineFfd.sip
%Include rcptr_TrilinearFfd.sip

// shorter names for double-templated classes

typedef SplineFfdResampler_DOUBLE_DOUBLE SplineFfdResampler_DOUBLE;
typedef SplineFfdResampler_FLOAT_FLOAT SplineFfdResampler_FLOAT;
typedef SplineFfdResampler_POINT3DF_FLOAT SplineFfdResampler_POINT3DF;
typedef SplineFfdResampler_RGB_U8 SplineFfdResampler_RGB;
typedef SplineFfdResampler_RGBA_U8 SplineFfdResampler_RGBA;
typedef SplineFfdResampler_U8_U8 SplineFfdResampler_U8;
typedef SplineFfdResampler_U16_U16 SplineFfdResampler_U16;
typedef SplineFfdResampler_S16_S16 SplineFfdResampler_S16;
typedef SplineFfdResampler_U32_U32 SplineFfdResampler_U32;
typedef SplineFfdResampler_S32_S32 SplineFfdResampler_S32;

typedef TrilinearFfdResampler_DOUBLE_DOUBLE TrilinearFfdResampler_DOUBLE;
typedef TrilinearFfdResampler_FLOAT_FLOAT TrilinearFfdResampler_FLOAT;
typedef TrilinearFfdResampler_POINT3DF_FLOAT TrilinearFfdResampler_POINT3DF;
typedef TrilinearFfdResampler_RGB_U8 TrilinearFfdResampler_RGB;
typedef TrilinearFfdResampler_RGBA_U8 TrilinearFfdResampler_RGBA;
typedef TrilinearFfdResampler_U8_U8 TrilinearFfdResampler_U8;
typedef TrilinearFfdResampler_U16_U16 TrilineareFfdResampler_U16;
typedef TrilinearFfdResampler_S16_S16 TrilinearFfdResampler_S16;
typedef TrilinearFfdResampler_U32_U32 TrilinearFfdResampler_U32;
typedef TrilinearFfdResampler_S32_S32 TrilinearFfdResampler_S32;
