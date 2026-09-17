#ifndef CARTODATA_IO_CARTO2AIMSHEADERTRANSLATOR_H
#define CARTODATA_IO_CARTO2AIMSHEADERTRANSLATOR_H

#include <cartobase/object/object.h>

namespace carto
{

  class Carto2AimsHeaderTranslator
  {
  public:
    virtual void translate( Object srcheader, 
                            Object dstheader = none() ) const;
    virtual ~Carto2AimsHeaderTranslator();
  };

}

#endif

