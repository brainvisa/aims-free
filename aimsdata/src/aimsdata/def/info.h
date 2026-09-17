#ifndef AIMS_DEF_INFO_H
#define AIMS_DEF_INFO_H

#include <cartobase/config/info.h>

namespace carto
{

  class AimsDataInfo : public Info
  {
  public:
    AimsDataInfo();
    virtual ~AimsDataInfo();
    virtual void printPaths( std::ostream & output = std::cout );
    virtual void printBuiltins( std::ostream & output = std::cout );
    virtual void printVersion( std::ostream & output = std::cout );
    virtual void printOtherInfo( std::ostream & output = std::cout );
  };

}

#endif

