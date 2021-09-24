class Gaussian3DSmoothing_%Template1typecode%
{
%TypeHeaderCode
#include <aims/signalfilter/g3dsmooth.h>

#ifndef PYAIMSALGOSIP_GAUSSIAN3DSMOOTHING_%Template1typecode%_DEFINED
#define PYAIMSALGOSIP_GAUSSIAN3DSMOOTHING_%Template1typecode%_DEFINED
	typedef Gaussian3DSmoothing<%Template1%> Gaussian3DSmoothing_%Template1typecode%;
#endif
%End

%Docstring
3D Deriche's recursive gaussian smoothing filter
%End

public:
  Gaussian3DSmoothing_%Template1typecode%(float sx=1.0, float sy=1.0,
    float sz=1.0);
%Docstring
Gaussian3DSmoothing_%Template1typecode%(sx=1., sy=1., sz=1.)

Parameters
----------
sx: float
    filter stdev (sigma) on x direction (mm)
sy: float
    filter stdev (sigma) on y direction (mm)
sz: float
    filter stdev (sigma) on z direction (mm)
%End

  virtual ~Gaussian3DSmoothing_%Template1typecode%();

  rc_ptr_Volume_%Template1typecode% doit(
    const rc_ptr_Volume_%Template1typecode% & ) /Factory, ReleaseGIL/;
%Docstring
doit(volume)

Actually perform the smoothing on the given data.

Parameters
----------
volume: Volume
    data to be smoothed

Returns
-------
smoothed: Volume
    smoothed volume
%End
};


class Gaussian2DSmoothing_%Template1typecode%
{
%TypeHeaderCode
#include <aims/signalfilter/g2dsmooth.h>

#ifndef PYAIMSALGOSIP_GAUSSIAN2DSMOOTHING_%Template1typecode%_DEFINED
#define PYAIMSALGOSIP_GAUSSIAN2DSMOOTHING_%Template1typecode%_DEFINED
        typedef Gaussian2DSmoothing<%Template1%> Gaussian2DSmoothing_%Template1typecode%;
#endif
%End

public:
  Gaussian2DSmoothing_%Template1typecode%(float sx=1.0, float sy=1.0);
  virtual ~Gaussian2DSmoothing_%Template1typecode%();

  rc_ptr_Volume_%Template1typecode% doit(
    const rc_ptr_Volume_%Template1typecode% & ) /Factory, ReleaseGIL/;
};


class MedianSmoothing_%Template1typecode%
{
%TypeHeaderCode
#include <aims/signalfilter/medianfilter.h>

#ifndef PYAIMSALGOSIP_MEDIANSMOOTHING_%Template1typecode%_DEFINED
#define PYAIMSALGOSIP_MEDIANSMOOTHING_%Template1typecode%_DEFINED
        typedef MedianSmoothing<%Template1%> MedianSmoothing_%Template1typecode%;
#endif
%End

  public:
    MedianSmoothing_%Template1typecode%( int sx = 3, int sy = 3, int sz = 3 );
    virtual ~MedianSmoothing_%Template1typecode%();
    rc_ptr_Volume_%Template1typecode% doit(
      const rc_ptr_Volume_%Template1typecode%& in ) const;

  private:
    MedianSmoothing_%Template1typecode% & operator = (
      const MedianSmoothing_%Template1typecode% & );
};

