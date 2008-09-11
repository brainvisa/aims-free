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

#ifndef GRAPH_TREE_BASE_H
#define GRAPH_TREE_BASE_H


//--- header files ------------------------------------------------------------

#include <graph/config/graph_config.h>
#include <list>
#include <algorithm>


//--- class declarations ------------------------------------------------------

/**	BaseTree element. 
	This class has to be derived to contain anything. The default BaseTree 
	class only has the tree structure, which can be used alone. \n
	A tree is a tree node like any other, therefore only one class is 
	provided.
*/
class GRAPH_API BaseTree
{
public:
  typedef std::list<BaseTree *>::iterator		iterator;
  typedef std::list<BaseTree *>::const_iterator		const_iterator;
  typedef std::list<BaseTree *>::reverse_iterator	reverse_iterator;
  typedef std::list<BaseTree *>::const_reverse_iterator	const_reverse_iterator;

  /**	Constructor
	@param	allowsChildren	if true (default) the node can hold children. 
				If false, the node is terminal (leaf) and 
				no children are allowed to be contained in it.
   */
  BaseTree( bool allowsChildren = true );
  virtual ~BaseTree();

  const std::list<BaseTree *> & children() const;
  ///	Returns true if the receiver allows children
  bool getAllowsChildren() const;
  ///	Returns the child BaseTree pointer at index \c childIndex
  BaseTree* getChildAt( unsigned childIndex ) const;
  ///	Returns the index of \c node in the receivers children
  int getIndex( BaseTree* node ) const;
  ///	Returns the parent BaseTree of the receiver
  BaseTree* getParent() const;
  ///	Returns the top-level parent BaseTree of the receiver
  BaseTree* getTopParent();
  ///	Returns the top-level parent of the receiver (const version)
  const BaseTree* getTopParent() const;
  ///	Returns true if the receiver is a leaf (terminal node)
  bool isLeaf() const;

  ///	Adds \c child to the receiver at \c index (default: end)
  void insert( BaseTree* child, int index=-1 );
  ///	Removes the child at index \c index from the receiver
  void remove( unsigned index );
  ///	Removes \c node from the receiver
  void remove( BaseTree* node );
  ///	Removes the receiver from its parent
  void removeFromParent();
  ///	Sets the parent of the receiver to \c newParent
  void setParent( BaseTree* newParent );
  ///	Deletes all children from the tree
  void clear();

  /**@name	List functions */
  //@{
  /**	deprecated - use childrenSize() for the number of 
	children. In a future release, size() will disapear to avoid ambiguity 
        with the GenericObject size in Tree specialization
  */
  virtual size_t size() const;
  ///	Returns the number of children BaseTree the receiver contains
  virtual size_t childrenSize() const;
  const_iterator begin() const;
  const_iterator end() const;
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;
  //@}

protected:

private:
  bool			_allowsChildren;
  BaseTree		*_parent;
  std::list<BaseTree *>	_children;
};


//--- inline methods ----------------------------------------------------------

inline BaseTree::BaseTree( bool allowsChildren )
  : _allowsChildren( allowsChildren ), _parent( 0 )
{
}


inline bool BaseTree::getAllowsChildren() const
{
  return _allowsChildren;
}


inline BaseTree* BaseTree::getChildAt( unsigned childIndex ) const
{
  const_iterator	it;
  unsigned		i;

  for( i=0, it=_children.begin(); i<childIndex && it!=_children.end(); 
       ++it, ++i ) {}
  if( it != _children.end() ) return *it;
  else return 0;
}


inline size_t BaseTree::size() const
{
  return _children.size();
}


inline size_t BaseTree::childrenSize() const
{
  return _children.size();
}


inline BaseTree* BaseTree::getParent() const
{
  return _parent;
}


inline bool BaseTree::isLeaf() const
{
  return _children.size() == 0;
}


inline BaseTree::const_iterator BaseTree::begin() const
{
  return _children.begin();
}


inline BaseTree::const_iterator BaseTree::end() const
{
  return _children.end();
}


inline BaseTree::const_reverse_iterator BaseTree::rbegin() const
{
  return _children.rbegin();
}


inline BaseTree::const_reverse_iterator BaseTree::rend() const
{
  return _children.rend();
}


inline void BaseTree::removeFromParent()
{
  if( getParent() ) getParent()->remove( this );
  setParent( 0 );
}


inline void BaseTree::setParent( BaseTree* newParent )
{
  _parent = newParent;
}


#endif
