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

  /// obsolete - use atatch()
  void open(const std::string& filename);
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
