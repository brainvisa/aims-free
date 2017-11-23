
//============================================================================
//   R E S A M P L I N G
//============================================================================

  /// Resampling image using Free Form Deformation transformations.
  ///
  /// To use this class you must read yourself each input image and
  /// apply \c processOneImage on it. Afterwards, the model is computed by
  /// applying \c learnClustering on computed vectors, and written in a
  /// Dpy file by calling \c saveDpy.
  class FfdResampler_%Template1typecode%
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
typedef aims::FfdResampler<%Template1% > FfdResampler_%Template1typecode%;
%End

    public:

      virtual ~FfdResampler_%Template1typecode%();
      virtual void init() = 0;
      virtual void setRef(const AimsData_%Template1typecode% & ref) = 0;
      virtual Point3df resample( const Point3df & output_location,
                                 %Template1% & output_value, int t = 0 ) = 0;
  };

