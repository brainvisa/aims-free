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

#include <cartodata/io/carto2aimsheadertranslator.h>
#include <cartobase/object/object.h>

using namespace carto;
using namespace std;

namespace
{

  void objdatatype( string & otype, string & dtype )
  {
    string::size_type	pos = otype.rfind( " of " );
    if( pos != string::npos )
      {
        dtype = otype.substr( pos + 4, otype.length() - pos - 4 );
        otype.erase( pos, otype.length() - pos );
      }
    else
      if( dtype.empty() )
        dtype = otype;
  }

}


void Carto2AimsHeaderTranslator::translate( Object srcheader, 
                                            Object dstheader ) const
{
  if( !dstheader.get() )
    dstheader = srcheader;
  else
    dstheader->copyProperties( srcheader );

  vector<int>	dim( 4, 1 );

  if( dstheader->getProperty( "sizeX", dim[0] ) )
    dstheader->removeProperty( "sizeX" );
  if( dstheader->getProperty( "sizeY", dim[1] ) )
    dstheader->removeProperty( "sizeY" );
  if( dstheader->getProperty( "sizeZ", dim[2] ) )
    dstheader->removeProperty( "sizeZ" );
  if( dstheader->getProperty( "sizeT", dim[3] ) )
    dstheader->removeProperty( "sizeT" );
  for( size_t i=4; i<8; ++i )
  {
    stringstream s;
    s << "sizeDim" << i;
    try
    {
      int vdim;
      if( dstheader->getProperty( s.str(), vdim ) )
      {
        while( dim.size() < i )
          dim.push_back( 1 );
        dim[i] = vdim;
      }
    }
    catch( ... )
    {
    }
  }
  /* keep erasing sizes because it causes a problem upon copy
  dstheader->getProperty( "sizeX", dim[0] );
  dstheader->getProperty( "sizeY", dim[1] );
  dstheader->getProperty( "sizeZ", dim[2] );
  dstheader->getProperty( "sizeT", dim[3] );
  */
  dstheader->setProperty( "volume_dimension", dim );

  string	x;
  if( dstheader->getProperty( "format", x ) )
    {
      dstheader->removeProperty( "format" );
      dstheader->setProperty( "file_type", x );
    }

  // data / object types
  string	otype, dtype;
  if( !dstheader->hasProperty( "data_type" ) 
      && dstheader->getProperty( "object_type", otype ) )
    {
      cout << "no data_type\n";
      objdatatype( otype, dtype );
      dstheader->setProperty( "object_type", otype );
      dstheader->setProperty( "data_type", dtype );
    }

  // possible_types
  vector<string>	ptypes, nptypes;
  if( !dstheader->hasProperty( "possible_data_types" ) 
      && dstheader->getProperty( "possible_types", ptypes ) )
    {
      unsigned	i=0, n = ptypes.size();
      nptypes.reserve( n );
      for( i=0; i<n; ++i )
        {
          otype = ptypes[i];
          objdatatype( otype, dtype );
          nptypes.push_back( dtype );
          dstheader->setProperty( "possible_data_types", nptypes );
        }
    }

  // post-process some properties
  Object p;
  try
  {
    p = srcheader->getProperty( "transformations" );
    if( p && p->type() != DataTypeCode<vector<vector<float> > >::name() )
    {
      Object iter, iter2;
      vector<vector<float> >  tr;
      tr.reserve( p->size() );
      for( iter=p->objectIterator(); iter->isValid(); iter->next() )
      {
        tr.push_back( vector<float>() );
        vector<float> & mat = *tr.rbegin();
        mat.reserve( iter->currentValue()->size() );
        for( iter2=iter->currentValue()->objectIterator();
              iter2->isValid(); iter2->next() )
          mat.push_back( (float) iter2->currentValue()->getScalar() );
      }
      dstheader->setProperty( "transformations", tr );
    }
  }
  catch( ... )
  {
  }
}


Carto2AimsHeaderTranslator::~Carto2AimsHeaderTranslator()
{
}


