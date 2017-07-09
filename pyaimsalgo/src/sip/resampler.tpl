namespace aims
{
  class ResamplerFactory_%Template1typecode%
  {
%Docstring
ResamplerFactory classes are used to instantiate a Resampler object for the chosen interpolation type. The main method is getResampler().
%End
%TypeHeaderCode
  #include <aims/resampling/resamplerfactory.h>
  #include <aims/resampling/resampler.h>
  
  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_RESAMPLERFACTORY_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLERFACTORY_%Template1typecode%_DEFINED
  namespace aims {
  	typedef ResamplerFactory<%Template1%> ResamplerFactory_%Template1typecode%;
  }
  #endif
  
%End

  public:

    enum ResamplerType
    {
      NearestNeighbor = 0,
      Linear = 1,
      Quadratic = 2,
      Cubic = 3,
      Quartic = 4,
      Quintic = 5,
      SixthOrder = 6,
      SeventhOrder = 7
    };

    virtual ~ResamplerFactory_%Template1typecode%();
    Resampler_%Template1typecode% * getResampler( int ) /Factory/;
%Docstring
result = getResampler( order )

Parameters
----------
order: int
    order of interpolation: 0 is nearest neighbour (no interpolation), 1 is linear, 2 is quadratic, etc, up to 7.

Returns
-------
result: Resampler_%Template1typecode%
    the selected resampler instance.
%End
  };
};

class Resampler_%Template1typecode% /Abstract/
{
%Docstring
Resampler resamples an input data to build or fill an output data, using an affine transformation.

Normally the input data should be set in the resampler using setRef().

Resampling is done using the doit() methods.
resample() methods also provide alternatives.
%End

%TypeHeaderCode
  #include <pyaims/vector/vector.h>
  #include <aims/resampling/resampler.h>
  
  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  #define PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  typedef AimsData<%Template1%> AimsData_%Template1typecode%;
  #endif
  
%End

public:
  Resampler_%Template1typecode%();
  virtual ~Resampler_%Template1typecode%();

  virtual void doit( const Motion&, AimsData_%Template1typecode% & ) throw()
    /ReleaseGIL/;
%Docstring
doit(transform, output_data)

Resample the reference input data (set via setRef()) into an existing output data.

Parameters
----------
transform: AffineTransformation3d
    transformation to apply
output_data: AimsData_%Template1typecode%
    resampled data will fill this existing output data

Returns
-------
None
%End

  virtual AimsData_%Template1typecode% doit( const Motion &, int, int, int,
    const Point3df & ) throw() /Factory, ReleaseGIL/;
%Docstring
output_data = doit(transform, dimx, dimy, dimz, voxel_size)

Resample the reference input data (set via setRef()) into a new output data.

Parameters
----------
transform: AffineTransformation3d
    transformation to apply
dimx, dimy, dimz: int
    number of voxels in 3 dimensions
voxel_size: Point3df (list of 3 floats)
    voxel size of the output data

Returns
-------
output_data: AimsData_%Template1typecode%
    resampled volume
%End

  virtual void resample( const AimsData_%Template1typecode% &,
                         const Motion &, const %Template1% &,
                         AimsData_%Template1typecode% &,
                         bool = false ) /ReleaseGIL/;
%Docstring
resample(input_data, transform, background, output_data, verbose=False)

Resample the input data into an existing output data.

Parameters
----------
input_data: AimsData_%Template1typecode%
    data to be resampled
transform: AffineTransformation3d
    transformation to apply
background: %Template1typecode%
    value set in output regions which are outside of the transformation space of the input volume
output_data: AimsData_%Template1typecode%
    resampled data will fill this existing output data
verbose: bool
    print more things during resampling

Returns
-------
None
%End

  virtual void resample( const AimsData_%Template1typecode% &,
                         const Motion &,
                         const %Template1% &,
                         const Point3df &,
                         %Template1% &, int ) /ReleaseGIL/;
%Docstring
resample(input_data, transform, background, output_location, output_value,
         timestep)

Resample a single voxel of the input data at a given specified output location, and set the output value.

Parameters
----------
input_data: AimsData_%Template1typecode%
    data to be resampled
transform: AffineTransformation3d
    transformation to apply
background: %Template1typecode%
    value set in output regions which are outside of the transformation space of the input volume
output_location: Point3df (list of 3 floats)
    position in the output space (warning, this is actually an input)
output_value: %Template1typecode%
    resampled output value
timestep: int
    for 4D volume, time step to be used

Returns
-------
None
%End

  void setRef( const AimsData_%Template1typecode% & /Transfer/ );
%Docstring
setRef(input_data)

set the input data to be resampled

Parameters
----------
input_data: AimsData_%Template1typecode%
%End

  void setDefaultValue( %Template1% );
%Docstring
setDefaultValue(value)

set the default background value
%End
};


class SplineResampler_%Template1typecode%:Resampler_%Template1typecode% /Abstract/
{
%TypeHeaderCode
  #include <pyaims/vector/vector.h>
  #include <aims/resampling/splineresampler.h>
  
  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif
    
  #ifndef PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  #define PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  typedef AimsData<%Template1%> AimsData_%Template1typecode%;
  #endif
    
  #ifndef PYAIMSSIP_AIMSDATA_DOUBLE_DEFINED
  #define PYAIMSSIP_AIMSDATA_DOUBLE_DEFINED
  typedef AimsData<double> AimsData_DOUBLE;
  #endif
%End
public:

  SplineResampler_%Template1typecode%();
  ~SplineResampler_%Template1typecode%();

  virtual int getOrder() const = 0;
  AimsData_DOUBLE getSplineCoef( const AimsData_%Template1typecode% &,
    int = 0, bool = false ) /Factory/;
  void reset();
};


class MaskLinearResampler_%Template1typecode%:Resampler_%Template1typecode%
{
%TypeHeaderCode
  #include <pyaims/vector/vector.h>
  #include <aims/resampling/masklinresampler.h>
  
  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_MASKLINEARRESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_MASKLINEARRESAMPLER_%Template1typecode%_DEFINED
  typedef MaskLinearResampler<%Template1%> MaskLinearResampler_%Template1typecode%;
  #endif
    
  #ifndef PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  #define PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  typedef AimsData<%Template1%> AimsData_%Template1typecode%;
  #endif

%End
public:

  MaskLinearResampler_%Template1typecode%();
  ~MaskLinearResampler_%Template1typecode%();

  void doit( const Motion &, AimsData_%Template1typecode% &  ) /ReleaseGIL/;
  AimsData_%Template1typecode% doit( const Motion &, int, int, int, const
    Point3df & ) /Factory, ReleaseGIL/;
};


class NearestNeighborResampler_%Template1typecode%:Resampler_%Template1typecode%
{
%TypeHeaderCode
  #include <pyaims/vector/vector.h>
  #include <aims/resampling/nearestneighborresampler.h>
  
  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_NEARESTNEIGHBORRESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_NEARESTNEIGHBORRESAMPLER_%Template1typecode%_DEFINED
  typedef NearestNeighborResampler<%Template1%> NearestNeighborResampler_%Template1typecode%;
  #endif

%End
public:

  NearestNeighborResampler_%Template1typecode%();
  ~NearestNeighborResampler_%Template1typecode%();
};


class CubicResampler_%Template1typecode%:SplineResampler_%Template1typecode%
{
%TypeHeaderCode
  #include <aims/resampling/cubicresampler.h>
  
  #ifndef PYAIMSALGOSIP_CUBICRESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_CUBICRESAMPLER_%Template1typecode%_DEFINED
  typedef CubicResampler<%Template1%> CubicResampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
%End

public:

  CubicResampler_%Template1typecode%();
  ~CubicResampler_%Template1typecode%();

  int getOrder() const;
};


class QuarticResampler_%Template1typecode%:SplineResampler_%Template1typecode%
{
%TypeHeaderCode
  #include <aims/resampling/quarticresampler.h>
  
  #ifndef PYAIMSALGOSIP_QUARTICRESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_QUARTICRESAMPLER_%Template1typecode%_DEFINED
  typedef QuarticResampler<%Template1%> QuarticResampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
%End

public:

  QuarticResampler_%Template1typecode%();
  ~QuarticResampler_%Template1typecode%();

  int getOrder() const;
};


class QuinticResampler_%Template1typecode%:SplineResampler_%Template1typecode%
{
%TypeHeaderCode
  #include <aims/resampling/quinticresampler.h>
  
  #ifndef PYAIMSALGOSIP_QUINTICRESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_QUINTICRESAMPLER_%Template1typecode%_DEFINED
  typedef QuinticResampler<%Template1%> QuinticResampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
%End

public:

  QuinticResampler_%Template1typecode%();
  ~QuinticResampler_%Template1typecode%();

  int getOrder() const;
};


class SixthOrderResampler_%Template1typecode%:SplineResampler_%Template1typecode%
{
%TypeHeaderCode
  #include <aims/resampling/sixthorderresampler.h>
  
  #ifndef PYAIMSALGOSIP_SIXTHORDERRESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SIXTHORDERRESAMPLER_%Template1typecode%_DEFINED
  typedef SixthOrderResampler<%Template1%> SixthOrderResampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
%End

public:

  SixthOrderResampler_%Template1typecode%();
  ~SixthOrderResampler_%Template1typecode%();

  int getOrder() const;
};


class SeventhOrderResampler_%Template1typecode%:SplineResampler_%Template1typecode%
{
%TypeHeaderCode
  #include <aims/resampling/seventhorderresampler.h>
  
  #ifndef PYAIMSALGOSIP_SEVENTHORDERRESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SEVENTHORDERRESAMPLER_%Template1typecode%_DEFINED
  typedef SeventhOrderResampler<%Template1%> SeventhOrderResampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
%End

public:

  SeventhOrderResampler_%Template1typecode%();
  ~SeventhOrderResampler_%Template1typecode%();

  int getOrder() const;
};


class LinearResampler_%Template1typecode%:SplineResampler_%Template1typecode%
{
%TypeHeaderCode
  #include <aims/resampling/linearresampler.h>
  
  #ifndef PYAIMSALGOSIP_LINEARRESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_LINEARRESAMPLER_%Template1typecode%_DEFINED
  typedef LinearResampler<%Template1%> LinearResampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
%End

public:

  LinearResampler_%Template1typecode%();
  ~LinearResampler_%Template1typecode%();

  int getOrder() const;
};

