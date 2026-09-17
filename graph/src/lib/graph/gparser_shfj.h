#ifndef GRAPH_GRAPH_GPARSER_SHFJ_H
#define GRAPH_GRAPH_GPARSER_SHFJ_H

#include <graph/config/graph_config.h>
#include <graph/graph/gparser.h>
#include <soma-io/reader/areader.h>

class Vertex;

class GRAPH_API GraphParser_shfj 
  : public GraphParser, public carto::AttributedReader
{
public:
  /**	configure the parametres
	@param syntax
	@param helpers
  */
  GraphParser_shfj(const carto::SyntaxSet& syntax,
		   const HelperSet& helpers = HelperSet());
  
  virtual ~GraphParser_shfj();

  /**	read a Graph attributes from a file
	@param graph graph to read into
  */
  virtual int read(Graph& graph);
  virtual carto::rc_ptr<carto::DataSource> dataSource();
  virtual std::string name() const;
  virtual void attach( carto::rc_ptr<carto::DataSource> ds );

private:
  //---------------------------------------------------------------------
  /**	@name Disable copy*/
  //---------------------------------------------------------------------
  //@{
  
  /// Copy constructor
  GraphParser_shfj(const GraphParser_shfj&);
  
  /// Assignment operator
  GraphParser_shfj& operator=(const GraphParser_shfj&);
  

  //@}
  
  //---------------------------------------------------------------------
  /**	@name Read methods*/
  //---------------------------------------------------------------------
  //@{
  
  void readVertex(Graph& graph);
  void readUEdge(Graph& graph);
  void readDEdge(Graph& graph);
  
  //@}
	    
  /// vertex pointer serialization
  std::map<int,Vertex*> _serializer;
};

#endif
