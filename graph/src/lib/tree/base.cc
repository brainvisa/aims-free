//--- header files ------------------------------------------------------------

#include <graph/tree/base.h>
#include <cartobase/smart/rcptrtrick.h>

using namespace carto;
using namespace std;


//--- methods -----------------------------------------------------------------

BaseTree::~BaseTree()
{
  clear();
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

  if( index >= (int) childrenSize() || index < 0 )
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

  RCObject* refcounting = dynamic_cast<RCObject *>( child );
  if( refcounting && rc_ptr_trick::refCount( *refcounting ) != 0 )
  {
    rc_ptr<RCObject>  r( refcounting );
    // bidouille: inc the ref counter because we don't keep it in a real
    //rc_ptr
    ++rc_ptr_trick::refCount(r);
  }
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
  if( pos != _children.end() )
  {
    BaseTree* child = *pos;
    _children.erase( pos );
    RCObject* refcounting = dynamic_cast<RCObject *>( child );
    if( refcounting && rc_ptr_trick::refCount( *refcounting ) != 0 )
    {
      rc_ptr<RCObject>  r( refcounting );
      // bidouille: dec the ref counter because we don't keep it in a real
      //rc_ptr
      --rc_ptr_trick::refCount(r);
    }
  }
}


void BaseTree::remove( BaseTree* node )
{
  iterator pos = find( _children.begin(), _children.end(), node );

  if( pos != _children.end() )
  {
    BaseTree* child = *pos;
    _children.erase( pos );
    RCObject* refcounting = dynamic_cast<RCObject *>( child );
    if( refcounting && rc_ptr_trick::refCount( *refcounting ) != 0 )
    {
      rc_ptr<RCObject>  r( refcounting );
      // bidouille: dec the ref counter because we don't keep it in a real
      //rc_ptr
      --rc_ptr_trick::refCount(r);
    }
  }
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
  for( iterator i=_children.begin(); i!=_children.end(); ++i )
  {
    BaseTree* child = *i;
    RCObject* refcounting = dynamic_cast<RCObject *>( child );
    if( refcounting && rc_ptr_trick::refCount( *refcounting ) != 0 )
    {
      rc_ptr<RCObject>  r( refcounting );
      // bidouille: dec the ref counter because we don't keep it in a real
      //rc_ptr
      --rc_ptr_trick::refCount(r);
    }
    else
      delete child;
  }
  _children.erase( _children.begin(), _children.end() );
}
