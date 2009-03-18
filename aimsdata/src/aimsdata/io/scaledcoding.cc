/* Copyright (c) 1995-2008 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#include <cstdlib>
#include <aims/io/scaledcoding.h>
#include <cartodata/volume/volume.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

template <typename T>
bool canencode( const carto::Volume<T> & thing, float & slope,
                float & offset, bool enableoffset, double *maxerr )
{
  int y, z, x, f, dx = thing.getSizeX(), dy = thing.getSizeY(),
    dz = thing.getSizeZ(), dt = thing.getSizeT();
  T       val, vmin = thing( 0 ), vmax = thing( 0 );
  typedef std::set<double> hset;
  hset    values;
  double  maxm = 0;
  double  off = 0, intv = 1;
  if( maxerr )
    *maxerr = 0;

  if( !enableoffset )
    values.insert( 0 ); // 0 must be a valid value if no offset is allowed
  // std::cout << "searching values\n";
  for( f=0; f<dt; ++f )
    for( z=0; z<dz; ++z )
      for( y=0; y<dy; ++y )
        for( x=0; x<dx; ++x )
        {
          val = thing.at( x, y, z, f );
          if( isnan( val ) || isinf( val ) )
            return false;
          if( val < vmin )
            vmin = val;
          if( val > vmax )
            vmax = val;
          values.insert( val );
          if( values.size() > 65536 )
            return false;
        }

  if( values.size() < 2 )
  {
    if( enableoffset )
    {
      slope = 1.;
      offset = *values.begin();
    }
    else
    {
      slope = *values.begin();
      offset = 0;
    }
    return true;
  }

  // find minimum interval
  std::set<double>::iterator  iv = values.begin(), ev = values.end();
  double      v, intv2;
  // interval list
  std::set<double>    intvl;
  intv = 0;
  off = *iv;
  for( ++iv; iv!=ev; ++iv )
  {
    v = *iv - off;
    //std::cout << v << "  ";
    if( intv == 0 || v < intv )
    {
      intv = v;
      intvl.insert( v );
    }
    if( ::fabs( ( v / intv ) - rint( v / intv ) ) > 1e-5 )
      intvl.insert( v );
    off = *iv;
  }

  if( enableoffset )
  {
    // take the offset as the nearest existing value around the mean
    // (possibly not a good criterion if values around mean are never taken)
    float mean = ( vmax + vmin ) / 2;
    iv = values.lower_bound( mean );
    float m1 = *iv;
    --iv;
    float m2 = *iv;
    if( mean - m1 < m2 - mean )
      off = m1;
    else
      off = m2;
  }
  else
    off = 0;

  iv = intvl.begin();
  ev = intvl.end();

  while( iv != ev )
    for( iv=intvl.begin(); iv!=ev; ++iv )
    {
      intv2 = *iv;
      if( ( vmax - off ) / intv2 >= 32767.1
            || ( vmin - off ) / intv2 <= -32768.1 )
      {
        // no hope
        return false;
      }
      v = ::fabs( intv2 - rint( intv2 / intv ) * intv );
      if( v > 0 ) //intv * 1e-3 )
      {
        if( ( vmax - off ) / v < 32767.1 && ( vmin - off ) / v > -32768.1 )
        {
          intv = v;     // use smaller interval
          break;
        }
      }
    }
  v = std::max( ::fabs( vmin - off ), ::fabs( vmax - off ) );
  intv = v / rint( v / intv );

  for( iv=intvl.begin(); iv!=ev; ++iv )
  {
    v = ::fabs( *iv / intv - rint( *iv / intv ) );
    if( v > maxm )
    {
      maxm = v;
      if( v > 1e-2 )
        return false;
    }
  }
  slope = intv;
  offset = off;
  if( maxerr )
    *maxerr = maxm;
  return true;
}

}

namespace aims
{

  template <>
  bool canEncodeAsScaledS16( const carto::Volume<float> & vol, float & slope,
                             float & offset, bool enableoffset,
                             double *maxerr )
  {
    return canencode( vol, slope, offset, enableoffset, maxerr );
  }


  template <>
  bool canEncodeAsScaledS16( const carto::Volume<double> & vol, float & slope,
                             float & offset, bool enableoffset,
                             double *maxerr )
  {
    return canencode( vol, slope, offset, enableoffset, maxerr );
  }

}

