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

