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

#include <aims/io/argR.h>
#include <cartobase/config/paths.h>
#include <aims/io/argheader.h>
#include <aims/io/aimsGraphR.h>
#include <aims/io/roiR.h>
#include <aims/resampling/standardreferentials.h>
#include <aims/graph/graphmanip.h>
#include <aims/transformation/affinetransformation3d.h>
#include <graph/graph/graph.h>
#include <graph/graph/greader.h>
#include <cartobase/object/sreader.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;

LowLevelArgReader::LowLevelArgReader()
{
}


LowLevelArgReader::~LowLevelArgReader()
{
}

// --------------------


struct ArgReader::StaticPrivate
{
  map<string,LowLevelArgReader*>	readers;
};


ArgReader::StaticPrivate* ArgReader::staticPrivate()
{
  static StaticPrivate	*sp = 0;
  if( !sp )
    {
      sp = new StaticPrivate;
      ArgReader::registerReader( "", new LowLevelStandardArgReader );
      ArgReader::registerReader( "roi", new LowLevelRoiReader );
    }
  return( sp );
}


ArgReader::ArgReader( const string& filename )
  : _name( filename )
{
}


ArgReader::~ArgReader()
{
}


void ArgReader::initLowLevelReaders()
{
  staticPrivate();
}


void ArgReader::registerReader( const string & syntax, 
				LowLevelArgReader *reader )
{
  StaticPrivate	*sp = staticPrivate();
  map<string,LowLevelArgReader*>::iterator	i = sp->readers.find( syntax );
  if( i != sp->readers.end() )
    delete i->second;
  sp->readers[ syntax ] = reader;
}


bool ArgReader::hasReader( const string & syntax )
{
  StaticPrivate	*sp = staticPrivate();
  map<string,LowLevelArgReader*>::iterator	i = sp->readers.find( syntax );
  if( i != sp->readers.end() )
    return( true );
  return( false );
}


void ArgReader::deleteReader( const string & syntax )
{
  StaticPrivate	*sp = staticPrivate();
  map<string,LowLevelArgReader*>::iterator	i = sp->readers.find( syntax );
  if( i != sp->readers.end() )
    {
      delete i->second;
      sp->readers.erase( i );
    }
}


Graph* ArgReader::read( int subobjectsfilter ) const
{
#ifdef AIMS_DEBUG_IO
  cout << "ArgReader::read " << _name << ": " << subobjectsfilter << endl;
#endif
  ArgHeader	h( _name );
  h.read();
  string	syntax;
  h.getProperty( "arg_syntax", syntax );
  Graph		*g = 0;
  int		eprio = 0, etype = -1;
  string	emsg;

  StaticPrivate	*sp = staticPrivate();
  map<string,LowLevelArgReader*>::iterator	i = sp->readers.find( syntax );
  if( i != sp->readers.end() )
    try
      {
	g = i->second->read( _name, subobjectsfilter );
      }
    catch( exception & e )
      {
	io_error::keepExceptionPriority( e, eprio, etype, emsg );
      }
  if( g )
  {
    i->second->mergeMinf( *g, h );
    return g;
  }

  if( syntax.empty() )
    return 0;

  // try standard GraphReader
  i = sp->readers.find( "" );
  if( i != sp->readers.end() )
    g = i->second->read( _name, subobjectsfilter );
  if( !g )
    io_error::launchExcept( etype, emsg );
  i->second->mergeMinf( *g, h );
  return g;
}


set<string> ArgReader::readers()
{
  map<string, LowLevelArgReader*>::iterator 
    ir, er = staticPrivate()->readers.end();
  set<string>	r;
  for( ir=staticPrivate()->readers.begin(); ir!=er; ++ir )
    r.insert( ir->first );
  return( r );
}

// --------------------

LowLevelStandardArgReader::LowLevelStandardArgReader()
  : LowLevelArgReader(), _syntax( new SyntaxSet )
{
  try
    {
      SyntaxReader	sr( Paths::findResourceFile(
        "nomenclature/syntax/graph.stx", "aims" ) );
      sr >> *_syntax;
    }
  catch( exception & )
    {
    }
}


LowLevelStandardArgReader::~LowLevelStandardArgReader()
{
  delete _syntax;
}


Graph* LowLevelStandardArgReader::read( const string & filename, 
					int subobjectsfilter )
{
#ifdef AIMS_DEBUG_IO
  cout << "LowLevelStandardArgReader::read " << filename << ": " 
       << subobjectsfilter << endl;
#endif
  GraphReader	r( filename, *_syntax );
  Graph	*g = new Graph;
  try
    {
      r >> *g;
    }
  catch( exception & e )
    {
#ifdef AIMS_DEBUG_IO
      cout << "could not use GraphReader: " << e.what() << endl;
#endif
      delete g;
      throw;
    }

#ifdef AIMS_DEBUG_IO
  cout << "Graph structure successfully read\n";
#endif
  AimsGraphReader	gr( filename );
  try
    {
      if( subobjectsfilter < 0 )
        gr.readElements( *g, 3 );
      else if( subobjectsfilter > 0 )
        gr.readElements( *g, 1 );
      else
        {
          g->setProperty( "aims_reader_filename", filename );
          g->setProperty( "aims_reader_loaded_objects", int(0) );
        }
    }
  catch( exception & e )
    {
#ifdef AIMS_DEBUG_IO
      cout << "graph elements loading failure: " << e.what() << endl;
#endif
      throw;
    }

  PythonHeader *ph = new PythonHeader;
  Object pho( static_cast<GenericObject *>( ph ) );
  ph->readMinf( filename + ".minf" );
  g->copyProperties( pho );

  return g;
}


void LowLevelArgReader::mergeMinf( Graph & g, const PythonHeader & hdr )
{
  Object d = Object::value( Dictionary() );
  d->copyProperties( Object::reference( hdr ) );
  g.setProperty( "header", d );
  mergeTransformations( g, hdr );
}


void LowLevelArgReader::mergeTransformations( Graph & g,
                                              const PythonHeader & hdr )
{
  try
  {
    Object refs = hdr.getProperty( "referentials" );
    Object trans = hdr.getProperty( "transformations" );
    g.setProperty( "referentials", refs );
    g.setProperty( "transformations", trans );

    // If the graph has the old Talairach transform attributes, ensure that
    // they are consistent with the Talairach transform that may be contained
    // in the referentials/transformations attributes.
    if( GraphManip::hasOldTalairachTransform(g) ) {
      AffineTransformation3d tal(GraphManip::talairach(g));
      GraphManip::storeTalairach(g, tal);
    }

    // remove these properties from graph["header"] to avoid duplicates that
    // could get out of sync
    Object gh = g.getProperty( "header" );
    gh->removeProperty( "referentials" );
    gh->removeProperty( "transformations" );
  }
  catch( ... )
  {
  }
  try
  {
    Object ref = hdr.getProperty( "referential" );
    g.setProperty( "referential", ref );

    // remove this property from graph["header"] to avoid having a duplicate
    // that could get out of sync
    Object gh = g.getProperty( "header" );
    gh->removeProperty( "referential" );
  }
  catch( ... )
  {
  }
}
