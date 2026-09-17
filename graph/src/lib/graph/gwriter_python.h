#ifndef GRAPH_GRAPH_GWRITER_PYTHON_H
#define GRAPH_GRAPH_GWRITER_PYTHON_H

#include <graph/config/graph_config.h>
#include <soma-io/writer/pythonwriter.h>

class Graph;

namespace carto
{

  class GraphWriter_Python : public carto::PythonWriter
  {
  public:
    GraphWriter_Python( const std::string& filename,
			const carto::SyntaxSet& syntax = SyntaxSet(),
			const HelperSet& helpers = defaultHelpers() );
    GraphWriter_Python( const carto::SyntaxSet& syntax = SyntaxSet(),
			const HelperSet& helpers = defaultHelpers() );
    virtual ~GraphWriter_Python();

    static const HelperSet & defaultHelpers();
  };

}

#endif
