
%ModuleHeaderCode
#include <aims/resampling/mask.h>
%End

namespace aims
{

  BucketMap_VOID* mask( const BucketMap_VOID & src,
                        const BucketMap_VOID & m, bool = true ) /Factory/;
  BucketMap_VOID* maskWithVolume( const BucketMap_VOID & src,
                                  const rc_ptr_Volume_S16 m, const short & = 0,
                                  bool insideval = false ) /Factory/;
};

