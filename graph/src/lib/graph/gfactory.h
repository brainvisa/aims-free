#ifndef GRAPH_GRAPH_GFACTORY_H
#define GRAPH_GRAPH_GFACTORY_H


//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <graph/config/graph_config.h>
#include <cartobase/smart/rcobject.h>
#include <map>
#include <string>


//=============================================================================
//	F O R W A R D  D E C L A R A T I O N S
//=============================================================================

class Vertex;
class UEdge;
class DEdge;
namespace carto
{
  class GenericObject;
}


//=============================================================================
//	C L A S S  D E C L A R A T I O N
//=============================================================================

/**	Default abstract factory for graphs. 
	The Abstract Factory pattern can be found in:
	- Erich Gamma, Richard Helm, Ralph Johnson, John M. Vlissides,
	  <a href="http://www.awl.com/cseng/titles/0-201-63361-2/">Design 
          patterns</a>, pp. 87-95. Addison Wesley, 1995.
 */
class GraphFactory : public carto::RCObject
{

public:
  GraphFactory();
  virtual ~GraphFactory();

  typedef carto::GenericObject* (*Generator)( const std::string & );
  static void registerGenerator( const std::string & syntax, 
                                 Generator f );
  static void unregisterGenerator( const std::string & syntax );

protected:

  //---------------------------------------------------------------------
  /**	@name Factory methods*/
  //---------------------------------------------------------------------
  //@{

  /**	Create a Vertex
   */
  virtual Vertex* makeVertex(const std::string& s);

  /**	Create an unitialized UEdge - without vertices
   */
  virtual UEdge* makeUEdge(const std::string& s);

  /**	Create an unitialized DEdge - without vertices
   */
  virtual DEdge* makeDEdge(const std::string& s);

  //@}

private:

  //---------------------------------------------------------------------
  /**	@name Friends*/
  //---------------------------------------------------------------------
  //@{

  ///	export makeVertex, makeUEdge and makeDEdge
  friend class Graph;

  //@}

  //---------------------------------------------------------------------
  /**	@name Disable copy*/
  //---------------------------------------------------------------------
  //@{

  /**	Assignment operator
   */
  GraphFactory& operator=(const GraphFactory&);

  //@}

  static std::map<std::string, Generator> & _generators();

};


#endif
