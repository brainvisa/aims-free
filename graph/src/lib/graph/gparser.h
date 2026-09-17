#ifndef GRAPH_GRAPH_GPARSER_H
#define GRAPH_GRAPH_GPARSER_H

#include <graph/config/graph_config.h>
#include <soma-io/datasource/datasource.h>
#include <iostream>

class Graph;

class GRAPH_API GraphParser
{
public:
  GraphParser();
  virtual ~GraphParser() = 0;

  virtual int read(Graph& graph) = 0;
  virtual void attach( carto::rc_ptr<carto::DataSource> ds );
  /// detaches the stream (forgets it without closing it)
  virtual void detach();
  virtual carto::rc_ptr<carto::DataSource> dataSource();
  carto::const_ref<carto::DataSource> dataSource() const
  {
    return const_cast<GraphParser*>(this)->dataSource();
  };
  virtual std::string name() const;

protected:
  carto::rc_ptr<carto::DataSource>	_datasource;

private:
  GraphParser& operator=(const GraphParser&);
};

#endif


