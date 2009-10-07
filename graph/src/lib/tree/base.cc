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

//--- header files ------------------------------------------------------------

#include <graph/tree/base.h>

using namespace std;


//--- methods -----------------------------------------------------------------

BaseTree::~BaseTree()
{
  const_iterator	it;

  for( it=_children.begin(); it!=_children.end(); ++it ) delete *it;
}


const list<BaseTree *> & BaseTree::children() const
{
  return( _children );
}


int BaseTree::getIndex( BaseTree* node ) const
{
  const_iterator	it;
  int			i;

  for( it=_children.begin(), i=0; it!=_children.end() && *it!=node; 
       ++it, ++i ) {}
  return( i );
}


void BaseTree::insert( BaseTree* child, int index )
{
  //	disable insertion if children are not allowed
  if( getAllowsChildren() == false ) return;

  iterator	pos;

  if( index >= (int) size() || index < 0 )
    {
      pos = _children.end();
    }
  else if( index <= (int) _children.size()/2 )
    {
      int	i=0;
      for( pos=_children.begin(); i<index; ++i, ++pos ) {}
    }
  else
    {
      int	i=childrenSize();
      for( pos=_children.end(); i>index; --i, --pos ) {}
    }
  _children.insert( pos, child );
  child->setParent( this );
}


void BaseTree::remove( unsigned index )
{
  iterator	pos;

  if( index >= _children.size() )
    {
      return;
    }
  else if( index <= _children.size()/2 )
    {
      unsigned	i=0;
      for( pos=_children.begin(); i<index; ++i, ++pos ) {}
    }
  else
    {
      unsigned	i = childrenSize();
      for( pos=_children.end(); i>index; --i, --pos ) {}
    }
  if( pos != _children.end() ) _children.erase( pos );
}


void BaseTree::remove( BaseTree* node )
{
  iterator pos = find( _children.begin(), _children.end(), node );

  if( pos != _children.end() ) _children.erase( pos );
}


BaseTree* BaseTree::getTopParent()
{
  BaseTree	*tr = this, *tr2;

  for( tr2=tr->getParent(); tr2!=0; tr=tr2, tr2=tr->getParent() ) {}
  return( tr );
}


const BaseTree* BaseTree::getTopParent() const
{
  const BaseTree	*tr = this, *tr2;

  for( tr2=tr->getParent(); tr2!=0; tr=tr2, tr2=tr->getParent() ) {}
  return( tr );
}


void BaseTree::clear()
{
  for( iterator i=_children.begin(); i!=_children.end(); ++i ) delete *i;
  _children.erase( _children.begin(), _children.end() );
}
