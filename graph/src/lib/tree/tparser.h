#ifndef GRAPH_TREE_TPARSER_H
#define GRAPH_TREE_TPARSER_H

#include <graph/config/graph_config.h>
#include <soma-io/datasource/datasource.h>
#include <iostream>

class Tree;

namespace carto
{

  class GRAPH_API TreeParser
  {
  public:
    TreeParser();
    virtual ~TreeParser();

    virtual Tree* read() = 0;
    virtual void read( Tree & ) = 0;

    virtual void close() = 0;
    virtual void attach( rc_ptr<DataSource> ds ) = 0;
    virtual void detach() = 0;
    virtual rc_ptr<DataSource> dataSource() = 0;
    const_ref<DataSource> dataSource() const {
      return const_cast<TreeParser*>(this)->dataSource();
    };
    virtual std::string name() const;
  };

}

#endif
