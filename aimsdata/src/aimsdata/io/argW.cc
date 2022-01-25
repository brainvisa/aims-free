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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/argW.h>
#include <cartobase/config/paths.h>
#include <aims/io/aimsGraphW.h>
#include <aims/data/pheader.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/graph/graphmanip.h>
#include <aims/resampling/standardreferentials.h>
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


bool ArgWriter::write( Graph & g, SavingMode mode,
                       bool saveOnlyModified ) const
{
  // cout << "ArgWriter::write " << _name << endl;
  string	syntax = g.getSyntax();
  string	name = FileUtil::removeExtension( _name ) + ".arg";

  StaticPrivate	*sp = staticPrivate();
  map<string,LowLevelArgWriter*>::iterator	i = sp->writers.find( syntax );
  if( i != sp->writers.end() )
    try
      {
        i->second->write( name, g, LowLevelArgWriter::SavingMode( mode ),
                          saveOnlyModified );
        Object minf = i->second->getMinf( g );
        if( minf->size() != 0 )
        {
          PythonHeader ph;
          ph.copyProperties( minf );
          ph.writeMinf( name + ".minf" );
        }
        return true;
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
        i->second->write( name, g, LowLevelArgWriter::SavingMode( mode ),
                          saveOnlyModified );
        Object minf = i->second->getMinf( g );
        if( minf->size() != 0 )
        {
          PythonHeader ph;
          ph.copyProperties( minf );
          ph.writeMinf( name + ".minf" );
        }
        return( true );
      }
    catch( exception & )
      {
      }
  return( false );
}

// --------------------

Object LowLevelArgWriter::getMinf( const Graph & g )
{
  Object hdr;
  try
  {
    hdr = g.getProperty( "header" );
  }
  catch( ... )
  {
    hdr = Object::value( Dictionary() );
  }
  try
  {
    Object refs = g.getProperty( "referentials" );
    Object trans = g.getProperty( "transformations" );
    if( refs && trans && refs->size() == trans->size() )
    {
      hdr->setProperty( "referentials", refs );
      hdr->setProperty( "transformations", trans );
    }
  }
  catch( ... )
  {
    AffineTransformation3d tal = GraphManip::talairach( g );
    if( !tal.isIdentity() )
    {
      vector<string> refs;
      refs.push_back( StandardReferentials::acPcReferential() );
      hdr->setProperty( "referentials", refs );
      vector<vector<float> > trans;
      trans.push_back( tal.toVector() );
      hdr->setProperty( "transformations", trans );
    }
  }
  try
  {
    Object ref = g.getProperty( "referential" );
    hdr->setProperty( "referential", ref );
  }
  catch( ... )
  {
  }

  return hdr;
}

// --------------------

LowLevelStandardArgWriter::LowLevelStandardArgWriter()
  : LowLevelArgWriter(), _syntax( new SyntaxSet )
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


LowLevelStandardArgWriter::~LowLevelStandardArgWriter()
{
  delete _syntax;
}


void LowLevelStandardArgWriter::write( const string & filename, Graph & g, 
                                       SavingMode mode, bool saveOnlyModified )
{
  //cout << "LowLevelStandardArgWriter::write " << filename << endl;
  GraphWriter	w( filename, *_syntax );
  AimsGraphWriter	gw( filename );
  AimsGraphWriter::SavingMode	sm
    = static_cast<AimsGraphWriter::SavingMode>( mode );
  gw.writeElements( g, sm, sm, saveOnlyModified );
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
  //cout << "LowLevelStandardArgWriter done\n";
}


