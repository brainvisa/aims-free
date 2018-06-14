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

#ifndef AIMS_RESAMPLING_MULTICHANNELRESAMPLER_H
#define AIMS_RESAMPLING_MULTICHANNELRESAMPLER_H


#define AIMS_RESAMPLING_DECLARE_MULTICHANNELRESAMPLER( R, T, O ) \
template <> \
class R< T > : public Resampler< T > \
{ \
public: \
\
  virtual void resample( const AimsData< T >& inVolume, \
                         const Motion& transform3d, \
                         const T& outBackground, \
                         AimsData< T >& outVolume, \
                         bool verbose = false ) const; \
\
  int getOrder() const { return O; } \
\
protected: \
\
  virtual void \
  doResample( const AimsData< T > &, \
              const Motion &, \
              const T &, const Point3df &, \
              T &, int ) const {} \
\
}; \


#define AIMS_RESAMPLING_INSTANCIATE_MULTICHANNELRESAMPLER( R, T ) \
void \
R< T >::resample( const AimsData< T >& inVolume, \
                  const Motion& transform3d, \
                  const T& outBackground, \
                  AimsData< T >& outVolume, \
                  bool verbose ) const \
{ \
  ChannelSelector< AimsData<T>, AimsData<uint8_t> > selector; \
\
  int dimX = outVolume.dimX(); \
  int dimY = outVolume.dimY(); \
  int dimZ = outVolume.dimZ(); \
  int dimT = outVolume.dimT(); \
  float sizeX = outVolume.sizeX(); \
  float sizeY = outVolume.sizeY(); \
  float sizeZ = outVolume.sizeZ(); \
  float sizeT = outVolume.sizeT(); \
\
  for (uint8_t channel = 0; channel < DataTypeInfo< T >::samples(); channel++) { \
\
    R< uint8_t > resampler; \
    AimsData<uint8_t> inChannel; \
    AimsData<uint8_t> outChannel( dimX, dimY, dimZ, dimT ); \
\
    outChannel.setSizeXYZT( sizeX, sizeY, sizeZ, sizeT ); \
    /* cout << "AimsData<" <<  DataTypeCode< T >::dataType() << "> -> resampling channel : " << toString(channel) << endl << flush; */ \
\
    if( outVolume.header() ) \
    { \
      outChannel.setHeader( outVolume.header()->cloneHeader( true ) ); \
    } \
\
    /* We split the data and process resampling on each component */ \
    inChannel = selector.select( inVolume, channel ); \
\
    resampler.setRef( inChannel ); \
    resampler.setDefaultValue( _defval[ channel ] ); \
    resampler.resample( inChannel, \
                       transform3d, \
                       outBackground[ channel ], \
                       outChannel, \
                       verbose ); \
\
    selector.set( outVolume, channel, outChannel ); \
  } \
\
} \


#endif
