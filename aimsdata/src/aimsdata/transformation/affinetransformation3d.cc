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
	
 # then, an AffineTransformation3d should contain the following attributes:
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
#include <aims/transformation/affinetransformation3d.h>
#include <aims/resampling/quaternion.h>
#include <aims/vector/vector.h>
#include <aims/data/pheader.h>
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
  : AffineTransformation3dBase(),
    _header( new PythonHeader )
{
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
  AffineTransformation3dBase( other ),
  _header( other.header() ? new PythonHeader( *other.header() )
    : new PythonHeader )
{
}


//-----------------------------------------------------------------------------
AffineTransformation3d::AffineTransformation3d(
  const AffineTransformation3dBase& other ) :
  RCObject(),
  AffineTransformation3dBase( other ),
  _header( new PythonHeader )
{
}


//-----------------------------------------------------------------------------
AffineTransformation3d::AffineTransformation3d( const vector<float> & mat ) :
    AffineTransformation3dBase( mat ), _header( new PythonHeader )
{
}


//-----------------------------------------------------------------------------
AffineTransformation3d::AffineTransformation3d( const Object mat ) :
    AffineTransformation3dBase( mat ), _header( new PythonHeader )
{
}


//-----------------------------------------------------------------------------
AffineTransformation3d::AffineTransformation3d( const Quaternion & quat ) :
    AffineTransformation3dBase(), _header( new PythonHeader )
{
  *this = quat;
}


//-----------------------------------------------------------------------------
AffineTransformation3d& AffineTransformation3d::operator = (
  const AffineTransformation3d& other )
{
  if( &other == this )
    return *this;
  AffineTransformation3dBase::operator = ( other );
  delete _header;
  if( other.header() )
    _header = new PythonHeader( *other.header() );
  else
    _header = new PythonHeader;
  return *this;
}


//-----------------------------------------------------------------------------
AffineTransformation3d& AffineTransformation3d::operator = (
  const vector<float> & mat )
{
  AffineTransformation3dBase::operator = ( mat );
  return *this;
}


//-----------------------------------------------------------------------------
AffineTransformation3d& AffineTransformation3d::operator = ( const Object mat )
{
  AffineTransformation3dBase::operator = ( mat );
  return *this;
}


//-----------------------------------------------------------------------------
AffineTransformation3d& AffineTransformation3d::operator = (
  const Quaternion & quat )
{
  AimsVector<float,16>  mat = quat.rotationMatrix();
  _matrix[0] = mat[0];
  _matrix[4] = mat[1];
  _matrix[8] = mat[2];
  _matrix[12] = mat[3];
  _matrix[1] = mat[4];
  _matrix[5] = mat[5];
  _matrix[9] = mat[6];
  _matrix[13] = mat[7];
  _matrix[2] = mat[8];
  _matrix[6] = mat[9];
  _matrix[10] = mat[10];
  _matrix[14] = mat[11];
  _matrix[3] = 0.F;
  _matrix[7] = 0.F;
  _matrix[11] = 0.F;
  _matrix[15] = 1.F;
  return *this;
}


//-----------------------------------------------------------------------------
bool AffineTransformation3d::operator == (
  const AffineTransformation3d & other ) const
{
  return AffineTransformation3dBase::operator == ( other );
}


bool AffineTransformation3d::operator != ( const AffineTransformation3d & other ) const
{
  return !this->operator==(other);
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
AffineTransformation3d AffineTransformation3d::inverse() const
{
//   AffineTransformation3d AffineTransformation3d;
//
//   AffineTransformation3d.setMatrix(AimsInversionLU( rotation() ));
//
//   Point3df tmp( -translation() );
//   AffineTransformation3d.matrix()( 0, 3 ) = AffineTransformation3d.rotation()( 0, 0 ) * tmp.item( 0 ) +
//                                    AffineTransformation3d.rotation()( 0, 1 ) * tmp.item( 1 ) +
//                                    AffineTransformation3d.rotation()( 0, 2 ) * tmp.item( 2 ) ;
//   AffineTransformation3d.matrix()( 1, 3 ) = AffineTransformation3d.rotation()( 1, 0 ) * tmp.item( 0 ) +
//                                    AffineTransformation3d.rotation()( 1, 1 ) * tmp.item( 1 ) +
//                                    AffineTransformation3d.rotation()( 1, 2 ) * tmp.item( 2 ) ;
//   AffineTransformation3d.matrix()( 2, 3 ) = AffineTransformation3d.rotation()( 2, 0 ) * tmp.item( 0 ) +
//                                    AffineTransformation3d.rotation()( 2, 1 ) * tmp.item( 1 ) +
//                                    AffineTransformation3d.rotation()( 2, 2 ) * tmp.item( 2 ) ;
//
//   return AffineTransformation3d;

  AffineTransformation3d inv( AffineTransformation3dBase::inverse() );
  inv.header()->copy( *header() );

  Object s;
  PythonHeader *hdr = inv.header();
  if( hdr->hasProperty( "source_referential" ) )
  {
    s = hdr->getProperty( "source_referential" );
    hdr->removeProperty( "source_referential" );
  }
  if( hdr->hasProperty( "destination_referential" ) )
  {
    hdr->setProperty( "source_referential",
                      hdr->getProperty( "destination_referential" ) );
    hdr->removeProperty( "destination_referential" );
  }
  if( s )
    hdr->setProperty( "destination_referential", s );

  return inv;
}

unique_ptr<soma::Transformation3d> AffineTransformation3d::getInverse() const
{
  return unique_ptr<soma::Transformation3d>(new AffineTransformation3d(inverse()));
}


//-----------------------------------------------------------------------------
void AffineTransformation3d::setRotationAffine( float rx, float ry, float rz,
                                const Point3df &cg )
/* NOTE: this method is UNCLEAR and SUBOPTIMAL.

Currently this methid destroys any already specified translation.
We could avoid this side effect with
  matrix()(0, 3) += tmp(3,0);
  matrix()(1, 3) += tmp(3,1);
  matrix()(2, 3) += tmp(3,2);
in the current code but it would be better to follow specification in NOTE,
line 38. */
{
  
  VolumeRef<float> Rx = rotationaroundx(rx);
  VolumeRef<float> Ry = rotationaroundy(ry);
  VolumeRef<float> Rz = rotationaroundz(rz);
  //Product transpose!!
  VolumeRef<float> vectRot = matrix_product( Rz, matrix_product( Ry, Rx ));


  VolumeRef<float> T(4,4), Tmoins1(4,4);
  T = 0.0; Tmoins1 = 0.0;


  T(3,0) = -cg.item(0); Tmoins1(3,0) = cg.item(0);
  T(3,1) = -cg.item(1); Tmoins1(3,1) = cg.item(1);
  T(3,2) = -cg.item(2); Tmoins1(3,2) = cg.item(2);
  T(0,0) = T(1,1) = T(2,2) = T(3, 3) = 1.0;
  Tmoins1(0,0) = Tmoins1(1,1) = Tmoins1(2,2) = Tmoins1(3,3) = 1.0;
  
  //Product transpos
  VolumeRef<float> tmp = matrix_product(T,
                                        matrix_product( vectRot, Tmoins1 ) );
  _matrix[12] = tmp(3,0);
  _matrix[13] = tmp(3,1);
  _matrix[14] = tmp(3,2);

  VolumeRef<float> m = affine();
  m->at(0,0) = tmp(0,0);  //Transpo
  m->at(0,1) = tmp(1,0);
  m->at(0,2) = tmp(2,0);
  m->at(1,0) = tmp(0,1);
  m->at(1,1) = tmp(1,1);
  m->at(1,2) = tmp(2,1);
  m->at(2,0) = tmp(0,2);
  m->at(2,1) = tmp(1,2);
  m->at(2,2) = tmp(2,2);
}


//-----------------------------------------------------------------------------
void AffineTransformation3d::setMatrix(const carto::VolumeRef<float> & mat)
{
  VolumeRef<float> m = affine();

  for(int16_t i=0; i<3; i++)
    for(int16_t j=0; j<3; j++)
      m->at(i, j) = mat->at(i, j);
}

//-----------------------------------------------------------------------------
VolumeRef<float> AffineTransformation3d::rotationaroundx(float rx)
/* this method return a 4-by-4 matrix, but a 3-by-3 matrix would be
enough and better. see NOTE, line 38. */
{
  VolumeRef<float> thing( 4,4 );
  thing.fill( 0.0 );
  thing->at(3,3) = 1.0;
  
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

  return thing;
}

//-----------------------------------------------------------------------------
VolumeRef<float> AffineTransformation3d::rotationaroundy(float ry)
/* this method return a 4-by-4 matrix, but a 3-by-3 matrix would be
enough and better. see NOTE, line 38. */
{
  VolumeRef<float> thing( 4,4 );
  thing->fill( 0.0 );
  thing(3,3) = 1.0;

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

  return thing;
}

//-----------------------------------------------------------------------------
VolumeRef<float> AffineTransformation3d::rotationaroundz(float rz)
/* this method return a 4-by-4 matrix, but a 3-by-3 matrix would be
enough and better. see NOTE, line 38. */
{
  VolumeRef<float> thing( 4,4 );
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

  return thing;
}


AffineTransformation3d & AffineTransformation3d::operator *= ( const AffineTransformation3dBase & m )
{
  AffineTransformation3dBase::operator *= ( m );
  return( *this );
}


AffineTransformation3d AffineTransformation3d::operator - () const
{
  AffineTransformation3dBase t = AffineTransformation3dBase::operator - ();
  return AffineTransformation3d( t );
}


//-----------------------------------------------------------------------------
namespace aims
{

AffineTransformation3d operator * (
  const AffineTransformation3d& AffineTransformation3d1, 
  const AffineTransformation3d& AffineTransformation3d2 )
{
  AffineTransformation3dBase out
    = operator * ( static_cast<const AffineTransformation3dBase &>(
        AffineTransformation3d1 ),
      static_cast<const AffineTransformation3dBase &>(
        AffineTransformation3d2 ) );
  return AffineTransformation3d( out );
}

}

//-----------------------------------------------------------------------------
namespace aims
{

void transformBoundingBox(
  const AffineTransformation3d &AffineTransformation3d, const Point3df & pmin1,
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
