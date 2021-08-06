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

#include <aims/io/dtitensorItemR.h>
#include <aims/io/tensorItemR.h>

using namespace aims;
using namespace std;


ItemReader<DtiTensor>* 
DefaultItemReader<DtiTensor>::reader( const string & openmode, 
				      bool bswap ) const
{
  if( openmode == "ascii" )
    return( new DefaultAsciiItemReader<DtiTensor> );
  if( bswap )
    return( new DefaultBSwapItemReader<DtiTensor> );
  return( new DefaultItemReader<DtiTensor> );
}


void DefaultItemReader<DtiTensor>::read( istream & is, DtiTensor* pitem, 
					 size_t n ) const
{
  DtiTensor	*ptr = pitem;
  for( size_t i=0; i<n; ++i )
    read( is, *ptr++ );
}


void DefaultItemReader<DtiTensor>::read( istream & is, DtiTensor & item ) const
{
  static DefaultItemReader<Tensor>			itemR1;
  static DefaultItemReader<Point3df>			itemR2;
  static DefaultItemReader<float>			itemR3;
  static DefaultItemReader<int>				itemR4;
  static DefaultItemReader<short>			itemR5;

  itemR1.read( is, item.base() );
  itemR2.read( is, item.dir() );
  itemR2.read( is, item.location() );
  itemR3.read( is, item.anisotropyVR() );
  itemR3.read( is, item.anisotropyFA() );
  itemR4.read( is, item.category() );
  itemR5.read( is, item.index() );
}


void DefaultAsciiItemReader<DtiTensor>::read( istream & is, 
					      DtiTensor & item ) const
{
  static DefaultAsciiItemReader<Tensor>			itemR1;
  static DefaultAsciiItemReader<Point3df>		itemR2;
  static DefaultAsciiItemReader<float>			itemR3;
  static DefaultAsciiItemReader<int>			itemR4;
  static DefaultAsciiItemReader<short>			itemR5;

  itemR1.read( is, item.base() );
  itemR2.read( is, item.dir() );
  itemR2.read( is, item.location() );
  itemR3.read( is, item.anisotropyVR() );
  itemR3.read( is, item.anisotropyFA() );
  itemR4.read( is, item.category() );
  itemR5.read( is, item.index() );
}


void DefaultBSwapItemReader<DtiTensor>::read( istream & is, 
					      DtiTensor & item ) const
{
  static DefaultBSwapItemReader<Tensor>			itemR1;
  static DefaultBSwapItemReader<Point3df>		itemR2;
  static DefaultBSwapItemReader<float>			itemR3;
  static DefaultBSwapItemReader<int>			itemR4;
  static DefaultBSwapItemReader<short>			itemR5;

  itemR1.read( is, item.base() );
  itemR2.read( is, item.dir() );
  itemR2.read( is, item.location() );
  itemR3.read( is, item.anisotropyVR() );
  itemR3.read( is, item.anisotropyFA() );
  itemR4.read( is, item.category() );
  itemR5.read( is, item.index() );
}


