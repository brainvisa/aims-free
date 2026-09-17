#ifndef GRAPH_GRAPH_GPARSER_PYTHON_H
#define GRAPH_GRAPH_GPARSER_PYTHON_H

#include <graph/config/graph_config.h>
#include <graph/graph/gparser.h>
#include <soma-io/reader/pythonreader.h>

class Vertex;
class Tree;

namespace carto
{

  class GRAPH_API GraphParser_Python 
    : public GraphParser, public PythonReader
  {
  public:
    GraphParser_Python( const SyntaxSet & syntax,
			const HelperSet & helpers 
			= GraphParser_Python::defaultHelperSet() );

    virtual ~GraphParser_Python();

    using PythonReader::read;
    virtual int read( Graph& graph );
    virtual int read( Tree& tree );
    static const HelperSet & defaultHelperSet();
    static const carto::SyntaxSet & defaultSyntax();
    void addSyntax( const carto::SyntaxSet & s );

    virtual void attach( carto::rc_ptr<carto::DataSource> ds );
    virtual carto::rc_ptr<carto::DataSource> dataSource();
    virtual void detach(); 

    std::map<std::string, Vertex *>	_vertextable;
  };

}

#endif

