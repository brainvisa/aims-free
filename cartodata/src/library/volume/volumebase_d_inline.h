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

#ifndef CARTODATA_VOLUME_VOLUMEBASE_D_INLINE_H
#define CARTODATA_VOLUME_VOLUMEBASE_D_INLINE_H

//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volumebase.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>
//----------------------------------------------------------------------------
#include <iomanip>

namespace carto {

  template < typename T >
  inline
  const T& Volume< T >::at( long x, long y, long z, long t ) const
  {
#ifdef CARTO_USE_BLITZ
//     return _blitz( blitz::TinyVector<long,4>( x, y, z, t ) );
//     return _blitz.data()[
//       blitz::dot( _blitz.stride(),
//                   blitz::TinyVector<long,4>( x, y, z, t ) ) ];
    const blitz::TinyVector<int, Volume<T>::DIM_MAX>& bstrides
      = _blitz.stride();
    return _items[ x * bstrides[0] + y * bstrides[1] + z * bstrides[2]
                   + t * bstrides[3] ];
#else
    return _items[ x + y * _lineoffset + z * _sliceoffset
                   + t * _volumeoffset ];
#endif
  }


  template < typename T >
  inline
  const T& Volume< T >::operator()( long x, long y, long z, long t ) const
  {
    return at( x, y, z, t );
  }


  template < typename T >
  inline
  T& Volume< T >::at( long x, long y, long z, long t )
  {
#ifdef CARTO_USE_BLITZ
//     return _blitz( blitz::TinyVector<long,4>( x, y, z, t ) );
//     return _blitz.data()[
//       blitz::dot( _blitz.stride(),
//                   blitz::TinyVector<long,4>( x, y, z, t ) ) ];
    const blitz::TinyVector<int, Volume<T>::DIM_MAX>& bstrides
      = _blitz.stride();
    return _items[ x * bstrides[0] + y * bstrides[1] + z * bstrides[2]
                   + t * bstrides[3] ];
#else
    return _items[ x + y * _lineoffset + z * _sliceoffset
                   + t * _volumeoffset ];
#endif
  }


  template < typename T >
  inline
  T& Volume< T >::operator()( long x, long y, long z, long t )
  {
    return at( x, y, z, t );
  }


  template < typename T >
  inline
  const T& Volume< T >::at( const Position4Di & pos ) const
  {
    return at( pos[0], pos[1], pos[2], pos[3] );
  }


  template < typename T >
  inline
  const T& Volume< T >::operator()( const Position4Di & pos ) const
  {
    return at( pos );
  }


  template < typename T >
  inline
  T& Volume< T >::at( const Position4Di & pos )
  {
    return at( pos[0], pos[1], pos[2], pos[3] );
  }


  template < typename T >
  inline
  T& Volume< T >::operator()( const Position4Di & pos )
  {
    return at( pos );
  }


  template < typename T > inline
  const T & Volume< T >::at( const std::vector<int> & index ) const
  {
#ifdef CARTO_USE_BLITZ
//     blitz::TinyVector<int, Volume<T>::DIM_MAX> pos;
//     int i, n = index.size();
//     for( i=0; i<n && i<Volume<T>::DIM_MAX; ++i )
//       pos[i] = index[i];
//     for( ; i<Volume<T>::DIM_MAX; ++i )
//       pos[i] = 0;
//     return _blitz( pos );

    const blitz::TinyVector<int, Volume<T>::DIM_MAX>& bstrides
      = _blitz.stride();
    size_t offset = 0;
    int n = index.size();
    for( int i=0; i<n; ++i )
      offset += index[i] * bstrides[i];
    return _blitz.dataZero()[ offset ];
#else
    if( index.size() >= 4 )
      return at( index[0], index[1], index[2], index[3] );
    else if( index.size() >= 3 )
      return at( index[0], index[1], index[2] );
    else if( index.size() >= 2 )
      return at( index[0], index[1] );
    else
      return at( index[0] );
#endif
  }

  template < typename T > inline
  T & Volume< T >::at( const std::vector<int> & index )
  {
#ifdef CARTO_USE_BLITZ
//     blitz::TinyVector<int, Volume<T>::DIM_MAX> pos;
//     int i, n = index.size();
//     for( i=0; i<n && i<Volume<T>::DIM_MAX; ++i )
//       pos[i] = index[i];
//     for( ; i<Volume<T>::DIM_MAX; ++i )
//       pos[i] = 0;
//     return _blitz( pos );

    const blitz::TinyVector<int, Volume<T>::DIM_MAX>& strides
      = _blitz.stride();

//     switch( index.size() )
//     {
//       case 1:
//         return _blitz.dataZero()[ index[0] * strides[0] ];
//       case 2:
//         return _blitz.dataZero()[ index[0] * strides[0]
//                      + index[1] * strides[1] ];
//       case 3:
//         return _blitz.dataZero()[ index[0] // * strides[0]
//                      + index[1] * strides[1]
//                      + index[2] * strides[2] ];
//       case 4:
//         if( strides[0] == 1 )
//         return _blitz.dataZero()[ index[0] // * strides[0]
//                      + index[1] * strides[1]
//                      + index[2] * strides[2]
//                      + index[3] * strides[3] ];
//         else
//         return _blitz.dataZero()[ index[0] * strides[0]
//                      + index[1] * strides[1]
//                      + index[2] * strides[2]
//                      + index[3] * strides[3] ];
//       case 5:
//         return _blitz.dataZero()[ index[0] * strides[0]
//                      + index[1] * strides[1]
//                      + index[2] * strides[2]
//                      + index[3] * strides[3]
//                      + index[4] * strides[4] ];
//       case 6:
//         return _blitz.dataZero()[ index[0] * strides[0]
//                      + index[1] * strides[1]
//                      + index[2] * strides[2]
//                      + index[3] * strides[3]
//                      + index[4] * strides[4]
//                      + index[5] * strides[5] ];
//       case 7:
//         return _blitz.dataZero()[ index[0] * strides[0]
//                      + index[1] * strides[1]
//                      + index[2] * strides[2]
//                      + index[3] * strides[3]
//                      + index[4] * strides[4]
//                      + index[5] * strides[5]
//                      + index[6] * strides[6] ];
//       case 8:
//         return _blitz.dataZero()[ index[0] * strides[0]
//                      + index[1] * strides[1]
//                      + index[2] * strides[2]
//                      + index[3] * strides[3]
//                      + index[4] * strides[4]
//                      + index[5] * strides[5]
//                      + index[6] * strides[6]
//                      + index[7] * strides[7] ];
//       default:
//       {
        size_t offset = 0;
        for( int i=0; i!=index.size(); ++i )
          offset += index[i] * strides[i];
        return _blitz.dataZero()[ offset ];
//       }
//     }
//
//     return *_blitz.dataZero();

//     size_t offset = 0;
//     int n = index.size();
//     for( int i=0; i<n; i++ )
//       offset += index[i] * bstrides[i];
//     return _blitz.dataZero()[ offset ];
#else
    if( index.size() >= 4 )
      return at( index[0], index[1], index[2], index[3] );
    else if( index.size() >= 3 )
      return at( index[0], index[1], index[2] );
    else if( index.size() >= 2 )
      return at( index[0], index[1] );
    else
      return at( index[0] );
#endif
  }

  template < typename T > inline
  const T& Volume< T >::operator() ( const std::vector<int> & position ) const
  {
    return at( position );
  }


  template < typename T > inline
  T& Volume< T >::operator() ( const std::vector<int> & position )
  {
    return at( position );
  }

#ifdef CARTO_USE_BLITZ
  template < typename T > inline
  const T & Volume< T >::at( const blitz::TinyVector<int,1> & index ) const
  {
    return _blitz( index );
  }

  template < typename T > inline
  T & Volume< T >::at( const blitz::TinyVector<int,1> & index )
  {
    return _blitz( index );
  }

  template < typename T > inline
  const T & Volume< T >::at( const blitz::TinyVector<int,2> & index ) const
  {
    return _blitz( index );
  }

  template < typename T > inline
  T & Volume< T >::at( const blitz::TinyVector<int,2> & index )
  {
    return _blitz( index );
  }

  template < typename T > inline
  const T & Volume< T >::at( const blitz::TinyVector<int,3> & index ) const
  {
    return _blitz( index );
  }

  template < typename T > inline
  T & Volume< T >::at( const blitz::TinyVector<int,3> & index )
  {
    return _blitz( index );
  }

  template < typename T > inline
  const T & Volume< T >::at( const blitz::TinyVector<int,4> & index ) const
  {
    return _blitz( index );
  }

  template < typename T > inline
  T & Volume< T >::at( const blitz::TinyVector<int,4> & index )
  {
    return _blitz( index );
  }

  template < typename T > inline
  const T & Volume< T >::at( const blitz::TinyVector<int,Volume<T>::DIM_MAX> & index ) const
  {
    return _blitz( index );
  }

  template < typename T > inline
  T & Volume< T >::at( const blitz::TinyVector<int,Volume<T>::DIM_MAX> & index )
  {
    return _blitz( index );
  }

  template < typename T > inline
  blitz::Array<T,Volume< T >::DIM_MAX>
  Volume< T >::at( const blitz::RectDomain<Volume<T>::DIM_MAX> & subdomain ) const
  {
    return _blitz( subdomain );
  }

  template < typename T > inline
  blitz::Array<T,Volume< T >::DIM_MAX>
  Volume< T >::at( const blitz::StridedDomain<Volume<T>::DIM_MAX> & subdomain ) const
  {
    return _blitz( subdomain );
  }

  template < typename T > inline
  blitz::Array<T,Volume< T >::DIM_MAX>
  Volume< T >::at( const blitz::Range & r0 ) const
  {
    return _blitz( r0 );
  }

  template < typename T > inline
  blitz::Array<T,Volume< T >::DIM_MAX>
  Volume< T >::at( const blitz::Range & r0, const blitz::Range & r1 ) const
  {
    return _blitz( r0, r1 );
  }

  template < typename T > inline
  blitz::Array<T,Volume< T >::DIM_MAX>
  Volume< T >::at( const blitz::Range & r0, const blitz::Range & r1,
                   const blitz::Range & r2 ) const
  {
    return _blitz( r0, r1, r2 );
  }

  template < typename T > inline
  blitz::Array<T,Volume< T >::DIM_MAX>
  Volume< T >::at( const blitz::Range & r0, const blitz::Range & r1,
                   const blitz::Range & r2, const blitz::Range & r3 ) const
  {
    return _blitz( r0, r1, r2, r3 );
  }

  template < typename T >
  inline
  const T& Volume< T >::at( long x1, long x2, long x3, long x4, long x5,
                            long x6, long x7, long x8 ) const
  {
    return _blitz( blitz::TinyVector<long,8>( x1, x2, x3, x4, x5, x6, x7,
                                              x8 ) );
  }

  template < typename T >
  inline
  const T& Volume< T >::operator()( long x1, long x2, long x3, long x4,
                                    long x5, long x6, long x7, long x8 ) const
  {
    return at( x1, x2, x3, x4, x5, x6, x7, x8 );
  }

  template < typename T >
  inline
  T& Volume< T >::at( long x1, long x2, long x3, long x4,
                      long x5, long x6, long x7, long x8 )
  {
    return _blitz( blitz::TinyVector<long,8>( x1, x2, x3, x4, x5, x6, x7,
                                              x8 ) );
  }


  template < typename T >
  inline
  T& Volume< T >::operator()( long x1, long x2, long x3, long x4,
                              long x5, long x6, long x7, long x8 )
  {
    return at( x1, x2, x3, x4, x5, x6, x7, x8 );
  }
#endif


  template <typename T>
  Object getObjectHeader( Volume<T> & obj )
  {
    return Object::reference( obj.header() );
  }

  template <typename T>
  inline
  std::ostream & operator<< ( std::ostream & out,
                              const Volume<T> & volume )
  {
    VolumeOStream volumeout( out );
    return volumeout << volume;
  }

  template <typename T>
  inline
  std::ostream & operator<< ( const VolumeOStream & out,
                              const Volume<T> & volume )
  {
    out.ostream() << "Volume of " << DataTypeCode<T>::dataType() << ": ";
    std::vector<int> dims = volume.getSize();
    for( int d=0, n=dims.size(); d<n; ++d )
    {
      out.ostream() << dims[d];
      if( d < n - 1 )
        out.ostream() << " x ";
    }
    out.ostream() << std::endl;

    bool hasT = volume.getSizeT() > 1;
    std::string tB = ( hasT ? "[" : "" );
    std::string tE = ( hasT ? "]" : "" );
    std::string tS = ( hasT ? " " : "" );
    bool hasZ = volume.getSizeZ() > 1;
    std::string zB = ( hasZ ? "[" : "" );
    std::string zE = ( hasZ ? "]" : "" );
    std::string zS = ( hasZ ? " " : "" );
    bool hasY = volume.getSizeY() > 1;
    std::string yB = ( hasY ? "[" : "" );
    std::string yE = ( hasY ? "]" : "" );
    std::string yS = ( hasY ? " " : "" );
    bool hasX = volume.getSizeX() > 1;
    std::string xB = ( hasX ? "[" : "" );
    std::string xE = ( hasX ? "]" : "" );
    std::string xS = ( hasX ? " " : "" );

    for( size_t t = 0; t < static_cast<size_t>(volume.getSizeT()); ++t )
    {
      if( hasT ) {
        if( t < out.maxT() ) {
          out.ostream() << std::setw(2) << std::left << tB;
        } else {
          out.ostream() << std::setw(3) << std::left << "...";
          break;
        }
      }
      for( size_t z = 0; z < static_cast<size_t>(volume.getSizeZ()); ++z )
      {
        if( hasZ ) {
          if( z < out.maxZ() ) {
            out.ostream() << std::setw(2) << std::left << zB;
          } else {
            out.ostream() << std::setw(3) << std::left << "...";
            break;
          }
        }
        for( size_t y = 0; y < static_cast<size_t>(volume.getSizeY()); ++y )
        {
          if( y < out.maxY() ) {
            out.ostream() << std::setw(2) << std::left << yB;
          } else {
            out.ostream() << std::setw(3) << std::left << "...";
            break;
          }
          for( size_t x = 0; x < static_cast<size_t>(volume.getSizeX()); ++x )
          {
            if( x < out.maxX() ) {
              out.ostream() << std::setw(10) << std::left
                            << toString( volume( x, y, z, t ) );
            } else {
              out.ostream() << std::setw(10) << std::left << "...";
              break;
            }
          }
          out.ostream() << std::setw(2) << std::left << yE;
          if( y < volume.getSizeY() - 1 )
            out.ostream() << std::endl;
          if( hasZ )
            out.ostream() << std::setw(2) << std::left << zS;
          if( hasT )
            out.ostream() << std::setw(2) << std::left << tS;
        }
        if( hasZ )
          out.ostream() << std::setw(2) << std::left << zE;
        if( z < volume.getSizeZ() - 1 )
          out.ostream()  << std::endl;
        if( hasT )
          out.ostream() << std::setw(2) << std::left << tS;
      }
      if( hasT )
        out.ostream() << std::setw(2) << std::left << tE;
      if( t < volume.getSizeT() - 1 )
        out.ostream() << std::endl;
    }

    return out.ostream();
  }
      
  template <typename T>
  inline
  void displayRefVolumes(const carto::Volume<T> & vol) {
    int p = 0;
    rc_ptr<Volume<T> > v(const_cast<Volume<T> *>(&vol));
    
    while (!v.isNull()) {
      std::cout << "Volume " << (p ? carto::toString(p) + " " : "") << "is " 
                << (v->allocatorContext().isAllocated() ? "" : "not ") 
                << "allocated" << std::endl << std::flush;
    
      // Display volume dimensions
      std::vector<int> dims = v->getSize();
      if (dims.size()>0) {
        std::cout << "Volume " << (p ? carto::toString(p) + " " : "") 
                  << "dimensions [";   
        for(int i=0; i<dims.size()-1; ++i)
          std::cout << dims[i] << ", ";
        std::cout << dims[dims.size()-1] << "]" << std::endl << std::flush;
      }
    
      // Display volume borders
      std::vector<int> borders = v->getBorders();
      std::cout << "Volume " << (p ? carto::toString(p) + " " : "") 
                << "borders [";
      if (borders.size()>0) {
        for(int i=0; i<borders.size()-1; ++i)
          std::cout << borders[i] << ", ";
        std::cout << borders[borders.size()-1] << "]" << std::endl << std::flush;
      }
    
      // Display position in parent
      typename Volume<T>::Position pos = v->posInRefVolume();
      if (v->refVolume().isNull()) {
        std::vector<int> pos = v->posInRefVolume();
        std::cout << "Volume " << (p ? carto::toString(p) + " " : "") 
                  << " position in parent [";
        for(int i=0; i<pos.size()-1; ++i)
          std::cout << pos[i] << ", ";
        std::cout << pos[pos.size()-1] << "]" << std::endl << std::flush;
      }
  
      v = v->refVolume();        
      p++;
    }
  }

} // namespace carto

#endif // CARTODATA_VOLUME_VOLUMEBASE_D_INLINE_H
