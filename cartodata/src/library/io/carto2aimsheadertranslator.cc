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

  vector<int>	dim;

  if( !dstheader->getProperty( "volume_dimension", dim ) )
  {
    while( dim.size() <  4 )
      dim.push_back( 1 );
    dstheader->getProperty( "sizeX", dim[0] );
    dstheader->getProperty( "sizeY", dim[1] );
    dstheader->getProperty( "sizeZ", dim[2] );
    dstheader->getProperty( "sizeT", dim[3] );
    dstheader->setProperty( "volume_dimension", dim );
  }

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


