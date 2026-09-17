
#ifndef AIMS_TALAIRACH_TALBOUNDINGBOX_POINTS_H
#define AIMS_TALAIRACH_TALBOUNDINGBOX_POINTS_H

#include <cstdlib>
#include <cartobase/smart/rcptr.h>
#include <aims/talairach/talBoxBase.h>
#include <aims/roi/roiIterator.h>
#include <list>


class TalairachBoundingBoxPoints : public TalairachBoxBase
{
public:
  TalairachBoundingBoxPoints() : TalairachBoxBase() {}
  inline virtual ~TalairachBoundingBoxPoints() {}
  void computeBox( carto::rc_ptr<aims::RoiIterator> roiIt );
  Motion computeTransformationAndBox( const TalairachPoints&,	
                                      carto::rc_ptr<aims::RoiIterator> roiIt );

  // Returns a motion from current subject to a normalized template, where
  // AC (0, 0, 0) PC (0, 1, 0) and IHP (0, 1, cout0)
};


inline
void TalairachBoundingBoxPoints::computeBox( carto::rc_ptr<aims::RoiIterator> roiIt )
{
  //int x, y, z, dx = roiIt->volumeDimension()[0], dy = roiIt->volumeDimension()[1], dz = roiIt->volumeDimension()[2] ;
  Point3df dsize( roiIt->voxelSize() );
  Point3df boxmax( -10000.0f, -10000.0f, -10000.0f );
  Point3df boxmin( 10000.0f, 10000.0f, 10000.0f );
  Point3df pt, npt;
  
  roiIt->restart() ;
  std::list<Point3d> points ;
  while(roiIt->isValid() ){
    carto::rc_ptr<aims::MaskIterator> maskIt = roiIt->maskIterator() ;
    
    while( maskIt->isValid() ){
      points.push_back( maskIt->value() ) ;
      
      maskIt->next() ;	
    }
    
    roiIt->next() ;
  }
  
  for( std::list<Point3d>::iterator it = points.begin() ; it != points.end() ; ++it ){
    pt = Point3df( float( (*it)[0] ), float( (*it)[1] ), float( (*it)[2] ) );
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


inline 
Motion TalairachBoundingBoxPoints::computeTransformationAndBox( const TalairachPoints& pt,
							        carto::rc_ptr<aims::RoiIterator> roiIt )
{
  computeTransformation( pt );
  computeBox( roiIt );
  
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
  
  _transformation.setTranslation(Point3df(0., 0., 0.));
  _transformation.setMatrix(rotation);  
  
  std::cout << "Scale : " << _scale << std::endl ;
  _transformation.scale( Point3df(1., 1., 1. ), 
    			 Point3df(1.0f/_scale[0], 1.0f/_scale[1], 1.0f/_scale[2] ) ) ;
  _transformation.setTranslation( _transformation.transform(translation) );
  return _transformation ;
}

#endif
