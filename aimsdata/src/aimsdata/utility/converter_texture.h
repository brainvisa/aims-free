/*
 *  Converter between the different A.I.M.S. types
 */

#ifndef AIMS_UTILITY_CONVERTER_TEXTURE_H
#define AIMS_UTILITY_CONVERTER_TEXTURE_H

#include <cartobase/type/converter.h>
#include <aims/mesh/texture.h>


namespace carto
{

  /** Partial specialization of Converter for different kind of textures.
  */
  template <typename INP, typename OUTP>
  class RawConverter<TimeTexture<INP>, TimeTexture<OUTP> >
  {
  public :
    void convert( const TimeTexture<INP> &in, TimeTexture<OUTP> & out ) const;
  };


  template <typename INP, typename OUTP> inline
  void RawConverter<TimeTexture<INP>,TimeTexture<OUTP> >::convert 
  ( const TimeTexture<INP> &in, TimeTexture<OUTP> &out ) const
  {
    typename TimeTexture<INP>::const_iterator it, et = in.end();

    for( it=in.begin(); it!=et; ++it )
      {
        Texture<OUTP>	& tout = out[ it->first ];
        unsigned	i, n = it->second.nItem();
        tout.reserve( n );
        for( i=0; i<n; ++i )
          tout.push_back( (OUTP) it->second.item( i ) );
      }
    out.setHeader( in.header() );
    out.header().setProperty( "data_type", carto::DataTypeCode<OUTP>::name() );
  }

}

#endif

