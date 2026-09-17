#ifndef GRAPH_GRAPH_GPARSER_XML_H
#define GRAPH_GRAPH_GPARSER_XML_H

#include <graph/config/graph_config.h>
#include <graph/graph/gparser.h>
#include <soma-io/reader/areader.h>

class GRAPH_API GraphParser_xml : public GraphParser
{
public:
  
  /* *	configure the parametres
	@param filename name of the file to read from
	@param syntax
	@param helpers
  */

/*   GraphParser_xml(const std::string& filename, */
/* 		   const SyntaxSet& syntax, */
/* 		   const HelperSet& helpers) = HelperSet()); */

  GraphParser_xml();

  /* *	configure the parametres
	@param syntax
	@param helpers
  */
  
  /* GraphParser_xml(const SyntaxSet& syntax, */
  /* 		   const HelperSet& helpers = HelperSet()); */
  
  
  virtual ~GraphParser_xml();
  
  
  //---------------------------------------------------------------------
  /**	@name Read method*/
  //---------------------------------------------------------------------
  //@{
  
  /**	read a Graph attributes from a file
	@param graph graph to read into
  */
  virtual int read(Graph& graph);
  //@}

  
private:
    
  //---------------------------------------------------------------------
  /**	@name Disable copy*/
  //---------------------------------------------------------------------
  //@{
  
  /// Copy constructor
  GraphParser_xml(const GraphParser_xml&);
  
  /// Assignment operator
  GraphParser_xml& operator=(const GraphParser_xml&);

  //@}

  //!FIXME:: put here methods needed by xml parsing ...
};

#endif
