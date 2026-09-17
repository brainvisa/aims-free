// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/tensorItemW.h>
#include <aims/io/triederItemW.h>

using namespace aims;
using namespace std;


ItemWriter<Tensor>* 
DefaultItemWriter<Tensor>::writer( const string & openmode, bool bswap ) const
{
  if( openmode == "ascii" )
    return( new DefaultAsciiItemWriter<Tensor> );
  if( bswap )
    return( new DefaultBSwapItemWriter<Tensor> );
  return( new DefaultItemWriter<Tensor> );
}


void DefaultItemWriter<Tensor>::write( ostream & is, const Tensor* pitem, 
				       size_t n ) const
{
  const Tensor	*ptr = pitem;
  for( size_t i=0; i<n; ++i )
    write( is, *ptr++ );
}


void DefaultItemWriter<Tensor>::write( ostream & is, 
				       const Tensor & item ) const
{
  static DefaultItemWriter<Trieder>			itemR1;
  static DefaultItemWriter<Point3df>			itemR2;
  static DefaultItemWriter<AimsVector<float,6> >	itemR3;
  static DefaultItemWriter<float>			itemR4;

  itemR1.write( is, item.trieder() );
  itemR2.write( is, item.eigenvalue() );
  itemR3.write( is, item.coef() );
  itemR4.write( is, item.trace() );
}


void DefaultAsciiItemWriter<Tensor>::write( ostream & is, 
					    const Tensor & item ) const
{
  static DefaultAsciiItemWriter<Trieder>		itemR1;
  static DefaultAsciiItemWriter<Point3df>		itemR2;
  static DefaultAsciiItemWriter<AimsVector<float,6> >	itemR3;
  static DefaultAsciiItemWriter<float>			itemR4;

  itemR1.write( is, item.trieder() );
  itemR2.write( is, item.eigenvalue() );
  itemR3.write( is, item.coef() );
  itemR4.write( is, item.trace() );
}


void DefaultBSwapItemWriter<Tensor>::write( ostream & is, 
					    const Tensor & item ) const
{
  static DefaultBSwapItemWriter<Trieder>		itemR1;
  static DefaultBSwapItemWriter<Point3df>		itemR2;
  static DefaultBSwapItemWriter<AimsVector<float,6> >	itemR3;
  static DefaultBSwapItemWriter<float>			itemR4;

  itemR1.write( is, item.trieder() );
  itemR2.write( is, item.eigenvalue() );
  itemR3.write( is, item.coef() );
  itemR4.write( is, item.trace() );
}

