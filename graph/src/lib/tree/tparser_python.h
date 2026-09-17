#ifndef GRAPH_TREE_TPARSER_PYTHON_H
#define GRAPH_TREE_TPARSER_PYTHON_H

#include <graph/config/graph_config.h>
#include <graph/graph/gparser_python.h>
#include <graph/tree/tparser.h>

class Tree;

namespace carto
{

  class GRAPH_API TreeParser_Python : public TreeParser
  {
  public:
    TreeParser_Python( const SyntaxSet & syntax );
    virtual ~TreeParser_Python();

    virtual Tree* read();
    virtual void read( Tree & );

    virtual void close();
    virtual void attach( rc_ptr<DataSource> ds );
    virtual void detach();
    virtual rc_ptr<DataSource> dataSource();

  private:
    struct Private;

    Private	*d;
  };

}

#endif
