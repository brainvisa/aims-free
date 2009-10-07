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

// This file is only used to work around a linker problem on Mac/Darwin

#ifdef __APPLE__

#ifndef AIMS_IO_MACOSXBUGS_H
#define AIMS_IO_MACOSXBUGS_H

#include <aims/io/fileFormat.h>

namespace aims
{
  namespace macosxbugs
  {
    // compiler and/or linker bug on gcc 3.3 on MacOS 10.3
#if (__GNUC__-0 == 3) && (__GNUC_MINOR__-0 == 3)
#define AIMS_APPLE_GCC33BUG
    void 
    fileFormatDictionary_dataint8_registerFormat
      ( const std::string & name, FileFormat<AimsData<int8_t> >* fmt, 
        const std::vector<std::string> & exts );
    void 
    fileFormatDictionary_dataRGB_registerFormat
      ( const std::string & name, FileFormat<AimsData<AimsRGB> >* fmt, 
        const std::vector<std::string> & exts );
    void 
    fileFormatDictionary_surfacetriangle_registerFormat
      ( const std::string & name, FileFormat<AimsSurfaceTriangle>* fmt, 
        const std::vector<std::string> & exts );
#endif
  }
}

#endif
#endif

