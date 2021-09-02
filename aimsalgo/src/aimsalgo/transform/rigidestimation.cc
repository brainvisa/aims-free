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


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/transform/rigidestimation.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/math/eigen.h>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;

RigidTransformEstimation::RigidTransformEstimation() :
  _looseCondition(true), _motionCalculated(true), _is2D(false)
{
  _motion = new AffineTransformation3d;
}

RigidTransformEstimation::RigidTransformEstimation(
  const vector<Point3df> & from, const vector<Point3df> & to,
  bool looseCondition ) :
  _pointsFrom(from), _pointsTo(to), _looseCondition(looseCondition), 
  _motionCalculated(false), _is2D(false)
{
  _motion = new AffineTransformation3d;
}


RigidTransformEstimation::~RigidTransformEstimation()
{
  delete _motion ;
}

void  
RigidTransformEstimation::setAppariatedPoints(
  const vector<Point3df> & from, const vector<Point3df> & to )
{
  _pointsFrom = from ;
  _pointsTo = to ;
  _motionCalculated = false ;
}

void 
RigidTransformEstimation::setLooseCondition( bool looseCondition )
{
  _looseCondition = looseCondition ;
  _motionCalculated = false ;
}

bool 
RigidTransformEstimation::motion( AffineTransformation3d & motion )
{
  if ( _motionCalculated )
  {
    motion = *_motion ; 
    return true ;
  }

  *_motion = AffineTransformation3d() ;
  if( _pointsFrom.size() < 1 || _pointsTo.size() < 1 )
    return 0 ;
  _motion->setTranslation( *(_pointsTo.begin() ) - *(_pointsTo.end() ) ) ;
    
  if( _pointsFrom.size() < 3 || _pointsTo.size() < 3 )
    return 1 ;
  
  if ( _looseCondition )
    looseEstimation() ;
  else
    pointToPointEstimation() ;
  
  _motionCalculated = true ;
  motion = *_motion ;

  return 1 ;
}  

void
RigidTransformEstimation::looseEstimation()
{
  cout << "Loose Estimation" << endl ;
  Point3df v1, v2, nFrom, nTo, axis ;
  float angle ;
  
  v1 = _pointsFrom[1] - _pointsFrom[0] ;
  v2 = _pointsFrom[2] - _pointsFrom[0] ;
  
  nFrom = cross( v1, v2 ) ;
  
  if( nFrom[0] == 0. && nFrom[1] == 0. && nFrom[2] == 0. )
    {
      cerr << "Both vectors should be colinear" << endl ;
      return ;
    }
  nFrom.normalize() ;

  v1 = _pointsTo[1] - _pointsTo[0] ;
  v2 = _pointsTo[2] - _pointsTo[0] ;

  nTo = cross(v1, v2 ) ;
  
  
  if( nTo[0] == 0. && nTo[1] == 0. && nTo[2] == 0. )
    {
      cerr << "Both vectors should be colinear" << endl ;
      return ;
    }
  nTo.normalize() ;

  v1 = _pointsFrom[1] - _pointsFrom[0] ;
  v2 = _pointsTo[1] - _pointsTo[0] ;
  
  
  v1.normalize() ;
  v2.normalize() ;
  cout << "\tv1 = " << v1 << "\tv2 = " << v2 << endl ;
  cout << "\tnFrom = " << nFrom << "\tnTo = " << nTo << endl ;
  
  

  rotationEstimation(v1, v2, nFrom, nTo, axis, angle ) ;
  
  Point3df n1( nFrom - axis.dot(nFrom)* axis ), n2( nTo - axis.dot(nTo)* axis ) ;
  n1.normalize() ; n2.normalize() ;
  
  cout << "\t axis = " << axis << "\tangle v1-v2 : " << angle 
       << "\tangle n1-n2 : " 
       << acos( ( n1.dot( n2 ) < -0.9999999 ? 0. : (n1.dot( n2 ) > .9999999 ? 1. : n1.dot( n2 ) ) ) ) <<endl ;
  
  
  float c = cos(angle), s = sin(angle), t = 1- cos(angle) ;
  
  _motion->rotation()(0, 0) = t * axis[0] * axis[0] + c ;
  _motion->rotation()(0, 1) = t * axis[0] * axis[1] - s * axis[2] ;
  _motion->rotation()(0, 2) = t * axis[0] * axis[2] + s * axis[1] ;
  _motion->rotation()(1, 0) = t * axis[0] * axis[1] + s * axis[2] ;
  _motion->rotation()(1, 1) = t * axis[1] * axis[1] + c ;
  _motion->rotation()(1, 2) = t * axis[1] * axis[2] - s * axis[0] ;
  _motion->rotation()(2, 0) = t * axis[0] * axis[2] - s * axis[1] ;
  _motion->rotation()(2, 1) = t * axis[1] * axis[2] + s * axis[0] ;
  _motion->rotation()(2, 2) = t * axis[2] * axis[2] + c ;
  
  _motion->setTranslation(Point3df(0.));
  Point3df gravCenter((float).5 * (_pointsFrom[0] + _pointsFrom[1]));
  
  _motion->setTranslation((float).5 * (_pointsTo[0] + _pointsTo[1]) 
    - _motion->transform(gravCenter[0], gravCenter[1], gravCenter[2]));
}


void 
RigidTransformEstimation::rotationEstimation(
  const Point3df& v1, const Point3df& v2,
  const Point3df& n1, const Point3df& n2,
  Point3df& axis, float& angle )
{
  Point3df vsum, vvect, vsumvectvvect, nsum, nvect, nsumvectnvect ;
  vsum = v1 + v2 ;
  vvect = cross( v1, v2 ) ;
  
  cout << "\tvsum = " << vsum << "\tvvect = " << vvect << endl ;

  nsum = n1 + n2 ;
  nvect = cross(n1, n2) ;
  
  cout << "\tnsum = " << nsum << "\tnvect = " << nvect << endl ;
  
  // cas particulier de vsum = 0 ou n sum = 0 a traiter separement.
  
  vsumvectvvect = cross( vsum, vvect ) ;
  nsumvectnvect = cross( nsum, nvect ) ;

  cout << "\tvsumvectvvect = " << vsumvectvvect << "\tnsumvectnvect = " << nsumvectnvect << endl ;
  
  // L'axe de rotation doit etre orthogonal a vsumvectvvect et a nsumvectnvect
  float zTox, zTox2, zToy, zToy2, xToz, xToz2, yToz, yToz2 ;
  
  zToy = -( vsumvectvvect[0]*nsumvectnvect[2] - vsumvectvvect[2]*nsumvectnvect[0] ) ;
  yToz = vsumvectvvect[0]*nsumvectnvect[1] - vsumvectvvect[1]*nsumvectnvect[0] ;
  xToz = yToz * nsumvectnvect[0] ;
  zTox = - ( nsumvectnvect[1] * zToy + nsumvectnvect[2] * yToz ) ;
  

  cout << "\tyToz = " << yToz << "\txToz = " << xToz << "\tzToy = " << zToy << "\tzTox = " << zTox << endl ;

  zTox2 = zTox*zTox ; zToy2 = zToy*zToy ;
  xToz2 = xToz*xToz ; yToz2 = yToz*yToz ;

  if ( zTox != 0. || xToz != 0. || zToy != 0. || yToz != 0. ){
    if( xToz == 0. || yToz == 0. ) {
      axis[2] = 0. ;

      axis[1] = vsumvectvvect[0] * vsumvectvvect[0] / ( vsumvectvvect[0] * vsumvectvvect[0] + 
							vsumvectvvect[1] * vsumvectvvect[1] ) ;
      axis[0] = vsumvectvvect[1] * vsumvectvvect[1] / ( vsumvectvvect[0] * vsumvectvvect[0] + 
							vsumvectvvect[1] * vsumvectvvect[1] ) ;
    }
    else {
      axis[2] = xToz2 * yToz2 / 
	( zTox2 *yToz2 + zToy2*xToz2 + xToz2*yToz2) ;
      
      axis[0] = zTox / xToz * axis[2] ;
      axis[1] = zToy / yToz * axis[2] ;
    }

    if( axis != Point3df(0., 0., 0.) )
      axis.normalize() ;
    
    Point3df u1( v1 - axis.dot(v1)* axis ), u2( v2 - axis.dot(v2)* axis ) ;
    
    if( cross( axis, vsum ) == Point3df(0., 0., 0.) )
      angle = M_PI ;
    else{
      u1.normalize() ; u2.normalize() ;
      angle = acos( ( u1.dot( u2 ) < -0.9999999 ? 0. : (u1.dot( u2 ) > .9999999 ? 1. : u1.dot( u2 ) ) ) ) ;
      cout << endl << endl 
	   << "U1 : " << u1 << "\tU2 : " << u2 << "\tAngle : " << angle << endl << endl ;
    }
  } else {
    // v1 colinear to v2
    
    if( cross(v1, v2) == Point3df(0., 0., 0.) ){
      if( v1.dot(v2) > 0. ) 
	axis = v1 ;
      else
	axis = -v1 ;
      
      Point3df u1( n1 - axis.dot(n1)* axis ), u2( n2 - axis.dot(n2)* axis ) ;
      angle = acos( ( u1.dot( u2 ) < -0.9999999 ? 0. : (u1.dot( u2 ) > .9999999 ? 1. : u1.dot( u2 )))) ;
    }else{
      if( cross( n1, n2 ) == Point3df(0., 0., 0.) ){
	if( n1.dot(n2) > 0. ) 
	  axis = n1 ;
	else
	  axis = -n1 ;
      }
      
      Point3df u1( v1 - axis.dot(v1)* axis ), u2( v2 - axis.dot(v2)* axis ) ;
      angle = acos( ( u1.dot( u2 ) < -0.9999999 ? 0. : (u1.dot( u2 ) > .9999999 ? 1. : u1.dot( u2 ) ) ) ) ;
    }
    axis.normalize() ;
  }  
  
  if ( axis.dot( cross(v1, v2) ) < 0 )
    angle = -angle ;
  
  
//   if ( axe != Point3df(0., 0., 0.) )
//     axe.normalize() ;
  
//   // DEBUG
//   Point3df axe( - _motion->rotation()(1, 2) + _motion->rotation()(2, 1),
// 		- _motion->rotation()(2, 0) + _motion->rotation()(0, 2),
// 		- _motion->rotation()(0, 1) + _motion->rotation()(1, 0) ) ;
  
  
//   ang = acos( ( _motion->rotation()(0, 0) + _motion->rotation()(1, 1) + _motion->rotation()(2, 2) - 1. ) /2. ) ;

//   if( axe[0] != 0. )
//     ang = ( (_motion->rotation()(1, 2) - _motion->rotation()(2, 1) )/axis[0] <= 0. ? ang : -ang ) ;
//   else if( axe[1] != 0. )
//     ang = ( ( _motion->rotation()(2, 0) - _motion->rotation()(0, 2) )/axis[1] <= 0. ? ang : -ang ) ;
//   else
//     ang = ( ( _motion->rotation()(0, 1) - _motion->rotation()(1, 0) )/axis[2] <= 0. ? ang : -ang ) ;  
}

void 
RigidTransformEstimation::pointToPointEstimation()
{
  cout << "Point to Point Estimation" << endl ;
  // Initialization
  vector<Point3df>::iterator iterFrom(_pointsFrom.begin()), 
    lastFrom(_pointsFrom.end()), 
    iterTo(_pointsTo.begin()), lastTo(_pointsTo.end()) ;
  

  int size = 0 ;

  Point3df gcFrom(0., 0., 0.), gcTo(0., 0., 0.), translation, f, t;
  _is2D=true;		//test sur la condition 2D en z
  while( ( iterFrom != lastFrom ) && ( iterTo != lastTo ) ){
    gcFrom += *iterFrom ;
    gcTo += *iterTo ;

	 f = *iterFrom ;
	 t = *iterTo ;

	 if( f[2] != 0 || t[2] != 0 )	
	 {
		_is2D=false;
	 }
    
    ++iterFrom ;
    ++iterTo ;
    ++size ;
  }
  gcFrom /= size ;
  gcTo /= size ;
  
  VolumeRef<float> criterionMatrix(4, 4) ;
  VolumeRef<float> eigenValues(4, 4) ;

  // Weight caculated to take into account the distance between a point and the rotation center.
  float rotWeight, errorWeight ; 

  // Weight taking into account the error committed form the point during previous itaration
  //float weightSum = 0. ;
  bool firstPass = true ;
  float d = 1. ; float dMax ;
  double dSum = 0; // (unused) , dOlderSum = -1 ;
  Point3df errorVect ;
  VolumeRef<float> critItem ;

  for( int iteration = 0 ; iteration < 1 ; ++iteration )
  {
    iterFrom = _pointsFrom.begin() ; iterTo = _pointsTo.begin() ;
    criterionMatrix = 0. ;
    
    while ( iterFrom != lastFrom && iterTo != lastTo ) {
      if( !firstPass ){
	errorVect = *iterTo - _motion->transform( (*iterFrom)[0], (*iterFrom)[1], (*iterFrom)[2] ) ;
	d = sqrt ( errorVect.dot(errorVect) ) ;
      }
      
      rotWeight = /*(*iterFrom - gcFrom ).dot(*iterFrom - gcFrom)*/ 1. ;
      errorWeight = /*( firstPass ? 1. : (dMax + 1. - d ) )*/ 1. ;
      
      //      weightSum += rotWeight * ( firstPass ? 1. : (dMax + 1. - d ) ) ;
      
      critItem = criterionItem( *iterFrom, *iterTo, gcFrom, gcTo, 
                                rotWeight * errorWeight ) ;
      
      for( int i = 0 ; i < 4 ; ++i )
      {
        cout << endl ;
        for ( int j = 0 ; j < 4 ; ++j )
        {
          criterionMatrix(i, j) = criterionMatrix(i, j) - critItem(i, j) ;
          cout << "\t" << criterionMatrix(i, j) ;
        }
      }
      ++iterFrom ; ++iterTo ;
      cout << endl<< endl ;
    }
    
    VolumeRef<float> eigenvectors( criterionMatrix ), eigenvalues ;
    
    AimsEigen<float> aimsEigen;
    eigenvalues = aimsEigen.doit(eigenvectors);
    aimsEigen.sort(eigenvectors, eigenvalues);
    
    Point4df q;
    for(int i =0; i<4;i++)
    {
      q[i] = eigenvectors(i,3) ;
    }
    
    q.normalize() ;
    
    Point3df axis( q[1], q[2], q[3] ) ;
    //on trouve le signe correct sachant que theta petit
    // (unused) double angle = 2 * acos( q[0] ) ;
    float cosa = q[0], sina = axis.norm(), c = 2* cosa * cosa - 1, s = 2 * cosa * sina, 
      t = 1 - c;

    if( axis != Point3df(0., 0., 0. ) )
      axis.normalize() ;
	
	 if(_is2D)
     {
       axis[0]=0;
       axis[1]=0;
    }
    
    _motion->rotation()(0, 0) = t * axis[0] * axis[0] + c ;
    _motion->rotation()(0, 1) = t * axis[0] * axis[1] - s * axis[2] ;
    _motion->rotation()(0, 2) = t * axis[0] * axis[2] + s * axis[1] ;
    _motion->rotation()(1, 0) = t * axis[0] * axis[1] + s * axis[2] ;
    _motion->rotation()(1, 1) = t * axis[1] * axis[1] + c ;
    _motion->rotation()(1, 2) = t * axis[1] * axis[2] - s * axis[0] ;
    _motion->rotation()(2, 0) = t * axis[0] * axis[2] - s * axis[1] ;
    _motion->rotation()(2, 1) = t * axis[1] * axis[2] + s * axis[0] ;
    _motion->rotation()(2, 2) = t * axis[2] * axis[2] + c ;
    
    _motion->setTranslation(0.);
    _motion->setTranslation(gcTo - _motion->transform( gcFrom[0], gcFrom[1], gcFrom[2] )) ;
	 if(_is2D)	_motion->translation()[2]=0;

    
//     cout << "Point to Point estimated angle : " << angle << " and axis : " << axis << endl ;

    int count = 0;
    dMax = 0. ;
    iterFrom = _pointsFrom.begin() ; iterTo = _pointsTo.begin() ;

    while ( iterFrom != lastFrom && iterTo != lastTo ) {
      errorVect = *iterTo - _motion->transform( (*iterFrom)[0], (*iterFrom)[1], (*iterFrom)[2] ) ;
      d = sqrt ( errorVect.dot(errorVect) ) ;      
      if (d > dMax)
	dMax = d ;
      
      dSum += (double)d ;
      
      ++count ;
      ++iterFrom ; ++iterTo ;
    }
    
    cout << "\tMean error : " << dSum / double(count) << endl ;
  }
}


Point3df
RigidTransformEstimation::cross( const Point3df& u, const Point3df& v )
{
  Point3df result ;
  
  result[0] = u[1]*v[2] - v[1]*u[2] ;
  result[1] = -u[0]*v[2] + v[0]*u[2] ;
  result[2] = u[0]*v[1] - v[0]*u[1] ;

  return result ;
}

VolumeRef<float>
RigidTransformEstimation::criterionItem(
  const Point3df& p1, const Point3df& p2,
  const Point3df& gc1, const Point3df& gc2, float weight )
{
  VolumeRef<float> a(4, 4) ;
  
  Point3df dep( weight * ( (p2 - gc2) - (p1 - gc1) ) ), add( weight * ( (p1 - gc1) + (p2 - gc2) ) )   ;
  
  a = 0. ; 	
  			a( 1, 0 ) = -dep[0] ;	a( 2, 0 ) = -dep[1] ;	a( 3, 0 ) = -dep[2] ;	
  a( 0, 1 ) = dep[0] ;				a( 2, 1 ) = -add[2] ;	a( 3, 1 ) = add[1] ;	
  a( 0, 2 ) = dep[1] ;	a( 1, 2 ) = add[2] ;				a( 3, 2 ) = -add[0] ;	
  a( 0, 3 ) = dep[2] ;	a( 1, 3 ) = -add[1] ;	a( 2, 3 ) = add[0] ;	

  return ( matrix_product( a, a ) );
}
