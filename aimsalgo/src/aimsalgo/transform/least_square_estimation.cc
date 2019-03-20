/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#include <aims/math/svd.h>
#include <aims/math/eigen.h>
#include <aims/transform/least_square_estimation.h>
#ifdef AIMS_USE_MPI
#include <mpi.h>
#endif

using namespace std ;
using namespace carto ;
using namespace aims ;
#ifdef AIMS_USE_MPI
using namespace MPI;
#endif

TransformLeastSquareEstimation::TransformLeastSquareEstimation(
          const std::vector<Point3df>& from, 
          const std::vector<Point3df>& to ) :
  _pointsFrom(from), _pointsTo(to), 
  _motion(0), _motionCalculated(false), _is2D(false)
{

}


TransformLeastSquareEstimation::~TransformLeastSquareEstimation()
{
  if( _motion )
    delete _motion ;
}



AffineLeastSquareEstimation::AffineLeastSquareEstimation(
          const std::vector<Point3df>& from, 
          const std::vector<Point3df>& to) : TransformLeastSquareEstimation(
          from, to )
{
  
}


RigidLeastSquareEstimation::RigidLeastSquareEstimation(
          const std::vector<Point3df>& from, 
          const std::vector<Point3df>& to) : TransformLeastSquareEstimation(
          from, to )
{
  
}


SimiLeastSquareEstimation::SimiLeastSquareEstimation(
          const std::vector<Point3df>& from, 
          const std::vector<Point3df>& to) : RigidLeastSquareEstimation(
          from, to )
{
  
}




bool 
AffineLeastSquareEstimation::computeMotion()
{
  if( _pointsFrom.size() != _pointsTo.size() )
    throw runtime_error("AffineLeastSquareEstimation:Both vectors must have same size !") ;
  unsigned size = _pointsFrom.size();
  AimsData<float> xx(3, 3), yx(3, 3) ;
  AimsData<float> x(_pointsFrom.size(), 3), y(_pointsTo.size(), 3) ;		// contient les coords Bary
  Point3df meanX(0., 0., 0.), meanY(0., 0., 0.) ;
  unsigned nn = 0; unsigned limit = size;
  for( ; nn < limit ; ++nn ) {
    meanX += _pointsFrom[nn] ;
    meanY += _pointsTo[nn] ;
  }

  meanX *=  (1./ size) ;
  meanY *=  (1./ size) ;

  //Faux dans qqs cas rares : a changer, detecter des le main le cas 2D.
  if( (meanX[2]==0) && (meanY[2]==0) )	_is2D=true;
   
  for( unsigned n = 0 ; n < size ; ++n )
    for(unsigned d = 0 ; d < 3 ; ++d) {
      x(n, d) = _pointsFrom[n][d] - meanX[d] ;
      y(n, d) = _pointsTo[n][d] - meanY[d] ;
  }
  
  xx = x.clone().transpose().cross(x) ;
//  cout << "xx dims : " << xx.dimX() << ", " << xx.dimY() << endl ;
  yx = (y.clone().transpose()).cross(x) ;
// cout << "yx dims : " << yx.dimX() << ", " << yx.dimY() << endl ;


  // Dans le cas 2 D, on remplace la derniere composante par 1
  if(_is2D)
  {
    xx(2,2)=1;
    yx(2,2)=1;
  }
  
  AimsData< float > u = xx.clone(); // because it is replaced on SVD output
  AimsData< float > v( xx.dimY(), xx.dimY() );

  AimsSVD< float > svd2;
  AimsData< float > w = svd2.doit( u, &v );
  

  // Set the singular values lower than threshold to zero
  float s = 0.000000000000000000000000000001 ;
  float ts = s * w.maximum();
  int i, n = w.dimX();
  for ( i=0; i<n; i++ )
  {
    if ( w( i, i ) > ts )  w( i, i ) = 1.0f / w( i, i );
    else w( i, i ) = 0.0f;
  }
  
  AimsData<float> invXX = v.cross( w.cross(  u.transpose() ) ) ;
  _motion = new DecomposedMotion ;
  
  _motion->rotation() = yx.cross(invXX) ;
  _motion->setTranslation(meanY - _motion->transform(meanX));

  return 1;
}


bool 
RigidLeastSquareEstimation::computeRigidMotion()
{
  if( _pointsFrom.size() != _pointsTo.size() )
    throw runtime_error("RigideLeastSquareEstimation:Both vectors must have same size !") ;
  unsigned size = _pointsFrom.size();

  //cout << "RigidLeastSquareEstimation::computeRigidMotion - size : " << size << std::endl << std::flush;

  // calcul du barycentre
  Point3df meanX(0., 0., 0.), meanY(0., 0., 0.) ;
  unsigned n = 0;
  for(;n < size ; ++n ){
    meanX += _pointsFrom[n] ;
    meanY += _pointsTo[n] ;
  }

  meanX *=  (1./ size) ;
  meanY *=  (1./ size) ;

  _meanX = meanX;
  _meanY = meanY;


  //Faux dans qqs cas rares : changer, detecter des le main le cas 2D.
  if( (meanX[2]==0) && (meanY[2]==0) )	_is2D=true;
  
  
  AimsData<float> criterionMatrix(4, 4);
  AimsData<float> critItem ;
  criterionMatrix = 0;
  AimsData<float> eigenValues(4, 4);
  vector<Point3df>::iterator iterFrom(_pointsFrom.begin()), 
                             lastFrom(_pointsFrom.end()), 
                             iterTo(_pointsTo.begin()), lastTo(_pointsTo.end()) ;

#ifdef AIMS_USE_MPI
/* Parallel criterionMatrix computation. This is the important time-consuming process in all computeMotion */
  int rank = COMM_WORLD.Get_rank();
  int mpiSize = COMM_WORLD.Get_size();
  int nbPts = size/mpiSize;
  int nbPtsLeft = size%mpiSize;
  if(rank < nbPtsLeft) {
    iterFrom += rank*(nbPts+1);
    lastFrom = iterFrom + nbPts+1;
    iterTo += rank*(nbPts+1);
    lastTo = iterTo + nbPts+1;
  }
  else {
    iterFrom += rank*nbPts + nbPtsLeft;
    lastFrom = iterFrom + nbPts;
    iterTo += rank*nbPts + nbPtsLeft;
    lastTo = iterTo + nbPts;
  }
#endif

  while ( iterFrom != lastFrom && iterTo != lastTo ) {
          
    critItem = criterionItem( *iterFrom, *iterTo, meanX, meanY, 1. ) ;
    
    for ( int i = 0 ; i < 4 ; ++i ) {
      for ( int j = 0 ; j < 4 ; ++j ) {
        criterionMatrix(i, j) = criterionMatrix(i, j) - critItem(i, j) ;
      }
    }

    ++iterFrom; ++iterTo;
  }

#ifdef AIMS_USE_MPI
  COMM_WORLD.Allreduce(IN_PLACE, &criterionMatrix[0], 16, FLOAT, SUM);
  COMM_WORLD.Barrier();
#endif

  AimsData<float> eigenvectors( criterionMatrix ), eigenvalues ;
  
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
  float cosa = q[0], sina = axis.norm(), c = 2* cosa * cosa - 1, s = 2 * cosa * sina, 
        t = 1 - c;

  if( axis != Point3df(0., 0., 0. ) )
    axis.normalize() ;

  if(_is2D) { 
    axis[0]=0;axis[1]=0;
  }

  _motion = new DecomposedMotion ;   

  _motion->rotation()(0, 0) = t * axis[0] * axis[0] + c ;
  _motion->rotation()(0, 1) = t * axis[0] * axis[1] - s * axis[2] ;
  _motion->rotation()(0, 2) = t * axis[0] * axis[2] + s * axis[1] ;
  _motion->rotation()(1, 0) = t * axis[0] * axis[1] + s * axis[2] ;
  _motion->rotation()(1, 1) = t * axis[1] * axis[1] + c ;
  _motion->rotation()(1, 2) = t * axis[1] * axis[2] - s * axis[0] ;
  _motion->rotation()(2, 0) = t * axis[0] * axis[2] - s * axis[1] ;
  _motion->rotation()(2, 1) = t * axis[1] * axis[2] + s * axis[0] ;
  _motion->rotation()(2, 2) = t * axis[2] * axis[2] + c ;
    
  _motion->rot() = _motion->rotation().clone();

  _motion->setTranslation(Point3df(0.));
  _motion->setTranslation(meanY - _motion->transform(meanX));
  
  if(_is2D)
    _motion->translation()[2] = 0;

  return 1;
}



AimsData<float> 
RigidLeastSquareEstimation::criterionItem( const Point3df& p1, const Point3df& p2, 
					 const Point3df& gc1, const Point3df& gc2, float weight )
{
  AimsData<float> a(4, 4) ;
  
  Point3df dep( weight * ( (p2 - gc2) - (p1 - gc1) ) ), add( weight * ( (p1 - gc1) + (p2 - gc2) ) )   ;
  
  a = 0. ; 	
  a( 1, 0 ) = -dep[0] ;	a( 2, 0 ) = -dep[1] ;	a( 3, 0 ) = -dep[2] ;	
  a( 0, 1 ) = dep[0] ;	a( 2, 1 ) = -add[2] ;	a( 3, 1 ) = add[1] ;	
  a( 0, 2 ) = dep[1] ;	a( 1, 2 ) = add[2] ;		a( 3, 2 ) = -add[0] ;	
  a( 0, 3 ) = dep[2] ;	a( 1, 3 ) = -add[1] ;	a( 2, 3 ) = add[0] ;	

  return ( a.cross( a ) ) ;
}



double
RigidLeastSquareEstimation::error()
{
  Point3df errorVect ;
  double dSum = 0;
  int count = 0;
  double dMax = 0. ;
  vector<Point3df>::iterator iterFrom(_pointsFrom.begin()), 
                             lastFrom(_pointsFrom.end()), 
                             iterTo(_pointsTo.begin()), lastTo(_pointsTo.end()) ;
  float d = 1.;


  while ( iterFrom != lastFrom && iterTo != lastTo ) {
    errorVect = *iterTo - _motion->transform( (*iterFrom)[0], (*iterFrom)[1], (*iterFrom)[2] ) ;
    d = sqrt ( errorVect.dot(errorVect) ) ;      
    if (d > dMax)
      dMax = d ;
    
    dSum += (double)d ;
      
    ++count ;
    ++iterFrom ; ++iterTo ;
  }
    
  return (dSum / double(count));
}

bool 
TranslationLeastSquareEstimation::computeTranslationMotion()
{
  if( _pointsFrom.size() != _pointsTo.size() )
    throw runtime_error("TranslationLeastSquareEstimation:Both vectors must have same size !") ;
  size_t size = _pointsFrom.size();

  _motion->setTranslation(Point3df(0., 0., 0.));
  for( size_t n = 0 ; n < size ; ++n ){
    _motion->translation() += _pointsTo[n] - _pointsFrom[n] ;
  }
  _motion->translation() /= float(size) ;
  
  if(_is2D)	_motion->translation()[2]=0;


  return 1;
}



AimsData<float> 
TranslationLeastSquareEstimation::criterionItem( const Point3df& p1, const Point3df& p2, 
					 const Point3df& gc1, const Point3df& gc2, float weight )
{
  AimsData<float> a(4, 4) ;
  
  Point3df dep( weight * ( (p2 - gc2) - (p1 - gc1) ) ), add( weight * ( (p1 - gc1) + (p2 - gc2) ) )   ;
  
  a = 0. ; 	
  a( 1, 0 ) = -dep[0] ;	a( 2, 0 ) = -dep[1] ;	a( 3, 0 ) = -dep[2] ;	
  a( 0, 1 ) = dep[0] ;	a( 2, 1 ) = -add[2] ;	a( 3, 1 ) = add[1] ;	
  a( 0, 2 ) = dep[1] ;	a( 1, 2 ) = add[2] ;		a( 3, 2 ) = -add[0] ;	
  a( 0, 3 ) = dep[2] ;	a( 1, 3 ) = -add[1] ;	a( 2, 3 ) = add[0] ;	

  return ( a.cross( a ) ) ;
}



double
TranslationLeastSquareEstimation::error()
{
  Point3df errorVect ;
  double dSum = 0;
  int count = 0;
  double dMax = 0. ;
  vector<Point3df>::iterator iterFrom(_pointsFrom.begin()), 
                             lastFrom(_pointsFrom.end()), 
                             iterTo(_pointsTo.begin()), lastTo(_pointsTo.end());
  float d = 1.;


  while ( iterFrom != lastFrom && iterTo != lastTo ) {
    errorVect = *iterTo - _motion->transform( (*iterFrom)[0], (*iterFrom)[1], (*iterFrom)[2] );
    d = sqrt ( errorVect.dot(errorVect) );      
    if (d > dMax)
          dMax = d;
    
    dSum += (double)d;
    
    ++count;
    ++iterFrom; ++iterTo;
  }
    
  return (dSum / double(count));
}


bool 
SimiLeastSquareEstimation::computeMotion()
{
  if( _pointsFrom.size() != _pointsTo.size() )
    throw runtime_error("SimiLeastSquareEstimation:Both vectors must have same size !") ;

  computeRigidMotion();

//	cout<<endl<<"Le motion en rigide vaut : "<<endl<<*(_motion)<<endl;

  int u, v;
  unsigned size = _pointsFrom.size();
  AimsData<double> Rotation(3,3);
  AimsData<float> RotationF = _motion->rotation().clone();
  ForEach2d(Rotation, u, v)	Rotation(u,v) = double( RotationF(u,v) );


  // On calcul la matrice K = somme(yi * xiT) et le scalaire sigmax2 = somme(xi²)
  AimsData<double> K(3,3);
  AimsData<double> temp(3,3);
  double sigmax2 = 0;
  K = 0;
  temp = 0;
  unsigned n=0;
#ifdef AIMS_USE_MPI
  /* Parallel K and sigmax2 computation */
  int rank = COMM_WORLD.Get_rank();
  int mpiSize = COMM_WORLD.Get_size();
  int nbPts = size/mpiSize;
  int nbPtsLeft = size%mpiSize;
  if(rank < nbPtsLeft) {
    n = rank*(nbPts+1);
    size = n + nbPts+1;
  }
  else {
    n = rank*nbPts + nbPtsLeft;
    size = n + nbPts;
  }
#endif
  for(;n<size;n++)  
  {
    Point3df x(_pointsFrom[n][0] - _meanX[0],_pointsFrom[n][1] - _meanX[1], 0);
    Point3df y(_pointsTo[n][0] - _meanY[0],_pointsTo[n][1] - _meanY[1], 0);

    temp=0;
    temp(0,0)=y[0]*x[0];
    temp(1,1)=y[1]*x[1];
    temp(0,1)=y[1]*x[0];
    temp(1,0)=y[0]*x[1];

    ForEach2d(K, u, v)
    {
      K(u,v) += temp(u,v) ;
    }

    sigmax2 += ( x[0]*x[0] + x[1]*x[1]  );
  }
#ifdef AIMS_USE_MPI
  COMM_WORLD.Allreduce(IN_PLACE, &K[0], 9, DOUBLE, SUM);
  COMM_WORLD.Allreduce(IN_PLACE, &sigmax2, 1, DOUBLE, SUM);
  COMM_WORLD.Barrier();
#endif
  // Transposition
  K = K.transpose();

  
  AimsData<double> RKt = Rotation.cross(K);
  double trace = RKt(0,0) + RKt(1,1) + RKt(2,2);

  // calcul de s, coefficient de similitude
  float s = float(trace/sigmax2), s0 = 1;

//	cout<<endl<<"Le coefficient de similitude vaut :"<<s<<endl;

  if(!_is2D)
    s0 = s;
  
  _motion->setScaling(s,s,s0);
  _motion->transAffine();

//	cout<<endl<<"scaling vaut : "<<endl<<_motion->scaling()<<endl;
//	cout<<endl<<"rot vaut (pareil que motion rigide?) : "<<endl<<_motion->rot()<<endl;
//	cout<<endl<<"...et le motion final (sc * rot?) : "<<endl<<*(_motion)<<endl;

  return 1;
}
