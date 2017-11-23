

  class SplineFfdResampler_%Template1typecode%_%Template2typecode%
    : FfdResampler_%Template1typecode%,
      CubicResampler_%Template2typecode%
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
typedef aims::SplineFfdResampler<%Template1%, %Template2% > SplineFfdResampler_%Template1typecode%_%Template2typecode%;
typedef aims::FfdResampler<%Template1% > FfdResampler_%Template1typecode%;
%End

    public:

      virtual ~SplineFfdResampler_%Template1typecode%_%Template2typecode%();
      virtual void init();
      SplineFfdResampler_%Template1typecode%_%Template2typecode%(
        const aims::SplineFfd & spline,
        %Template1% background = SplineFfdResampler_%Template1typecode%_%Template2typecode%::defaultBackground() );
      SplineFfdResampler_%Template1typecode%_%Template2typecode%(
        const aims::SplineFfd & spline,
        const aims::AffineTransformation3d & affine,
        %Template1% background = SplineFfdResampler_%Template1typecode%_%Template2typecode%::defaultBackground() );
      virtual void setRef( const AimsData_%Template1typecode% & ref );
      virtual Point3df resample( const Point3df & output_location,
                                 %Template1% & output_value, int t = 0 );
      static %Template1% defaultBackground();

  };

  class NearestNeighborFfdResampler_%Template1typecode%_%Template2typecode%
    : FfdResampler_%Template1typecode%,
      NearestNeighborResampler_%Template2typecode%
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
typedef aims::NearestNeighborFfdResampler<%Template1%, %Template2% > NearestNeighborFfdResampler_%Template1typecode%_%Template2typecode%;
typedef aims::FfdResampler<%Template1% > FfdResampler_%Template1typecode%;
%End

    public:

      virtual ~NearestNeighborFfdResampler_%Template1typecode%_%Template2typecode%();
      virtual void init();
      NearestNeighborFfdResampler_%Template1typecode%_%Template2typecode%(
        const aims::SplineFfd & spline,
        %Template1% background = NearestNeighborFfdResampler_%Template1typecode%_%Template2typecode%::defaultBackground() );
      NearestNeighborFfdResampler_%Template1typecode%_%Template2typecode%(
        const aims::SplineFfd & spline, Motion affine,
        %Template1% background = NearestNeighborFfdResampler_%Template1typecode%_%Template2typecode%::defaultBackground() );
      virtual void setRef( const AimsData_%Template1typecode% & ref );
      virtual Point3df resample( const Point3df & output_location,
                                 %Template1% & output_value, int t = 0 );
      static %Template1% defaultBackground();
  };


  class TrilinearFfdResampler_%Template1typecode%_%Template2typecode%
    : FfdResampler_%Template1typecode%,
      LinearResampler_%Template2typecode%
  {
%TypeHeaderCode
#include <aims/registration/ffd.h>
typedef aims::TrilinearFfdResampler<%Template1%, %Template2% > TrilinearFfdResampler_%Template1typecode%_%Template2typecode%;
typedef aims::FfdResampler<%Template1% > FfdResampler_%Template1typecode%;
%End

    public:

      virtual ~TrilinearFfdResampler_%Template1typecode%_%Template2typecode%();
      virtual void init();
      TrilinearFfdResampler_%Template1typecode%_%Template2typecode%(
        const aims::SplineFfd & spline, %Template1% background = TrilinearFfdResampler_%Template1typecode%_%Template2typecode%::defaultBackground() );
      TrilinearFfdResampler_%Template1typecode%_%Template2typecode%(
        const aims::SplineFfd & spline,
        const aims::AffineTransformation3d & affine,
        %Template1% background = TrilinearFfdResampler_%Template1typecode%_%Template2typecode%::defaultBackground() );
      virtual void setRef( const AimsData_%Template1typecode% & ref );
      virtual Point3df resample( const Point3df & output_location,
                                 %Template1% & output_value, int t = 0 );
      static %Template1% defaultBackground();
  };

