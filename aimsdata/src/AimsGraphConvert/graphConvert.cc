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
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/io/aimsGraphW.h>
#include <cartobase/config/paths.h>
#include <aims/graph/graphmanip.h>
#include <graph/graph/graph.h>
#include <graph/graph/gwriter.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/object/sreader.h>
#include <cartobase/stream/fileutil.h>
#include <soma-io/reader/pythonreader.h>

using namespace carto;
using namespace aims;
using namespace std;


void copy( carto::AttributedObject *ao, const string & attfrom, 
           const string & attto, bool chk, const Semantic & sem, 
           const SyntaxSet & syntax )
{
  const Semantic	*sm = &sem;
  if( !chk )
    {
      SyntaxSet::const_iterator	is = syntax.find( ao->getSyntax() );
      if( is == syntax.end() )
	return;
      Syntax::const_iterator	iss = is->second.find( attfrom );
      if( iss == is->second.end() )
	return;
      sm = &iss->second;
    }
  const string	& type = sm->type;
  if( type == "string" )
    {
      string	x;
      if( ao->getProperty( attfrom, x ) )
	ao->setProperty( attto, x );
    }
  else if( type == "int" )
    {
      int	x;
      if( ao->getProperty( attfrom, x ) )
	ao->setProperty( attto, x );
    }
  else if( type == "float" )
    {
      float	x;
      if( ao->getProperty( attfrom, x ) )
	ao->setProperty( attto, x );
    }
  else if( type == "int_vector" )
    {
      vector<int>	x;
      if( ao->getProperty( attfrom, x ) )
	ao->setProperty( attto, x );
    }
  else if( type == "float_vector" )
    {
      vector<float>	x;
      if( ao->getProperty( attfrom, x ) )
	ao->setProperty( attto, x );
    }
}

void remaneRoiNodes( Graph& g )
{
  Graph::iterator		iv, ev = g.end();
  
  int roiLabel ;
  string roiName ;
  
  for( iv=g.begin(); iv!=ev; ++iv )
    if( (*iv)->getProperty("name", roiName ) )
      if( roiName == "Unknown" || roiName == "unknown" )
       	if( (*iv)->getProperty("roi_label", roiLabel ) )
 	  (*iv)->setProperty("name", carto::toString<int>(roiLabel) ) ;
  
}


void substitute( Graph& graph,
                 const string& filename )
{

  try
  {

    // opening substitution file
    ValueObject< Dictionary > dictionary;
    PythonReader reader( filename );

    reader.read( dictionary );


    Dictionary syntaxSubstitutions;
    if ( !dictionary.getProperty( "syntax_substitutions",
                                   syntaxSubstitutions ) )
    {

      throw runtime_error(
                      std::string( "'syntax_substitutions' not present in '" ) +
                      filename + "' substitution file" );

    }
    IntDictionary labelSubstitutions;
    if ( !dictionary.getProperty( "label_substitutions",
                                   labelSubstitutions ) )
    {

      throw runtime_error(
                      std::string( "'label_substitutions' not present in '" ) +
                      filename + "' substitution file" );

    }

    string actualArgSyntax = graph.getSyntax();

    Dictionary::const_iterator 
      s = syntaxSubstitutions.find( actualArgSyntax );
    if ( s != syntaxSubstitutions.end() )
    {

      graph.setSyntax( ( s->second )->value< string >() );

    }

    rc_ptr< GraphElementTable > graphElementTable;
    if( !graph.getProperty( "aims_objects_table", graphElementTable ) )
    {

      throw runtime_error( "graph without sub-objects inside of it" );

    }

    Graph::iterator i = graph.begin(), ie = graph.end();

    while ( i != ie )
    {

      string currentNodeSyntax = ( *i )->getSyntax();
      Dictionary::const_iterator 
        s = syntaxSubstitutions.find( currentNodeSyntax );

      if ( s != syntaxSubstitutions.end() )
      {

        string newNodeSyntax = ( s->second )->value< string >();

        GraphElementTable::iterator
          e = graphElementTable->find( currentNodeSyntax );

        if ( e != graphElementTable->end() )
        {

          map< string, GraphElementCode >&
            mgec = ( *graphElementTable )[ newNodeSyntax ];
          mgec = e->second;

          map< string, GraphElementCode >::iterator c = mgec.begin(),
                                                    ce = mgec.end();
          while ( c != ce )
          {

            c->second.syntax = newNodeSyntax;
            ++ c;

          }
          graphElementTable->erase( e );

        }        
        ( *i )->setSyntax( ( s->second )->value< string >() );
        currentNodeSyntax = newNodeSyntax;

      }

      GraphElementTable::const_iterator
         ce = graphElementTable->find( currentNodeSyntax );
      if ( ce == graphElementTable->end() )
      {

        throw runtime_error( "graph without sub-objects inside of it" );

      } 

      if ( ce->second.empty() )
      {

        throw runtime_error( "for given syntax, map of graph element code"
                             " is empty" );

      } 

      map< string, GraphElementCode >::const_iterator
         cc = ce->second.begin();

      int actualLabel;
      if ( ( *i )->getProperty( cc->second.global_index_attribute,
                                 actualLabel ) )
      {

        try
        {

          IntDictionary::const_iterator 
            l = labelSubstitutions.find( actualLabel );
          if ( l != labelSubstitutions.end() )
          {

            ( *i )->setProperty( "name", ( l->second )->value< string >() ) ;

          }
          else
          {

            ( *i )->setProperty( "name", string( "unknown" ) );

          }

        }
        catch ( exception& )
        {

          ( *i )->setProperty( "name", string( "unknown" ) );

        }

      }     
      ++ i;

    }
  
  }
  catch ( exception& e )
  {

    throw runtime_error(
        string( "void renameCustomNodes( Graph& g, const string& filename )" ) +
        " : " + e.what() );

  }

}


void copyatt( Graph & g, const string & attfrom, const string & attto, 
	      const string & stx, const SyntaxSet & syntax  )
{
  Graph::iterator		iv, ev = g.end();
  const set<Edge *>		& edges = g.edges();
  set<Edge *>::const_iterator	ie, ee = edges.end();
  bool				chk = !stx.empty();

  Semantic			sem;
  if( chk )
    {
      SyntaxSet::const_iterator	is = syntax.find( stx );
      if( is == syntax.end() )
	{
	  cerr << "Element syntax " << stx << " is not known - " 
	       << "can't perform copy\n";
	  exit( EXIT_FAILURE );
	}
      Syntax::const_iterator	iss = is->second.find( attfrom );
      if( iss == is->second.end() )
	{
	  cerr << "Element attrinute " << stx << " / " << attfrom 
	       << " isn't in the syntax - can't perform copy\n";
	  exit( EXIT_FAILURE );
	}
      sem = iss->second;
    }

  for( iv=g.begin(); iv!=ev; ++iv )
    if( !chk || (*iv)->getSyntax() == stx )
      copy( *iv, attfrom, attto, chk, sem, syntax );

  for( ie=edges.begin(); ie!=ee; ++ie )
    if( !chk || (*ie)->getSyntax() == stx )
      copy( *ie, attfrom, attto, chk, sem, syntax );
}


int main( int argc, const char **argv )
{
  string	filein, fileout, basename, attfrom, attto, stx;
  bool		global = false;
  bool		local = false;
  bool		volume = false, bucket = false, roi = false;
  string        substitutionFileName;
  AimsApplication	app( argc, argv, "Performs graph storage conversion " 
			     "or volume-S16 labels volume to graph conversion" 
			     );

  app.addOption( filein, "-i", "input data graph" );
  app.alias( "-input", "-i" );
  app.addOption( fileout, "-o", "output data graph (default: same as input)", 
		 true );
  app.alias( "-output", "-o" );
  app.addOption( basename, "-b", "directory where to put sub-data in "
		 "(default: <input>.data)", true );
  app.alias( "-basename", "-b" );
  app.addOption( global, "-g", "save sub-objects in 'global' mode "
		 "(1 file for all objects of same type)", true );
  app.alias( "-global", "-g" );
  app.addOption( local, "-l", "save sub-objects in 'local' mode "
		 "(each object with a specific file) (contradictory with -g)", 
		 true );
  app.alias( "-local", "-l" );
  app.addOption( attfrom, "-c", "copy attribute (use with -d)", true );
  app.addOption( attto, "-d", "destination attribute (for use with -c)", 
		 true );
  app.alias( "-copy", "-c" );
  app.alias( "-dest", "-d" );
  app.addOption( stx, "-s", "syntax of elements involved by copy operations "
		 "(use with -c and -d) [default: no filtering]", true );
  app.alias( "-syntax", "-s" );
  app.addOption( volume, "--volume", "convert buckets to volume of labels", 
		 true );
  app.addOption( bucket, "--bucket", "convert volumes of labels to buckets", 
		 true );

  app.addOption( roi, "--roi", "Output graph will be a roi graph, and all " 
                 "unnamed regions will be named by their roi label", 
		 true );

  app.addOption( substitutionFileName, "--substitution",
                 "Graph substitution file for syntax and label", 
                 true );

  Graph		g( "ClusterArg" );
  SyntaxSet	syntax;

  try
    {
      app.initialize();

      if( global && local )
	{
	  cerr << "cant't use both -g and -l options\n";
	  return EXIT_FAILURE;
	}
      if( volume && bucket )
	{
	  cerr << "can't use both --volume and --bucket options" << endl;
	  return EXIT_FAILURE;
	}

      if( !substitutionFileName.empty() && roi )
	{
	  cerr << "can't use both --roi and --substitution options" << endl;
	  return EXIT_FAILURE;
	}


      if( fileout.empty() )
	fileout = filein;

      char	sep = FileUtil::separator();
      // syntax
      SyntaxReader	sr( Paths::findResourceFile(
        "nomenclature/syntax/graph.stx", "aims" ) );
      sr >> syntax;

      // read
      Reader<Graph>	grd( filein );
      grd.read( g );

      // conversions
      if( volume )
	GraphManip::buckets2Volume( g );
      if( bucket )
	GraphManip::volume2Buckets( g );

      string		base;

      if( !basename.empty() )
	base = basename;
      else
	{
	  base = fileout;
	  string::size_type pos = base.rfind( '.' );
	  if( pos != string::npos )
	    base.erase( pos, base.length() - pos );
	  pos = base.rfind( sep );
	  if( pos != string::npos )
	    base.erase( 0, pos + 1 );
	  base += ".data";
	}
      //cout << "filename_base : " << base << endl;
      if( !base.empty() )
	g.setProperty( "filename_base", base );
      else g.setProperty( "filename_base", string( "*" ) );

      bool	cp = false;
      if( !attfrom.empty() )
	{
	  if( attto.empty() )
	    {
	      cerr << "No destination attribute specified for copy (use -d)\n";
	      return( EXIT_FAILURE );
	    }
	  copyatt( g, attfrom, attto, stx, syntax );
	  cp = true;
	}
      else if( !attto.empty() )
	{
	  cerr << "No source attribute specified for copy (use -c)\n";
	  return( EXIT_FAILURE );
	}
      if( !cp && !stx.empty() )
	{
	  cerr << "-s option must be used with -c and -d\n";
	  return( EXIT_FAILURE );
	}
      
     if( roi )
	remaneRoiNodes( g ) ;
	

     if ( !substitutionFileName.empty() )
     {

        substitute( g, substitutionFileName );        

     }
      
      // write
      AimsGraphWriter	agw( fileout );
      AimsGraphWriter::SavingMode	sm = AimsGraphWriter::Keep;
      if( global )
	sm = AimsGraphWriter::Global;
      else if( local )
	sm = AimsGraphWriter::Local;

      agw.writeElements( g, sm, sm, false );
      GraphWriter	gw( fileout, syntax );
      gw << g;

      return EXIT_SUCCESS;
    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return( 1 );
    }

  return EXIT_FAILURE;
}



