// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/dtitensorItemW.h>
#include <aims/io/tensorItemW.h>

using namespace aims;
using namespace std;


ItemWriter<DtiTensor>* 
DefaultItemWriter<DtiTensor>::writer( const string & openmode, 
				      bool bswap ) const
{
  if( openmode == "ascii" )
    return( new DefaultAsciiItemWriter<DtiTensor> );
  if( bswap )
    return( new DefaultBSwapItemWriter<DtiTensor> );
  return( new DefaultItemWriter<DtiTensor> );
}


void DefaultItemWriter<DtiTensor>::write( ostream & is, 
					  const DtiTensor* pitem, 
					  size_t n ) const
{
  const DtiTensor	*ptr = pitem;
  for( size_t i=0; i<n; ++i )
    write( is, *ptr++ );
}


void DefaultItemWriter<DtiTensor>::write( ostream & os, 
					  const DtiTensor & item ) const
{
  static DefaultItemWriter<Tensor>	itemW1;
  static DefaultItemWriter<Point3df>	itemW2;
  static DefaultItemWriter<float>	itemW3;
  static DefaultItemWriter<int>		itemW4;
  static DefaultItemWriter<short>	itemW5;

  itemW1.write( os, item.base() );
  itemW2.write( os, item.dir() );
  itemW2.write( os, item.location() );
  itemW3.write( os, item.anisotropyVR() );
  itemW3.write( os, item.anisotropyFA() );
  itemW4.write( os, item.category() );
  itemW5.write( os, item.index() );
}


void DefaultAsciiItemWriter<DtiTensor>::write( ostream & os, 
					       const DtiTensor & item ) const
{
  static DefaultAsciiItemWriter<Tensor>		itemW1;
  static DefaultAsciiItemWriter<Point3df>	itemW2;
  static DefaultAsciiItemWriter<float>		itemW3;
  static DefaultAsciiItemWriter<int>		itemW4;
  static DefaultAsciiItemWriter<short>		itemW5;

  itemW1.write( os, item.base() );
  itemW2.write( os, item.dir() );
  itemW2.write( os, item.location() );
  itemW3.write( os, item.anisotropyVR() );
  itemW3.write( os, item.anisotropyFA() );
  itemW4.write( os, item.category() );
  itemW5.write( os, item.index() );
}


void DefaultBSwapItemWriter<DtiTensor>::write( ostream & os, 
					       const DtiTensor & item ) const
{
  static DefaultBSwapItemWriter<Tensor>		itemW1;
  static DefaultBSwapItemWriter<Point3df>	itemW2;
  static DefaultBSwapItemWriter<float>		itemW3;
  static DefaultBSwapItemWriter<int>		itemW4;
  static DefaultBSwapItemWriter<short>		itemW5;

  itemW1.write( os, item.base() );
  itemW2.write( os, item.dir() );
  itemW2.write( os, item.location() );
  itemW3.write( os, item.anisotropyVR() );
  itemW3.write( os, item.anisotropyFA() );
  itemW4.write( os, item.category() );
  itemW5.write( os, item.index() );
}

