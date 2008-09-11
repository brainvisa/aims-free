namespace aims
{
  class ResamplerFactory_%Template1typecode%
  {
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
  };
};

class Resampler_%Template1typecode% /Abstract/
{
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

  virtual void doit( const Motion&, AimsData_%Template1typecode% & )
    /ReleaseGIL/;
  virtual AimsData_%Template1typecode% doit( const Motion &, int, int, int,
    const Point3df & ) /Factory, ReleaseGIL/;
  virtual void resample( const AimsData_%Template1typecode% &,
                         const Motion &, const %Template1% &,
                         AimsData_%Template1typecode% &,
                         bool = false ) /ReleaseGIL/;
  virtual void resample( const AimsData_%Template1typecode% &,
                         const Motion &,
                         const %Template1% &,
                         const Point3df &,
                         %Template1% &, int ) /ReleaseGIL/;
  void setRef( const AimsData_%Template1typecode% & /Transfer/ );
  void setDefaultValue( %Template1% );
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
%End
public:

  LinearResampler_%Template1typecode%();
  ~LinearResampler_%Template1typecode%();

  int getOrder() const;
};

