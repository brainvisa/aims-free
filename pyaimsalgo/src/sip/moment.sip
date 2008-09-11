class MomentBase
{
%TypeHeaderCode
  #include <aims/moment/momBase.h>
%End

  public:
  
    enum MomentType
    {
      Normal,
      Incremental,
      Volumic,
      Surfacic
    };

    MomentBase();
    virtual ~MomentBase();
};
  
class TriangulationMoment:MomentBase, Moment_S16
{
%TypeHeaderCode
  #include <cartobase/type/types.h>
  #include <aims/mesh/surface.h>
  #include <aims/moment/momBase.h>
  #include <aims/moment/moment.h>
  #include <aims/moment/triMoment.h>
  #if SIP_VERSION < 0x040700
  #include "sipaimsalgosipMoment_S16.h"
  #endif
%End

%TypeCode
#if SIP_VERSION >= 0x040700
#ifndef PYAIMS_BUCKETMAP_VOID_DEFINED
#define PYAIMS_BUCKETMAP_VOID_DEFINED
typedef aims::BucketMap< Void > BucketMap_VOID;
#endif
#ifndef PYAIMSSIP_MOMENT_S16_DEFINED
#define PYAIMSSIP_MOMENT_S16_DEFINED
typedef Moment< int16_t > Moment_S16;
#endif
#endif
%End

  public:

    TriangulationMoment( MomentBase::MomentType = MomentBase::Volumic );
    void setMomentType( MomentBase::MomentType );
    void doit( AimsSurfaceTriangle & ) /ReleaseGIL/;
};
