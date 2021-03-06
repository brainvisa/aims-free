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

