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

#include <aims/roi/fill.h>
#include <aims/bucket/bucketMap.h>
#include <set>


namespace aims
{

  template <typename T>
  void floodFill( carto::Volume<T> & vol, const Point3d & pos, T value,
                  Connectivity::Type connectivity, T tolerence, bool verbose )
  {
    typedef Point3d Pt;
    typedef std::list<Pt> Pset;

    std::vector<int> size = vol.getSize(), bsize = vol.getSize();
    bsize[0] += 2;
    bsize[1] += 2;
    bsize[2] += 2;
    carto::VolumeRef<int16_t> bdone( bsize );
    std::vector<int> bpos( 3, 1 );
    carto::VolumeRef<int16_t> done( bdone, bpos, size );
    bdone.fill( 1 );
    done.fill( 0 );

    T initVal = vol.at( pos );
    T minVal = initVal - tolerence;
    T maxVal = initVal + tolerence;
    std::cout << "minVal: " << minVal << ", maxVal: " << maxVal << std::endl;
    Pset front1, front2;
    Pset *front = &front1, *next_front = &front2;
    next_front->push_back( pos );
    Connectivity c( &vol( 0, 1 ) - &vol( 0 ), &vol( 0, 0, 1 ) - &vol( 0 ),
                    connectivity );
    int i, n = c.nbNeighbors();

    done.at( pos ) = 1;
    Connectivity c2( &done( 0, 1 ) - &done( 0 ), &done( 0, 0, 1 ) - &done( 0 ),
                    connectivity );

    while( !next_front->empty() )
    {
      if( verbose )
        std::cout << "front size: " << next_front->size() << std::endl;
      // swap fronts
      Pset * t = front;
      front = next_front;
      next_front = t;
      next_front->clear();
      Pset::const_iterator ip, ep = front->end();

      for( ip=front->begin(); ip!=ep; ++ip )
      {
        const Pt & p = *ip;
        T* pp = &vol.at( p );
        *pp = value;
        int16_t *pd = &done.at( p );
        for( i=0; i<n; ++i )
        {
          int16_t *ppd = pd + c2.offset( i );
          const T & val = *( pp + c.offset( i ) );
          if( *ppd == 0 && val >= minVal && val <= maxVal )
          {
            next_front->push_back( p + c.xyzOffset( i ) );
            *ppd = 1;
          }
        }
      }
    }
  }

}

