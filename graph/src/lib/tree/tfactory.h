
#ifndef GRAPH_TREE_TFACTORY_H
#define GRAPH_TREE_TFACTORY_H

#include <graph/config/graph_config.h>
#ifndef GRAPH_TREE_TREE_H
#include <graph/tree/tree.h>
#endif


/**	Tree creation.

	Creates tree from its syntactic attribute
 */
class GRAPH_API TreeFactory
{
public:
  TreeFactory();
  virtual ~TreeFactory();
  virtual TreeFactory* clone() const;

  virtual Tree* makeTree( const std::string & syntax, 
			  bool allowChildren=true ) const;

protected:

private:
};


#endif
