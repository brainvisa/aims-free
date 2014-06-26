class Gaussian3DSmoothing_%Template1typecode%
{
%TypeHeaderCode
#include <aims/signalfilter/g3dsmooth.h>

#ifndef PYAIMSALGOSIP_GAUSSIAN3DSMOOTHING_%Template1typecode%_DEFINED
#define PYAIMSALGOSIP_GAUSSIAN3DSMOOTHING_%Template1typecode%_DEFINED
	typedef Gaussian3DSmoothing<%Template1%> Gaussian3DSmoothing_%Template1typecode%;
#endif
%End

public:
  Gaussian3DSmoothing_%Template1typecode%(float sx=1.0, float sy=1.0,
    float sz=1.0);
  virtual ~Gaussian3DSmoothing_%Template1typecode%();

  AimsData_%Template1typecode% doit( const AimsData_%Template1typecode% &)
    /Factory, ReleaseGIL/;
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

  AimsData_%Template1typecode% doit( const AimsData_%Template1typecode% &)
    /Factory, ReleaseGIL/;
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
    virtual AimsData_%Template1typecode% doit(
      const AimsData_%Template1typecode%& in );

  private:
    MedianSmoothing_%Template1typecode% & operator = (
      const MedianSmoothing_%Template1typecode% & );
};

