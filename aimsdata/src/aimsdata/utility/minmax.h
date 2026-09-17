#ifndef AIMS_UTILITY_MINMAX_H
#define AIMS_UTILITY_MINMAX_H

#include <cartodata/volume/volume.h>

namespace aims
{

  //-----------------------------------//
 //  minmax< ObjectType, ValueType >  //
//-----------------------------------//
template <typename ObjectType, typename ValueType>
void minmax( const ObjectType &, ValueType &min, ValueType &max );

//-----------------------------------------------------------------------------
template <typename ObjectType, typename ValueType>
void minmax( const ObjectType &, ValueType &min, ValueType &max )
{
  ObjectType::error_minmax_not_specialized;
}


  //-------------------------------------------------//
 //  minmax< carto::Volume<ValueType>, ValueType >  //
//-------------------------------------------------//
template <typename ValueType>
void minmax( const carto::Volume<ValueType> &volume, ValueType &min,
             ValueType &max )
{
  const ValueType *it;
  int dx = volume.getSizeX();
  int dy = volume.getSizeY();
  int dz = volume.getSizeZ();
  int dt = volume.getSizeT();

  min = max = volume.at( 0 );
  for ( int t = 0; t < dt; t++ )
  {
    for ( int z = 0; z < dz; z++ )
    {
      for ( int y = 0; y < dy; y++ )
      {
        it = &volume.at( 0, y, z, t );
        for ( int x = 0; x < dx; x++ )
        {
          if ( *it < min ) min = *it;
          if ( *it > max ) max = *it;
          it++;
        }
      }
    }
  }
}


  //-----------------------------------------------------------------//
 //  minmax< carto::rc_prr<carto::Volume<ValueType> >, ValueType >  //
//-----------------------------------------------------------------//
template <typename ValueType>
void minmax( const carto::rc_ptr<carto::Volume<ValueType> > &volume,
             ValueType &min, ValueType &max )
{
  minmax( *volume, min, max );
}


  //----------------------------------------------------//
 //  minmax< carto::VolumeRef<ValueType>, ValueType >  //
//----------------------------------------------------//
template <typename ValueType>
void minmax( const carto::VolumeRef<ValueType> &volume,
             ValueType &min, ValueType &max )
{
  minmax( *volume, min, max );
}


} // namespace aims

#endif // ifndef AIMS_UTILITY_MINMAX_H
