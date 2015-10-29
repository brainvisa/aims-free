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

#include <cstdlib>
#include <graph/graph/gparser_shfj.h>
#include <graph/graph/graph.h>
#include <cartobase/object/lexicon.h>
#include <cartobase/exception/ioexcept.h>
#include <soma-io/datasource/filteringdatasource.h>
#include <soma-io/utilities/asciidatasourcetraits.h>

using namespace std;
using namespace carto;


//=============================================================================
//	M E T H O D S
//=============================================================================

GraphParser_shfj::GraphParser_shfj( const SyntaxSet& syntax, 
                                    const HelperSet& helpers )
  : GraphParser(), AttributedReader( syntax, helpers )
{
}


GraphParser_shfj::~GraphParser_shfj()
{
}


rc_ptr<DataSource> GraphParser_shfj::dataSource()
{
  return GraphParser::dataSource();
}


string GraphParser_shfj::name() const
{
  return GraphParser::name();
}


void GraphParser_shfj::attach( rc_ptr<DataSource> ds )
{
  GraphParser::attach( ds );
}


int
GraphParser_shfj::read(Graph& graph)
{
  rc_ptr<DataSource>	ds( new FilteringDataSource( dataSource() ) );
  /* cout << "GraphParser_shfj, ds: " << ds.get() << endl;
  if( ds )
  cout << "url: " << ds->url() << endl; */
  AttributedReader::attach( ds );

  string token;

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  if (token != Lexicon::begin())
    throw wrong_format_error( name() );

  StreamUtil::skip( *ds, " \t" );
  token = StreamUtil::readUntil( *ds );
  if (token != Lexicon::graph() && token != "BINARYGRAPH") // compatibility
    throw parse_error(Lexicon::graph(), token, \
                      name(), line());
  graph.clear();
  graph.clearProperties();

  StreamUtil::skip( *ds, " \t" );
  token = StreamUtil::readUntil( *ds );
  if (token == "1.0") // compatibility
    {
      StreamUtil::skip( *ds );
      token = StreamUtil::readUntil( *ds );
    }
  if (_rules.find(token) == _rules.end())
    throw parse_error("{syntactic attribute}", token, \
                      name(), line());
  graph.getInterface<SyntaxedInterface>()->setSyntax(token);

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  while (token != Lexicon::end())
    {
      if ( !ds )
        throw parse_error(Lexicon::end(), token, \
                          name(), line());
      if (token == Lexicon::begin())
        {
          StreamUtil::skip( *ds );
          token = StreamUtil::readUntil( *ds );
          if (token == Lexicon::vertex())
            readVertex(graph);
          else if (token == Lexicon::uedge() || token == "UBEDGE")
            readUEdge(graph);
          else if (token == Lexicon::dedge())
            readDEdge(graph);
          else
            throw parse_error("{graph component}", token, \
                              name(), line());
        }
      else
        readAttribute( graph, token );
      StreamUtil::skip( *ds );
      token = StreamUtil::readUntil( *ds );
    }

  set<string> missing;
  if (!graph.check(_rules, missing))
    {
      string s("missing semantic attribute ");
      s+= *(missing.begin());

      set<string>::const_iterator	i, e = missing.end();
      for( i=missing.begin(), ++i; i!=e; ++i )
        s += string( ", " ) + *i;
      throw syntax_check_error( s.c_str(), name() );
    }
  return 0;
}


void
GraphParser_shfj::readVertex(Graph& graph)
{
  string token;
  rc_ptr<DataSource>	ds = dataSource();

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  if (_rules.find(token) == _rules.end())
    throw parse_error("{valid syntactic attribute}", token,
                      name(), line());

  string syntactic = token;

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  int id = atoi(token.c_str());
  if (id <= 0 || _serializer.find(id) != _serializer.end())
    throw parse_error("{valid vertex id}", token,
                      name(), line());

  Vertex* vertex = graph.addVertex(syntactic);
  _serializer[id] = vertex;

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  while (token != Lexicon::end())
    {
      if (eof())
        throw parse_error(Lexicon::end(), token,
                          name(), line());
      else
        readAttribute(vertex, token);
      StreamUtil::skip( *ds );
      token = StreamUtil::readUntil( *ds );
    }
}


void
GraphParser_shfj::readUEdge(Graph& graph)
{
  rc_ptr<DataSource>	ds = dataSource();
  string token;

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  if (_rules.find(token) == _rules.end())
    throw parse_error("{valid syntactic attribute}", token,
                      name(), line());

  string syntactic = token;

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  int id1 = atoi(token.c_str());
  if (id1 <= 0 || _serializer.find(id1) == _serializer.end())
    throw parse_error("{valid vertex id}", token,
                      name(), line());

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  int id2 = atoi(token.c_str());
  if (id2 <= 0 || _serializer.find(id2) == _serializer.end())
    throw parse_error("{valid vertex id}", token,
                      name(), line());

  Edge* edge = graph.addUndirectedEdge(_serializer[id1], \
                                       _serializer[id2], syntactic);

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  while (token != Lexicon::end())
    {
      if (eof())
        throw parse_error(Lexicon::end(), token, \
                          name(), line());
      else
        readAttribute(edge, token);
      StreamUtil::skip( *ds );
      token = StreamUtil::readUntil( *ds );
    }
}


void
GraphParser_shfj::readDEdge(Graph& graph)
{
  rc_ptr<DataSource>	ds = dataSource();
  string token;

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  if (_rules.find(token) == _rules.end())
    throw parse_error("{valid syntactic attribute}", token,
                      name(), line());

  string syntactic = token;

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  int id1 = atoi(token.c_str());
  if (id1 <= 0 || _serializer.find(id1) == _serializer.end())
    throw parse_error("{valid vertex id}", token,
                      name(), line());

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  int id2 = atoi(token.c_str());
  if (id2 <= 0 || _serializer.find(id2) == _serializer.end())
    throw parse_error("{valid vertex id}", token,
                      name(), line());

  Edge* edge = graph.addDirectedEdge(_serializer[id1], \
                                     _serializer[id2], syntactic);

  StreamUtil::skip( *ds );
  token = StreamUtil::readUntil( *ds );
  while (token != Lexicon::end())
    {
      if (eof())
        throw parse_error(Lexicon::end(), token, \
                          name(), line());
      else
        readAttribute(edge, token);
      StreamUtil::skip( *ds );
      token = StreamUtil::readUntil( *ds );
    }
}
