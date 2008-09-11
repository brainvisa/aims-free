/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
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

#include <aims/io/baseFormats_graph.h>
#include <aims/io/argR.h>
#include <aims/io/argW.h>
#include <aims/io/reader.h>
#include <aims/data/data.h>
#include <aims/def/path.h>
#include <aims/graph/graphmanip.h>
#include <graph/graph/graph.h>

using namespace carto;
using namespace aims;
using namespace std;

ArgFormat::~ArgFormat()
{
}


bool ArgFormat::read( const string & filename, Graph & obj, 
		      const AllocatorContext & /*context*/, Object options )
{
  int subobjectsfilter = -1;
  try
  {
    Object filt = options->getProperty( "subobjectsfilter" );
    subobjectsfilter = (int) filt->getScalar();
  }
  catch( ... )
  {
  }
  ArgReader	r( filename );
  Graph	*g = r.read( subobjectsfilter );
  if( !g )
    return( false );
  obj.clear();
  obj.clearProperties();
  obj.setSyntax( g->getSyntax() );
  g->extract( obj, g->begin(), g->end() );
  obj.copyProperties( Object::reference
                      ( g->value<AttributedObject::ContentType>() ) );
  delete g;
  return( true );
}


Graph* ArgFormat::read( const string & filename, 
                        const AllocatorContext & /*context*/, Object options )
{
  int subobjectsfilter = -1;
  try
  {
    Object filt = options->getProperty( "subobjectsfilter" );
    subobjectsfilter = (int) filt->getScalar();
  }
  catch( ... )
  {
  }
  ArgReader	r( filename );
  return r.read( subobjectsfilter );
}


bool ArgFormat::write( const string & filename, const Graph & obj, 
		       bool forceglobal )
{
  //cout << "ArgFormat::write " << filename << endl;
  ArgWriter	w( filename );
  return w.write( (Graph &) obj, forceglobal );
}


GraphVolumeFormat::~GraphVolumeFormat()
{
}


bool GraphVolumeFormat::read( const string & filename, Graph & obj, 
			      const AllocatorContext & context, 
                              Object options )
{
  Reader<AimsData<short> >	r( filename );
  r.setOptions( options );
  r.setAllocatorContext( context );
  AimsData<short>		data;
  r.read( data );
  GraphManip::graphFromVolume( data, obj );
  return( true );
}


Graph* GraphVolumeFormat::read( const string & filename, 
                                const carto::AllocatorContext & context, 
                                carto::Object options )
{
  Reader<AimsData<short> >	r( filename );
  AimsData<short>		data;
  r.setOptions( options );
  r.setAllocatorContext( context );
  r.read( data );
  return GraphManip::graphFromVolume( data );
}
