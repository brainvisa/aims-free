/*
 *  Binary inverse of volume sequence,volume,matrix,vector.
 */
#ifndef AIMS_UTILITY_BININVERSE_H
#define AIMS_UTILITY_BININVERSE_H

#include <aims/config/aimsdata_config.h>
#include <cartodata/volume/volume.h>

/** The object function for binary inverse.
    This object function returns the inverse of a binary short volume.
*/
class AimsBinaryInverse
{
public:
  /// Constructor does nothing
  AimsBinaryInverse() { }
  /// Destructor does nothing
  virtual ~AimsBinaryInverse() { }

  /// Object function which returns the binary inverse of a short data
  carto::VolumeRef<short> operator () (
    const carto::rc_ptr<carto::Volume<short> > &data )
  {
    carto::VolumeRef<short> res( new carto::Volume<short>( *data ) );
    carto::Volume<short>::iterator it;

    for( it=res->begin(); it!=res->end(); it++ )
      *it = (*it ? 0 : 32767);
    return res;
  }
};

#endif



