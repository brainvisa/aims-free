/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef GRAPH_TREE_TVISITOR_H
#define GRAPH_TREE_TVISITOR_H

#include <cartobase/object/attributed.h>

//=============================================================================
//	F O R W A R D  D E C L A R A T I O N S
//=============================================================================

#include <graph/config/graph_config.h>

class Tree;


//--- class declarations ------------------------------------------------------


/**	Abstract base class for visiting \link Tree Trees\endlink. 
	The Visitor pattern can be found in:
	- Erich Gamma, Richard Helm, Ralph Johnson, John M. Vlissides,
	  <a href="http://www.awl.com/cseng/titles/0-201-63361-2/">Design 
          patterns</a>, pp. 331-344. Addison Wesley, 1995.
*/	
class GRAPH_API TreeVisitor
{
public:
  virtual ~TreeVisitor();

  /**@name	Visitor pattern */
  //@{
  virtual void visitAttributedObject( carto::AttributedObject* ao );
  virtual void visitTree( Tree* tree );
  //@}

protected:
  TreeVisitor();

private:
};


//	inline functions


inline TreeVisitor::TreeVisitor()
{
}


//=============================================================================
//	C L A S S  D E C L A R A T I O N
//=============================================================================

/**	Abstract base class for visiting constant \link Tree Trees\endlink. 
	The Vistor pattern can be found in:
	- Erich Gamma, Richard Helm, Ralph Johnson, John M. Vlissides,
	  <a href="http://www.awl.com/cseng/titles/0-201-63361-2/">Design 
          patterns</a>, pp. 331-344. Addison Wesley, 1995.
*/
class GRAPH_API ConstTreeVisitor : protected TreeVisitor
{
public:
  /**	@name Constructors, destructor*/
  //@{
  virtual ~ConstTreeVisitor();

  //@}

  /**	@name Visitor pattern*/
  //@{
  virtual void visitAttributedObject( const carto::AttributedObject* ao );
  virtual void visitTree( const Tree* tr );
  //@}

protected:
  /**	@name Constructors */
  //@{
  /**	The programmer should not call the constructor of an
	abstract base class.
  */
  ConstTreeVisitor();

  //@}

};


//	inline functions


inline ConstTreeVisitor::ConstTreeVisitor()
{
}



#endif


