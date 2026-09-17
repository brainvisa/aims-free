#ifndef GRAPH_TREE_TWRITER_H
#define GRAPH_TREE_TWRITER_H


//--- header files ------------------------------------------------------------

#include <graph/config/graph_config.h>
#include <soma-io/writer/awriter.h>
#include <graph/tree/tvisitor.h>


//--- forward declarations ----------------------------------------------------

class Tree;


//--- class declarations ------------------------------------------------------

/**	To write an Tree to a file
*/

class GRAPH_API TreeWriter 
  : public carto::AttributedWriter, public ConstTreeVisitor
{
public:
  /**	configure the parametres
	\param filename name of the file to write into
	\param syntax syntactic attribute / rules pairs
        \param helpers alternate/specialized IO routines
  */
  TreeWriter( const std::string& filename,
	      const carto::SyntaxSet& syntax,
	      const HelperSet& helpers = HelperSet() );
  TreeWriter( const carto::SyntaxSet& syntax,
	      const HelperSet& helpers = HelperSet() );
  virtual ~TreeWriter();

  /**@name	Visitor methods */
  //@{
  /**	write a Tree to a file
	@param tree tree to save
  */
  virtual void visitTree( const Tree* tree );

  //@}

protected:
  /**@name	Visitor methods */
  //@{
  virtual void writeTree( const Tree* node, const std::string & type );
  //@}

private:
  TreeWriter(const TreeWriter&);
  TreeWriter& operator=(const TreeWriter&);
};


//=============================================================================
//	F U N C T I O N S
//=============================================================================

/**	Save a tree to a file using a TreeWriter
	@param writer TreeWriter to restore from
	@param tree tree to save
	@return the TreeWriter given as a parameter
*/
GRAPH_API TreeWriter& operator << ( TreeWriter & writer, const Tree& tree );


#endif 
