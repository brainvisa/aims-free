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
 *  Non elastic motion ( rotation + translation )
 */
#include <cstdlib>
#include <aims/resampling/motion.h>
#include <aims/resampling/quaternion.h>
#include <aims/vector/vector.h>
#include <aims/data/data.h>
#include <aims/math/gausslu.h>
#include <iostream>
#include <fstream>
#include <stdexcept>

using namespace aims;
using namespace carto;
using namespace std;

  //----------//
 //  Motion  //
//----------//

//-----------------------------------------------------------------------------
ostream& operator << ( ostream& os, const Motion& thing )
{
  // os << "{translation=" << thing.translation() << ", "<<endl
  //   << "rotation=" << thing.rotation() << "}";
  os << "{translation=" << thing.translation() << ", "<<endl
     << "rotation=[[" << thing.rotation()(0,0) 
     << ", " << thing.rotation()(1,0) 
     << ", " << thing.rotation()(2,0) 
     << "]," << endl << "[" << thing.rotation()(0,1) 
     << ", " << thing.rotation()(1,1) 
     << ", " << thing.rotation()(2,1) 
     << "]," << endl << "[" << thing.rotation()(2,0) 
     << ", " << thing.rotation()(1,2) 
     << ", " << thing.rotation()(2,2) 
     << "]]}";

  return os;
}


//-----------------------------------------------------------------------------
Motion::Motion()
  : _translation( 0.0 ), _rotation( 3, 3 ),
    _header( 0 )
{
  _rotation = 0.0f;
  _translation = 0.0f;
  for ( int i = 0; i < 3; i++ )
  {
    _rotation( i, i ) = 1.0;
  }
}


//-----------------------------------------------------------------------------
Motion::~Motion()
{
  delete _header;
}


//-----------------------------------------------------------------------------
Motion::Motion( const Motion& other ) :
  RCObject(),
  _translation( other._translation ), 
  _rotation( other._rotation.clone() ), 
  _header( other.header() ? new PythonHeader( *other.header() ) : 0 )
{
}


//-----------------------------------------------------------------------------
Motion::Motion( const vector<float> & mat ) :
    RCObject(), _rotation( 3, 3 ), _header( 0 )
{
  *this = mat;
}


//-----------------------------------------------------------------------------
Motion::Motion( const Object mat ) :
    RCObject(), _rotation( 3, 3 ), _header( 0 )
{
  *this = mat;
}


//-----------------------------------------------------------------------------
Motion::Motion( const Quaternion & quat ) :
    RCObject(), _rotation( 3, 3 ), _header( 0 )
{
  *this = quat;
}


//-----------------------------------------------------------------------------
Motion& Motion::operator = ( const Motion& other )
{
  if( &other == this )
    return *this;
  _translation = other._translation;      // ie cloning (from stl library
  _rotation = (other._rotation).clone();  // explicit cloning from AimsData
  delete _header;
  if( other.header() )
    _header = new PythonHeader( *other.header() );
  else
    _header = 0;
  return *this;
}


//-----------------------------------------------------------------------------
Motion& Motion::operator = ( const vector<float> & mat )
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
Motion& Motion::operator = ( const Object mat )
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
Motion& Motion::operator = ( const Quaternion & quat )
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
bool Motion::operator == ( const Motion & other )
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
void Motion::setHeader( PythonHeader* ph )
{
  delete _header;
  _header = ph;
}


//-----------------------------------------------------------------------------
Point3df Motion::transform( float x, float y, float z ) const
{
  return Point3df( _rotation(0,0) * x + _rotation(0,1) * y
                   + _rotation(0,2) * z, 
                   _rotation(1,0) * x + _rotation(1,1) * y
                   + _rotation(1,2) * z, 
                   _rotation(2,0) * x + _rotation(2,1) * y 
                   + _rotation(2,2) * z
                   ) + _translation;
}

Point3df Motion::transform_normal( float x, float y, float z ) const
{
  Point3df      dir(x, y, z);
  Point3df	u = vectProduct( dir, Point3df( 0, 0, 1 ) );
  if( u.norm() <= 1e-4 ) u = vectProduct( dir, Point3df( 0, 1, 0 ) );
  Point3df      w = vectProduct( dir, u );
  Point3df      porg = transform( Point3df( 0, 0, 0 ) );

  u = transform( u ) - porg;
  w = transform( w ) - porg;
  dir = vectProduct( u, w );
  dir.normalize();
  return dir;
}

//-----------------------------------------------------------------------------
bool Motion::isIdentity() const
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
void Motion::setToIdentity()
{
  _translation = Point3df( 0., 0., 0. ) ;
  _rotation = 0 ;
  _rotation(0,0) = _rotation(1,1) = _rotation(2,2) = 1. ;
}


//-----------------------------------------------------------------------------
Motion Motion::inverse() const
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
  Motion motion;

  motion.rotation() = AimsInversionLU( rotation() );

  Point3df tmp( -translation() );
  motion.translation().item( 0 ) = motion.rotation()( 0, 0 ) * tmp.item( 0 ) +
                                   motion.rotation()( 0, 1 ) * tmp.item( 1 ) +
                                   motion.rotation()( 0, 2 ) * tmp.item( 2 ) ;
  motion.translation().item( 1 ) = motion.rotation()( 1, 0 ) * tmp.item( 0 ) +
                                   motion.rotation()( 1, 1 ) * tmp.item( 1 ) +
                                   motion.rotation()( 1, 2 ) * tmp.item( 2 ) ;
  motion.translation().item( 2 ) = motion.rotation()( 2, 0 ) * tmp.item( 0 ) +
                                   motion.rotation()( 2, 1 ) * tmp.item( 1 ) +
                                   motion.rotation()( 2, 2 ) * tmp.item( 2 ) ;

  return motion;
}



//-----------------------------------------------------------------------------
void Motion::scale( const Point3df& sizeFrom, const Point3df& sizeTo )
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
bool Motion::isDirect() const
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
void Motion::setTranslation(Point3df t)
{
  AimsData<float> THomogene(4,4), MotionHomogene(4,4);


  THomogene(3,0) = t.item(0);
  THomogene(3,1) = t.item(1);
  THomogene(3,2) = t.item(2);
  THomogene(0,0) = THomogene(1,1) = THomogene(2,2) = THomogene(3,3) = 1.0;

  MotionHomogene(3,0) = translation().item(0);
  MotionHomogene(3,1) = translation().item(1);
  MotionHomogene(3,2) = translation().item(2);
  MotionHomogene(0,0) = rotation()(0,0);  //Transp
  MotionHomogene(0,1) = rotation()(1,0);
  MotionHomogene(0,2) = rotation()(2,0);
  MotionHomogene(1,0) = rotation()(0,1);
  MotionHomogene(1,1) = rotation()(1,1);
  MotionHomogene(1,2) = rotation()(2,1);
  MotionHomogene(2,0) = rotation()(0,2);
  MotionHomogene(2,1) = rotation()(1,2);
  MotionHomogene(2,2) = rotation()(2,2);
  MotionHomogene(3,3) = 1.0;

  AimsData<float> tmp = MotionHomogene.cross( THomogene ); // produit transpose

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
void Motion::setRotationAffine( float rx, float ry, float rz, 
                                const Point3df &cg )
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
AimsData<float> Motion::rotationaroundx(float rx)
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
AimsData<float> Motion::rotationaroundy(float ry)
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
AimsData<float> Motion::rotationaroundz(float rz)
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
// void Motion::setRotationVectorial( const Point3df& u1, const Point3df& v1, 
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




Motion & Motion::operator *= ( const Motion & m )
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
Motion & Motion::operator += ( const Motion & t )
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


Motion Motion::operator - () const
{
  Motion t;

  t._translation = _translation;
  t._rotation = t._rotation - _rotation;

  return t;
}



//-----------------------------------------------------------------------------
Motion operator * (const Motion& motion1, const Motion& motion2)
{
  Motion motionOut;

  AimsData< float > mat1(4,4), mat2(4,4), mat3(4,4);
  
  int i, j, k;

  for(i=0;i<3;++i)
    for(j=0;j<3;++j)
      {
	mat1(j,i) = motion1.rotation()(i,j);
	mat2(j,i) = motion2.rotation()(i,j);
      }

  for(i=0;i<3;++i)
    {
      mat1(3,i) = motion1.translation().item(i);
      mat2(3,i) = motion2.translation().item(i);
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
      motionOut.translation()[i] = mat3(3,i);
    }
  
  for(i=0;i<3;++i)
    for(j=0;j<3;++j)
      {
	motionOut.rotation()(j,i) = mat3(i,j);
      }
  // cout << "Compose  : " << motionOut << endl << endl; 

  return motionOut;
}


//-----------------------------------------------------------------------------
vector<float> Motion::toVector() const
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






  //--------------------//
 //  DecomposedMotion  //
//--------------------//

//-----------------------------------------------------------------------------
DecomposedMotion::DecomposedMotion() : 
  Motion(),
  _shear( AimsData<float>( 3, 3 ) ),
  _scaling( AimsData<float>( 3, 3 ) ),
  _rot( AimsData<float>( 3, 3 ) )
			
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
DecomposedMotion::DecomposedMotion( const DecomposedMotion& other ) :
  RCObject(), Motion( other ),
  _shear( other._shear.clone() ),
  _scaling( other._scaling.clone() ),
  _rot ( other._rot.clone() )
{
}


//-----------------------------------------------------------------------------
DecomposedMotion &
DecomposedMotion::operator =( const DecomposedMotion& other )
{
  Motion::operator =( other );
  _scaling = (other._scaling).clone();
  _shear = (other._shear).clone();
  _rot = (other._rot).clone();
  return *this;
}


//-----------------------------------------------------------------------------
Motion &DecomposedMotion::operator =( const Motion & )
{
  throw runtime_error( "Affectation of a Motion to a DecomposedMotion not "
                       "implemented" );
}


//-----------------------------------------------------------------------------
void DecomposedMotion::setToIdentity()
{
  Motion::setToIdentity();
  _scaling(0,0) = _scaling(1,1) = _scaling(2,2) = 1. ;
  _shear(0,0) = _shear(1,1) = _shear(2,2) = 1. ;
  _rot(0,0) = _rot(1,1) = _rot(2,2) = 1. ;
}


//-----------------------------------------------------------------------------
void DecomposedMotion::scale( const Point3df &, const Point3df & )
{
  throw runtime_error( "scale() not implemented for DecomposedMotion" );
}


//-----------------------------------------------------------------------------
void setRotationAffine( float, float, float, 
                        const Point3df & )
{
  throw runtime_error( "setRotationAffine() not implemented for "
                       "DecomposedMotion" );
}


//-----------------------------------------------------------------------------
void DecomposedMotion::setShearing(float Cx, float Cy, float Cz )
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
void DecomposedMotion::setScaling(float Sx, float Sy, float Sz )
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
void DecomposedMotion::transAffine(Point3df cg) 
{
  AimsData<float> tmp_sc (4,4), tmp_sh (4,4), tmp_rt(4,4), mat (4,4);

  tmp_sc = 0.0;
  tmp_sc (3,3) = 1.0;  
  tmp_sc (0,0) = scaling() (0,0);
  tmp_sc (0,1) = scaling() (1,0);
  tmp_sc (0,2) = scaling() (2,0);
  tmp_sc (1,0) = scaling() (0,1);
  tmp_sc (1,1) = scaling() (1,1);
  tmp_sc (1,2) = scaling() (2,1);
  tmp_sc (2,0) = scaling() (0,2);
  tmp_sc (2,1) = scaling() (1,2);
  tmp_sc (2,2) = scaling() (2,2);

//  cout<<"scale = "<<endl<<tmp_sc<<endl;

  tmp_sh = 0.0;
  tmp_sh (3,3) = 1.0;  
  tmp_sh (0,0) = shearing() (0,0);
  tmp_sh (0,1) = shearing() (1,0);
  tmp_sh (0,2) = shearing() (2,0);
  tmp_sh (1,0) = shearing() (0,1);
  tmp_sh (1,1) = shearing() (1,1);
  tmp_sh (1,2) = shearing() (2,1);
  tmp_sh (2,0) = shearing() (0,2);
  tmp_sh (2,1) = shearing() (1,2);
  tmp_sh (2,2) = shearing() (2,2);

//  cout<<"shearing = "<<endl<<tmp_sh<<endl;

  tmp_rt = 0.0;
  tmp_rt (3,3) = 1.0;  
  tmp_rt (0,0) = rot() (0,0);
  tmp_rt (0,1) = rot() (1,0);
  tmp_rt (0,2) = rot() (2,0);
  tmp_rt (1,0) = rot() (0,1);
  tmp_rt (1,1) = rot() (1,1);
  tmp_rt (1,2) = rot() (2,1);
  tmp_rt (2,0) = rot() (0,2);
  tmp_rt (2,1) = rot() (1,2);
  tmp_rt (2,2) = rot() (2,2);


//  cout<<"rot = "<<endl<<tmp_rt<<endl;


  
  mat = tmp_sh.cross(tmp_sc.cross(tmp_rt));


//  cout<<"mat = "<<endl<<mat<<endl;


  rotation()(0,0) = mat(0,0);
  rotation()(0,1) = mat(1,0);
  rotation()(0,2) = mat(2,0);
  rotation()(1,0) = mat(0,1);
  rotation()(1,1) = mat(1,1);
  rotation()(1,2) = mat(2,1);
  rotation()(2,0) = mat(0,2);
  rotation()(2,1) = mat(1,2);
  rotation()(2,2) = mat(2,2);

  mat(0,0) = rotation()(0,0);
  mat(0,1) = rotation()(0,1);
  mat(0,2) = rotation()(0,2);
  mat(1,0) = rotation()(1,0);
  mat(1,1) = rotation()(1,1);
  mat(1,2) = rotation()(1,2);
  mat(2,0) = rotation()(2,0);
  mat(2,1) = rotation()(2,1);
  mat(2,2) = rotation()(2,2);


  AimsData<float> T(4,4), Tmoins1(4,4);
  T = 0.0; Tmoins1 = 0.0;

  T(3,0) = -cg.item(0); Tmoins1(3,0) = cg.item(0);
  T(3,1) = -cg.item(1); Tmoins1(3,1) = cg.item(1);
  T(3,2) = -cg.item(2); Tmoins1(3,2) = cg.item(2);
  T(0,0) = T(1,1) = T(2,2) = T(3, 3) = 1.0;
  Tmoins1(0,0) = Tmoins1(1,1) = Tmoins1(2,2) = Tmoins1(3,3) = 1.0;
  


  AimsData<float> tmp = T.cross( mat.cross( Tmoins1 ) ); //Produit transpos

/*  rotation()(0,0) = tmp(0,0);  //Transpo
  rotation()(0,1) = tmp(1,0);
  rotation()(0,2) = tmp(2,0);
  rotation()(1,0) = tmp(0,1);
  rotation()(1,1) = tmp(1,1);
  rotation()(1,2) = tmp(2,1);
  rotation()(2,0) = tmp(0,2);
  rotation()(2,1) = tmp(1,2);
  rotation()(2,2) = tmp(2,2);*/

  translation().item(0) += tmp(3,0);
  translation().item(1) += tmp(3,1);
  translation().item(2) += tmp(3,2);

}   


//-----------------------------------------------------------------------------
namespace aims
{

void transformBoundingBox( const Motion &motion, const Point3df & pmin1,
		const Point3df & pmax1, Point3df & pmin2, Point3df & pmax2 )
{
  pmin2 = pmax2 = motion.transform( pmin1 );
  Point3df	p = Point3df( pmax1[0], pmin1[1], pmin1[2] ), p2;
  p2 = motion.transform( p );
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
  p2 = motion.transform( p );
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
  p2 = motion.transform( p );
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
  p2 = motion.transform( p );
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
  p2 = motion.transform( p );
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
  p2 = motion.transform( p );
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
  p2 = motion.transform( p );
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
