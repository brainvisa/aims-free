//--- header files ------------------------------------------------------------

#define AIMS_GRAPH_SIZE_NO_DEPREC_WARNING
#include <graph/tree/tree.h>
#undef AIMS_GRAPH_SIZE_NO_DEPREC_WARNING
#include <graph/tree/tvisitor.h>
#include <soma-io/checker/achecker.h>

using namespace carto;
using namespace std;


//--- methods -----------------------------------------------------------------

Tree::~Tree()
{
}


void Tree::accept( TreeVisitor & visitor )
{
  visitor.visitTree( this );
}


void Tree::const_accept( ConstTreeVisitor & visitor ) const
{
  visitor.visitTree( this );
}


bool Tree::check( const SyntaxSet& syntax, set<string>& missing ) const
{
  AttributedChecker	ac( syntax );
  // check base object
  missing = ac.check( *this );

  bool	result = missing.empty();

  // check sub-trees
  const_iterator	it, ft=end();
  for( it=begin(); it!=ft; ++it )
    if( !((Tree *) *it)->check( syntax, missing ) )
      result = false;

  return result;
}


void Tree::recurseElementsWith( const string& s, bool recurse, const Tree *p,
				set< Tree * >& trees ) const
{
  for ( const_iterator e = p->begin(); e != p->end(); ++e )
    {
      Tree *ao;

      if ( ( ao = dynamic_cast< Tree * >( *e ) ) != 0 )
	{
	  if ( ao->hasProperty( s ) )
            trees.insert( ao );

	  if ( recurse )
	    recurseElementsWith( s, recurse, ao, trees );
	}
    }
}


set< Tree * > Tree::getElementsWith( const string& s, bool recurse ) const
{
  set< Tree * > trees;

  recurseElementsWith( s, recurse, this, trees );

  return trees;
}


size_t Tree::childrenSize() const
{
  return BaseTree::childrenSize();
}


size_t Tree::size() const
{
  return BaseTree::childrenSize();
}


#include <cartobase/object/object_d.h>
INSTANTIATE_GENERIC_OBJECT_TYPE( Tree * )
