
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/talairach/talACPCNorm.h>
#include <aims/talairach/cramer.h>
#include <cartodata/volume/volume.h>

using namespace carto;

TalairachACPCNorm::TalairachACPCNorm() :
  TalairachReferential(), _norm(1.)
{
}


Motion TalairachACPCNorm::computeTransformation( const TalairachPoints& pt )
{
  computeVectors( pt );
  VolumeRef<float> rotation(3, 3) ;
  Point3df translation = -pt.ACmm() ;
  
  rotation(0, 0) = _hemiVec[ 0 ];
  rotation(0, 1) = _hemiVec[ 1 ];
  rotation(0, 2) = _hemiVec[ 2 ];
  
  rotation(1, 0) = _ACPCVec[ 0 ];
  rotation(1, 1) = _ACPCVec[ 1 ];
  rotation(1, 2) = _ACPCVec[ 2 ];

  rotation(2, 0) = -_crossVec[ 0 ];
  rotation(2, 1) = -_crossVec[ 1 ];
  rotation(2, 2) = -_crossVec[ 2 ];
  _transformation.setMatrix(rotation);
  _transformation.scale( Point3df(1., 1., 1. ), Point3df(_norm, _norm, _norm) ) ;
  _transformation.setTranslation( _transformation.transform(translation) );
  //m.rotation().transpose() ;
  return _transformation ;
}


void TalairachACPCNorm::computeVectors( const TalairachPoints& pt )
{
  Point3df M( 1.0f, 1.0f, 1.0f );

  _ACPCVec[ 0 ] = -( pt.ACmm()[ 0 ] - pt.PCmm()[ 0 ] );
  _ACPCVec[ 1 ] = -( pt.ACmm()[ 1 ] - pt.PCmm()[ 1 ] );
  _ACPCVec[ 2 ] = -( pt.ACmm()[ 2 ] - pt.PCmm()[ 2 ] );

  _norm = _ACPCVec.norm() ;
  _ACPCVec /= _norm;

  CramerSolver cs;
  _hemiVec = cs.doit( pt.ACmm(), pt.PCmm(), pt.Hemimm(), M );
  
  _hemiVec = _hemiVec - _hemiVec.dot(_ACPCVec) *_ACPCVec ;
  
  _crossVec[0] = _hemiVec[2] * _ACPCVec[1] - _hemiVec[1] * _ACPCVec[2];
  _crossVec[1] = _hemiVec[0] * _ACPCVec[2] - _hemiVec[2] * _ACPCVec[0];
  _crossVec[2] = _hemiVec[1] * _ACPCVec[0] - _hemiVec[0] * _ACPCVec[1];

  _hemiVec /= _hemiVec.norm();
  _crossVec /= _crossVec.norm();
}
