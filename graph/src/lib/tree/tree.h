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

#ifndef GRAPH_TREE_TREE_H
#define GRAPH_TREE_TREE_H


//--- header files ------------------------------------------------------------

#include <graph/config/graph_config.h>
#include <set>

#ifndef GRAPH_TREE_BASE_H
#include <graph/tree/base.h>
#endif
#include <cartobase/object/attributed.h>
#include <cartobase/object/syntax.h>


//--- forward declarations ----------------------------------------------------

class TreeVisitor;
class ConstTreeVisitor;


//--- class declarations ------------------------------------------------------

/**	Attributed tree element
 */
class GRAPH_API Tree : public carto::AttributedObject, public BaseTree
{
public:
  Tree( bool allowChildren=true, const std::string & str="" );
  virtual ~Tree();

  /**@name	Visitor pattern */
  //@{
  virtual void accept( TreeVisitor & visitor );
  virtual void const_accept( ConstTreeVisitor & visitor ) const;
  //@}

  virtual bool check (const carto::SyntaxSet& syntax, 
		      std::set<std::string>& missing) const;

  std::set< Tree * > getElementsWith( const std::string&, 
				      bool recurse=true ) const;
  template< class T >
  std::set< Tree * > getElementsWith( const std::string&, const T&, 
				      bool recurse=true ) const;
  /// deprecated
  virtual size_t size() const;
  virtual size_t childrenSize() const;

protected:

private:
  void recurseElementsWith( const std::string&, bool, const Tree *, 
			    std::set< Tree * >&  ) const;

  template< class T >
  void recurseElementsWith( const std::string&, const T&, bool, const Tree *, 
			    std::set< Tree * >& ) const;
};


//--- inline methods ----------------------------------------------------------

inline Tree::Tree( bool allowChildren, const std::string & str )
  : carto::AttributedObject( str ), BaseTree( allowChildren )
{
}


template< class T >
inline
void
Tree::recurseElementsWith( const std::string& s, const T& t, bool recurse,
			   const Tree *p, std::set< Tree * >& trees ) const
{
  for ( const_iterator e = p->begin(); e != p->end(); ++e )
    {
      Tree *ao;

      if ( ( ao = dynamic_cast< Tree * >( *e ) ) != 0 )
	{
	  if ( ao->hasProperty( s ) )
	    {
	      T tmp;

	      if ( ao->getProperty( s, tmp ) && tmp == t ) 
		trees.insert( ao );
	    }

	  if ( recurse )
	    recurseElementsWith( s, t, recurse, ao, trees );
	}
    }
}


template< class T >
inline
std::set< Tree * > 
Tree::getElementsWith( const std::string& s, const T& t, bool recurse ) const
{
  std::set< Tree * > trees;

  recurseElementsWith( s, t, recurse, this, trees );

  return trees;
}


namespace carto
{
  // type needed in anatomist
  DECLARE_GENERIC_OBJECT_TYPE( Tree * )
}


#endif
