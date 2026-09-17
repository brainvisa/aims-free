
#ifndef AIMS_IO_HIER_H
#define AIMS_IO_HIER_H

#include <aims/config/aimsdata_config.h>
#include <fstream>
#include <string>

#include <graph/tree/treader.h>


namespace aims
{

  class HierarchyReader;

  AIMSDATA_API HierarchyReader& operator >> ( HierarchyReader&, Tree& );


  class AIMSDATA_API HierarchyReader : public TreeReader
  { 
  public:
    HierarchyReader( const std::string& name, const carto::SyntaxSet& stx =
        carto::SyntaxSet() );
    virtual ~HierarchyReader();

    using TreeReader::read;
    void read( Hierarchy& thing );

    friend HierarchyReader& operator >> ( HierarchyReader&, Hierarchy& );

  public:
    static carto::SyntaxSet & syntaxSet();

  private:
    std::string   _name;
  };


  inline HierarchyReader& operator >> ( HierarchyReader& reader, 
					Hierarchy& thing  )
  {
    reader.read( thing );
    return reader;
  }

}


#endif
