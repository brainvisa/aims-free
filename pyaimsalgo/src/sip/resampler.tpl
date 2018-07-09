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
  typedef aims::Resampler<%Template1%> Resampler_%Template1typecode%;
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
Resampling of data from a volume, applying a transformation.

The doit() and resample() methods can be used to apply an **affine
transformation** (aims::AffineTransformation3d). They take a _direct_
transformation, i.e. the transformation goes from the space of the _input_
image (unit: mm) to the space of the _output_ image (unit: mm). The
transformation is inverted and normalized internally as needed, because the
resamplers "pull" data by transforming output coordinates into input
coordinates.

The doit() methods work on input data passed to the setRef() method.
setDefaultValue() can also be called to set the background value.

The resample() methods provide stateless alternatives.

You can also use arbitrary **non-affine transformations** (inheriting
aims::Transformation3d) by using the resample_inv() family of methods.
In this case, you must pass the backward transformation (_from output space to
input space_), because of the "pulling" mechanism described above.

Beware that contrary to the other methods, the resample_inv_to_vox() overloads
take a transformation that maps to **voxel** coordinates of the input image.
These methods can be slightly faster than resample_inv() because they map
directly to the API of the actual resamplers are implementing (doResample()).
This is especially true of the overload that performs resampling for a single
point only.
%End

%TypeHeaderCode
  #include <pyaims/vector/vector.h>
  #include <aims/resampling/resampler.h>

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef aims::Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  #define PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  typedef AimsData<%Template1%> AimsData_%Template1typecode%;
  #endif
%End

public:
  Resampler_%Template1typecode%();
  virtual ~Resampler_%Template1typecode%();


  void doit( const aims::AffineTransformation3d& transform,
             AimsData_%Template1typecode% & output_data )
    const throw(std::runtime_error) /ReleaseGIL/;
%Docstring
doit(transform, output_data)

Resample the input volume set with setRef() into an existing volume.

Parameters
----------
transform: AffineTransformation3d

    transformation from coordinates of the *input* volume (unit: mm), to
    coordinates of the *output* volume (unit: mm) (its inverse is used
    for resampling)

output_data: AimsData_%Template1typecode%

    existing volume to be filled with resampled data (its pre-existing
    dimensions and voxel size are used)

Raises
------
RuntimeError: if no input volume has been set with setRef.

Returns
-------
None

Details
-------

The background value (to be used for regions that are outside of the
input volume) can be set with setDefaultValue().

The transformations, referentials, and referential header attributes of
output_data are not touched; it is up to the calling code to update them
accordingly.

The level of verbosity is taken from carto::verbose (i.e. the
`--verbose` command-line argument is honoured).
%End


  AimsData_%Template1typecode% doit(const aims::AffineTransformation3d & transform,
                                    int dimx, int dimy, int dimz,
                                    const Point3df & voxel_size )
  const throw(std::runtime_error) /Factory, ReleaseGIL/;
%Docstring
output_data = doit(transform, dimx, dimy, dimz, voxel_size)

Resample the input volume set with setRef() in a newly allocated volume.

Parameters
----------

transform: AffineTransformation3d

    transformation from coordinates of the *input* volume (unit: mm), to
    coordinates of the *output* volume (unit: mm) (its inverse is used
    for resampling)

dimX, dimY, dimZ: int

    dimensions of the newly allocated volume

voxel_size: Point3df (list of 3 floats)

    voxel size of the newly allocated volume (unit: mm)

Returns
-------

output_data: AimsData_%Template1typecode%

    a newly allocated volume containing the resampled data (its size
    along the t axis is the same as the input volume).

Raises
------
RuntimeError: if no input volume has been set with setRef.

Details
-------

The background value (to be used for regions that are outside of the
input volume) can be set with setDefaultValue().

The level of verbosity is taken from carto::verbose (i.e. the
`--verbose` command-line argument is honoured).

The transformations, referentials, and referential header attributes of
the new volume are reset if `transform.isIdentity()` is false:

- the referential attribute is removed

- each transformation in transformations is composed with transform so
  that the output volume still points to the original space. If that is
  not possible (e.g. the transformations attribute is missing or
  invalid), then a new transformation is added that points to the input
  volume.
%End


  virtual void resample( const AimsData_%Template1typecode% & input_data,
                         const aims::AffineTransformation3d & transform,
                         const %Template1% & background,
                         AimsData_%Template1typecode% & output_data,
                         bool verbose = false ) const /ReleaseGIL/;
%Docstring
resample(input_data, transform, background, output_data, verbose=False)

Resample a volume into an existing output volume.

Parameters
----------

input_data: AimsData_%Template1typecode%
    data to be resampled (its voxel size is taken into account)

transform: AffineTransformation3d

    transformation from coordinates of the *input* volume (unit: mm), to
    coordinates of the *output* volume (unit: mm) (its inverse is used
    for resampling)

background: %Template1PyType%

    value set in output regions that are outside of the transformed
    input volume

output_data: AimsData_%Template1typecode%

    existing volume to be filled with resampled data (its pre-existing
    dimensions and voxel size are used)

verbose: bool

    print progress to stdout

Details
-------

The transformations, referentials, and referential header attributes of
output_data are not touched; it is up to the calling code to update them
accordingly.

This method does *not* use the instance state set by setRef() or
setDefaultValue().

Derived classes can override this method to optimize interpolation of a
full volume. The base class method simply calls doResample for each
point.

Returns
-------
None
%End


  void resample( const AimsData_%Template1typecode% & input_data,
                 const aims::AffineTransformation3d & transform,
                 const %Template1% & background,
                 const Point3df & output_location,
                 %Template1% & output_value /Out/,
                 int timestep) const /ReleaseGIL/;
%Docstring
output_value = resample(input_data, transform, background, output_location, timestep)

Resample a volume at a single location.

input_data: AimsData_%Template1typecode%

    data to be resampled (its voxel size is taken into account)

transform: AffineTransformation3d

    transformation from coordinates of the *input* volume (unit: mm), to
    *output* coordinates (its inverse is used for resampling)

background: %Template1PyType%

    value set in output regions that are outside of the transformed
    input volume

output_location: Point3df (list of 3 floats)

    coordinates in output space (destination space of transform)

timestep: int

    for 4D volume, time step to be used

Returns
-------

output_value: %Template1PyType%

    resampled value

Details
-------

This method does *not* use the instance state set by setRef() or
setDefaultValue().
%End


  void setRef( const AimsData_%Template1typecode% & /Transfer/ );
%Docstring
setRef(input_data)

Set the input data to be resampled by the doit() methods

Parameters
----------

input_data: AimsData_%Template1typecode%
%End


  virtual void resample_inv( const AimsData_%Template1typecode% & input_data,
                             const aims::Transformation3d & inverse_transform,
                             const %Template1% & background,
                             AimsData_%Template1typecode% & output_data,
                             bool verbose = false ) const /ReleaseGIL/;
%Docstring
resample(input_data, inverse_transform, background, output_data, verbose=False)

Resample a volume into an existing output volume.

Parameters
----------

input_data: AimsData_%Template1typecode%
    data to be resampled (its voxel size is taken into account)

inverse_transform: Transformation3d

    transformation from coordinates of the *output* volume (unit: mm), to
    coordinates of the *input* volume *(unit: mm)*

background: %Template1PyType%

    value set in output regions that are outside of the transformed
    input volume

output_data: AimsData_%Template1typecode%

    existing volume to be filled with resampled data (its pre-existing
    dimensions and voxel size are used)

verbose: bool

    print progress to stdout

Details
-------

The transformations, referentials, and referential header attributes of
output_data are not touched; it is up to the calling code to update them
accordingly.

This method does *not* use the instance state set by setRef() or
setDefaultValue().

Returns
-------
None
%End


  void resample_inv( const AimsData_%Template1typecode% & input_data,
                     const aims::Transformation3d & inverse_transform,
                     const %Template1% & background,
                     const Point3df & output_location,
                     %Template1% & output_value /Out/,
                     int timestep ) const /ReleaseGIL/;
%Docstring
output_value = resample(input_data, inverse_transform, background, output_location, timestep)

Resample a volume at a single location.

input_data: AimsData_%Template1typecode%

    data to be resampled (its voxel size is taken into account)

inverse_transform: Transformation3d

    transformation from output coordinates to coordinates of the input
    volume *(unit: mm)*

background: %Template1PyType%

    value set in output regions that are outside of the transformed
    input volume

output_location: Point3df (list of 3 floats)

    coordinates in output space (destination space of transform)

timestep: int

    for 4D volume, time step to be used

Returns
-------

output_value: %Template1PyType%

    resampled value

Details
-------

This method does *not* use the instance state set by setRef() or
setDefaultValue().
%End

  virtual void resample_inv_to_vox( const AimsData_%Template1typecode% & input_data,
                                    const aims::Transformation3d & inverse_transform_to_vox,
                                    const %Template1% & background,
                                    AimsData_%Template1typecode% & output_data,
                                    bool verbose=false ) const /ReleaseGIL/;
%Docstring
resample(input_data, inverse_transform_to_vox, background, output_data, verbose=False)

Resample a volume into an existing output volume.

Parameters
----------

input_data: AimsData_%Template1typecode%
    data to be resampled (its voxel size is taken into account)

inverse_transform_to_vox: Transformation3d

    transformation from coordinates of the *output* volume (unit: mm),
    to coordinates of the *input* volume **(unit: voxel)**

background: %Template1PyType%

    value set in output regions that are outside of the transformed
    input volume

output_data: AimsData_%Template1typecode%

    existing volume to be filled with resampled data (its pre-existing
    dimensions and voxel size are used)

verbose: bool

    print progress to stdout

Details
-------

The transformations, referentials, and referential header attributes of
output_data are not touched; it is up to the calling code to update them
accordingly.

This method does *not* use the instance state set by setRef() or
setDefaultValue().

Derived classes can override this method to optimize interpolation of a
full volume. The base class method simply calls doResample for each
point.

Returns
-------
None
%End


  void resample_inv_to_vox( const AimsData_%Template1typecode% & input_data,
                            const aims::Transformation3d & inverse_transform_to_vox,
                            const %Template1% & background,
                            const Point3df & output_location,
                            %Template1% & output_value /Out/,
                            int timestep ) const /ReleaseGIL/;
%Docstring
output_value = resample(input_data, inverse_transform_to_vox, background, output_location, timestep)

Resample a volume at a single location.

input_data: AimsData_%Template1typecode%

    data to be resampled (its voxel size is taken into account)

inverse_transform_to_vox: Transformation3d

    transformation from output coordinates to coordinates of the input
    volume (**unit: voxel**)

background: %Template1PyType%

    value set in output regions that are outside of the transformed
    input volume

output_location: Point3df (list of 3 floats)

    coordinates in output space (destination space of transform)

timestep: int

    for 4D volume, time step to be used

Returns
-------

output_value: %Template1PyType%

    resampled value

Details
-------

This method does *not* use the instance state set by setRef() or
setDefaultValue().
%End


  const AimsData_%Template1typecode%& ref() const;
%Docstring
input_data = ref()

Input data to be resampled by the doit() methods

Returns
-------

input_data: AimsData_%Template1typecode%
%End


  void setDefaultValue( %Template1% );
%Docstring
setDefaultValue(value)

Set the background value to be used by the doit() methods

Parameters
----------

value: %Template1PyType%
%End

  %Template1% defaultValue() const;
%Docstring
value = defaultValue()

Background value used by the doit() methods

Returns
-------

value: %Template1PyType%
%End
};


class SplineResampler_%Template1typecode%:Resampler_%Template1typecode% /Abstract/
{
%TypeHeaderCode
  #include <pyaims/vector/vector.h>
  #include <aims/resampling/splineresampler.h>
  
  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef aims::Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef aims::SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
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


class NearestNeighborResampler_%Template1typecode%:Resampler_%Template1typecode%
{
%TypeHeaderCode
  #include <pyaims/vector/vector.h>
  #include <aims/resampling/nearestneighborresampler.h>
  
  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef aims::Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_NEARESTNEIGHBORRESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_NEARESTNEIGHBORRESAMPLER_%Template1typecode%_DEFINED
  typedef aims::NearestNeighborResampler<%Template1%> NearestNeighborResampler_%Template1typecode%;
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
  typedef aims::CubicResampler<%Template1%> CubicResampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef aims::SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef aims::Resampler<%Template1%> Resampler_%Template1typecode%;
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
  typedef aims::QuarticResampler<%Template1%> QuarticResampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef aims::SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef aims::Resampler<%Template1%> Resampler_%Template1typecode%;
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
  typedef aims::QuinticResampler<%Template1%> QuinticResampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef aims::SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef aims::Resampler<%Template1%> Resampler_%Template1typecode%;
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
  typedef aims::SixthOrderResampler<%Template1%> SixthOrderResampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef aims::SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef aims::Resampler<%Template1%> Resampler_%Template1typecode%;
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
  typedef aims::SeventhOrderResampler<%Template1%> SeventhOrderResampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef aims::SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef aims::Resampler<%Template1%> Resampler_%Template1typecode%;
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
  typedef aims::LinearResampler<%Template1%> LinearResampler_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_SPLINERESAMPLER_%Template1typecode%_DEFINED
  typedef aims::SplineResampler<%Template1%> SplineResampler_%Template1typecode%;
  #endif

  #ifndef PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  #define PYAIMSALGOSIP_RESAMPLER_%Template1typecode%_DEFINED
  typedef aims::Resampler<%Template1%> Resampler_%Template1typecode%;
  #endif
%End

public:

  LinearResampler_%Template1typecode%();
  ~LinearResampler_%Template1typecode%();

  int getOrder() const;
};

