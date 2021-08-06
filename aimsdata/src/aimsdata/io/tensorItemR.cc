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


