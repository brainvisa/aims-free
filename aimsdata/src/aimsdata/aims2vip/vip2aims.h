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
 *  VIP to AIMS converter
 */
#ifndef AIMS_AIMS2VIP_VIP2AIMS_H
#define AIMS_AIMS2VIP_VIP2AIMS_H

#include <aims/config/aimsdata_config.h>
#include <aims/def/assert.h>
#include <aims/data/data.h>
#include <aims/vip/vip_def.h>
#include <aims/vip/vip_gen.h>
#include <aims/vip/vip_vol.h>
#include <vidaIO/kernel.h>


namespace aims {


template <class T>
class AIMSDATA_API Vip2Aims
{ 
  public:

    Vip2Aims();
    ~Vip2Aims();

    AimsData<T> doit( const Volume* vol ) const;
    void doit( const Volume* vol, AimsData<T>& data ) const;

  protected:
    int code() const;
};


template <class T>
inline
Vip2Aims<T>::Vip2Aims()
{
}


template <class T>
inline
Vip2Aims<T>::~Vip2Aims()
{
}


template <class T>
inline
AimsData<T> Vip2Aims<T>::doit( const Volume* vol ) const
{
  ASSERT( vol );
  ASSERT( vol->type == code() );
  AimsData<T> data( int( vol->size.x ),
                    int( vol->size.y ),
                    int( vol->size.z ),
                    int( vol->size.t ), int( vol->borderWidth ) );
  data.setSizeX( vol->voxelSize.x );
  data.setSizeY( vol->voxelSize.y );
#include <aims/config/config_aimsdata.h>
  data.setSizeZ( vol->voxelSize.z );
  data.setSizeT( vol->voxelSize.t );

  data.setId( int( vol->id ) );

  memcpy( (char *)data.begin(), vol->data, 
          ( data.dimX() + 2 * data.borderWidth() ) *
          ( data.dimY() + 2 * data.borderWidth() ) *
          ( data.dimZ() + 2 * data.borderWidth() ) *
          data.dimT() * sizeof( T ) );

  return data;
}


template <class T>
inline
void Vip2Aims<T>::doit( const Volume* vol, AimsData<T>& data ) const
{
  ASSERT( vol );
  ASSERT( vol->type == code() );
  ASSERT( vol->size.x == data.dimX() && 
          vol->size.y == data.dimY() &&
          vol->size.z == data.dimZ() &&
          vol->size.t == data.dimT() );
  ASSERT( vol->borderWidth == data.borderWidth() );

  data.setSizeX( vol->voxelSize.x );
  data.setSizeY( vol->voxelSize.y );
  data.setSizeZ( vol->voxelSize.z );
  data.setSizeT( vol->voxelSize.t );

  data.setId( int( vol->id ) );

#include <aims/config/config_aimsdata.h>
  memcpy( (char *)data.begin(), vol->data,
          ( data.dimX() + 2 * data.borderWidth() ) *
          ( data.dimY() + 2 * data.borderWidth() ) *
          ( data.dimZ() + 2 * data.borderWidth() ) *
          data.dimT() * sizeof( T ) );

}


inline
int Vip2Aims<int8_t>::code() const
{
  return S8BIT;
}


inline
int Vip2Aims<uint8_t>::code() const
{
  return U8BIT;
}


inline
int Vip2Aims<int16_t>::code() const
{
  return S16BIT;
}


inline
int Vip2Aims<uint16_t>::code() const
{
  return U16BIT;
}


inline
int Vip2Aims<int32_t>::code() const
{
  return S32BIT;
}


inline
int Vip2Aims<uint32_t>::code() const
{
  return U32BIT;
}


inline
int Vip2Aims<float>::code() const
{
  return FLOAT;
}


inline
int Vip2Aims<double>::code() const
{
  return DOUBLE;
}


template <class T>
inline
int Vip2Aims<T>::code() const
{
  return -1;
}


}


#endif
