// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/tensorItemR.h>
#include <aims/io/triederItemR.h>

using namespace aims;
using namespace std;


ItemReader<Tensor>* 
DefaultItemReader<Tensor>::reader( const string & openmode, bool bswap ) const
{
  if( openmode == "ascii" )
    return( new DefaultAsciiItemReader<Tensor> );
  if( bswap )
    return( new DefaultBSwapItemReader<Tensor> );
  return( new DefaultItemReader<Tensor> );
}


void DefaultItemReader<Tensor>::read( istream & is, Tensor* pitem, 
				      size_t n ) const
{
  Tensor	*ptr = pitem;
  for( size_t i=0; i<n; ++i )
    read( is, *ptr++ );
}


void DefaultItemReader<Tensor>::read( istream & is, Tensor & item ) const
{
  static DefaultItemReader<Trieder>			itemR1;
  static DefaultItemReader<Point3df>			itemR2;
  static DefaultItemReader<AimsVector<float,6> >	itemR3;
  static DefaultItemReader<float>			itemR4;

  itemR1.read( is, item.trieder() );
  itemR2.read( is, item.eigenvalue() );
  itemR3.read( is, item.coef() );
  itemR4.read( is, item.trace() );
}


void DefaultAsciiItemReader<Tensor>::read( istream & is, Tensor & item ) const
{
  static DefaultAsciiItemReader<Trieder>		itemR1;
  static DefaultAsciiItemReader<Point3df>		itemR2;
  static DefaultAsciiItemReader<AimsVector<float,6> >	itemR3;
  static DefaultAsciiItemReader<float>			itemR4;

  itemR1.read( is, item.trieder() );
  itemR2.read( is, item.eigenvalue() );
  itemR3.read( is, item.coef() );
  itemR4.read( is, item.trace() );
}


void DefaultBSwapItemReader<Tensor>::read( istream & is, Tensor & item ) const
{
  static DefaultBSwapItemReader<Trieder>		itemR1;
  static DefaultBSwapItemReader<Point3df>		itemR2;
  static DefaultBSwapItemReader<AimsVector<float,6> >	itemR3;
  static DefaultBSwapItemReader<float>			itemR4;

  itemR1.read( is, item.trieder() );
  itemR2.read( is, item.eigenvalue() );
  itemR3.read( is, item.coef() );
  itemR4.read( is, item.trace() );
}


