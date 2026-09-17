#ifndef GRAPH_TREE_TREADER_H
#define GRAPH_TREE_TREADER_H


//--- header files ------------------------------------------------------------

#include <graph/config/graph_config.h>
#include <soma-io/reader/areader.h>
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
  /// \deprecated{OBSOLETE, use read( Tree & ) instead}
  virtual void readTree( Tree * )
    __attribute__((__deprecated__("OBSOLETE, use read( Tree & ) instead")));

  /// \deprecated{obsolete - use attach() instead}
  void open(const std::string& filename)
    __attribute__((__deprecated__("use attach() instead")));
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
