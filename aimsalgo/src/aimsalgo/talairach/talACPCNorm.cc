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


// we don't want to issue a warning
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/talairach/talACPCNorm.h>
#include <aims/talairach/cramer.h>
#include <aims/data/data.h>


TalairachACPCNorm::TalairachACPCNorm() :
  TalairachReferential(), _norm(1.)
{
}


Motion TalairachACPCNorm::computeTransformation( const TalairachPoints& pt )
{
  computeVectors( pt );
  AimsData<float> rotation(3, 3) ;
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
