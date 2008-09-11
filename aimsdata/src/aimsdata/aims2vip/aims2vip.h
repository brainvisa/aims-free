/* Copyright (c) 1995-2005 CEA
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

/*
 *  AIMS to VIP converter
 */
#ifndef AIMS_AIMS2VIP_AIMS2VIP_H
#define AIMS_AIMS2VIP_AIMS2VIP_H

#include <aims/config/aimsdata_config.h>
#include <aims/def/assert.h>
#include <aims/data/data.h>
#include <aims/vip/vip_def.h>
#include <aims/vip/vip_gen.h>
#include <aims/vip/vip_vol.h>


namespace aims {


template <class T>
class AIMSDATA_API Aims2Vip
{ 
  public:

    Aims2Vip();
    ~Aims2Vip();

    Volume* doit( const AimsData<T>& data ) const;
    void doit( const AimsData<T>& data, Volume* vol ) const;

  protected:
    int code() const;
};


template <class T>
inline
Aims2Vip<T>::Aims2Vip()
{
}


template <class T>
inline
Aims2Vip<T>::~Aims2Vip()
{
}


template <class T>
inline
Volume* Aims2Vip<T>::doit( const AimsData<T>& data ) const
{
  Volume* vol = NULL;
  vol = (Volume*)malloc( sizeof( Volume ) );
  ASSERT( vol );
  vol->id = data.id();
  strcpy( vol->name, "" );
  vol->ref.x = vol->ref.y = vol->ref.z = vol->ref.t = 0;
  vol->type = code();
  vol->size.x = (int)data.dimX();
  vol->size.y = (int)data.dimY();
  vol->size.z = (int)data.dimZ();
  vol->size.t = (int)data.dimT();
  vol->voxelSize.x = data.sizeX();
  vol->voxelSize.y = data.sizeY();
  vol->voxelSize.z = data.sizeZ();
  vol->voxelSize.t = data.sizeT();
  vol->borderWidth = (int)data.borderWidth();
  vol->data = (char *)malloc( ( data.dimX() + 2 * data.borderWidth() ) *
                              ( data.dimY() + 2 * data.borderWidth() ) *
                              ( data.dimZ() + 2 * data.borderWidth() ) *
                              data.dimT() * sizeof( T ) );
  ASSERT( vol->data );
  
  memcpy( vol->data, (char *)data.begin(), 
          ( data.dimX() + 2 * data.borderWidth() ) *
          ( data.dimY() + 2 * data.borderWidth() ) *
          ( data.dimZ() + 2 * data.borderWidth() ) *
          data.dimT() * sizeof( T ) );

  vol->tab         = NULL;
  vol->state       = AIMS_STATE_FILLED;

  vol->shfj = (SHFJ_private_struct *)calloc(1,sizeof(SHFJ_private_struct));
  vol->enst = (ENST_private_struct *)calloc(1,sizeof(ENST_private_struct));
  ASSERT( vol->shfj );
  ASSERT( vol->enst );
  vol->shfj->unite = VIDA_SANSUNITE;
  vol->shfj->VmdType = FX_SHORT_FX;
  vol->shfj->scale = 1.0;
  vol->shfj->offset = 0.0;
  vol->shfj->start_time = NULL;
  vol->shfj->dur_time = NULL;

  return vol;
}


template <class T>
inline
void Aims2Vip<T>::doit( const AimsData<T>& data, Volume *vol ) const
{
  ASSERT( vol );
  ASSERT( vol->type == code() );
  ASSERT( vol->state > AIMS_STATE_DECLARED );
  ASSERT( vol->size.x == data.dimX() &&
          vol->size.y == data.dimY() &&
          vol->size.z == data.dimZ() &&
          vol->size.t == data.dimT() );
  ASSERT( vol->borderWidth == data.borderWidth() );
  vol->id = data.id();
  strcpy( vol->name, "" );
  vol->ref.x = vol->ref.y = vol->ref.z = vol->ref.t = 0;
  vol->type = code();
  vol->voxelSize.x = data.sizeX();
  vol->voxelSize.y = data.sizeY();
  vol->voxelSize.z = data.sizeZ();
  vol->voxelSize.t = data.sizeT();
  vol->borderWidth = (int)data.borderWidth();

  vol->tab         = NULL;
  memcpy( vol->data, (char *)data.begin(),
          ( data.dimX() + 2 * data.borderWidth() ) *
          ( data.dimY() + 2 * data.borderWidth() ) *
          ( data.dimZ() + 2 * data.borderWidth() ) *
          data.dimT() * sizeof( T ) );

  vol->state       = AIMS_STATE_FILLED;

  if (!vol->shfj)
     vol->shfj = (SHFJ_private_struct *)calloc(1,sizeof(SHFJ_private_struct));
  if (!vol->enst) 
     vol->enst = (ENST_private_struct *)calloc(1,sizeof(ENST_private_struct));
  ASSERT( vol->shfj );
  ASSERT( vol->enst );
  vol->shfj->unite = VIDA_SANSUNITE;
  vol->shfj->VmdType = FX_SHORT_FX;
  vol->shfj->scale = 1.0;
  vol->shfj->offset = 0.0;
  vol->shfj->start_time = NULL;
  vol->shfj->dur_time = NULL;

}


inline
int Aims2Vip<int8_t>::code() const {
  return S8BIT;
}


inline
int Aims2Vip<uint8_t>::code() const
{
  return U8BIT;
}


inline
int Aims2Vip<int16_t>::code() const
{
  return S16BIT;
}


inline
int Aims2Vip<uint16_t>::code() const
{
  return U16BIT;
}


inline
int Aims2Vip<int32_t>::code() const
{
  return S32BIT;
}


inline
int Aims2Vip<uint32_t>::code() const
{
  return U32BIT;
}


inline
int Aims2Vip<float>::code() const
{
  return FLOAT;
}


inline
int Aims2Vip<double>::code() const
{
  return DOUBLE;
}


template <class T>
inline
int Aims2Vip<T>::code() const
{
  return -1;
}


}


#endif
