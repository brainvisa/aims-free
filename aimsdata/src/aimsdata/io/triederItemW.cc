// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/triederItemW.h>

using namespace aims;
using namespace std;


ItemWriter<Trieder>* 
DefaultItemWriter<Trieder>::writer( const string & openmode, 
				      bool bswap ) const
{
  if( openmode == "ascii" )
    return( new DefaultAsciiItemWriter<Trieder> );
  if( bswap )
    return( new DefaultBSwapItemWriter<Trieder> );
  return( new DefaultItemWriter<Trieder> );
}


void DefaultItemWriter<Trieder>::write( ostream & is, const Trieder* pitem, 
					size_t n ) const
{
  const Trieder	*ptr = pitem;
  for( size_t i=0; i<n; ++i )
    write( is, *ptr++ );
}


void DefaultItemWriter<Trieder>::write( ostream & os, 
					const Trieder & item ) const
{
  static DefaultItemWriter<Point3df> itemW;
  itemW.write( os, item.dirX() );
  itemW.write( os, item.dirY() );
  itemW.write( os, item.dirZ() );
}


void DefaultAsciiItemWriter<Trieder>::write( ostream & os, 
					     const Trieder & item ) const
{
  static DefaultAsciiItemWriter<Point3df> itemW;
  itemW.write( os, item.dirX() );
  itemW.write( os, item.dirY() );
  itemW.write( os, item.dirZ() );
}


void DefaultBSwapItemWriter<Trieder>::write( ostream & os, 
					     const Trieder & item ) const
{
  static DefaultBSwapItemWriter<Point3df> itemW;
  itemW.write( os, item.dirX() );
  itemW.write( os, item.dirY() );
  itemW.write( os, item.dirZ() );
}


