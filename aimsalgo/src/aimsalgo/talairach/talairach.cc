
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/talairach/talairach.h>
#include <aims/talairach/cramer.h>
#include <cartodata/volume/volume.h>

using namespace aims;
using namespace carto;
using namespace std;


TalairachReferential::TalairachReferential()
{
  _transformation.setToIdentity() ;
}


AffineTransformation3d TalairachReferential::computeTransformation(
  const TalairachPoints& pt )
{
  computeVectors( pt );
  VolumeRef<float> rotation(3, 3) ;
  Point3df translation = -pt.ACmm() ;

/*   rotation(0, 0) = _hemiVec[ 0 ];
  rotation(0, 1) = _hemiVec[ 1 ];
  rotation(0, 2) = _hemiVec[ 2 ];

  rotation(1, 0) = _ACPCVec[ 0 ];
  rotation(1, 1) = _ACPCVec[ 1 ];
  rotation(1, 2) = _ACPCVec[ 2 ];

  rotation(2, 0) = -_crossVec[ 0 ];
  rotation(2, 1) = -_crossVec[ 1 ];
  rotation(2, 2) = -_crossVec[ 2 ];
 */  

  rotation(0, 0) = -_crossVec[ 0 ];
  rotation(0, 1) = -_crossVec[ 1 ];
  rotation(0, 2) = -_crossVec[ 2 ];

  rotation(1, 0) = _ACPCVec[ 0 ];
  rotation(1, 1) = _ACPCVec[ 1 ];
  rotation(1, 2) = _ACPCVec[ 2 ];

  rotation(2, 0) = -_hemiVec[ 0 ];
  rotation(2, 1) = -_hemiVec[ 1 ];
  rotation(2, 2) = -_hemiVec[ 2 ];
   
  _transformation.setTranslation( Point3df(0., 0., 0.) );
  _transformation.setMatrix(rotation);
  _transformation.setTranslation( _transformation.transform(translation) );
  //m.rotation().transpose() ;
  return _transformation ;
}


void TalairachReferential::computeVectors( const TalairachPoints& pt )
{
  Point3df M( 1.0f, 1.0f, 1.0f );

  _ACPCVec[ 0 ] = -( pt.ACmm()[ 0 ] - pt.PCmm()[ 0 ] );
  _ACPCVec[ 1 ] = -( pt.ACmm()[ 1 ] - pt.PCmm()[ 1 ] );
  _ACPCVec[ 2 ] = -( pt.ACmm()[ 2 ] - pt.PCmm()[ 2 ] );

  _ACPCVec /= _ACPCVec.norm();

//  CramerSolver cs;
//  _hemiVec = cs.doit( pt.ACmm(), pt.PCmm(), pt.Hemimm(), M );
  
  _hemiVec = pt.Hemimm() -  pt.ACmm() ;
//  _hemiVec = -( pt.Hemimm() -  pt.ACmm() ) ;
  _hemiVec = _hemiVec - _hemiVec.dot(_ACPCVec) *_ACPCVec ;
  
  _crossVec[0] = _hemiVec[2] * _ACPCVec[1] - _hemiVec[1] * _ACPCVec[2];
  _crossVec[1] = _hemiVec[0] * _ACPCVec[2] - _hemiVec[2] * _ACPCVec[0];
  _crossVec[2] = _hemiVec[1] * _ACPCVec[0] - _hemiVec[0] * _ACPCVec[1];

  _hemiVec /= _hemiVec.norm();
  _crossVec /= _crossVec.norm();
}


Point3df TalairachReferential::toTalairach( const Point3df& pt )
{
  return _transformation.transform( pt ) ;
}


Point3df TalairachReferential::fromTalairach( const Point3df& pt )
{
  return _transformation.inverse()->transform( pt )  ;
}
