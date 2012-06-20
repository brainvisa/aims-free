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

#ifndef AIMSALGO_SIGNALFILTER_MULTICHANNELFILTER_H
#define AIMSALGO_SIGNALFILTER_MULTICHANNELFILTER_H

#include <aims/utility/channel.h>

#define AIMSALGO_SIGNALFILTER_SPECIALIZE_MULTICHANNELFILTER( R, T ) \
template <class F> \
class R< T, F > \
{ \
  public: \
\
    R ( int sx = 3, int sy = 3, int sz = 1 ); \
    virtual ~R () {} \
\
    AimsData< T > doit( const AimsData<T>& ); \
\
protected: \
\
  int                        _win_size_x; \
  int                        _win_size_y; \
  int                        _win_size_z; \
  const F                    _func; \
\
}; \
\
template< class F > \
inline \
R< T, F >::R(int sx, int sy, int sz ) \
  : _win_size_x(sx), \
    _win_size_y(sy),\
    _win_size_z(sz) \
{ \
}\
\
template <class F> \
inline \
AimsData< T > \
R< T, F >::doit( const AimsData<T>& ref ) \
{ \
  ChannelSelector< AimsData<T>, AimsData<uint8_t> > selector; \
  DataTypeInfo< T > info; \
\
  AimsData<T> outVolume(ref.dimX(), ref.dimY(), ref.dimZ(), ref.dimT()); \
  if( ref.header() ) { \
    outVolume.setHeader( ref.header()->cloneHeader( true ) ); \
  } \
\
  R<uint8_t, F> filter(_win_size_x, _win_size_y, _win_size_z); \
  for (uint8_t channel = 0; channel < DataTypeInfo< T >::samples(); channel++) { \
\
    /* We split the data and process filter on each component */ \
    const AimsData<uint8_t> & inChannel = selector.select( ref, channel ); \
\
    const AimsData<uint8_t> & outChannel = filter.doit( inChannel ); \
    selector.set( outVolume, channel, outChannel ); \
  } \
\
  return outVolume; \
\
} \


#endif
