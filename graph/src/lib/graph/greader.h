#ifndef GRAPH_GRAPH_GREADER_H
#define GRAPH_GRAPH_GREADER_H


//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================


#include <graph/config/graph_config.h>
#include <soma-io/reader/areader.h>


//=============================================================================
//	F O R W A R D  D E C L A R A T I O N S
//=============================================================================

class Graph;
class GraphParser;


//=============================================================================
//	C L A S S  D E C L A R A T I O N
//=============================================================================

/**	To read a Graph from a file
*/
class GRAPH_API GraphReader
{

public:

  //---------------------------------------------------------------------
  /**	@name Constructors, destructor*/
  //---------------------------------------------------------------------
  //@{

  /**	configure the parametres
     @param filename name of the file to read from
     @param syntax
     @param helpers
  */
  GraphReader(const std::string& filename,
              const carto::SyntaxSet& syntax,
              const carto::AttributedReader::HelperSet& helpers 
              = carto::AttributedReader::HelperSet());

  /**	configure the parametres
     @param syntax
     @param helpers
  */
  GraphReader(const carto::SyntaxSet& syntax,
              const carto::AttributedReader::HelperSet& helpers 
              = carto::AttributedReader::HelperSet());

  virtual ~GraphReader();

  //@}

  //---------------------------------------------------------------------
  /**	@name Read method*/
  //---------------------------------------------------------------------
  //@{

  /**	read a Graph attributes from a file
     @param graph graph to read into
  */
  virtual void read(Graph& graph);

  //@}

  /// \deprecated{obsolete - use attach()}
  void open(const std::string& filename)
    __attribute__((__deprecated__("use attach() instead")));
  void close();
  void attach( std::istream & s, int line_num = 1 );
  void attach( carto::rc_ptr<carto::DataSource> ds );
  void attach( const std::string & filename );
  virtual std::string name() const;
  int line() const;
  bool operator!() const;
  bool is_open() const;
  bool eof() const;
  carto::rc_ptr<carto::DataSource> dataSource();
  const carto::rc_ptr<carto::DataSource> dataSource() const;
  const carto::SyntaxSet & syntaxSet() const { return _syntax; }
  void setSyntax( const carto::SyntaxSet & syntax );

private:

  //---------------------------------------------------------------------
  /**	@name Disable copy*/
  //---------------------------------------------------------------------
  //@{

  /// Copy constructor
  GraphReader(const GraphReader&);

  /// Assignment operator
  GraphReader& operator=(const GraphReader&);

  //@}

  //---------------------------------------------------------------------
  /**	@name Data*/
  //---------------------------------------------------------------------
  //@{

  int					_parser;
  std::map<std::string,int>		_parsernames;
  std::vector<GraphParser *>		_parsers;
  std::string				_filename;
  carto::SyntaxSet			_syntax;
  carto::AttributedReader::HelperSet	_helpers;
  carto::rc_ptr<carto::DataSource>	_datasource;

  //@}
};


//=============================================================================
//	F U N C T I O N S
//=============================================================================

/**	Restore a graph from a file using a GraphReader
	@param reader GraphReader to read from
	@param graph graph to restore
	@return the GraphReader given as a parameter
*/
GRAPH_API GraphReader& operator>>(GraphReader& reader, Graph& graph);


#endif
