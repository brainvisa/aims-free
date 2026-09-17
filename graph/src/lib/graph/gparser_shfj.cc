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
        readAttribute(*vertex, token);
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
        readAttribute(*edge, token);
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
        readAttribute(*edge, token);
      StreamUtil::skip( *ds );
      token = StreamUtil::readUntil( *ds );
    }
}
