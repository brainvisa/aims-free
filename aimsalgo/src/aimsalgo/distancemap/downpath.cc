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


#include <aims/distancemap/downpath.h>
#include <aims/connectivity/connectivity.h>

namespace aims
{

template <typename T>
BucketMap<Void>*
downPath( const BucketMap<T> & region, const Point3d & pos )
{
  Point3d p = pos, q, p1;
  BucketMap<Void> *bck = new BucketMap<Void>;
  bck->setSizeXYZT( region.sizeX(), region.sizeY(), region.sizeZ(),
                    region.sizeT() );
  BucketMap<Void>::Bucket & bk = (*bck)[0];
  const typename BucketMap<T>::Bucket & r0 = region.begin()->second;
  typename BucketMap<T>::Bucket::const_iterator ir, er = r0.end();
  Connectivity c( 0, 0, Connectivity::CONNECTIVITY_26_XYZ );
  int i, n = c.nbNeighbors();
  T val = r0.find(p)->second, tmpval;
  bool moved;

  do
  {
    bk[ p ];
    tmpval = val;
    moved = false;
    for( i=0; i<n; ++i )
    {
      q = p + c.xyzOffset( i );
      ir = r0.find( q );
      if( ir != er && ir->second < tmpval )
      {
        p1 = q;
        tmpval = ir->second;
        moved = true;
      }
    }
    if( moved )
    {
      val = tmpval;
      p = p1;
    }
  }
  while( moved );

  return bck;
}

// ---

template
BucketMap<Void>* downPath( const BucketMap<float> & region,
                           const Point3d & pos );

}

