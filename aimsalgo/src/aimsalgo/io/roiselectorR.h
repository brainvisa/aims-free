
#ifndef AIMS_IO_ROISELECTORR_H
#define AIMS_IO_ROISELECTORR_H

#include <aims/config/aimsalgo_config.h>
#include <aims/roi/roiselector.h>
#include <graph/tree/tree.h>
#include <graph/tree/treader.h>
#include <cartobase/object/syntax.h>
#include <fstream>
#include <string>

namespace aims
{

  class RoiSelectorReader;

  RoiSelectorReader& operator >> ( RoiSelectorReader&, Tree& );


  class AIMSALGO_API RoiSelectorReader
  {
  public:

    RoiSelectorReader( const std::string& name, const carto::SyntaxSet& stx );
    virtual    ~RoiSelectorReader();
    std::string     removeExtension( const std::string& name );

    void       read( RoiSelector& thing );

    friend RoiSelectorReader& operator >> ( RoiSelectorReader&, RoiSelector& );

  private:

    std::string   _name;
  };


  inline RoiSelectorReader& operator >> ( RoiSelectorReader& reader,
					  RoiSelector& thing  )
  {
    reader.read( thing );
    return reader;
  }

}


#endif
