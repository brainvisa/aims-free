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

#include <graph/tree/tree.h>
#include <graph/tree/tvisitor.h>
#ifdef USE_SOMA_IO
  #include <soma-io/checker/achecker.h>
#else
  #include <cartobase/object/achecker.h>
#endif

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
  return BaseTree::size();
}


size_t Tree::size() const
{
  return BaseTree::size();
}


#include <cartobase/object/object_d.h>
INSTANTIATE_GENERIC_OBJECT_TYPE( Tree * )
