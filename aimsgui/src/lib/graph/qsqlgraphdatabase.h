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

#ifndef AIMS_GRAPH_QSQLGRAPHDATABASE_H
#define AIMS_GRAPH_QSQLGRAPHDATABASE_H

#include <graph/graph/graph.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlerror.h>


namespace aims
{

  /** Graph manipulation in SQL databases utilities.

      A QSqlGraphDatabase instance stores the database connection, and caches
      several needed information on the database, amongst which the database
      schema (elements inheritance and attributes).

      It is useful to facilitate IO for graphs in SQL databases. It is used by
      Reader\<Graph\> and Writer\<Graph\> for database formats. It can also be
      used to perform partial reading on multiple graphs:

      (the example is in python language, it is just a bit clearer)

      \code
      from soma import aims
      from soma.aims import aimsgui

      db = aims.QSqlGraphDatabase()
      db.setUrl( '/tmp/morphee2.sqlite' )
      db.open()
      db.readSchema()
      lg = aims.aimsguisip.list_CurrentGraphData()
      ng = db.partialReadFromVertexQuery( 'SELECT fold.eid, fold.graph, ' \
        'fold.graph_index, fold.name, fold.label, class.class_name FROM fold ' \
        'JOIN class ON class.eid=fold.eid WHERE fold.name=\'ventricle_left\'',
        lg )
      \endcode
  */
  class QSqlGraphDatabase
  {
  public:
    typedef std::map<std::string, std::map<std::string,
      std::vector<std::string> > > Syntax;

    /** This structure stores information needed to help and speed-up
        graph queries in SQL databases.

        It is required for several methods of
        the QSqlGraphDatabase class. It must be intantiated, then filled with
        the database information, using the fillGraphData method.
    */
    struct CurrentGraphData
    {
      CurrentGraphData( int eid, Graph* g ) : gid( eid ), graph( g ) {}
      bool operator == ( const CurrentGraphData & ) const;

      int gid;
      Graph* graph;
      std::map<int, int> vertexindex2eid;
      std::map<Vertex *, int> vertexeid;
      std::map<int, int> edgeindex2eid;
      std::map<Edge *, int> edgeeid;
    };

    QSqlGraphDatabase();
    ~QSqlGraphDatabase();

    /** Set the database address, and parse it. Or it can be already parsed in
        an Object. Only after using this method, hostname(), header(),
        parsedUrl() and open() methods may be used.

        The url may contain an initial selection query, to restrict to a given
        single graph, especially for Reader\<Graph\> operations.

        \code
        db.setUrl( "/home/totor/mysqlgraphs.sqlite?Graph.uuid="
          "'259049f8-e93c-095d-064f-fad19a73dc0a'" );
        \endcode
    */
    void setUrl( const std::string & url,
                 carto::Object header = carto::none() );
    /// Actually open the database connection
    bool open();
    bool open( const std::string & user, const std::string & password );
    /// Close the connection. After this call, queries cannot be done.
    void close();
    /// Shortcut to the Qt QSqlDatabase exec() query for SQL code
    QSqlQuery exec( const std::string & query ) const;
    /// Shortcut to the Qt QSqlDatabase lastError() call for SQL code
    QSqlError lastError() const;

    /// Address of the database (may be a local file for SQLite)
    std::string hostname() const;
    /// Qt database
    QSqlDatabase & database() const;
    /** The header stores information about how to open the database, and
        possibly an initial selection query, to restrict to a given single
        graph, especially for Reader\<Graph\> operations.
    */
    carto::Object header();
    /// Decomposed URL
    carto::Object parsedUrl();
    void setHeader( carto::Object hdr );

    /** Read the database schema (or syntax) from the SQL database.

        This fills the attributesSyntax and syntaxInheritance fields.
    */
    bool readSchema();
    /** Build an attributes/value list from a graph element according to the
        database schema, in a SQL-compatible way */
    void sqlAttributes( const GraphObject & el,
                        std::map<std::string, std::string> & atts,
                        bool addinherited = true,
                        const std::string & assyntax = std::string() ) const;
    /** Same as sqlAttributes but fills two strings for attributes names and
        values, as needed for SQL insert queries */
    void sqlAttributesAsStrings( const GraphObject & el, std::string & names,
                                 std::string & values, bool first = true,
                                 bool addinherited = true ) const;
    /** Store a graph vertex into the database, or updates it if it already
        exists */
    void insertOrUpdateVertex( Vertex* vert, CurrentGraphData & data );
    /** Store a graph edge into the database, or updates it if it already
        exists */
    void insertOrUpdateEdge( Edge* v, CurrentGraphData & data );
    /** Reads information from a given SQL selection query, and fills
        graph/vertices information from it.

        The SQL query is executed as is, and must include eid and graph columns
        from vertices.

        It is also strongly advised to also get the class_name column from the
        class table (via a JOIN in the SQL query) because it avoids individual
        queries for it in each vertex.

        Referenced graphs are queried and added to the CurrentGraphData list if
        allownewgraphs is true, otherwise vertices belonging to an "unknown"
        graph are dropped.

        \return a list of new graphs which have been allocated if allownewgraphs
        is true. New graphs must be deleted when not needed any longer.

        for instance:
        \code
        // classes will be queried separately
        ng = db.partialReadFromVertexQuery( 'SELECT fold.eid, fold.graph,
          fold.graph_index, fold.name, fold.label, class.class_name FROM fold
          JOIN class ON class.eid=fold.eid WHERE fold.name=\'ventricle_left\'',
          lg );

        // here classes are part of the same query, which will avoid
        // additional queries
        ng = db.partialReadFromVertexQuery( 'SELECT fold.eid, fold.graph,
          fold.graph_index, fold.name, fold.label, class.class_name FROM fold
          JOIN class ON class.eid=fold.eid', lg );
        \endcode
    */
    std::list<carto::rc_ptr<Graph> >
    partialReadFromVertexQuery( const std::string & sqlquery,
                                std::list<CurrentGraphData> & graphsinfo,
                                bool allownewgraphs = true );
    /** Reads information from a given SQL selection query, and fills
        edge information from it.

        The SQL query is executed as is, and must include eid, vertex1 and
        vertex2 from edges.

        It is also strongly advised to also get the class_name column from the
        class table (via a JOIN in the SQL query) because it avoids individual
        queries for it in each edge, and if possible the graph eid (via a JOIN
        on the vertices in ths SQL query).

        Referenced graphs /vertices are queried and added to the
        CurrentGraphData list if allownewgraphs is true, otherwise edges
        belonging to an "unknown" graph are dropped.

        \return a list of new graphs which have been allocated if allownewgraphs
        is true. New graphs must be deleted when not needed any longer.

        for instance:
        \code
        // graphs will be queried separately
        ng = db.partialReadFromEdgeQuery( 'SELECT cortical.eid,
        cortical.vertex1, cortical.vertex2, cortical.size, class.class_name FROM
        cortical JOIN class ON class.eid=cortical.eid WHERE cortical.size > 50',
        lg )

        // here graph info is part of the same query
        ng = db.partialReadFromEdgeQuery( 'SELECT cortical.eid,
        cortical.vertex1, cortical.vertex2, cortical.size, class.class_name,
        _Vertex.graph FROM cortical JOIN class, _Vertex ON
        class.eid=cortical.eid AND _Vertex.eid=cortical.vertex1 WHERE
        cortical.size > 50', lg )
        \endcode
    */
    std::list<carto::rc_ptr<Graph> >
    partialReadFromEdgeQuery( const std::string & sqlquery,
                              std::list<CurrentGraphData> & graphsinfo,
                              bool allownewgraphs = true );

    /** Store in the database the attributes of an existing element (graph,
    vertex or edge) */
    void updateElement( const GraphObject & el, int eid );
    /** Fills the CurrentGraphData structure using database information
        (performs appropriate queries) */
    void fillGraphData( CurrentGraphData & data, Graph & g, int geid=-1 );
    /// Fills the CurrentGraphData vertexindex2eid map
    void updateVertexIndexMap( CurrentGraphData & data );
    /// Fills the CurrentGraphData edgeindex2eid map
    void updateEdgeIndexMap( CurrentGraphData & data );
    /// Fills the CurrentGraphData vertexeid map
    void updateVertexMap( CurrentGraphData & data );
    /// Fills the CurrentGraphData edgeeid map
    void updateEdgeMap( CurrentGraphData & data );
    /// Set the element attributes contained in the query result
    void readElementAttributes( GraphObject &,
                                const std::list<std::string> & attributes,
                                QSqlQuery & query, int eid );
    /** Set the element attributes contained in the query result, but read
        from an already filled vector of QVariant instrad of directly the SQL
        query result
    */
    void readElementAttributes( GraphObject & item,
                                const std::list<std::string> & attributes,
                                const std::vector<QVariant> & query,
                                int i );
    /** read/set *all* the attributes of the given element, which must have all
        been queried in the attributes map order.
        \param i is the index in the query of the first attribute
    */
    void readElementAttributes( GraphObject & item, QSqlQuery & query,
        const std::map<std::string, std::vector<std::string> > & vatts,
        int i );
    /* * read/set *all* the attributes of the given element, but read
        from an already filled vector of QVariant instrad of directly the SQL
        query result
    */
    /*
    void readElementAttributes( GraphObject & item, QSqlQuery & query,
        const std::map<std::string, std::vector<std::string> > & vatts,
        int i );
        */
    void readGraphAttributes( Graph & g, int eid );
    // read all attributes of multiple graphs
    void readGraphAttributes( std::list<CurrentGraphData> & graphsinfo );

    /** Database schema syntax: supported attributes (database columns) for
        each element type
    */
    carto::rc_ptr<Syntax> attributesSyntax;
    /// Database schema syntax: elements types inheritance
    std::map<std::string, std::list<std::string> > syntaxInheritance;

  private:
    struct Private;
    Private *d;
    std::string _hostname;
    carto::Object _header;
    carto::Object _parsedurl;
    mutable QSqlDatabase *_database;
    std::string _connectionname;
  };


  // ------

  inline QSqlDatabase & QSqlGraphDatabase::database() const
  {
    return *_database;
  }


  inline carto::Object QSqlGraphDatabase::header()
  {
    return _header;
  }


  inline carto::Object QSqlGraphDatabase::parsedUrl()
  {
    return _parsedurl;
  }


  inline std::string QSqlGraphDatabase::hostname() const
  {
    return _hostname;
  }


  inline QSqlQuery QSqlGraphDatabase::exec( const std::string & query ) const
  {
    return database().exec( query.c_str() );
  }


  inline QSqlError QSqlGraphDatabase::lastError() const
  {
    return database().lastError();
  }


  inline bool QSqlGraphDatabase::CurrentGraphData::operator ==
  ( const CurrentGraphData & other ) const
  {
    return gid == other.gid && graph == other.graph;
  }

}

#endif

