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

