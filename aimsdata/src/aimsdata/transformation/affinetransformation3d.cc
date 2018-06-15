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
 
/** \class AffineTransformation3d
NOTE: the class AffineTransformation3d has got real problem of CLARITY and IMPLEMENTATION.
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
#include <cstdlib>
#include <aims/resampling/motion.h>
#include <aims/resampling/quaternion.h>
#include <aims/vector/vector.h>
#include <aims/data/data.h>
#include <aims/math/gausslu.h>
#include <ostream>

using namespace aims;
using namespace carto;
using namespace std;

  //----------//
 //  AffineTransformation3d  //
//----------//

//-----------------------------------------------------------------------------
namespace aims
{
    
ostream& operator << ( ostream& os, const AffineTransformation3d& thing )
{
  // os << "{translation=" << thing.translation() << ", "<<endl
  //   << "rotation=" << thing.rotation() << "}";
  os << "{translation=" << thing.translation() << ", "<<endl
     << "rotation=[[" << thing.rotation()(0,0) 
     << ", " << thing.rotation()(0,1)
     << ", " << thing.rotation()(0,2)
     << "]," << endl << "[" << thing.rotation()(1,0)
     << ", " << thing.rotation()(1,1) 
     << ", " << thing.rotation()(1,2)
     << "]," << endl << "[" << thing.rotation()(2,0)
     << ", " << thing.rotation()(2,1)
     << ", " << thing.rotation()(2,2) 
     << "]]}";

  return os;
}

}

//-----------------------------------------------------------------------------
AffineTransformation3d::AffineTransformation3d()
  : _translation( 0.0 ), _rotation( 3, 3 ),
    _header( new PythonHeader )
{
  _rotation = 0.0f;
  _translation = 0.0f;
  for ( int i = 0; i < 3; i++ )
  {
    _rotation( i, i ) = 1.0;
  }
}


//-----------------------------------------------------------------------------
AffineTransformation3d::~AffineTransformation3d()
{
  delete _header;
}


//-----------------------------------------------------------------------------
AffineTransformation3d::AffineTransformation3d(
  const AffineTransformation3d& other ) :
  RCObject(),
  Transformation3d( other ),
  _translation( other._translation ),
  _rotation( other._rotation.clone() ), 
  _header( other.header() ? new PythonHeader( *other.header() )
    : new PythonHeader )
{
}


//-----------------------------------------------------------------------------
AffineTransformation3d::AffineTransformation3d( const vector<float> & mat ) :
    Transformation3d(), _rotation( 3, 3 ), _header( new PythonHeader )
{
  *this = mat;
}


//-----------------------------------------------------------------------------
AffineTransformation3d::AffineTransformation3d( const Object mat ) :
    Transformation3d(), _rotation( 3, 3 ), _header( new PythonHeader )
{
  *this = mat;
}


//-----------------------------------------------------------------------------
AffineTransformation3d::AffineTransformation3d( const Quaternion & quat ) :
    Transformation3d(), _rotation( 3, 3 ), _header( new PythonHeader )
{
  *this = quat;
}


//-----------------------------------------------------------------------------
AffineTransformation3d& AffineTransformation3d::operator = ( const AffineTransformation3d& other )
{
  if( &other == this )
    return *this;
  _translation = other._translation;      // ie cloning (from stl library
  _rotation = (other._rotation).clone();  // explicit cloning from AimsData
  delete _header;
  if( other.header() )
    _header = new PythonHeader( *other.header() );
  else
    _header = new PythonHeader;
  return *this;
}


//-----------------------------------------------------------------------------
AffineTransformation3d& AffineTransformation3d::operator = ( const vector<float> & mat )
{
  _translation[0] = mat[3];
  _translation[1] = mat[7];
  _translation[2] = mat[11];
  _rotation( 0, 0 ) = mat[0];
  _rotation( 0, 1 ) = mat[1];
  _rotation( 0, 2 ) = mat[2];
  _rotation( 1, 0 ) = mat[4];
  _rotation( 1, 1 ) = mat[5];
  _rotation( 1, 2 ) = mat[6];
  _rotation( 2, 0 ) = mat[8];
  _rotation( 2, 1 ) = mat[9];
  _rotation( 2, 2 ) = mat[10];
  return *this;
}


//-----------------------------------------------------------------------------
AffineTransformation3d& AffineTransformation3d::operator = ( const Object mat )
{
  if( mat->size() < 12 )
    return *this; // or throw something ?
  Object  iter = mat->objectIterator();
  if( !iter->isValid() )
    return *this; // or throw something ?
  _rotation( 0, 0 ) = iter->currentValue()->getScalar();
  iter->next();
  _rotation( 0, 1 ) = iter->currentValue()->getScalar();
  iter->next();
  _rotation( 0, 2 ) = iter->currentValue()->getScalar();
  iter->next();
  _translation[0] = iter->currentValue()->getScalar();
  iter->next();
  _rotation( 1, 0 ) = iter->currentValue()->getScalar();
  iter->next();
  _rotation( 1, 1 ) = iter->currentValue()->getScalar();
  iter->next();
  _rotation( 1, 2 ) = iter->currentValue()->getScalar();
  iter->next();
  _translation[1] = iter->currentValue()->getScalar();
  iter->next();
  _rotation( 2, 0 ) = iter->currentValue()->getScalar();
  iter->next();
  _rotation( 2, 1 ) = iter->currentValue()->getScalar();
  iter->next();
  _rotation( 2, 2 ) = iter->currentValue()->getScalar();
  iter->next();
  _translation[2] = iter->currentValue()->getScalar();
  return *this;
}


//-----------------------------------------------------------------------------
AffineTransformation3d& AffineTransformation3d::operator = ( const Quaternion & quat )
{
  AimsVector<float,16>  mat = quat.rotationMatrix();
  _translation[0] = mat[3];
  _translation[1] = mat[7];
  _translation[2] = mat[11];
  _rotation( 0, 0 ) = mat[0];
  _rotation( 0, 1 ) = mat[1];
  _rotation( 0, 2 ) = mat[2];
  _rotation( 1, 0 ) = mat[4];
  _rotation( 1, 1 ) = mat[5];
  _rotation( 1, 2 ) = mat[6];
  _rotation( 2, 0 ) = mat[8];
  _rotation( 2, 1 ) = mat[9];
  _rotation( 2, 2 ) = mat[10];
  return *this;
}


//-----------------------------------------------------------------------------
bool AffineTransformation3d::operator == ( const AffineTransformation3d & other )
{
  static float eps = 1e-5;
  return fabs( _translation[0] - other._translation[0] ) < eps
      && fabs( _translation[1] - other._translation[1] ) < eps
      && fabs( _translation[2] - other._translation[2] ) < eps
      && fabs( _rotation(0,0) - other._rotation(0,0) ) < eps
      && fabs( _rotation(0,1) - other._rotation(0,1) ) < eps
      && fabs( _rotation(0,1) - other._rotation(0,1) ) < eps
      && fabs( _rotation(1,0) - other._rotation(1,0) ) < eps
      && fabs( _rotation(1,1) - other._rotation(1,1) ) < eps
      && fabs( _rotation(1,1) - other._rotation(1,1) ) < eps
      && fabs( _rotation(2,0) - other._rotation(2,0) ) < eps
      && fabs( _rotation(2,1) - other._rotation(2,1) ) < eps
      && fabs( _rotation(2,1) - other._rotation(2,1) ) < eps;
}


//-----------------------------------------------------------------------------
void AffineTransformation3d::setHeader( PythonHeader* ph )
{
  delete _header;
  if( ph )
    _header = ph;
  else
    _header = new PythonHeader;
}


//-----------------------------------------------------------------------------
Point3dd
AffineTransformation3d::transformDouble( double x, double y, double z ) const
{
  return Point3dd( _rotation(0,0) * x + _rotation(0,1) * y
                   + _rotation(0,2) * z,
                   _rotation(1,0) * x + _rotation(1,1) * y
                   + _rotation(1,2) * z,
                   _rotation(2,0) * x + _rotation(2,1) * y 
                   + _rotation(2,2) * z
                 ) + Point3dd( _translation[0], _translation[1],
                              _translation[2] );
}


Point3df
AffineTransformation3d::transformFloat( float x, float y, float z ) const
{
  return Point3df( _rotation(0,0) * x + _rotation(0,1) * y
                   + _rotation(0,2) * z, 
                   _rotation(1,0) * x + _rotation(1,1) * y
                   + _rotation(1,2) * z, 
                   _rotation(2,0) * x + _rotation(2,1) * y 
                   + _rotation(2,2) * z
                   ) + _translation;
}


Point3dd
AffineTransformation3d::transformVectorDouble( double x, double y,
                                               double z ) const
{
  return Point3dd( _rotation(0,0) * x + _rotation(0,1) * y
                   + _rotation(0,2) * z,
                   _rotation(1,0) * x + _rotation(1,1) * y
                   + _rotation(1,2) * z,
                   _rotation(2,0) * x + _rotation(2,1) * y 
                   + _rotation(2,2) * z
                   );
}


Point3df
AffineTransformation3d::transformVectorFloat( float x, float y, float z ) const
{
  return Point3df( _rotation(0,0) * x + _rotation(0,1) * y
                   + _rotation(0,2) * z, 
                   _rotation(1,0) * x + _rotation(1,1) * y
                   + _rotation(1,2) * z, 
                   _rotation(2,0) * x + _rotation(2,1) * y 
                   + _rotation(2,2) * z
                   );
}


Point3dd AffineTransformation3d::transformNormalDouble( double x, double y,
                                                  double z ) const
{
  Point3dd      dir2( x, y, z );
  Point3dd	u = vectProduct( dir2, Point3dd( 0, 0, 1 ) );
  if( u.norm() <= 1e-4 ) u = vectProduct( dir2, Point3dd( 0, 1, 0 ) );
  Point3dd      w = vectProduct( dir2, u );

  u = transformVector( u );
  w = transformVector( w );
  dir2 = vectProduct( u, w );
  return dir2;
}


//-----------------------------------------------------------------------------
bool AffineTransformation3d::isIdentity() const
{
  if ( _rotation( 0, 0 )  != 1.0 ) return 0;
  if ( _rotation( 1, 1 )  != 1.0 ) return 0;
  if ( _rotation( 2, 2 )  != 1.0 ) return 0;
  if ( _rotation( 0, 1 )  != 0.0 ) return 0;
  if ( _rotation( 0, 2 )  != 0.0 ) return 0;
  if ( _rotation( 1, 0 )  != 0.0 ) return 0;
  if ( _rotation( 1, 2 )  != 0.0 ) return 0;
  if ( _rotation( 2, 0 )  != 0.0 ) return 0;
  if ( _rotation( 2, 1 )  != 0.0 ) return 0;
  
  if ( _translation.item(0)  != 0.0 ) return 0;
  if ( _translation.item(1)  != 0.0 ) return 0;
  if ( _translation.item(2)  != 0.0 ) return 0;

  return 1;
}


//-----------------------------------------------------------------------------
void AffineTransformation3d::setToIdentity()
{
  _translation = Point3df( 0., 0., 0. ) ;
  _rotation = 0 ;
  _rotation(0,0) = _rotation(1,1) = _rotation(2,2) = 1. ;
}


//-----------------------------------------------------------------------------
AffineTransformation3d AffineTransformation3d::inverse() const
{
/* from Anatomist:

  float det = _rotation[0][0]*
  (_rotation[1][1]*_rotation[2][2]-_rotation[2][1]*_rotation[1][2])
  - _rotation[1][0]*
  (_rotation[0][1]*_rotation[2][2]-_rotation[2][1]*_rotation[0][2])
  + _rotation[2][0]*
  (_rotation[0][1]*_rotation[1][2]-_rotation[1][1]*_rotation[0][2]);

  if( det == 0 )
  {
  cerr << "Transformation::invert : determinant = 0\n";
  _rotation[0][0] = 1.;
  _rotation[1][0] = 0.;
  _rotation[2][0] = 0.;
  _rotation[0][1] = 0.;
  _rotation[1][1] = 1.;
  _rotation[2][1] = 0.;
  _rotation[0][2] = 0.;
  _rotation[1][2] = 0.;
  _rotation[2][2] = 1.;
  _translation[0] = 0.;
  _translation[1] = 0.;
  _translation[2] = 0.;
  return;
}
  else
  {
  float	i[3][3];
  i[0][0] = (_rotation[1][1]*_rotation[2][2]
  -_rotation[2][1]*_rotation[1][2]) / det;
  i[0][1] = -(_rotation[1][0]*_rotation[2][2]
  -_rotation[2][0]*_rotation[1][2]) / det;
  i[0][2] =  (_rotation[1][0]*_rotation[2][1]
  -_rotation[2][0]*_rotation[1][1]) / det;
  i[1][0] = -(_rotation[0][1]*_rotation[2][2]
  -_rotation[2][1]*_rotation[0][2]) / det;
  i[1][1] =  (_rotation[0][0]*_rotation[2][2]
  -_rotation[2][0]*_rotation[0][2]) / det;
  i[1][2] = -(_rotation[0][0]*_rotation[2][1]
  -_rotation[2][0]*_rotation[0][1]) / det;
  i[2][0] =  (_rotation[0][1]*_rotation[1][2]
  -_rotation[1][1]*_rotation[0][2]) / det;
  i[2][1] = -(_rotation[0][0]*_rotation[1][2]
  -_rotation[1][0]*_rotation[0][2]) / det;
  i[2][2] =  (_rotation[0][0]*_rotation[1][1]
  -_rotation[1][0]*_rotation[0][1]) / det;

  _rotation[0][0] = i[0][0];
  _rotation[0][1] = i[1][0];
  _rotation[0][2] = i[2][0];
  _rotation[1][0] = i[0][1];
  _rotation[1][1] = i[1][1];
  _rotation[1][2] = i[2][1];
  _rotation[2][0] = i[0][2];
  _rotation[2][1] = i[1][2];
  _rotation[2][2] = i[2][2];

  float tmp0, tmp1, tmp2;

  // translation inverse = - Rtransposee * T

  tmp0 = _translation[0];
  tmp1 = _translation[1];
  tmp2 = _translation[2];
  _translation[0] = - ( _rotation[0][0] * tmp0 + _rotation[0][1] * tmp1
      + _rotation[0][2] * tmp2 );
  _translation[1] = - ( _rotation[1][0] * tmp0 + _rotation[1][1] * tmp1
      + _rotation[1][2] * tmp2 );
  _translation[2] = - ( _rotation[2][0] * tmp0 + _rotation[2][1] * tmp1
      + _rotation[2][2] * tmp2 );
}

  */
  AffineTransformation3d AffineTransformation3d;

  AffineTransformation3d.rotation() = AimsInversionLU( rotation() );

  Point3df tmp( -translation() );
  AffineTransformation3d.translation().item( 0 ) = AffineTransformation3d.rotation()( 0, 0 ) * tmp.item( 0 ) +
                                   AffineTransformation3d.rotation()( 0, 1 ) * tmp.item( 1 ) +
                                   AffineTransformation3d.rotation()( 0, 2 ) * tmp.item( 2 ) ;
  AffineTransformation3d.translation().item( 1 ) = AffineTransformation3d.rotation()( 1, 0 ) * tmp.item( 0 ) +
                                   AffineTransformation3d.rotation()( 1, 1 ) * tmp.item( 1 ) +
                                   AffineTransformation3d.rotation()( 1, 2 ) * tmp.item( 2 ) ;
  AffineTransformation3d.translation().item( 2 ) = AffineTransformation3d.rotation()( 2, 0 ) * tmp.item( 0 ) +
                                   AffineTransformation3d.rotation()( 2, 1 ) * tmp.item( 1 ) +
                                   AffineTransformation3d.rotation()( 2, 2 ) * tmp.item( 2 ) ;

  return AffineTransformation3d;
}



//-----------------------------------------------------------------------------
void AffineTransformation3d::scale( const Point3df& sizeFrom, const Point3df& sizeTo )
{
  _rotation( 0, 0 ) *= sizeFrom[ 0 ] / sizeTo[ 0 ]; 
  _rotation( 0, 1 ) *= sizeFrom[ 1 ] / sizeTo[ 0 ]; 
  _rotation( 0, 2 ) *= sizeFrom[ 2 ] / sizeTo[ 0 ]; 

  _rotation( 1, 0 ) *= sizeFrom[ 0 ] / sizeTo[ 1 ]; 
  _rotation( 1, 1 ) *= sizeFrom[ 1 ] / sizeTo[ 1 ]; 
  _rotation( 1, 2 ) *= sizeFrom[ 2 ] / sizeTo[ 1 ]; 

  _rotation( 2, 0 ) *= sizeFrom[ 0 ] / sizeTo[ 2 ]; 
  _rotation( 2, 1 ) *= sizeFrom[ 1 ] / sizeTo[ 2 ]; 
  _rotation( 2, 2 ) *= sizeFrom[ 2 ] / sizeTo[ 2 ]; 

  _translation.item( 0 ) /= sizeTo[ 0 ];
  _translation.item( 1 ) /= sizeTo[ 1 ];
  _translation.item( 2 ) /= sizeTo[ 2 ];
}


//-----------------------------------------------------------------------------
bool AffineTransformation3d::isDirect() const
{
  float	det = _rotation(0,0) * ( _rotation(1,1) * _rotation(2,2) 
				  - _rotation(2,1) * _rotation(1,2) ) 
    + _rotation(1,0) * ( _rotation(2,1) * _rotation(0,2) 
			  - _rotation(0,1) * _rotation(2,2) ) 
    + _rotation(2,0) * ( _rotation(0,1) * _rotation(1,2) 
			  - _rotation(1,1) * _rotation(0,2) );
  return( det >= 0 );
}


//-----------------------------------------------------------------------------
void AffineTransformation3d::setTranslation(Point3df t)
/* NOTE: for the moment, in spite of its name, this method actually ADD t to the
current translation ( _translation <- (_translation + t) ). This LONG and
UNCLEAR code seems to be strictly equivalent to
	  translation() += t;
To be more relevent (and consistent with the name of the method), one should
even use
		translation() = t;
however, it should be noted that for the moment, the informations about the
center of transformations are contained in the _translation attribute (see for
example AffineTransformation3d::setRotationAffine or DecomposedAffineTransformation3d::transAffine) so that
 using this last form would reset the center of transformation to the origin
(0,0,0).

see NOTE, line 38. */
{
  AimsData<float> THomogene(4,4), AffineTransformation3dHomogene(4,4);


  THomogene(3,0) = t.item(0);
  THomogene(3,1) = t.item(1);
  THomogene(3,2) = t.item(2);
  THomogene(0,0) = THomogene(1,1) = THomogene(2,2) = THomogene(3,3) = 1.0;

  AffineTransformation3dHomogene(3,0) = translation().item(0);
  AffineTransformation3dHomogene(3,1) = translation().item(1);
  AffineTransformation3dHomogene(3,2) = translation().item(2);
  AffineTransformation3dHomogene(0,0) = rotation()(0,0);  //Transp
  AffineTransformation3dHomogene(0,1) = rotation()(1,0);
  AffineTransformation3dHomogene(0,2) = rotation()(2,0);
  AffineTransformation3dHomogene(1,0) = rotation()(0,1);
  AffineTransformation3dHomogene(1,1) = rotation()(1,1);
  AffineTransformation3dHomogene(1,2) = rotation()(2,1);
  AffineTransformation3dHomogene(2,0) = rotation()(0,2);
  AffineTransformation3dHomogene(2,1) = rotation()(1,2);
  AffineTransformation3dHomogene(2,2) = rotation()(2,2);
  AffineTransformation3dHomogene(3,3) = 1.0;

  AimsData<float> tmp = AffineTransformation3dHomogene.cross( THomogene ); // produit transpose

  translation().item(0) = tmp(3,0);
  translation().item(1) = tmp(3,1);
  translation().item(2) = tmp(3,2);
  rotation()(0,0) = tmp(0,0);
  rotation()(0,1) = tmp(1,0);
  rotation()(0,2) = tmp(2,0);
  rotation()(1,0) = tmp(0,1);
  rotation()(1,1) = tmp(1,1);
  rotation()(1,2) = tmp(2,1);
  rotation()(2,0) = tmp(0,2);
  rotation()(2,1) = tmp(1,2);
  rotation()(2,2) = tmp(2,2);
}

//-----------------------------------------------------------------------------
void AffineTransformation3d::setRotationAffine( float rx, float ry, float rz,
                                const Point3df &cg )
/* NOTE: this method is UNCLEAR and SUBOPTIMAL.

Currently this methid destroys any already specified translation.
We could avoid this side effect with
  translation().item(0) += tmp(3,0);
  translation().item(1) += tmp(3,1);
  translation().item(2) += tmp(3,2);
in the current code but it would be better to follow specification in NOTE,
line 38. */
{
  
  AimsData<float> Rx = rotationaroundx(rx);
  AimsData<float> Ry = rotationaroundy(ry);
  AimsData<float> Rz = rotationaroundz(rz);
  AimsData<float> vectRot = Rz.cross(Ry.cross(Rx));  //Produit transpose!!


  AimsData<float> T(4,4), Tmoins1(4,4);
  T = 0.0; Tmoins1 = 0.0;


  T(3,0) = -cg.item(0); Tmoins1(3,0) = cg.item(0);
  T(3,1) = -cg.item(1); Tmoins1(3,1) = cg.item(1);
  T(3,2) = -cg.item(2); Tmoins1(3,2) = cg.item(2);
  T(0,0) = T(1,1) = T(2,2) = T(3, 3) = 1.0;
  Tmoins1(0,0) = Tmoins1(1,1) = Tmoins1(2,2) = Tmoins1(3,3) = 1.0;
  


  AimsData<float> tmp = T.cross( vectRot.cross( Tmoins1 ) ); //Produit transpos
  translation().item(0) = tmp(3,0);
  translation().item(1) = tmp(3,1);
  translation().item(2) = tmp(3,2);

  rotation()(0,0) = tmp(0,0);  //Transpo
  rotation()(0,1) = tmp(1,0);
  rotation()(0,2) = tmp(2,0);
  rotation()(1,0) = tmp(0,1);
  rotation()(1,1) = tmp(1,1);
  rotation()(1,2) = tmp(2,1);
  rotation()(2,0) = tmp(0,2);
  rotation()(2,1) = tmp(1,2);
  rotation()(2,2) = tmp(2,2);
}


//-----------------------------------------------------------------------------
AimsData<float> AffineTransformation3d::rotationaroundx(float rx)
/* this method return a 4-by-4 matrix, but a 3-by-3 matrix would be
enough and better. see NOTE, line 38. */
{
  AimsData<float> thing( 4,4 );
  thing = 0.0; thing(3,3) = 1.0;
  
  double a = (double) rx / 180.0 * M_PI;
  thing(0,0) = 1.0;
  thing(0,1) = 0.0;
  thing(0,2) = 0.0;

  thing(1,0) = 0.0;
  thing(1,1) = (float) cos(a);
  thing(1,2) = (float) sin(a);
  
  thing(2,0) = 0.0;
  thing(2,1) = (float)-sin(a);
  thing(2,2) = (float) cos(a);

  return( thing );
}

//-----------------------------------------------------------------------------
AimsData<float> AffineTransformation3d::rotationaroundy(float ry)
/* this method return a 4-by-4 matrix, but a 3-by-3 matrix would be
enough and better. see NOTE, line 38. */
{
  AimsData<float> thing( 4,4 );
  thing = 0.0; thing(3,3) = 1.0;
	
  double a = (double) ry / 180.0 * M_PI;
  thing(0,0) = (float) cos(a);
  thing(0,1) = 0.0;
  thing(0,2) = (float) sin(a);

  thing(1,0) = 0.0;
  thing(1,1) = 1.0;
  thing(1,2) = 0.0;
  
  thing(2,0) = (float)-sin(a);
  thing(2,1) = 0.0;
  thing(2,2) = (float) cos(a);

  return( thing );
}

//-----------------------------------------------------------------------------
AimsData<float> AffineTransformation3d::rotationaroundz(float rz)
/* this method return a 4-by-4 matrix, but a 3-by-3 matrix would be
enough and better. see NOTE, line 38. */
{
  AimsData<float> thing( 4,4 );
  thing = 0.0; thing(3,3) = 1.0;

  double a = (double) rz / 180.0 * M_PI;
  thing(0,0) = (float) cos(a);
  thing(0,1) = (float) sin(a);
  thing(0,2) = 0.0;

  thing(1,0) = (float)-sin(a);
  thing(1,1) = (float) cos(a );
  thing(1,2) = 0.0;
  
  thing(2,0) = 0.0;
  thing(2,1) = 0.0;
  thing(2,2) = 1.0;

  return( thing );
}



//-----------------------------------------------------------------------------
// void AffineTransformation3d::setRotationVectorial( const Point3df& u1, const Point3df& v1,
// 				   const Point3df& u2, const Point3df& v2 )
// {
  

  
//   Point3df axis, u1(v1), u2(v2) ;
//   double angle, c, s, t ;
  
//   u1.normalize() ; u2.normalize() ;
//   angle = acos( ( u1.dot( u2 ) < 0. ? 0. : (u1.dot( u2 ) > 1. ? 1. : u1.dot( u2 ) ) ) ) ; 
//   axis[0] = u1[1]*u2[2] - u2[1]*u1[2] ;
//   axis[1] = -u1[0]*u2[2] + u2[0]*u1[2] ;
//   axis[2] = u1[0]*u2[1] - u2[0]*u1[1] ;

//   cout << "Axis dot v1 = " << axis.dot(u1) 
//        << "\t" << "Axis dot v2 = " << axis.dot(u2) << endl ; 
  
//   axis.normalize() ;
  
//   c = cos(angle) ; s = sin(angle) ; t = 1- cos(angle) ;
  
//   rotation()(0, 0) = t * axis[0] * axis[0] + c ;
//   rotation()(0, 1) = t * axis[0] * axis[1] - s * axis[2] ;
//   rotation()(0, 2) = t * axis[0] * axis[2] + s * axis[1] ;
//   rotation()(1, 0) = t * axis[0] * axis[1] + s * axis[2] ;
//   rotation()(1, 1) = t * axis[1] * axis[1] + c ;
//   rotation()(1, 2) = t * axis[1] * axis[2] - s * axis[0] ;
//   rotation()(2, 0) = t * axis[0] * axis[2] - s * axis[1] ;
//   rotation()(2, 1) = t * axis[1] * axis[2] + s * axis[0] ;
//   rotation()(2, 2) = t * axis[2] * axis[2] + c ;
  
//   AimsData<float> vect( 3, 1 ) ;
//   vect(0, 0) = v1[0] ; vect(1, 0) = v1[1] ; vect(2, 0) = v1[2] ;
//   AimsData<float> vectTransformed = rotation().cross( vect ) ;
//   Point3df v1Transformed( vectTransformed(0, 0), vectTransformed(1, 0), vectTransformed(2, 0) ) ;
  
//   //DEBUG START
//   cout << "v1Transformed = " << v1Transformed << endl ;
  
//   vect(0, 0) = axis[0] ; vect(1, 0) = axis[1] ; vect(2, 0) = axis[2] ;
//   vectTransformed = rotation().cross( vect ) ;
  
//   cout << "axis = " << axis 
//        << "\taxis transformed = ( " << vectTransformed(0, 0) << " , " 
//        << vectTransformed(1, 0) << " , " 
//        << vectTransformed(2, 0) << " ) " << endl ;
//     // DEBUG END
//   if( v2.dot( v1Transformed ) < 0 )
//     {
//       //Transposition
//       rotation()(0, 1) = t * axis[0] * axis[1] + s * axis[2] ;
//       rotation()(0, 2) = t * axis[0] * axis[2] - s * axis[1] ;
//       rotation()(1, 0) = t * axis[0] * axis[1] - s * axis[2] ;
//       rotation()(1, 2) = t * axis[1] * axis[2] + s * axis[0] ;
//       rotation()(2, 0) = t * axis[0] * axis[2] + s * axis[1] ;
//       rotation()(2, 1) = t * axis[1] * axis[2] - s * axis[0] ;      
//     }

  
  
  
// //   Point3df u1, u2, r, t(translation()), cross ;

// //   for( int i = 0 ; i < 3 ; ++i ){
// //     u1 = v1 ; u2 = v2 ;
// //     u1[i] = 0. ; 
// //     if( u1 != Point3df(0., 0.,0.) ) u1.normalize() ;
// //     u2[i] = 0. ; 
// //     if( u2 != Point3df(0., 0.,0.) ) u2.normalize() ;
// //     r[i] = acos( ( u1.dot( u2 ) < 0. ? 0. : (u1.dot( u2 ) > 1. ? 1. : u1.dot( u2 ) ) ) ) * 180. / M_PI ;
// //   }
// //   if( v1[0]*v2[1] - v2[0]*v1[1] < 0. ) 
// //     r[2] += M_PI ;
// //   if( v1[0]*v2[2] - v2[0]*v1[2] < 0. ) 
// //     r[1] += M_PI ;
// //   if( v1[1]*v2[2] - v2[1]*v1[2] < 0. ) 
// //     r[0] += M_PI ;
    
// //   AimsData<float> Rx = rotationaroundx(r[0]);
// //   AimsData<float> Ry = rotationaroundy(r[1]);
// //   AimsData<float> Rz = rotationaroundz(r[2]);
// //   AimsData<float> vectRot = Rz.cross(Ry.cross(Rx));  //Produit transpose!!
  
// //   for( int i = 0 ; i < 3 ; ++i )
// //     for( int j = 0 ; j < 3 ; ++j )
// //       _rotation(i, j) = vectRot(i, j) ;
// }


AffineTransformation3d & AffineTransformation3d::operator *= ( const AffineTransformation3d & m )
{
  Point3df	r2 = transform( m.translation() );
  _translation[0] = r2[0];
  _translation[1] = r2[1];
  _translation[2] = r2[2];

  float	x[3];
  x[0] = _rotation(0,0);
  x[1] = _rotation(0,1);
  x[2] = _rotation(0,2);
  _rotation(0,0) = x[0] * m._rotation(0,0) + x[1] * m._rotation(1,0)
      + x[2] * m._rotation(2,0);
  _rotation(0,1) = x[0] * m._rotation(0,1) + x[1] * m._rotation(1,1)
      + x[2] * m._rotation(2,1);
  _rotation(0,2) = x[0] * m._rotation(0,2) + x[1] * m._rotation(1,2)
      + x[2] * m._rotation(2,2);
  x[0] = _rotation(1,0);
  x[1] = _rotation(1,1);
  x[2] = _rotation(1,2);
  _rotation(1,0) = x[0] * m._rotation(0,0) + x[1] * m._rotation(1,0)
      + x[2] * m._rotation(2,0);
  _rotation(1,1) = x[0] * m._rotation(0,1) + x[1] * m._rotation(1,1)
      + x[2] * m._rotation(2,1);
  _rotation(1,2) = x[0] * m._rotation(0,2) + x[1] * m._rotation(1,2)
      + x[2] * m._rotation(2,2);
  x[0] = _rotation(2,0);
  x[1] = _rotation(2,1);
  x[2] = _rotation(2,2);
  _rotation(2,0) = x[0] * m._rotation(0,0) + x[1] * m._rotation(1,0)
      + x[2] * m._rotation(2,0);
  _rotation(2,1) = x[0] * m._rotation(0,1) + x[1] * m._rotation(1,1)
      + x[2] * m._rotation(2,1);
  _rotation(2,2) = x[0] * m._rotation(0,2) + x[1] * m._rotation(1,2)
      + x[2] * m._rotation(2,2);

  return( *this );
}


/*
AffineTransformation3d & AffineTransformation3d::operator += ( const AffineTransformation3d & t )
{
  _translation[0] += t._translation[0];
  _translation[1] += t._translation[1];
  _translation[2] += t._translation[2];

  _rotation[0][0] += t._rotation[0][0];
  _rotation[0][1] += t._rotation[0][1];
  _rotation[0][2] += t._rotation[0][2];
  _rotation[1][0] += t._rotation[1][0];
  _rotation[1][1] += t._rotation[1][1];
  _rotation[1][2] += t._rotation[1][2];
  _rotation[2][0] += t._rotation[2][0];
  _rotation[2][1] += t._rotation[2][1];
  _rotation[2][2] += t._rotation[2][2];

  return( *this );
}
*/


AffineTransformation3d AffineTransformation3d::operator - () const
{
  AffineTransformation3d t;

  t._translation = -_translation;
  t._rotation = -VolumeRef<float>( _rotation.volume() );

  return t;
}



//-----------------------------------------------------------------------------
namespace aims
{

AffineTransformation3d operator * (
  const AffineTransformation3d& AffineTransformation3d1, 
  const AffineTransformation3d& AffineTransformation3d2 )
{
  AffineTransformation3d AffineTransformation3dOut;

  AimsData< float > mat1(4,4), mat2(4,4), mat3(4,4);
  
  int i, j, k;

  for(i=0;i<3;++i)
    for(j=0;j<3;++j)
      {
	mat1(j,i) = AffineTransformation3d1.rotation()(i,j);
	mat2(j,i) = AffineTransformation3d2.rotation()(i,j);
      }

  for(i=0;i<3;++i)
    {
      mat1(3,i) = AffineTransformation3d1.translation().item(i);
      mat2(3,i) = AffineTransformation3d2.translation().item(i);
      mat1(i,3) = mat2(i,3) = 0;
    }

  mat1(3,3) = mat2(3,3) = 1.0;
  mat3 = 0;

  ForEach2d(mat3, i, j)
    {
      for(k=0;k<4;++k)
        mat3(i,j) += mat2(i,k)*mat1(k,j);
    }
  
  
  for(i=0;i<3;++i)
    {
      AffineTransformation3dOut.translation()[i] = mat3(3,i);
    }
  
  for(i=0;i<3;++i)
    for(j=0;j<3;++j)
      {
	AffineTransformation3dOut.rotation()(j,i) = mat3(i,j);
      }
  // cout << "Compose  : " << AffineTransformation3dOut << endl << endl;

  return AffineTransformation3dOut;
}

}

//-----------------------------------------------------------------------------
vector<float> AffineTransformation3d::toVector() const
{
  vector<float> vec( 16 );
  vec[0] = _rotation( 0,0 );
  vec[1] = _rotation( 0,1 );
  vec[2] = _rotation( 0,2 );
  vec[3] = _translation[0];
  vec[4] = _rotation( 1,0 );
  vec[5] = _rotation( 1,1 );
  vec[6] = _rotation( 1,2 );
  vec[7] = _translation[1];
  vec[8] = _rotation( 2,0 );
  vec[9] = _rotation( 2,1 );
  vec[10] = _rotation( 2,2 );
  vec[11] = _translation[2];
  vec[12] = 0;
  vec[13] = 0;
  vec[14] = 0;
  vec[15] = 1.;
  return vec;
}


//-----------------------------------------------------------------------------
vector<float> AffineTransformation3d::toColumnVector() const
{
  vector<float> vec( 16 );
  vec[0] = _rotation( 0,0 );
  vec[1] = _rotation( 1,0 );
  vec[2] = _rotation( 2,0 );
  vec[3] = 0;
  vec[4] = _rotation( 0,1 );
  vec[5] = _rotation( 1,1 );
  vec[6] = _rotation( 2,1 );
  vec[7] = 0;
  vec[8] = _rotation( 0,2 );
  vec[9] = _rotation( 1,2 );
  vec[10] = _rotation( 2,2 );
  vec[11] = 0;
  vec[12] = _translation[0];
  vec[13] = _translation[1];
  vec[14] = _translation[2];
  vec[15] = 1.;
  return vec;
}


//-----------------------------------------------------------------------------
void AffineTransformation3d::fromColumnVector( const float* vec )
{
  _rotation( 0, 0 ) = vec[0];
  _rotation( 1, 0 ) = vec[1];
  _rotation( 2, 0 ) = vec[2];
  _translation[0] = vec[3];
  _rotation( 0, 1 ) = vec[4];
  _rotation( 1, 1 ) = vec[5];
  _rotation( 2, 1 ) = vec[6];
  _translation[1] = vec[7];
  _rotation( 0, 2 ) = vec[8];
  _rotation( 1, 2 ) = vec[9];
  _rotation( 2, 2 ) = vec[10];
  _translation[2] = vec[11];
}

//-----------------------------------------------------------------------------
namespace aims
{

void transformBoundingBox( const AffineTransformation3d &AffineTransformation3d, const Point3df & pmin1,
		const Point3df & pmax1, Point3df & pmin2, Point3df & pmax2 )
{
  pmin2 = pmax2 = AffineTransformation3d.transform( pmin1 );
  Point3df	p = Point3df( pmax1[0], pmin1[1], pmin1[2] ), p2;
  p2 = AffineTransformation3d.transform( p );
  if( p2[0] < pmin2[0] )
    pmin2[0] = p2[0];
  if( p2[1] < pmin2[1] )
    pmin2[1] = p2[1];
  if( p2[2] < pmin2[2] )
    pmin2[2] = p2[2];
  if( p2[0] > pmax2[0] )
    pmax2[0] = p2[0];
  if( p2[1] > pmax2[1] )
    pmax2[1] = p2[1];
  if( p2[2] > pmax2[2] )
    pmax2[2] = p2[2];

  p[1] = pmax1[1];
  p2 = AffineTransformation3d.transform( p );
  if( p2[0] < pmin2[0] )
    pmin2[0] = p2[0];
  if( p2[1] < pmin2[1] )
    pmin2[1] = p2[1];
  if( p2[2] < pmin2[2] )
    pmin2[2] = p2[2];
  if( p2[0] > pmax2[0] )
    pmax2[0] = p2[0];
  if( p2[1] > pmax2[1] )
    pmax2[1] = p2[1];
  if( p2[2] > pmax2[2] )
    pmax2[2] = p2[2];

  p[0] = pmin1[0];
  p2 = AffineTransformation3d.transform( p );
  if( p2[0] < pmin2[0] )
    pmin2[0] = p2[0];
  if( p2[1] < pmin2[1] )
    pmin2[1] = p2[1];
  if( p2[2] < pmin2[2] )
    pmin2[2] = p2[2];
  if( p2[0] > pmax2[0] )
    pmax2[0] = p2[0];
  if( p2[1] > pmax2[1] )
    pmax2[1] = p2[1];
  if( p2[2] > pmax2[2] )
    pmax2[2] = p2[2];

  p[1] = pmin1[1];
  p[2] = pmax1[2];
  p2 = AffineTransformation3d.transform( p );
  if( p2[0] < pmin2[0] )
    pmin2[0] = p2[0];
  if( p2[1] < pmin2[1] )
    pmin2[1] = p2[1];
  if( p2[2] < pmin2[2] )
    pmin2[2] = p2[2];
  if( p2[0] > pmax2[0] )
    pmax2[0] = p2[0];
  if( p2[1] > pmax2[1] )
    pmax2[1] = p2[1];
  if( p2[2] > pmax2[2] )
    pmax2[2] = p2[2];

  p[0] = pmax1[0];
  p2 = AffineTransformation3d.transform( p );
  if( p2[0] < pmin2[0] )
    pmin2[0] = p2[0];
  if( p2[1] < pmin2[1] )
    pmin2[1] = p2[1];
  if( p2[2] < pmin2[2] )
    pmin2[2] = p2[2];
  if( p2[0] > pmax2[0] )
    pmax2[0] = p2[0];
  if( p2[1] > pmax2[1] )
    pmax2[1] = p2[1];
  if( p2[2] > pmax2[2] )
    pmax2[2] = p2[2];

  p[1] = pmax1[1];
  p2 = AffineTransformation3d.transform( p );
  if( p2[0] < pmin2[0] )
    pmin2[0] = p2[0];
  if( p2[1] < pmin2[1] )
    pmin2[1] = p2[1];
  if( p2[2] < pmin2[2] )
    pmin2[2] = p2[2];
  if( p2[0] > pmax2[0] )
    pmax2[0] = p2[0];
  if( p2[1] > pmax2[1] )
    pmax2[1] = p2[1];
  if( p2[2] > pmax2[2] )
    pmax2[2] = p2[2];

  p[0] = pmin1[0];
  p2 = AffineTransformation3d.transform( p );
  if( p2[0] < pmin2[0] )
    pmin2[0] = p2[0];
  if( p2[1] < pmin2[1] )
    pmin2[1] = p2[1];
  if( p2[2] < pmin2[2] )
    pmin2[2] = p2[2];
  if( p2[0] > pmax2[0] )
    pmax2[0] = p2[0];
  if( p2[1] > pmax2[1] )
    pmax2[1] = p2[1];
  if( p2[2] > pmax2[2] )
    pmax2[2] = p2[2];
}

}
