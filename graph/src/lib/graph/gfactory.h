/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

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
