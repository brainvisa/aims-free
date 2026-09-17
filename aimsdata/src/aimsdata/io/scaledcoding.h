#ifndef AIMS_IO_SCALEDCODING_H
#define AIMS_IO_SCALEDCODING_H

#include <soma-io/io/scaledcoding.h>

namespace carto
{
  template <typename T> class Volume;
}


namespace aims
{

  /** Checks if a volume can be encoded as 16 bit signed ints with a scale
      factor and optionally an offset. If so, the slope and offset are
      determined and set.
  */
  template <typename T>
  bool canEncodeAsScaledS16( const carto::Volume<T> & vol, float & slope,
                             float & offset, bool enableoffset = true,
                             double *maxerr = 0 );

  // declare specializations that actually do something
  template <>
  bool canEncodeAsScaledS16( const carto::Volume<float> & vol, float & slope,
                             float & offset, bool enableoffset, double * );
  template <>
  bool canEncodeAsScaledS16( const carto::Volume<double> & vol, float & slope,
                             float & offset, bool enableoffset, double * );

  // default implementation always fails
  template <typename T>
  inline bool canEncodeAsScaledS16( const carto::Volume<T> &, float &,
                                    float &, bool, double * )
  {
    return false;
  }

  template <typename INP, typename OUTP>
  class ScaledEncoding
  {
    public :

      /** Get the slope, offset and maximum error to encode a volume of float or double 
          using an integer type.
      */
      static soma::ScaledEncodingInfo info( const carto::Volume<INP> & thing );

      static soma::ScaledEncodingInfo rescale( const carto::Volume<INP> & in,
                                               carto::Volume<OUTP> & out );
  };
}

#endif



