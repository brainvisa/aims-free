
class LightResampler_%Template1typecode%
{
%TypeHeaderCode
#include <cartodata/transformation/lightresampler.h>

%Template1typeinclude%
#ifndef PYAIMS_LIGHTRESAMPLER_%Template1typecode%_DEFINED
#define PYAIMS_LIGHTRESAMPLER_%Template1typecode%_DEFINED
typedef carto::LightResampler<%Template1% > LightResampler_%Template1typecode%;
#endif
%End

%Docstring
    Light, simple resampler used for flipping operations.

    Contrarily to Resampler classes in aimsalgo, no interpolation is done,
    neither in space nor in values. We take the nearest voxel, report its
    value in the output. This also ensures the conservation of values in
    flipping operations.
%End

  public:
      static rc_ptr_Volume_%Template1typecode% allocateResampledVolume(
        const Volume_%Template1typecode% & vol,
        const soma::AffineTransformationBase & tr );
      static void resampleVolume(
        const Volume_%Template1typecode% & input,
        Volume_%Template1typecode% & output,
        const soma::AffineTransformationBase & tr,
        const %Template1% & background = 0 );
      static carto::Object transformHeader( const carto::Object & header,
                                     const soma::AffineTransformationBase & tr );
%Docstring
      adapt info in header (referential, transformations)
%End
      static vector_S32 getTransformedDims(
        const vector_S32 & dims, const soma::AffineTransformationBase & tr );
      static vector_FLOAT getTransformedVoxelSize(
        const vector_FLOAT & vs, const soma::AffineTransformationBase & tr );
};


