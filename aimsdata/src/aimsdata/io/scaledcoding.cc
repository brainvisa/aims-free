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

#include <cstdlib>
#include <limits>
#include <aims/io/scaledcoding.h>
#include <cartodata/volume/volume.h>
#include <aims/utility/converter_volume.h>

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


  template <typename INP, typename OUTP> 
  ScaledEncodingInfo ScaledEncoding<INP, OUTP>::info( const AimsData<INP> & thing )
  {
    ScaledEncodingInfo sei;
    double slope, offset, maxerr = 0;
    int y, z, x, f, dx = thing.dimX(), dy = thing.dimY(),
                    dz = thing.dimZ(), dt = thing.dimT();
    INP val, vmin = thing( 0 ), vmax = thing( 0 );
  
    double minlim = (double)numeric_limits<OUTP>::min(), maxlim = (double)numeric_limits<OUTP>::max();
    double dynsize = maxlim - minlim + 1;
    double minintv = 0, intv, intv2, v1, err;
    bool isexact = true;
    std::set<double> values;
    std::set<double> intvl;
  
    // Get values to be scaled
    for( f=0; f<dt; ++f )
      for( z=0; z<dz; ++z )
        for( y=0; y<dy; ++y )
          for( x=0; x<dx; ++x )
          {
            val = thing( x, y, z, f );
            if( isnan( val ) || isinf( val ) )
            {
              maxerr = val;
            }
            else
            {
              if( val < vmin )
                vmin = val;
              if( val > vmax )
                vmax = val;

              if (isexact)
                if (values.size() < dynsize)
                  values.insert( val );
                else
                  isexact = false;
            }
          }

    // Initialize offset and slope
    if( values.size() < 2 )
    {
      if( values.size() == 0 )
      {
        slope = 1.;
        offset = 0;
      }
      else
      {
        slope = 1.;
        offset = *values.begin();
      }
    }
    else 
    {
      offset = vmin;

      if (isexact)
      {
        // Find minimum interval of values and not dividable intervals
        std::set<double>::iterator  iv, ev = values.end();
    
        iv = values.begin(); 
        v1 = *iv;
        
        while( ++iv != ev )
        {
          intv = fabs(*iv - v1);
          if( minintv == 0 || intv < minintv )
          {
            minintv = intv;
            intvl.insert( intv );
          }
          else if( fabs( ( intv / minintv ) - rint( intv / minintv ) ) > numeric_limits<double>::epsilon() )
          {
            intvl.insert( intv );
          }
    
          v1 = *iv;
        }
    
        // Subdivide intervals
        iv = intvl.begin();
        ev = intvl.end();
    
        while( iv != ev )
          for( iv = intvl.begin(); iv != ev; ++iv )
          {
            intv2 = *iv;
            if ( ( ( vmax - vmin ) / intv2) > maxlim )
            {
              // Scaling won't be exact but we can not store more intervals
              // so we stop the loop
              iv = ev;
              isexact = false;
              break;
            }
  
            // Subdivide the interval
            intv = fabs( intv2 - rint( intv2 / minintv ) * minintv );
            if( intv > 0 )
            {
              if ( ( ( vmax - vmin ) / intv ) < maxlim )
              {
                minintv = intv;     // use smaller interval
                break;
              }
            }
          }
      }
  
      // Process the slope
      intv = (vmax - vmin);
      if (isexact)
      {
        slope = intv / rint( intv / minintv );
      }
      else
      {
        // We get a simple approximation
        slope = intv / (maxlim - minlim);
      }
    }

    // Find the maximum error
    if ( maxerr == 0 )
    {
      for( f=0; f<dt; ++f )
        for( z=0; z<dz; ++z )
          for( y=0; y<dy; ++y )
            for( x=0; x<dx; ++x )
            {
              val = thing( x, y, z, f );
              err = fabs( (double)val / slope - rint( (double)val / slope ) );
              if (err > maxerr)
                maxerr = err;
            }
    }

    sei.offset() = offset;
    sei.slope() = slope;
    sei.maxerr() = maxerr;

    return sei;
  }

  template <typename INP, typename OUTP> 
  ScaledEncodingInfo ScaledEncoding<INP, OUTP>::rescale( const AimsData<INP> & in, 
                                                         AimsData<OUTP> & out )
  {
    ScaledEncodingInfo sei;
    sei = ScaledEncoding<INP, OUTP>::info( in );
    RescalerInfo ri;
    ri.vmin = sei.offset();
    ri.vmax = in.maximum();
    ri.omin = numeric_limits<OUTP>::min();
    ri.omax = (ri.vmax - ri.vmin) / sei.slope() + ri.omin;
    Rescaler< AimsData< INP >, AimsData< OUTP > > rescaler( ri );
    rescaler.convert( in, out );

    return sei;
  }


  // Instanciates ScaledEncoding class
  template class ScaledEncoding<float, int8_t>;
  template class ScaledEncoding<double, int8_t>;
  template class ScaledEncoding<float, uint8_t>;
  template class ScaledEncoding<double, uint8_t>;
  template class ScaledEncoding<float, int16_t>;
  template class ScaledEncoding<double, int16_t>;
  template class ScaledEncoding<float, uint16_t>;
  template class ScaledEncoding<double, uint16_t>;
  template class ScaledEncoding<float, int32_t>;
  template class ScaledEncoding<double, int32_t>;
  template class ScaledEncoding<float, uint32_t>;
  template class ScaledEncoding<double, uint32_t>;
}

