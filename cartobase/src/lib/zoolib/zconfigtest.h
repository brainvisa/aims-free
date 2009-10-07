
/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */
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


