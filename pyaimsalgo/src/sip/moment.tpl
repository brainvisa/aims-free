class Moment_%Template1typecode%
{
%TypeHeaderCode
  #include <aims/moment/moment.h>
  #include <pyaims/vector/stdvector.h>
  
  #ifndef PYAIMSSIP_MOMENT_%Template1typecode%_DEFINED
  #define PYAIMSSIP_MOMENT_%Template1typecode%_DEFINED
  typedef Moment<%Template1% > Moment_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  #define PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  typedef AimsData<%Template1% > AimsData_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSSIP_AIMSDATA_DOUBLE_DEFINED
  #define PYAIMSSIP_AIMSDATA_DOUBLE_DEFINED
  typedef AimsData<double> AimsData_DOUBLE;
  #endif
    
  #ifndef PYAIMSSIP_AIMSDATA_BUCKETMAP_VOID_DEFINED
  #define PYAIMSSIP_AIMSDATA_BUCKETMAP_VOID_DEFINED
  typedef aims::BucketMap<Void> BucketMap_VOID;
  #endif

  #include <aims/mesh/surface.h>
  #ifndef PYAIMS_AIMSTIMESURFACE_3_VOID_DEFINED
  #define PYAIMS_AIMSTIMESURFACE_3_VOID_DEFINED
  typedef AimsTimeSurface<3,Void> AimsTimeSurface_3_VOID;
  #endif

  #ifndef PYAIMSSIP_VECTOR_DOUBLE_DEFINED
  #define PYAIMSSIP_VECTOR_DOUBLE_DEFINED
  typedef std::vector<double> vector_DOUBLE;
  #endif
%End
  
  public:
  
    enum Operation
    {
      mSub = -1,
      mAdd = 1
    };
  
    enum MomentId
    {
      m000 = 0,
    
      m100 = 0,
      m010 = 1,
      m001 = 2,
    
      m200 = 0,
      m020 = 1,
      m002 = 2,
      m110 = 3,
      m101 = 4,
      m011 = 5,
    
      m300 = 0,
      m030 = 1,
      m003 = 2,
      m210 = 3,
      m201 = 4,
      m120 = 5,
      m021 = 6,
      m102 = 7,
      m012 = 8,
      m111 = 9
    };
    
    Moment_%Template1typecode%();
    Moment_%Template1typecode%( const Moment_%Template1typecode% & /Transfer/ );
    virtual ~Moment_%Template1typecode%();

	// Moment information
    vector_DOUBLE* gravity() /Factory/;
%MethodCode
      sipRes = pyaimsCopyFrom_Array(sipCpp->gravity(), 3);
%End

    double sum();
    double m0();

    vector_DOUBLE* m1() /Factory/;
%MethodCode
      sipRes = pyaimsCopyFrom_Array(sipCpp->m1(), 3);
%End

    vector_DOUBLE* m2() /Factory/;
%MethodCode
      sipRes = pyaimsCopyFrom_Array(sipCpp->m2(), 6);
%End

    vector_DOUBLE * m3();
%MethodCode
      sipRes = pyaimsCopyFrom_Array(sipCpp->m3(), 10);
%End
	
    const AimsData_DOUBLE& eigenValue() const;
    const AimsData_DOUBLE& eigenVector() const;
	
    virtual void doit( AimsSurfaceTriangle& ) /ReleaseGIL/;
    virtual void doit( const BucketMap_VOID& ) /ReleaseGIL/;
    virtual void orientation();

    SIP_PYOBJECT __str__();
%MethodCode
      try
      {
        std::ostringstream ss;
        std::string stmp;
        const char *s;

        ss << *sipCpp << std::endl;
        stmp = ss.str();
        s = stmp.c_str();
        if( s == 0 )
          s = "";

        sipRes = PyString_FromString(s);
      }
      catch( std::exception & e )
      {
        sipIsErr = 1;
        sipRes = 0;
        PyErr_SetString( PyExc_RuntimeError, e.what() );
      }
%End
};

class MomentInvariant_%Template1typecode%
{
%TypeHeaderCode
  #include <aims/moment/momInvariant.h>
  
  #ifndef PYAIMSSIP_MOMENTINVARIANT_%Template1typecode%_DEFINED
  #define PYAIMSSIP_MOMENTINVARIANT_%Template1typecode%_DEFINED
  typedef MomentInvariant<%Template1%> MomentInvariant_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSSIP_MOMENT_%Template1typecode%_DEFINED
  #define PYAIMSSIP_MOMENT_%Template1typecode%_DEFINED
  typedef Moment<%Template1%> Moment_%Template1typecode%;
  #endif

  #ifndef PYAIMSSIP_VECTOR_DOUBLE_DEFINED
  #define PYAIMSSIP_VECTOR_DOUBLE_DEFINED
  typedef std::vector<double> vector_DOUBLE;
  #endif
  
  #ifndef PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  #define PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  typedef AimsData<%Template1%> AimsData_%Template1typecode%;
  #endif
  
%End

  public:

    enum InvariantId
    {
      I00_2 = 0,
      I22_2 = 1,
      I222_3 = 2,
      I11_3 = 3,
      I33_3 = 4,
      I3111_3 = 5,
      I3131_3 = 6,
      I3331_3 = 7,
      I3333_3 = 8,
      I112_23 = 9,
      I123_23 = 10,
      I233_23 = 11
    };
  
    MomentInvariant_%Template1typecode%( Moment_%Template1typecode% *  /Transfer/ = 0 );
    MomentInvariant_%Template1typecode%( const MomentInvariant_%Template1typecode% & /Transfer/ );
    virtual ~MomentInvariant_%Template1typecode%();

    Moment_%Template1typecode% & moment() const;
    vector_DOUBLE* invariant() const /Factory/;
%MethodCode
      sipRes = pyaimsCopyFrom_Array((double*)sipCpp->invariant(), 12);
%End

    void update( double, double, double,
      int = Moment_%Template1typecode%::mAdd ) /ReleaseGIL/;
    void doit( AimsSurfaceTriangle& ) /ReleaseGIL/;
    void doit( Moment_%Template1typecode% * /Transfer/ ) /ReleaseGIL/;
      
    SIP_PYOBJECT __str__();
%MethodCode
      try
      {
        std::ostringstream ss;
        std::string stmp;
        const char *s;

		ss << *sipCpp << std::endl;
		stmp = ss.str();
        s = stmp.c_str();
        if( s == 0 )
          s = "";
        
		sipRes = PyString_FromString(s);
      }
      catch( std::exception & e )
      {
        sipIsErr = 1;
        sipRes = 0;
        PyErr_SetString( PyExc_RuntimeError, e.what() );
      }
%End
};

class GeometricMoment_%Template1typecode% : MomentBase, Moment_%Template1typecode%
{
%TypeHeaderCode
  #include <cartobase/type/types.h>
  #include <aims/moment/momBase.h>
  #include <aims/moment/moment.h>
  #include <aims/moment/geomMoment.h>
  #if SIP_VERSION < 0x040700
  #include "sipaimsalgosipMoment_%Template1typecode%.h"
  #endif

  #ifndef PYAIMSSIP_MOMENT_%Template1typecode%_DEFINED
  #define PYAIMSSIP_MOMENT_%Template1typecode%_DEFINED
  typedef Moment<%Template1%> Moment_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSSIP_GEOMETRICMOMENT_%Template1typecode%_DEFINED
  #define PYAIMSSIP_GEOMETRICMOMENT_%Template1typecode%_DEFINED
  typedef GeometricMoment<%Template1%> GeometricMoment_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  #define PYAIMSSIP_AIMSDATA_%Template1typecode%_DEFINED
  typedef AimsData<%Template1%> AimsData_%Template1typecode%;
  #endif
  
  #ifndef PYAIMSSIP_AIMSDATA_BUCKETMAP_VOID_DEFINED
  #define PYAIMSSIP_AIMSDATA_BUCKETMAP_VOID_DEFINED
  typedef aims::BucketMap<Void> BucketMap_VOID;
  #endif
%End

%TypeCode
#if SIP_VERSION >= 0x040700
#ifndef PYAIMS_AIMSTIMESURFACE_3_VOID_DEFINED
#define PYAIMS_AIMSTIMESURFACE_3_VOID_DEFINED
typedef AimsTimeSurface<3,Void> AimsTimeSurface_3_VOID;
#endif
#endif
%End

  public:
    GeometricMoment_%Template1typecode%(
      MomentBase::MomentType = MomentBase::Incremental );
    void setMomentType( MomentBase::MomentType );
    void update( double, double, double,
      int = Moment_%Template1typecode%::mAdd ) /ReleaseGIL/;
    void doit( AimsData_%Template1typecode% &,
      int = Moment_%Template1typecode%::mAdd ) /ReleaseGIL/;
    void doit( const BucketMap_VOID & ,
      int = Moment_%Template1typecode%::mAdd ) /ReleaseGIL/;

};
