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
