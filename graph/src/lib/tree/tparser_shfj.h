#ifndef GRAPH_TREE_TPARSER_SHFJ_H
#define GRAPH_TREE_TPARSER_SHFJ_H


//--- header files ------------------------------------------------------------

#include <graph/config/graph_config.h>
#include <soma-io/reader/areader.h>
#include <graph/tree/tparser.h>
#include <graph/tree/tvisitor.h>
#include <map>


//--- forward declarations ----------------------------------------------------

class Tree;
class TreeFactory;

namespace carto
{

  //--- class declarations ----------------------------------------------------

  /**	To read a Tree from a file
   */

  class GRAPH_API TreeParser_shfj 
    : public TreeParser, public AttributedReader, public TreeVisitor
  {
  public:
    /**	configure the parameters
	\param syntax
        \param helpers
    */
    TreeParser_shfj( const SyntaxSet& syntax, 
		     const HelperSet& helpers = HelperSet() );
    /**	configure the parametres
	\param factory factory used to build the tree nodes
	\param filename name of the file to read from
	\param syntax
        \param helpers
    */
    TreeParser_shfj( const TreeFactory & factory, const std::string& filename,
		     const SyntaxSet& syntax, 
		     const HelperSet& helpers = HelperSet() );
    /**	configure the parametres
	\param factory factory used to build the tree nodes
	\param syntax
        \param helpers
    */
    TreeParser_shfj( const TreeFactory & factory, 
		     const SyntaxSet& syntax, 
		     const HelperSet& helpers = HelperSet() );

    ///	clean up
    virtual ~TreeParser_shfj();

    /**@name	Visitor methods */
    //@{

    /**	read a Tree from a file
	@param tree tree to read into
    */
    virtual void visitTree( Tree* tree );

    //@}

    /**@name	IO functions */
    //@{

    ///	creates and reads a tree
    virtual Tree* read();
    virtual void read( Tree & );

    virtual void close();
    virtual void attach( rc_ptr<DataSource> ds );
    virtual void detach();
    virtual rc_ptr<DataSource> dataSource();
    std::string name() const;

  protected:
    /**@name	Helpers */
    //@{

    /**	read a Tree attributes from a file
	@param tree	tree to read into
    */
    virtual void readTree( Tree* tree );

    virtual Tree* readSubTree();
    //@}

    /// Nodes builder
    TreeFactory	*_factory;
    /// Remind the top-level tree
    Tree		*_topTree;

  private:
    /// Disable copy constructor
    TreeParser_shfj( const TreeParser_shfj& );

    /// Disable assignment operator
    TreeParser_shfj& operator = ( const TreeParser_shfj& );
  };

}

#endif

