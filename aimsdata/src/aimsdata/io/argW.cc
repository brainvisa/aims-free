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

#include <aims/io/argW.h>
#include <aims/def/path.h>
#include <aims/io/aimsGraphW.h>
#include <aims/data/pheader.h>
#include <graph/graph/graph.h>
#include <graph/graph/gwriter.h>
#include <cartobase/object/sreader.h>
#include <cartobase/stream/fileutil.h>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;

LowLevelArgWriter::LowLevelArgWriter()
{
}


LowLevelArgWriter::~LowLevelArgWriter()
{
}

// --------------------


struct ArgWriter::StaticPrivate
{
  map<string,LowLevelArgWriter*>	writers;
};


ArgWriter::StaticPrivate* ArgWriter::staticPrivate()
{
  static StaticPrivate	*sp = 0;
  if( !sp )
    sp = new StaticPrivate;
  return( sp );
}


ArgWriter::ArgWriter( const string& filename )
  : _name( filename )
{
  if( !hasWriter( "" ) )
    registerWriter( "", new LowLevelStandardArgWriter );
}


ArgWriter::~ArgWriter()
{
}


void ArgWriter::registerWriter( const string & syntax, 
				LowLevelArgWriter *writer )
{
  StaticPrivate	*sp = staticPrivate();
  map<string,LowLevelArgWriter*>::iterator	i = sp->writers.find( syntax );
  if( i != sp->writers.end() )
    delete i->second;
  sp->writers[ syntax ] = writer;
}


bool ArgWriter::hasWriter( const string & syntax )
{
  StaticPrivate	*sp = staticPrivate();
  map<string,LowLevelArgWriter*>::iterator	i = sp->writers.find( syntax );
  if( i != sp->writers.end() )
    return( true );
  return( false );
}


void ArgWriter::deleteWriter( const string & syntax )
{
  StaticPrivate	*sp = staticPrivate();
  map<string,LowLevelArgWriter*>::iterator	i = sp->writers.find( syntax );
  if( i != sp->writers.end() )
    {
      delete i->second;
      sp->writers.erase( i );
    }
}


bool ArgWriter::write( Graph & g, bool forceglobal ) const
{
  // cout << "ArgWriter::write " << _name << endl;
  string	syntax = g.getSyntax();
  string	name = FileUtil::removeExtension( _name ) + ".arg";

  StaticPrivate	*sp = staticPrivate();
  map<string,LowLevelArgWriter*>::iterator	i = sp->writers.find( syntax );
  if( i != sp->writers.end() )
    try
      {
	i->second->write( _name, g, forceglobal );
	return( true );
      }
    catch( exception & )
      {
      }
  if( syntax.empty() )
    return( false );

  // try standard GraphWriter
  i = sp->writers.find( "" );
  if( i != sp->writers.end() )
    try
      {
	i->second->write( _name, g, forceglobal );
	return( true );
      }
    catch( exception & )
      {
      }
  return( false );
}

// --------------------

LowLevelStandardArgWriter::LowLevelStandardArgWriter()
  : LowLevelArgWriter(), _syntax( new SyntaxSet )
{
  try
    {
      SyntaxReader	sr( Path::singleton().syntax() 
                            + FileUtil::separator() + "graph.stx" );
      sr >> *_syntax;
    }
  catch( exception & )
    {
    }
}


LowLevelStandardArgWriter::~LowLevelStandardArgWriter()
{
  delete _syntax;
}


void LowLevelStandardArgWriter::write( const string & filename, Graph & g, 
                                       bool forceglobal )
{
  //cout << "LowLevelStandardArgWriter::write " << filename << endl;
  GraphWriter	w( filename, *_syntax );
  AimsGraphWriter	gw( filename );
  AimsGraphWriter::SavingMode	sm = AimsGraphWriter::Keep;
  if( forceglobal )
    sm = AimsGraphWriter::Global;
  gw.writeElements( g, sm, sm );
  //cout << "writeElements done\n";
  try
    {
      w << g;
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      throw;
    }

  // make a .minf header
  PythonHeader ph;
  try
  {
    Object v = g.getProperty( "uuid" );
    ph.setProperty( "uuid", v );
  }
  catch( ... )
  {}
  try
  {
    Object v = g.getProperty( "referential" );
    ph.setProperty( "referential", v );
  }
  catch( ... )
  {}
  try
  {
    Object v = g.getProperty( "referentials" );
    ph.setProperty( "referentials", v );
  }
  catch( ... )
  {}
  try
  {
    Object v = g.getProperty( "transformations" );
    ph.setProperty( "transformations", v );
  }
  catch( ... )
  {}
  if( ph.size() > 0 )
    ph.writeMinf( filename + ".minf" );
  //cout << "LowLevelStandardArgWriter done\n";
}


