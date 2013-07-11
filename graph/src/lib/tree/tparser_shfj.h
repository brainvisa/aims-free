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

#ifndef GRAPH_TREE_TPARSER_SHFJ_H
#define GRAPH_TREE_TPARSER_SHFJ_H


//--- header files ------------------------------------------------------------

#include <graph/config/graph_config.h>
#ifdef USE_SOMA_IO
  #include <soma-io/reader/areader.h>
#else
  #include <cartobase/object/areader.h>
#endif
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
    std::string name();

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

