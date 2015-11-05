/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL license version 2 under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#include <aims/io/qsqlgraphformat.h>
#include <aims/graph/qsqlgraphformatheader.h>
#include <aims/graph/qsqlgraphdatabase.h>
#include <aims/graph/qsqlgraphhelper.h>
#include <aims/io/finder.h>
#include <aims/io/aimsGraphR.h>
#include <aims/io/aimsGraphW.h>
#include <aims/def/path.h>
#include <graph/graph/graph.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/directory.h>
#include <cartobase/uuid/uuid.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>
#include <qvariant.h>

using namespace aims;
using namespace carto;
using namespace std;


bool FinderQSqlGraphFormat::check( const string & filename, Finder & f ) const
{
  QSqlGraphFormatHeader *hdr = new QSqlGraphFormatHeader( filename );
  try
    {
      hdr->read();
    }
  catch( exception & e )
    {
      delete hdr;
      return( false );
    }
  f.setObjectType( "Graph" );
  string        format, datatype;
  vector<string> possibledatatypes;
  hdr->getProperty( "file_type", format );
  hdr->getProperty( "data_type", datatype );
  hdr->getProperty( "possible_data_type", possibledatatypes );
  f.setFormat( format );
  f.setDataType( datatype );
  f.setPossibleDataTypes( possibledatatypes );
  f.setHeader( hdr );

  return true;
}


// -------

Graph* QSqlGraphFormat::read( const std::string & filename,
                              const carto::AllocatorContext & context,
                              carto::Object options )
{
  QSqlGraphFormatHeader *hdr = new QSqlGraphFormatHeader( filename );
  try
  {
    hdr->read();
  }
  catch( exception & e )
  {
    // cout << "exception: " << e.what() << endl;
    delete hdr;
    throw;
  }
  string syntax;
  hdr->getProperty( "arg_syntax", syntax );
  Graph *graph = new Graph( syntax );
  graph->setProperty( "header", Object( static_cast<GenericObject *>( hdr ) ) );
  if( !read( filename, *graph, context, options ) )
    throw invalid_format_error( "SQL graph reading failed", filename );
  return graph;
}


bool QSqlGraphFormat::read( const std::string & filename1, Graph & graph,
                            const carto::AllocatorContext &,
                            carto::Object options )
{
  // cout << "QSqlGraphFormat::read\n";
  bool useJoins = false;
  try
  {
    Object uj = options->getProperty( "use_joins" );
    if( !uj.isNull() )
      useJoins = uj->getScalar();
  }
  catch( ... )
  {
  }
  QSqlGraphDatabase db;
  Object hdr;
  bool hadhdr = graph.getProperty( "header", hdr );
  if( hadhdr )
    db.setHeader( hdr );
  db.setUrl( filename1, hdr );
  string syntax, filename = db.hostname();
  hdr = db.header();
  if( !hadhdr )
    static_cast<QSqlGraphFormatHeader *>( hdr.get() )->read();

  vector<int> gids;
  int gid = -1, targetgid = -1;
  vector<string> syntaxs;

  if( !hdr->getProperty( "graph_sql_eid", gids ) || gids.size() == 0 )
    throw wrong_format_error( "database does not contain any graph",
                              filename );

  if( gids.size() == 1 )
  {
    hdr->getProperty( "arg_syntax", syntax );
    syntaxs.push_back( syntax );
  }
  else
  {
    hdr->getProperty( "arg_syntax", syntaxs );
  }

  // parse URL and retreive request if any
  Object parsedurl = db.parsedUrl();
  string query, graphquery;
  parsedurl->getProperty( "query", query );
  // simple pre-parsing
  string::size_type t = query.find( "Graph.eid=" );
  if( t != string::npos )
  {
    istringstream sst( query.substr( t + 10, query.length() - t - 10 ) );
    sst >> targetgid;
  }
  else
  {
    t = query.find( "Graph." );
    if( t != string::npos )
    {
      string::size_type t2 = query.find( ';', t );
      if( t2 != string::npos )
        graphquery = query.substr( t2, t - t2 - 1 );
      else
        graphquery = query.substr( t, query.length() - t );
    }
  }

  if( targetgid >= 0 )
  {
    ostringstream osgid;
    osgid << targetgid;
    string sgid = osgid.str();
    unsigned i, n = gids.size();
    for( i=0; i<n; ++i )
      if( gids[i] == targetgid )
        break;
    if( i == n )
      throw invalid_format_error( "database does not contain the requested "
        "graph " + sgid, filename );
    gid = targetgid;
    if( syntaxs.size() <= i )
      throw invalid_format_error( "graphs IDs and types mismatch", filename );
    syntax = syntaxs[i];
  }
  else if( gids.size() == 1 )
  {
    gid = gids[0];
    syntax = syntaxs[0];
  }

  // (re-)open DB connection
  bool ok = db.open();
  if( !ok )
  {
    throw wrong_format_error( db.lastError().text().toStdString(),
                              filename );
  }

  // find graph if a SQL query is provided
  if( !graphquery.empty() )
  {
    QSqlQuery res = db.exec( string( "SELECT eid FROM Graph WHERE " )
      + graphquery );
    if( res.lastError().type() != 0 )
      throw invalid_format_error( string( "Graph reader, graph selection:" )
        + res.lastError().text().toStdString(), filename );
    set<int> selgids;
    while( res.next() )
      selgids.insert( res.value(0).toInt() );
    if( selgids.size() != 1 )
      throw invalid_format_error(
        "Graph reader, graph selection: did not select one graph", filename );
    gid = *selgids.begin();
    unsigned i, n = gids.size();
    for( i=0; i<n; ++i )
      if( gids[i] == gid )
        break;
    if( i == n )
      throw invalid_format_error( "database does not contain the requested "
        "graph", filename );
    if( syntaxs.size() <= i )
      throw invalid_format_error( "graphs IDs and types mismatch", filename );
    syntax = syntaxs[i];
  }
  if( gid < 0 )
    throw invalid_format_error( "database contains multiple graphs",
                                filename );
  ostringstream osgid;
  osgid << gid;
  string sgid = osgid.str();
  graph.setSyntax( syntax );
  // cout << "gid: " << gid << endl;

  // read syntax in DB
  db.readSchema();
  rc_ptr<map<string, map<string, vector<string> > > > syntaxattributes
    = db.attributesSyntax;
  map<string, map<string, vector<string> > > & attributes = *syntaxattributes;

  // read graph attributes
  db.readGraphAttributes( graph, gid );

  // retreive vertices types
  set<string> vtypes;
  map<int, int> gindex;
  QString vidstring;
  string sql;
  QSqlQuery res;

  if( useJoins )
  {
    sql = "SELECT class_name FROM class JOIN _Vertex ON _Vertex.eid=class.eid"
      " WHERE _Vertex.graph=" + sgid + " GROUP BY class_name";
    res = db.exec( sql );
    if( res.lastError().type() != 0 )
      throw wrong_format_error( res.lastError().text().toStdString(),
                                filename );
    while( res.next() )
      vtypes.insert( res.value(0).toString().toStdString() );
  }
  else
  {
    QString sql = QString( "SELECT eid, graph_index, class_name FROM _Vertex "
      "WHERE _Vertex.graph=" ) + sgid.c_str();
    res = db.database().exec( sql );
    if( res.lastError().type() != 0 )
      throw wrong_format_error( res.lastError().text().toStdString(),
                                filename );
    bool first = true;
    while( res.next() )
    {
      int eid = res.value(0).toInt();
      gindex[ eid ] = res.value(1).toInt();
      if( first )
        first = false;
      else
        vidstring += ", ";
      vidstring += QString::number( eid );
      vtypes.insert( res.value(2).toString().toStdString() );
    }
  }
  map<int, Vertex *> id2vertex;

  // cout << "vertices types: " << vtypes.size() << endl;
  // select vertices
  set<string>::iterator ivt, evt = vtypes.end();
  for( ivt=vtypes.begin(); ivt!=evt; ++ivt )
  {
    const string & vt = *ivt;
    const map<string, vector<string> > & vatts = attributes[ vt ];
    int start = 1;
    sql = "SELECT eid";
    if( useJoins )
    {
      sql += ", graph_index";
      start = 2;
    }
    map<string, vector<string> >::const_iterator ivat, evat = vatts.end();
    for( ivat=vatts.begin(); ivat!=evat; ++ivat )
      sql += string( ", " ) + ivat->first;
    sql += " FROM ";
    if( useJoins )
      sql += vt + " WHERE graph=" + sgid;
    else
      sql += "_" + vt + " WHERE eid IN ( " + vidstring.toStdString() + " )";
      // sql += "_" + vt + " WHERE graph=" + sgid;
    // cout << "vtype: " << vt << ", SQL:\n" << sql << endl;
    res = db.exec( sql );
    if( res.lastError().type() != 0 )
      throw wrong_format_error( res.lastError().text().toStdString(),
                                filename );
    while( res.next() )
    {
      Vertex *v = graph.addVertex( vt );
      int vid = res.value(0).toInt();
      id2vertex[ vid ] = v;
      db.readElementAttributes( *v, res, vatts, start );
      if( useJoins )
        v->setProperty( "index", res.value(1).toInt() );
      else
        v->setProperty( "index", gindex[ vid ] );
    }
  }

  // retreive edges types
  map<string, QString> etypes;
  map<string, QString>::iterator iet, eet = etypes.end();
  map<int, vector<int> > eindex;
  QString eidstring;
  if( useJoins )
  {
    sql = "SELECT class_name FROM class JOIN _Vertex,_Edge ON "
      "_Edge.eid=class.eid AND _Edge.eid=class.eid WHERE "
      "(_Vertex.eid=_Edge.vertex1 OR _Vertex.eid=_Edge.vertex2) AND "
      "_Vertex.graph=" + sgid + " GROUP BY class_name";
    res = db.exec( sql );
    if( res.lastError().type() != 0 )
      throw wrong_format_error( res.lastError().text().toStdString(),
                                filename );
    while( res.next() )
      etypes[ res.value(0).toString().toStdString() ] = "";

    // cout << "edges types: " << etypes.size() << endl;
    // select edges
    for( iet=etypes.begin(); iet!=eet; ++iet )
    {
      const string & et = iet->first;
      // cout << "edge type: " << et << endl;
      const map<string, vector<string> > & eatts = attributes[ et ];
      sql = string( "SELECT " ) + et + ".vertex1, " + et + ".vertex2";
      map<string, vector<string> >::const_iterator ieat, eeat = eatts.end();
      for( ieat=eatts.begin(); ieat!=eeat; ++ieat )
        sql += string( ", " ) + et + "." + ieat->first;
      sql += " FROM " + et + " JOIN _Vertex ON _Vertex.eid=vertex1 WHERE "
        "_Vertex.graph=" + sgid;
      // cout << "etype: " << et << ", SQL:\n" << sql << endl;
      res = db.exec( sql );
      if( res.lastError().type() != 0 )
        throw wrong_format_error( res.lastError().text().toStdString(),
                                  filename );
      while( res.next() )
      {
        Vertex *v1 = id2vertex[ res.value(0).toInt() ];
        Vertex *v2 = id2vertex[ res.value(1).toInt() ];
        Edge* e = graph.addEdge( v1, v2, et );
        db.readElementAttributes( *e, res, eatts, 2 );
      }
    }
  }
  else
  {
    QString sql = QString( "SELECT eid, vertex1, vertex2, graph_index, "
      "class_name FROM _Edge WHERE graph=" ) + sgid.c_str();
    res = db.database().exec( sql );
    if( res.lastError().type() != 0 )
      throw wrong_format_error( res.lastError().text().toStdString(),
                                filename );
    while( res.next() )
    {
      int eid = res.value(0).toInt();
      vector<int> & ee = eindex[ eid ];
      ee.reserve( 3 );
      ee.push_back( res.value(1).toInt() );
      ee.push_back( res.value(2).toInt() );
      ee.push_back( res.value(3).toInt() );
      QString & s = etypes[ res.value(4).toString().toStdString() ];
      if( !s.isEmpty() )
        s += ", ";
      s += QString::number( eid );
    }

    // cout << "edges types: " << etypes.size() << endl;
    // select edges
    for( iet=etypes.begin(); iet!=eet; ++iet )
    {
      const string & et = iet->first;
      // cout << "edge type: " << et << endl;
      const map<string, vector<string> > & eatts = attributes[ et ];
      sql = "SELECT eid";
      map<string, vector<string> >::const_iterator ieat, eeat = eatts.end();
      for( ieat=eatts.begin(); ieat!=eeat; ++ieat )
        sql += ", " + QString( ieat->first.c_str() );
      sql += QString( " FROM _" ) + et.c_str() + " WHERE eid IN ( "
        + iet->second + " )";
      // cout << "etype: " << et << ", SQL:\n" << sql.toStdString() << endl;
      res = db.database().exec( sql );
      if( res.lastError().type() != 0 )
        throw wrong_format_error( res.lastError().text().toStdString(),
                                  filename );
      while( res.next() )
      {
        int eid = res.value(0).toInt();
        const vector<int> & ee = eindex[ eid ];
        Vertex *v1 = id2vertex[ ee[0] ];
        Vertex *v2 = id2vertex[ ee[1] ];
        Edge* e = graph.addEdge( v1, v2, et );
        db.readElementAttributes( *e, res, eatts, 1 );
        e->setProperty( "index", ee[2] );
      }
    }
  }

  db.close();

  // read the .data directory part
  int subobjectsfilter = -1;
  try
  {
    Object filt = options->getProperty( "subobjectsfilter" );
    subobjectsfilter = (int) filt->getScalar();
  }
  catch( ... )
  {
  }

  AimsGraphReader     gr( filename );
  if( subobjectsfilter < 0 )
    gr.readElements( graph, 3 );
  else if( subobjectsfilter > 0 )
    gr.readElements( graph, 1 );
  else
  {
    graph.setProperty( "aims_reader_filename", filename );
    graph.setProperty( "aims_reader_loaded_objects", int(0) );
  }


  return true;
}


bool QSqlGraphFormat::write( const std::string & filename1,
                             const Graph & graph, carto::Object )
{
  // cout << "QSqlGraphFormat::write " << filename1 << endl;

  // WARNING we may require non-const access to the graph to update its
  // internal IO state
  Graph & nonconst_graph = const_cast<Graph &>( graph );

  // now SQL part
  QSqlGraphDatabase db;
  db.setUrl( filename1 );
  QSqlGraphFormatHeader h( filename1 );
  Object parsedurl = db.parsedUrl();
  string filename = db.hostname();

  /* for now, erase the output file if it exists.
     This behaviour is consistent with any other IO routines,
     but is unusual and *dangerous* on a database, which may contain several
     graphs and any other data.
     TODO: fix it */
/*  string st = FileUtil::fileStat( filename );
  if( st.find( '+' ) != string::npos )
  {
    unlink( filename.c_str() );
  }
*/

  bool ok = db.open();
  if( !ok )
  {
    throw wrong_format_error( db.lastError().text().toStdString(),
                              filename );
  }

  // check if DB already contains the graph schema
  string sql = "SELECT name FROM _soma_classes WHERE name='Graph'";
  QSqlQuery res = db.exec( sql );
  ok = false;
  if( res.lastError().type() == 0 )
    while( res.next() )
    {
      ok = true;
      break;
    }
  if( !ok )
  {
    // cout << "creating schema\n";
    // create schema using pre-recorded map<string, vector<string> > SQL code
    // generated by soma-databases
    // (soma.database.graph_schema python module)
    string schemapath = Path::singleton().syntax() + FileUtil::separator()
      + "graphschema.sql";
    // cout << "SQL schema file: " << schemapath << endl;
    ifstream f( schemapath.c_str() );
    if( !f )
      io_error::launchErrnoExcept( schemapath );
    // read schema line by line
    while( !f.eof() )
    {
      string line;
      bool multiline = false;
      do
      {
        char c = f.get();
        if( f.eof() )
          break;
        while( c != '\0' && c != '\n' )
        {
          if( c != '\r' )
            line += c;
          c = f.get();
        }
        if( !multiline && line.length() >= 12
          && line.substr( line.length() - 12, 12 ) == "FOR EACH ROW" )
          multiline = true;
        else if( multiline && line.length() >= 3
          && line.substr( line.length() - 3, 3 ) == "END" )
          multiline = false;
        if( multiline )
          line += '\n';
      }
      while( multiline && !f.eof() );
      // execute SQL line
      res = db.exec( line );
      if( res.lastError().type() != 0 )
        throw wrong_format_error( res.lastError().text().toStdString(),
                                  filename );
    }
    // cout << "SQL graph schema created\n";
  }

  // retreive or generate UUID
  string guuid;
  bool existinggraph = false;
  if( !graph.getProperty( "uuid", guuid ) )
  {
    guuid = UUID().toString();
    nonconst_graph.setProperty( "uuid", guuid );
  }
  else
    existinggraph = true;

  // first write meshes, buckets, volumes etc

  string datadir = FileUtil::removeExtension( filename ) + ".data";
  Directory ddir( datadir );
  if( !ddir.isValid() )
    ddir.mkdir();
  datadir
    = FileUtil::basename( datadir ) + FileUtil::separator() + guuid;
  nonconst_graph.setProperty( "filename_base", datadir );
  AimsGraphWriter   gw( filename );
  AimsGraphWriter::SavingMode   sm = AimsGraphWriter::Keep;
  /* if( forceglobal )
  sm = AimsGraphWriter::Global; */
  gw.writeElements( nonconst_graph, sm, sm );
  //cout << "writeElements done\n";

  // nonconst_graph.setProperty( "name", "noname" );
  // nonconst_graph.setProperty( "side", "noside" );
  // get or generate a UUID
  int gid = -1;
  string sgid;

  if( existinggraph )
  {
    existinggraph = false;
    // check if there are any graph with the same uuid
    res = db.exec( string( "SELECT Graph.eid, class.class_name FROM Graph"
      " JOIN class ON class.eid=Graph.eid WHERE Graph.uuid='" )
      + guuid + "'" );
    if( res.lastError().type() == 0 )
    {
      if( res.next() )
      {
        gid = res.value(0).toInt();
        existinggraph = true;
        cout << "found the graph already in the DB: eid = " << gid << endl;

        stringstream ssgid;
        ssgid << gid;
        sgid = ssgid.str();
      }
    }
  }
  // if yes, erase it with all nodes ?

  // FIXME: for now erase all because we don't have uids to identify data

  /* FIXME: don't delete because we cannot delete views (Graph, Vertex...)
      but we must delete concrete instances (CorticalFoldArg, fold...)
  cout << "deleting graphs\n";
  sql = "DELETE FROM Graph";
  res = db.exec( sql.c_str() );
  if( res.lastError().type() != QSqlError::NoError )
    throw invalid_format_error( res.lastError().text().toStdString(),
                                filename );
  cout << "deleting vertices\n";
  sql = "DELETE FROM Vertex";
  res = db.exec( sql.c_str() );
  if( res.lastError().type() != QSqlError::NoError )
    throw invalid_format_error( res.lastError().text().toStdString(),
                                filename );
  cout << "deleting edges\n";
  sql = "DELETE * FROM Edge";
  res = db.exec( sql.c_str() );
  if( res.lastError().type() != QSqlError::NoError )
    throw invalid_format_error( res.lastError().text().toStdString(),
                                filename );
  */

  //(re-) read syntax in DB
  db.readSchema();

  QSqlGraphDatabase::CurrentGraphData gdata( gid, &nonconst_graph );
  if( existinggraph )
    db.fillGraphData( gdata, nonconst_graph, gid );

  // write graph
  if( existinggraph )
    db.updateElement( graph, gid );
  else
  {
    sql = "INSERT INTO " + graph.getSyntax() + " ( ";
    string values;
    db.sqlAttributesAsStrings( graph, sql, values );
    sql += string( " ) values ( " ) + values + " )";
    // cout << "graph SQL: " << sql << endl;
    res = db.exec( sql );
    if( res.lastError().type() != QSqlError::NoError )
      throw invalid_format_error( res.lastError().text().toStdString(),
                                  filename );

    // get graph id
    // cout << "querying graph eid\n";
    res = db.exec( "SELECT eid FROM Graph ORDER BY eid DESC LIMIT 1" );
    if( res.lastError().type() != QSqlError::NoError )
      throw invalid_format_error( res.lastError().text().toStdString(),
                                  filename );
    res.next();
    int gid = res.value(0).toInt();
    cout << "graph eid: " << gid << endl;

    gdata.gid = gid;
    stringstream ssgid;
    ssgid << gid;
    sgid = ssgid.str();
  }

  // write vertices

  Graph::const_iterator iv, ev = graph.end();
  for( iv=graph.begin(); iv!=ev; ++iv )
    db.insertOrUpdateVertex( *iv, gdata );

  db.updateVertexIndexMap( gdata );
  db.updateVertexMap( gdata );

  // cout << "vertices written\n";

  // write edges

  set<Edge *>::const_iterator ie, ee = graph.edges().end();
  // cout << "saving edges: " << graph.edges().size() << endl;
  for( ie=graph.edges().begin(); ie!=ee; ++ie )
    db.insertOrUpdateEdge( *ie, gdata );

  db.close();

  return true;
}

