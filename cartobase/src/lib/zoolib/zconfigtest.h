
#ifndef CARTOBASE_ZOOLIB_ZCONFIG_H
#define CARTOBASE_ZOOLIB_ZCONFIG_H

/* this source only tries to detect if ZooLib config will fail or not, 
   because zconfigl.h emits #error on failure
*/
#if ( defined( __GNUC__ ) &&                                            \
      ( defined( __unix__ ) || defined( __BEOS__ )                      \
	    || defined( __APPLE__ ) ) &&                                    \
      ( #cpu(i386) || #cpu(sparc) || #cpu(powerpc)                      \
	    || defined( __POWERPC__ ) )                                    \
      )                                                                 \
  || ( defined( __MWERKS__ ) &&                                         \
       ( defined( TARGET_API_MAC_CARBON ) || defined( __MACOS__ )       \
         || defined( __INTEL__ ) || defined( __BEOS__ ) ) &&            \
       ( defined( __POWERPC__ ) || defined( __MC68K__ )                 \
         || defined( mw_rez ) || defined( __INTEL__ ) )                 \
       )                                                                \
  || defined( _MSC_VER )

// then it's OK to include ZooLib config
#include "zconfig.h"

// disable threaded implementation (too slow)
#ifdef ZCONFIG_API_Thread
#undef ZCONFIG_Compiler
#undef ZCONFIG_OS
#undef ZCONFIG_Processor
#endif

#endif

#endif


