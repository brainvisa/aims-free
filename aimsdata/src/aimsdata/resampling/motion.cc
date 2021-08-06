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
 *  AFFINE 3-D transformations (Linear + Translation)
 */
 
/* NOTE: the class AffineTransformation3d has got real problem of CLARITY and IMPLEMENTATION.
For example the order of specification of the attributes can change the
resulting AffineTransformation3d.
At this stage of the code (11/03/2010, submitted by Thierry Delzescaux, Nicolas
Souedet and Hugo Raguet), one must respect this order of specification to get
the right AffineTransformation3d:

	AffineTransformation3d::
		- create the AffineTransformation3d
		- first specify setRotationAffine
		- then specify setTranslation
		- one can then redo this process any time as long as we first specify
setRotationAffine

	DecomposedAffineTransformation3d::
		- create the AffineTransformation3d
		- first specify setTranslation
		- then specify transAffine
		- it is then IMPOSSIBLE to reset the transformation because neither the
setTranslation nor the transAffine methods resets the _translation attribute

One way to address those problems (and several others, see methods below) would
be to create the Affine::AffineTransformation3d class according to the following specifications:

 # the AffineTransformation3d, applied to a point P, should do an AFFINE transformation under
the form
	AffineTransformation3d( P ) = C + A x ( P - C ) + T                            (1)
where
	A is the linear transformation, composition of rotation, scaling, shearing
	C is the "center" of the linear transformation
	T is the translation.
	
 # then, an AffineAffineTransformation3d should contain the following attributes:
 	_rotation (the equivalent of _rot in DecomposedAffineTransformation3d), 3x3 matrix
 	_scaling (equivalent in DecomposedAffineTransformation3d), 3x3 matrix
 	_shearing (equivalent in DecomposedAffineTransformation3d), 3x3 matrix
 	_translation (would contain only T), 3x1 vector
 	_center ( would contain C ), 3x1 vector
 	_linear ( would contain A = _rotation x _scaling x _shearing, i.e. the
equivalent of the current _rotation attribute, whose name is highly confusing),
3x3 matrix
 	_shift ( would contain t = C - A x C + T ), 3x1 vector
 
 # by rewritting (1) as
 	AffineTransformation3d( P ) = A x P + ( C - A x C + T ) = A x P + t            (2)
we see that the method AffineAffineTransformation3d::transform corresponds the current
AffineTransformation3d::transform, implementing P = _linear x P + _shift.
The reason of the attributes _linear and _shift is a matter of optimization:
each transformation require only 9 multiplications et 12 additions. Otherwise,
computing directly the trnsformation as written in (1) will more than double
the number of necessary operations.

 # there must be methods which simply SETS the attributes _rotation, _scaling,
_shearing, _translation, _center, BUT DO NOT CHANGE _linear and _shift, so it
does not change the results of a transformation by this AffineTransformation3d

 # finally a method like "AffineAffineTransformation3d::updateTransformation" should update
the attributes _linear and _shift so that the transformation (2) correctly
computes (1) consistently with all attributes i.e. this methods do
	_linear <- _rotation x _scaling x _shearing;
	_shift <- _center - _linear x _center + _translation;

*/

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cstdlib>
#include <aims/resampling/motion.h>
#include <aims/resampling/quaternion.h>
#include <aims/vector/vector.h>
#include <stdexcept>

using namespace aims;
using namespace carto;
using namespace std;

  //--------------------//
 //  DecomposedAffineTransformation3d  //
//--------------------//

//-----------------------------------------------------------------------------
DecomposedAffineTransformation3d::DecomposedAffineTransformation3d() :
  AffineTransformation3d(),
  _shear( VolumeRef<float>( 3, 3 ) ),
  _scaling( VolumeRef<float>( 3, 3 ) ),
  _rot( VolumeRef<float>( 3, 3 ) )
			
{
  _shear = 0.0f;
  _scaling = 0.0f;
  _rot = 0.0f;
  for ( int i = 0; i < 3; i++ )
  {
    _shear (i, i) = 1.0;
    _scaling (i, i) = 1.0;
    _rot (i, i) = 1.0;
  }
}


//-----------------------------------------------------------------------------
DecomposedAffineTransformation3d::DecomposedAffineTransformation3d( const DecomposedAffineTransformation3d& other ) :
  RCObject(), AffineTransformation3d( other ),
  _shear( other._shear.deepcopy() ),
  _scaling( other._scaling.deepcopy() ),
  _rot ( other._rot.deepcopy() )
{
}


//-----------------------------------------------------------------------------
DecomposedAffineTransformation3d &
DecomposedAffineTransformation3d::operator =( const DecomposedAffineTransformation3d& other )
{
  AffineTransformation3d::operator =( other );
  _scaling = other._scaling.deepcopy();
  _shear = other._shear.deepcopy();
  _rot = other._rot.deepcopy();
  return *this;
}


//-----------------------------------------------------------------------------
AffineTransformation3d &DecomposedAffineTransformation3d::operator =( const AffineTransformation3d & )
{
  throw runtime_error( "Affectation of a AffineTransformation3d to a DecomposedAffineTransformation3d not "
                       "implemented" );
}


//-----------------------------------------------------------------------------
void DecomposedAffineTransformation3d::setToIdentity()
{
  AffineTransformation3d::setToIdentity();
  _scaling(0,0) = _scaling(1,1) = _scaling(2,2) = 1. ;
  _shear(0,0) = _shear(1,1) = _shear(2,2) = 1. ;
  _rot(0,0) = _rot(1,1) = _rot(2,2) = 1. ;
}


//-----------------------------------------------------------------------------
void DecomposedAffineTransformation3d::scale( const Point3df &, const Point3df & )
{
  throw runtime_error( "scale() not implemented for DecomposedAffineTransformation3d" );
}


//-----------------------------------------------------------------------------
void DecomposedAffineTransformation3d::setRotationAffine( float, float, float, 
                                                          const Point3df & )
{
  throw runtime_error( "setRotationAffine() not implemented for "
                       "DecomposedAffineTransformation3d" );
}


//-----------------------------------------------------------------------------
void DecomposedAffineTransformation3d::setShearing(float Cx, float Cy, float Cz )
{
/*  shearing()(0,0) = 1.0;
  shearing()(0,1) = Cx;
  shearing()(0,2) = Cy;

  shearing()(1,0) = 0.0;
  shearing()(1,1) = 1.0;
  shearing()(1,2) = Cz;
  
  shearing()(2,0) = 0.0;
  shearing()(2,1) = 0.0;
  shearing()(2,2) = 1.0;
*/
  shearing()(0,0) = 1.0;
  shearing()(1,0) = Cx;
  shearing()(2,0) = Cy;

  shearing()(0,1) = 0.0;
  shearing()(1,1) = 1.0;
  shearing()(2,1) = Cz;
  
  shearing()(0,2) = 0.0;
  shearing()(1,2) = 0.0;
  shearing()(2,2) = 1.0;

}

//-----------------------------------------------------------------------------
void DecomposedAffineTransformation3d::setScaling(float Sx, float Sy, float Sz )
{
  scaling()(0,0) = Sx;
  scaling()(0,1) = 0.0;
  scaling()(0,2) = 0.0;

  scaling()(1,0) = 0.0;
  scaling()(1,1) = Sy;
  scaling()(1,2) = 0.0;
  
  scaling()(2,0) = 0.0;
  scaling()(2,1) = 0.0;
  scaling()(2,2) = Sz;

}

//-----------------------------------------------------------------------------
void DecomposedAffineTransformation3d::setRotation(float Rx, float Ry, float Rz )
/* this method sets uses 4-by-4 matrices, but 3-by-3 matrices would be
enough and better. see NOTE, line 38. */
{
  VolumeRef<float> rx = rotationaroundx(Rx);
  VolumeRef<float> ry = rotationaroundy(Ry);
  VolumeRef<float> rz = rotationaroundz(Rz);
  VolumeRef<float> uselessMatrix_seeLine38( 4, 4 );
  uselessMatrix_seeLine38 = matrix_product( rz, matrix_product( ry, rx ));
  for( int16_t i=0; i<3; i++ ){
  	for( int16_t j=0; j<3; j++ ){
			rot()( i, j ) = uselessMatrix_seeLine38( i, j );
		}
	}
}

//-----------------------------------------------------------------------------
void DecomposedAffineTransformation3d::transAffine(Point3df C)
{
// given the rotation (R), scaling (Sc) and shearing (Sh) matrices,
// given the translation (T), and given a center of transformation (C),
// compute the resulting linear transformation (A) and the new translation (t),
// such that the AffineTransformation3d can be algebraicly described as, for any point P,
// AffineTransformation3d(P) = C + A x (P - C) + T = A x P + (C - A x C + T)
//                                 = A x P +        t
// where A = R x Sc x Sh
// the method AffineTransformation3d::transform actually uses the form A x P + t, such that
// A is put in the _rotation attribute and t is in the _translation attribute,
// in spite of those IMPROPER terms. see NOTE, line 38.

  AffineTransformation3d::Table<float> & m = matrix();
  setMatrix( matrix_product( _rot, matrix_product( _scaling, _shear ) ) );

  m(0, 3) += C.item(0) - m(0,0)*C.item(0) - m(0,1)*C.item(1) - m(0,2)*C.item(2);

  m(1, 3) += C.item(1) - m(1,0)*C.item(0) - m(1,1)*C.item(1) - m(1,2)*C.item(2);

  m(2, 3) += C.item(2) - m(2,0)*C.item(0) - m(2,1)*C.item(1) - m(2,2)*C.item(2);
}
