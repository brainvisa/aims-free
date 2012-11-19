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

#ifndef AIMS_DATA_VOLUMEMANIP_H
#define AIMS_DATA_VOLUMEMANIP_H

#include <aims/data/cartodatavolume.h>

namespace aims
{

  template < typename T >
  inline 
  AimsData<T> diag( const AimsData<T>& thing )
  {
    ASSERT( thing.dimT() == 1 && thing.dimZ() == 1 && thing.dimY() == 1 );

    // allocate the new thing
    AimsData<T> m( thing.dimX(), thing.dimX() );
    // do the operations
    for ( int x = 0; x < thing.dimX(); x++ )
      m( x, x ) = thing( x );

    return m;
  }


  template < typename T >
  inline 
  AimsData<T> undiag( const AimsData<T>& thing )
  {
    ASSERT( thing.dimT() == 1 && thing.dimZ() == 1 );

    // allocate the new thing
    AimsData<T> m( std::min( thing.dimX(), thing.dimY() ) , 1 );
    // do the operations
    for ( int x = 0; x < std::min( thing.dimX(), thing.dimY() ); x++ )
      m( x, 0 ) = thing( x, x );

    return m;
  }


  template < typename T >
  inline 
  AimsData<T> transpose( const AimsData<T>& thing )
  {
    ASSERT( thing.dimT() == 1 && thing.dimZ() == 1 );

    // allocate the new thingrix
    AimsData<T> m( thing.dimY(), thing.dimX() );
    // do the operations
    for ( int y = 0; y < thing.dimY(); y++ )
      for ( int x = 0; x < thing.dimX(); x++ )
        m( y, x ) = thing( x, y );

    return m;
  }


  template < typename T >
  inline 
  void incSorting( AimsData<T>& thing )
  {
    ASSERT( thing.dimY() == 1 && thing.dimZ() == 1 && thing.dimT() == 1 );

    int i,j;
    T tmp;

    for ( j = 1; j < thing.dimX(); j++ )
      {
        tmp = thing( j );
        i = j - 1;
        while ( i >= 0L && thing( i ) > tmp )
          {
            thing( i + 1 ) = thing( i );
            i--;
          }
        thing( i + 1 ) = tmp;
      }
  }


  template < typename T >
  inline 
  void decSorting( AimsData<T>& thing )
  {
    ASSERT( thing.dimY() == 1 && thing.dimZ() == 1 && thing.dimT() == 1 );

    int i,j;
    T tmp;

    for ( j= 1 ; j < thing.dimX(); j++ )
      {
        tmp = thing( j );
        i = j - 1;
        while ( i >= 0L && thing( i ) < tmp )
          {
            thing( i + 1 ) = thing( i );
            i--;
          }
        thing( i + 1 ) = tmp;
      }
  }


  template < typename T >
  inline 
  AimsData<int32_t> incOrder( const AimsData<T>& thing )
  {
    ASSERT( thing.dimY() == 1 && thing.dimZ() == 1 && thing.dimT() == 1 );

    AimsData<T> temp = thing.clone();
    AimsData<int32_t> order( thing.dimX() );
    int i,j;
    T tmp1, tmp2;

    for ( i = 0; i < order.dimX(); i++ )
      order( i ) = i;

    for ( j = 1; j < temp.dimX(); j++ )
      {
        tmp1 = temp( j );
        tmp2 = order( j );
        i = j - 1;
        while( i >= 0L && temp( i ) > tmp1 )
          {
            temp( i + 1 ) = temp( i );
            order( i + 1 ) = order( i );
            i--;
          }
        temp( i + 1 ) = tmp1;
        order( i + 1 ) = tmp2;
      }

    return order;
  }


  template < typename T >
  inline 
  AimsData<int32_t> decOrder( const AimsData<T>& thing )
  {
    ASSERT( thing.dimY() == 1 && thing.dimZ() == 1 && thing.dimT() == 1 );

    AimsData<T> temp = thing.clone();
    AimsData<int32_t> order( thing.dimX() );
    int i,j;
    T tmp1, tmp2;

    for ( i = 0; i < order.dimX(); i++ )
      order( i ) = i;

    for ( j = 1; j < temp.dimX(); j++ )
      {
        tmp1 = temp( j );
        tmp2 = order( j );
        i = j - 1;
        while( i >= 0L && temp( i ) < tmp1 )
          {
            temp( i + 1 ) = temp( i );
            order( i + 1 ) = order( i );
            i--;
          }
        temp( i + 1 ) = tmp1;
        order( i + 1 ) = tmp2;
      }

    return order;
  }


  template<typename T>
  inline
  bool hasSameDim( const AimsData<T> & v1, const AimsData<T> & v2 )
  {
    return v1.dimX() == v2.dimX() 
      && v1.dimY() == v2.dimY() 
      && v1.dimZ() == v2.dimZ() 
      && v1.dimT() == v2.dimT();
  }


  template<typename T>
  inline
  bool hasSameSize( const AimsData<T> & v1, const AimsData<T> & v2 )
  {
    return v1.sizeX() == v2.sizeX() 
      && v1.sizeY() == v2.sizeY() 
      && v1.sizeZ() == v2.sizeZ() 
      && v1.sizeT() == v2.sizeT();
  }


  template < typename T >
  inline 
  AimsData<T> triInf( const AimsData<T>& thing )
  {
    ASSERT( thing.dimT() == 1 && thing.dimZ() == 1 );

    // allocate the new thing
    AimsData<T> m( thing.dimX(), thing.dimY(), thing.borderWidth() );
    m = T( 0 );
    // do the operations
    for ( int y = 0; y < thing.dimY(); y++ )
      for ( int x = 0; x < thing.dimX(); x++ )
        if ( x <= y)
          m( x, y ) = thing( x, y );

    return m;
  }


  template < typename T >
  inline 
  AimsData<T> triSup( const AimsData<T>& thing )
  {
    ASSERT( thing.dimT() == 1 && thing.dimZ() == 1 );

    // allocate the new thing
    AimsData<T> m( thing.dimX(), thing.dimY(), thing.borderWidth() );
    m = T( 0 );
    // do the operations
    for ( int y = 0; y < thing.dimY(); y++ )
      for ( int x = 0; x < thing.dimX(); x++ )
        if ( x >= y )
          m( x, y ) = thing( x, y );

    return m;
  }

} // namespace aims


template < typename T >
inline 
std::ostream& operator << ( std::ostream& os, const AimsData<T> & thing )
{
  int dimx = thing.dimX();
  int dimy = thing.dimY();
  int dimz = thing.dimZ();
  int dimt = thing.dimT();

  os << "[";

  for ( int t = 0; t < dimt; t++ )
    {
      os << "[";
      for ( int z = 0; z < dimz; z++ )
        {
          os << "[";
          for ( int y = 0; y < dimy; y++ )
            {
              os << "[";
              for ( int x = 0; x < dimx; x++ )
                {
                  os << thing( x, y, z, t );
                  if ( x != dimx - 1 )
                    os << ", ";
                  if ( x == dimx - 1 && y != dimy - 1 )
                    os << "]," << std::endl << "   ";
                  if ( x == dimx - 1 && y == dimy - 1 && z != dimz - 1 )
                    os << "]]," << std::endl << "  ";
                  if ( x == dimx - 1 && y == dimy - 1 && z == dimz - 1 &&
                        t != dimt - 1 )
                    os << "]]]," << std::endl << " ";
                  if ( x == dimx - 1 && y == dimy - 1 && z == dimz - 1 &&
                        t == dimt - 1 )
                    os << "]]]";
                }
            }
        }
    }

  return os << "]";
}


template < typename T >
inline 
std::istream& operator >> ( std::istream& is, AimsData<T>& thing )
{
  char  c=0;
  int dimx = thing.dimX();
  int dimy = thing.dimY();
  int dimz = thing.dimZ();
  int dimt = thing.dimT();

  is >> c;
  for ( int t = 0; t < dimt; t++ )
    {
      is >> c;
      for ( int z = 0; z < dimz; z++ )
        {
          is >> c;
          for ( int y = 0; y < dimy; y++ )
            {
              is >> c;
              for ( int x = 0; x < dimx; x++ )
                { 
                  is >> thing( x, y, z, t );
                  if ( x != dimx - 1 )
                    is >> c;
                  if ( x == dimx - 1 && y != dimy - 1 )
                    {
                      is >> c;
                      is >> c;
                    }
                  if ( x == dimx - 1 && y == dimy - 1 && z != dimz - 1 )
                    {
                      is >> c;
                      is >> c;
                      is >> c;
                    }
                  if ( x == dimx - 1 && y == dimy - 1 && z == dimz - 1 &&
                       t != dimt - 1 )
                    {
                      is >> c;
                      is >> c;
                      is >> c;
                      is >> c;
                    }
                  if ( x == dimx - 1 && y == dimy - 1 && z == dimz - 1 &&
                       t == dimt - 1 ) 
                    {
                      is >> c;
                      is >> c;
                      is >> c;
                      is >> c;
                    }
                }
            }
        }
    } 

  return is;
}

#endif

