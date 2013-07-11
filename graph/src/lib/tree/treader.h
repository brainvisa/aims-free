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

#ifndef GRAPH_TREE_TREADER_H
#define GRAPH_TREE_TREADER_H


//--- header files ------------------------------------------------------------

#include <graph/config/graph_config.h>
#ifdef USE_SOMA_IO
  #include <soma-io/reader/areader.h>
#else
  #include <cartobase/object/areader.h>
#endif
#include <graph/tree/tvisitor.h>
#include <list>


//--- forward declarations ----------------------------------------------------

class Tree;
class TreeFactory;
namespace carto
{
  class TreeParser;
}


//--- class declarations ------------------------------------------------------

/**	To read a Tree from a file
*/

class GRAPH_API TreeReader : public TreeVisitor
{
public:
  TreeReader( const std::string& filename, const carto::SyntaxSet& syntax, 
	      const carto::AttributedReader::HelperSet& helpers 
	      = carto::AttributedReader::HelperSet() );
  TreeReader( const carto::SyntaxSet& syntax, 
	      const carto::AttributedReader::HelperSet& helpers 
	      = carto::AttributedReader::HelperSet() );
  TreeReader( const TreeFactory & factory, const std::string& filename,
	      const carto::SyntaxSet& syntax, 
	      const carto::AttributedReader::HelperSet& helpers 
	      = carto::AttributedReader::HelperSet() );
  /**	configure the parametres
        \param factory factory used to build the tree nodes
        \param syntax syntactic attribute / rules pairs
        \param helpers alternate/specialized IO routines
  */
  TreeReader( const TreeFactory & factory, const carto::SyntaxSet& syntax, 
	      const carto::AttributedReader::HelperSet& helpers 
	      = carto::AttributedReader::HelperSet() );

  virtual ~TreeReader();

  virtual Tree* read();
  virtual void read( Tree & );
  /// OBSOLETE, use read( Tree & ) instead
  virtual void readTree( Tree * );

  /// obsolete - use attach() instead
  void open(const std::string& filename);
  void close();
  void attach( std::istream & s, int line_num = 1 );
  void attach( const std::string & filename );
  void attach( carto::rc_ptr<carto::DataSource> ds );
  void detach();
  virtual std::string name() const;
  const carto::rc_ptr<carto::DataSource> dataSource() const;
  carto::rc_ptr<carto::DataSource> dataSource();

  std::list<carto::TreeParser *> & parsers();
  const carto::SyntaxSet & syntaxSet() const { return _syntax; }
  void setSyntax( const carto::SyntaxSet & syntax );

private:
  /// Disable copy constructor
  TreeReader( const TreeReader& );
  /// Disable assignment operator
  TreeReader& operator = ( const TreeReader& );

  carto::SyntaxSet			_syntax;
  carto::AttributedReader::HelperSet	_helpers;
  TreeFactory				*_factory;
  carto::rc_ptr<carto::DataSource>	_datasource;
  std::list<carto::TreeParser *>	_parsers;

};


//--- functions ---------------------------------------------------------------

/**	?
	@return the TreeReader given as a parameter
*/
GRAPH_API TreeReader& operator>>(TreeReader& reader, Tree& tree);


#endif
