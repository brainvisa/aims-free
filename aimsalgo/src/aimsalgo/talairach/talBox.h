
#ifndef AIMS_TALAIRACH_TALBOX_H
#define AIMS_TALAIRACH_TALBOX_H

#include <cstdlib>
#include <cartodata/volume/volume.h>
#include <aims/talairach/talBoxBase.h>


template< class T >
class TalairachBox : public TalairachBoxBase
{
public:
  TalairachBox() : TalairachBoxBase() {}
  inline virtual ~TalairachBox() {}
  void computeBox( const carto::rc_ptr<carto::Volume< T > > & );
  aims::AffineTransformation3d computeTransformationAndBox(
    const TalairachPoints&, const carto::rc_ptr<carto::Volume< T >  > & );

  // Returns a motion from current subject to a normalized template, where
  // AC (0, 0, 0) PC (0, 1, 0) and IHP (0, 1, cout0)
};


template< class T > inline
void TalairachBox< T >::computeBox(
  const carto::rc_ptr<carto::Volume< T > > & d )
{
  int x, y, z, dx = d->getSizeX(), dy = d->getSizeY(), dz = d->getSizeZ();
  Point3df dsize( d->getVoxelSize() );
  Point3df boxmax( -10000.0f, -10000.0f, -10000.0f );
  Point3df boxmin( 10000.0f, 10000.0f, 10000.0f );
  Point3df pt, npt;
  
  for ( z=0; z<dz; z++ )
    for ( y=0; y<dy; y++ )
      for ( x=0; x<dx; x++ )
        if ( d->at( x, y, z ) )
        {
          pt = Point3df( float( x ), float( y ), float( z ) );
          pt[ 0 ] *= dsize[ 0 ];
          pt[ 1 ] *= dsize[ 1 ];
          pt[ 2 ] *= dsize[ 2 ];
          npt = TalairachReferential::toTalairach( pt );

          if ( npt[ 0 ] < boxmin[ 0 ] )  boxmin[ 0 ] = npt[ 0 ];
          if ( npt[ 1 ] < boxmin[ 1 ] )  boxmin[ 1 ] = npt[ 1 ];
          if ( npt[ 2 ] < boxmin[ 2 ] )  boxmin[ 2 ] = npt[ 2 ];
          if ( npt[ 0 ] > boxmax[ 0 ] )  boxmax[ 0 ] = npt[ 0 ];
          if ( npt[ 1 ] > boxmax[ 1 ] )  boxmax[ 1 ] = npt[ 1 ];
          if ( npt[ 2 ] > boxmax[ 2 ] )  boxmax[ 2 ] = npt[ 2 ];
        }

  if ( fabs( boxmin[ 0 ] ) > fabs( boxmax[ 0 ] ) )
    _scale[ 0 ] = 1.0f / fabs( boxmin[ 0 ] );
  else _scale[ 0 ] = 1.0f / fabs( boxmax[ 0 ] );

  std::cout << "Box Min : " << boxmin << "\tBox Max :" << boxmax << std::endl ;
  _scale[ 1 ] = 1.0f / fabs( boxmax[ 1 ] );
  // On peut ausi prendre
  // _scale[ 1 ] = 1.0f/_ACPCVec.norm()
  _scale[ 2 ] = 1.0f / fabs( boxmin[ 2 ] );
}


template< class T > inline 
aims::AffineTransformation3d TalairachBox< T >::computeTransformationAndBox(
  const TalairachPoints& pt, const carto::rc_ptr<carto::Volume< T > > & d )
{
  computeTransformation( pt );
  computeBox( d );
  
  carto::VolumeRef<float> rotation(3, 3) ;
  Point3df translation = -pt.ACmm() ;

  rotation(0, 0) = -_crossVec[ 0 ];
  rotation(0, 1) = -_crossVec[ 1 ];
  rotation(0, 2) = -_crossVec[ 2 ];

  rotation(1, 0) = _ACPCVec[ 0 ];
  rotation(1, 1) = _ACPCVec[ 1 ];
  rotation(1, 2) = _ACPCVec[ 2 ];

  rotation(2, 0) = _hemiVec[ 0 ];
  rotation(2, 1) = _hemiVec[ 1 ];
  rotation(2, 2) = _hemiVec[ 2 ];
  
  _transformation.setTranslation( Point3df(0., 0., 0.) );
  _transformation.setMatrix(rotation);  
  
  std::cout << "Scale : " << _scale << std::endl ;
  _transformation.scale( Point3df(1., 1., 1. ), 
    			 Point3df(1.0f/_scale[0], 1.0f/_scale[1], 1.0f/_scale[2] ) ) ;
  _transformation.setTranslation( _transformation.transform(translation) );
  return _transformation ;
}

#endif
