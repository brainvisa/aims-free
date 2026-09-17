// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  Trieder item reader class
 */
#include <aims/io/triederItemR.h>

using namespace aims;
using namespace std;


ItemReader<Trieder>* 
DefaultItemReader<Trieder>::reader( const string & openmode, bool bswap ) const
{
  if( openmode == "ascii" )
    return( new DefaultAsciiItemReader<Trieder> );
  if( bswap )
    return( new DefaultBSwapItemReader<Trieder> );
  return( new DefaultItemReader<Trieder> );
}


void DefaultItemReader<Trieder>::read( istream & is, Trieder* pitem, 
				       size_t n ) const
{
  Trieder	*ptr = pitem;
  for( size_t i=0; i<n; ++i )
    read( is, *ptr++ );
}


void DefaultItemReader<Trieder>::read( istream & is, Trieder & item ) const
{
  static DefaultItemReader<Point3df> itemR;
  itemR.read( is, item.dirX() );
  itemR.read( is, item.dirY() );
  itemR.read( is, item.dirZ() );
}


void DefaultAsciiItemReader<Trieder>::read( istream & is, 
					    Trieder & item ) const
{
  static DefaultAsciiItemReader<Point3df> itemR;
  itemR.read( is, item.dirX() );
  itemR.read( is, item.dirY() );
  itemR.read( is, item.dirZ() );
}


void DefaultBSwapItemReader<Trieder>::read( istream & is, 
					    Trieder & item ) const
{
  static DefaultBSwapItemReader<Point3df> itemR;
  itemR.read( is, item.dirX() );
  itemR.read( is, item.dirY() );
  itemR.read( is, item.dirZ() );
}


