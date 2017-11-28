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
#ifndef AIMSALGO_REGISTRATION_PREPROCESSING_D_H
#define AIMSALGO_REGISTRATION_PREPROCESSING_D_H

#include <aims/utility/dynamic.h>
#include <aims/math/mathelem.h>                // aims::MathUtil
#include <algorithm>                           // std::max_element
#include <limits>
#include <vector>

namespace aims {

  //==========================================================================
  //   Utils
  //==========================================================================
  namespace util {

    template <typename T>
    T epsilon()
    {
      if( std::numeric_limits<T>::is_integer )
        return (T)1;
      else
        return std::numeric_limits<T>::epsilon();
    }
  }

  //==========================================================================
  //   clip
  //==========================================================================
  template <typename T>
  carto::VolumeRef<T> clip( carto::VolumeRef<T> & in,
                            const T & min, const T & max,
                            bool in_place )
  {
    carto::VolumeRef<T> tmp = ( in_place ? in : in.deepcopy() );

    for( long t = 0; t < in.getSizeT(); ++t )
    for( long z = 0; z < in.getSizeZ(); ++z )
    for( long y = 0; y < in.getSizeY(); ++y )
    for( long x = 0; x < in.getSizeX(); ++x )
    {
      T val = tmp(x, y, z, t);
      if( val < min )
        tmp(x, y, z, t) = min;
      if( val > max )
        tmp(x, y, z, t) = max;
    }
    return tmp;
  }

  template <typename T>
  carto::VolumeRef<T> clip( const carto::VolumeRef<T> & in,
                            const T & min, const T & max )
  {
    return clip( in.deepcopy(), min, max );
  }

  //==========================================================================
  //   clipDynamic
  //==========================================================================

  template <typename T>
  carto::VolumeRef<T> clipDynamic( const carto::VolumeRef<T> & in,
                                   bool flatten_mask,
                                   float pct_kept )
  {
    carto::VolumeRef<T> out = in.deepcopy();
    return clipDynamic( out, flatten_mask, pct_kept, true );
  }

  template <typename T>
  carto::VolumeRef<T> clipDynamic( carto::VolumeRef<T> & in,
                                   bool  flatten_mask,
                                   float pct_kept,
                                   bool  in_place )
  {
    carto::VolumeRef<T> vol = in;
    if( !in_place )
      vol = carto::deepcopy(in);

    T min, max;

    // Find second minimum
    max = *std::max_element( vol.begin(), vol.end() );
    if( flatten_mask )
    {
      min = max;
      T trueMin = *std::min_element( vol.begin(), vol.end() );
      for( int z = 0; z < vol.getSizeZ(); ++z )
      for( int y = 0; y < vol.getSizeY(); ++y )
      for( int x = 0; x < vol.getSizeX(); ++x )
      {
        if( vol(x, y, z) != trueMin && vol(x, y, z) < min )
          min = vol(x, y, z);
      }
    }
    else
      min = *std::min_element( vol.begin(), vol.end() );

    // Find pct% max
    if( pct_kept < 1. )
    {
      std::vector<long> histo( 100, 0 );
      double histoDelta = double(max - min) / 99.;
      for( int z = 0; z < vol.getSizeZ(); ++z )
      for( int y = 0; y < vol.getSizeY(); ++y )
      for( int x = 0; x < vol.getSizeX(); ++x )
      {
        T val = vol(x, y, z);
        if( val >= min )
          histo[ int( double(val - min) / histoDelta ) ] += 1;
      }
      long histoSize = aims::MathUtil<long>::sum( histo.begin(), histo.end() );
      long accumulate = 0;
      int i99 = 99;
      while( i99 >= 0 && (double)accumulate / histoSize < 1. - pct_kept )
      {
        accumulate += histo[i99];
        max = min + histoDelta * double(i99);
        --i99;
      }
      histo.clear();
    }

    if( flatten_mask )
      min -= util::epsilon<T>();

    clip( vol, min, max );
    return vol;
  }

} // namespace aims

#endif // AIMSALGO_REGISTRATION_PREPROCESSING_D_H
