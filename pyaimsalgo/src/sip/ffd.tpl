
//============================================================================
//   R E S A M P L I N G
//============================================================================

  class FfdResampler_%Template1typecode%
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
typedef aims::FfdResampler<%Template1% > FfdResampler_%Template1typecode%;
%End

%Docstring
      Resampling image using Free Form Deformation transformations.

      FfdResampler is using a SplineFfd or TrilinearFfd transformation to
      resample a full volume. It is an abstract class which is specialized to
      implement cubic spline, trilinear, or nearest neighbor interpolation in
      voxels values.
%End

    public:

      virtual ~FfdResampler_%Template1typecode%();
      virtual void init() = 0;
      virtual void setRef(const AimsData_%Template1typecode% & ref) = 0;
      virtual Point3df resample( const Point3df & output_location,
                                 %Template1% & output_value, int t = 0 ) = 0;
  };

