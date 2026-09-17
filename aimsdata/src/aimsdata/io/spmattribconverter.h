#ifndef AIMS_IO_SPMATTRIBCONVERTER_H
#define AIMS_IO_SPMATTRIBCONVERTER_H

#include <cartobase/object/object.h>

namespace aims
{

  void spmAttributesConverter( carto::Object header );
  void spmAttributesConverter( carto::GenericObject & header );


  inline void spmAttributesConverter( carto::Object header )
  {
    spmAttributesConverter( *header );
  }

}

#endif

