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

