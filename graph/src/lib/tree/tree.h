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

/**    Attributed tree element
 */
class GRAPH_API Tree : public carto::AttributedObject, public BaseTree
{
public:
  Tree( bool allowChildren=true, const std::string & str="" );
  virtual ~Tree();

  /**@name    Visitor pattern */
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
  virtual size_t size() const
#ifndef AIMS_GRAPH_SIZE_NO_DEPREC_WARNING
    __attribute__((__deprecated__))
#endif
    ;
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
