/* Copyright (c) 1995-2007 CEA
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

#include <cstdlib>
#include "veinextraction.h"
#include <math.h>
#include <aims/data/data.h>
#include <aims/io/writer.h>

static float distance2d( int x1, int y1, int x2, int y2 )
{
  return ::sqrt( (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) ) ;
}

template <typename T>
VeinExtraction<T>::VeinExtraction( Volume<T>* image, 
				   const Point3d& initialPoint, 
				   const Point3d& finalPoint, 
				   float diameter, 
				   int roiBorder,
				   bool lowerValueInaccessible,
				   bool usingWholeImage,
				   bool fillDisk ) :
  pathExtractor_(0), recomputePath_(true), diameter_(diameter), roiBorder_(roiBorder), 
  initialPoint_(initialPoint), finalPoint_(finalPoint), lowerValueInaccessible_(lowerValueInaccessible),
  usingWholeImage_(usingWholeImage), fillDisk_(fillDisk)
{
  ASSERT( image != 0 ) ;
  
  image_ = image ;
  //std::cout << "Size : " << image_->size() << "\tmin = " << image_->min() << "\tmax = " << image_->max() <<std::endl ;
  Point3d initialToFinal( finalPoint_ - initialPoint_ ) ;
  
  if( usingWholeImage_){
    roiOffset_ = Point3d( 0, 0, 0 ) ;
    roiSize_ = image->size() ;
    Volume<float> potential( roiSize_ ) ;

    for( int z = 0 ; z < roiSize_[2] ; z++ )
      for( int y = 0 ; y < roiSize_[1] ; y++ )
        for( int x = 0 ; x < roiSize_[0] ; x++ )
	  potential.setVoxel( x, y, z, -(*image_)(x, y, z) ) ;
    if( pathExtractor_){
      pathExtractor_->setPotential( potential ) ;
      pathExtractor_->setInitialPoint( initialPoint_ - roiOffset_ ) ;
    }else
      pathExtractor_ = new GeodesicMinimalPath( potential, initialPoint_ - roiOffset_, 0.01, lowerValueInaccessible_ ) ;

    segmentedVein_ = new Volume<short>( image_->size(), 0, image_->voxelSize() ) ;
  } else {
    roiOffset_ = Point3d( ( initialPoint_[0] < finalPoint_[0] ? 
	  		  initialPoint_[0] - roiBorder_ : finalPoint_[0] - roiBorder_ ), 
		  	  (  initialPoint_[1] < finalPoint_[1] ? 
			    initialPoint_[1] - roiBorder_ : finalPoint_[1] - roiBorder_ ), 
			  ( initialPoint_[2] < finalPoint_[2] ? initialPoint_[2] : finalPoint_[2]) ) ;
  
    roiSize_ = Point3d( ( initialToFinal[0] > 0 ? initialToFinal[0] + 2*roiBorder_ : -initialToFinal[0] + 2*roiBorder_ ), 
		      ( initialToFinal[1] > 0 ? initialToFinal[1] + 2*roiBorder_ : -initialToFinal[1] + 2*roiBorder_ ), 
		      ( initialToFinal[2] > 0 ? initialToFinal[2] : -initialToFinal[2] ) ) ;
  
    roiSize_ += Point3d( 1, 1, 1 ) ;

    for( int i = 0 ; i < 3 ; i++ ){
      if( roiOffset_[i] < 0 ) 
        roiOffset_[i] = 0 ;
      if( roiOffset_[i] + roiSize_[i] > image_->size()[i]  ) 
        roiSize_[i] = image_->size()[1] - roiOffset_[i] ;    
    }

    Volume<float> potential( roiSize_ ) ;

    for( int z = 0 ; z < roiSize_[2] ; z++ )
      for( int y = 0 ; y < roiSize_[1] ; y++ )
        for( int x = 0 ; x < roiSize_[0] ; x++ )
	  potential.setVoxel( x, y, z, -(*image_)( x + roiOffset_[0] , y + roiOffset_[1], z + roiOffset_[2] ) ) ;
    if( pathExtractor_){
      pathExtractor_->setPotential( potential ) ;
      pathExtractor_->setInitialPoint( initialPoint_ - roiOffset_ ) ;
    }else
      pathExtractor_ = new GeodesicMinimalPath( potential, initialPoint_ - roiOffset_, 0.01, lowerValueInaccessible_ ) ;

    segmentedVein_ = new Volume<short>( image_->size(), 0, image_->voxelSize() ) ;
  }
}

template <typename T>
VeinExtraction<T>::~VeinExtraction( ) 
{
  if( pathExtractor_ )
    delete pathExtractor_ ;
  if( segmentedVein_ )
    delete segmentedVein_ ;
  if( segmentedVein_ )
    delete segmentedVein_ ;
}

template <typename T>
void
VeinExtraction<T>::init() 
{
//  if( !convolutedImage_ )
//     convolutedImage_ = new Volume<float>( roiSize_ ) ;
  
//  imageConvolution( ) ;
}

template <typename T>
void 
VeinExtraction<T>::setInitialPoint( const Point3d& initialPoint ) 
{
  bool outside = false ;
  for( int i = 0 ; i < 3 ; i++ )
    if( initialPoint[i] < ( initialPoint_[i] < finalPoint_[i] ? initialPoint_[i] : finalPoint_[i] ) ||
	initialPoint[i] > ( initialPoint_[i] > finalPoint_[i] ? initialPoint_[i] : finalPoint_[i] ) ) 
      outside = true ;

  initialPoint_ = initialPoint ;
  recomputePath_ = true ;

  if( outside )
    init( ) ;
}

template <typename T>
void 
VeinExtraction<T>::setFinalPoint( const Point3d& finalPoint ) {
  bool outside = false ;
  for( int i = 0 ; i < 3 ; i++ )
    if( finalPoint[i] < ( initialPoint_[i] < finalPoint_[i] ? initialPoint_[i] : finalPoint_[i] ) ||
	finalPoint[i] > ( initialPoint_[i] > finalPoint_[i] ? initialPoint_[i] : finalPoint_[i] ) ) 
      outside = true ;

  finalPoint_ = finalPoint ;
  recomputePath_ = true ;

  if( outside )
    init( ) ;  
}

template <typename T>
void 
VeinExtraction<T>::setImage( Volume<T>* image )
{
  ASSERT( image != 0 ) ;
  image_ = image ;
  recomputePath_ = true ;
  init( ) ;
}


template <typename T>
void 
VeinExtraction<T>::findVeinCenter( ) 
{
  std::list < Point3d > path( getExtractedPath( ) ) ;

  //std::cout << " In findVeinCenter : ExtractedPath size = " << path.size() << std::endl ;
  centeredPath_.clear( ) ;
  std::list < Point3d >::const_iterator pathFirst( path.begin( ) ), pathIter( path.begin( ) ), 
    pathLast( path.end( ) ) ;
  
  Point3d pointBefore, p, pointAfter ;
  Point3df n, u, v, center ;

  float minVox = image_->voxelSize()[0] ;
  for ( int i = 1 ; i < 3 ; i++ )
    if( image_->voxelSize()[i] < minVox && image_->voxelSize()[i] != 0. )
      minVox = image_->voxelSize()[i] ;
  
  // Masks Construction

  float diam = diameter_ / minVox ;
  int maskSize = 2* ( static_cast<int> (0.5*diam) + 1 + 2 ) + 1 ;
  int maskCenter = ( maskSize - 1 ) / 2 ; 
  int interiorCardinal = 0, borderCardinal = 0, exteriorCardinal = 0 ;
  int * interiorMask = new int[maskSize*maskSize] ;
  int * exteriorMask = new int[maskSize*maskSize] ;  
  int * borderMask = new int[maskSize*maskSize] ;
  
  for ( int i = 0 ; i < maskSize ; i++ )
    for ( int j = 0 ; j < maskSize ; j++ ){
      float d = distance2d(i, j, maskCenter, maskCenter) ;
      if ( d <= 0.5*(diam-1) && ( fillDisk_|| !(i == maskCenter && j == maskCenter) ) ) {
	interiorMask[j*maskSize + i] = 1 ;
	++interiorCardinal ;
      }
      else interiorMask[j*maskSize + i] = 0 ;
      if ( fabs( d - 0.5*diam ) <= 0.5 ) {
	borderMask[j*maskSize + i] = 1 ;
	++borderCardinal ;
      }
      else borderMask[j*maskSize + i] = 0 ;
      if( d >= 0.5*diam && d <= 0.5*diam + 2 ){ 
	exteriorMask[j*maskSize + i] = 1 ;
	++exteriorCardinal ;
      }
      else exteriorMask[j*maskSize + i] = 0 ;
    }
     
  //Display Masks
//   std::cout << "maskSize = " << maskSize << std::endl ;
//   for ( int i = 0 ; i < maskSize ; i++ ){
//     for ( int j = 0 ; j < maskSize ; j++ )
//       if ( interiorMask[j*maskSize + i] )
// 	std::cout << "i " ;
//       else cout << "n " ;
//     std::cout << endl ;
//   }
  
//   cout << std::endl ;
//   for ( int i = 0 ; i < maskSize ; i++ ){
//     for ( int j = 0 ; j < maskSize ; j++ )
//       if ( borderMask[j*maskSize + i] )
// 	std::cout << "b " ;
//       else std::cout << "n " ;
//     cout << std::endl ;
//   }
  
//   std::cout << std::endl ;
//   for ( int i = 0 ; i < maskSize ; i++ ){
//     for ( int j = 0 ; j < maskSize ; j++ )
//       if ( exteriorMask[j*maskSize + i] )
// 	std::cout << "e " ;
//       else std::cout << "n " ;
//     std::cout << std::endl ;
//   }
  
  int ray = static_cast<int>( diam / 2. ) + 2 ;
  int size = 2*(ray + maskCenter) + 1 ;
  int sectionCenter = ray + maskCenter ;
  
  float* section = new float[ size * size ] ;
  
  double *gradEnergy = new double[size*size],  *exteriorEnergy = new double[size*size], 
    *interiorEnergy = new double[size*size], *smoothEnergy = new double[size*size] ;
  double * energy = new double[size*size] ;
  for( int j = 0 ; j < size ; j++ )
    for( int i = 0 ; i < size ; i++ ){
      energy[j*size+i] = 0. ;
      gradEnergy[j*size+i] = 0. ;
      exteriorEnergy[j*size+i] = 0. ;
      interiorEnergy[j*size+i] = 0. ;
    }


  
  double gradWeight = 2., interiorWeight = 0.25, exteriorWeight = 0.5, smoothWeight = 0. ;
  if( fillDisk_ )
    gradWeight = 0., interiorWeight = 1., exteriorWeight = 0., smoothWeight = 0. ;
  
  Point2df gradient, position ;
  Point3df previousCenter( -1, -1, -1) ;
  
  std::list< Point3df > tangentList, smoothTangentList ;
  // Calcul de la tangente grossiere au chemin minimal.
  pathIter = pathFirst ;
  pointBefore = *pathIter ;
  ++pathIter ;
  p = *pathIter ;
  ++pathIter ;
  
  Point3df tangent ;
  for( int t = 0 ; t < 3 ; t++ )
    tangent[t] = p[t]-pointBefore[t] ; 
  tangent /= tangent.norm( ) ;
  tangentList.push_back( tangent ) ;
  
  while ( pathIter != pathLast ){
    pointAfter = *pathIter ;
    for( int t = 0 ; t < 3 ; t++ )
      tangent[t] = pointAfter[t]-pointBefore[t] ; 
    tangent /= tangent.norm( ) ;
    tangentList.push_back( tangent ) ;
    
    pointBefore = p ; 
    p = pointAfter ;
    ++pathIter ;
  }
  
  for( int t = 0 ; t < 3 ; t++ )
    tangent[t] = p[t] - pointBefore[t] ;
  tangent /= tangent.norm( ) ;
  tangentList.push_back( tangent ) ;
  
  float invexp1 = exp((double)-1), invexp2 = std::exp((double)-2) ;
  float kernel[5] = { invexp2, invexp1, 1, invexp1, invexp2 } ;
  float sum = 1 + 2*invexp2 + 2*invexp1 ;
  for ( int i = 0 ; i < 5 ; i++ ){
    kernel[i] /= sum ;
  }
  
  
  std::list< Point3df >::const_iterator tangentIter( tangentList.begin() ), 
    tangentLast( tangentList.end() ) ;
  Point3df tangentNeighbourhood[5] ;
  Point3df smoothTangent ;
  
  smoothTangentList.push_back(*tangentIter) ;

  for (int i = 0 ; i < 4 ; i++, tangentIter++ )
    tangentNeighbourhood[i] = *tangentIter ;
    
  smoothTangentList.push_back( ( tangentNeighbourhood[0] * invexp1  + 
				 tangentNeighbourhood[1] +
				 tangentNeighbourhood[2] * invexp1  ) / 
			       (1 + 2*invexp1 ) ) ;

  
  while( tangentIter != tangentLast ){
    tangentNeighbourhood[4] = *tangentIter ;
    smoothTangent = tangentNeighbourhood[0]*kernel[0] ;
    for ( int i = 1 ; i < 5 ; i++ )
      smoothTangent += tangentNeighbourhood[i]*kernel[i] ;
    smoothTangentList.push_back( smoothTangent ) ;
    for ( int i = 0 ; i < 4 ; i++ )
      tangentNeighbourhood[i] = tangentNeighbourhood[i+1] ;
    ++tangentIter ;
  }
  
  //std::cout << "Tangent = " << tangentNeighbourhood[3] ;
 //  std::cout << "\tSmoothed" <<  ( tangentNeighbourhood[2] * invexp1 + 
// 			     tangentNeighbourhood[3] +
// 			     tangentNeighbourhood[4] * invexp1 ) / (1 + 2*invexp1 )<< endl ;
  
  smoothTangentList.push_back( ( tangentNeighbourhood[2] * invexp1 + 
				 tangentNeighbourhood[3] +
				 tangentNeighbourhood[4] * invexp1 ) / (1 + 2*invexp1 ) ) ;
  
  //std::cout << "Tangent = " << tangentNeighbourhood[4]<< std::endl ;
  
  smoothTangentList.push_back( tangentNeighbourhood[4] ) ;
  

  std::list< Point3df >::const_iterator smoothTangentIter( smoothTangentList.begin() ), 
    smoothTangentLast( smoothTangentList.end() ) ;
  
  AimsData<float> interiorEnergies( size, size, path.size() ) ;
  AimsData<float> exteriorEnergies( size, size, path.size() ) ;
  AimsData<float> gradEnergies( size, size, path.size() ) ;
  AimsData<float> smoothEnergies( size, size, path.size() ) ;
  AimsData<float> energies( size, size, path.size() ) ;

  float yGradKernel[3][3] = 
  {{ -invexp2/( 1 + invexp2 + invexp1 ), 0, invexp2/( 1 + invexp2 + invexp1 ) }, 
  { -invexp1/( 1 + invexp2 + invexp1 ), 0, invexp1/( 1 + invexp2 + invexp1 ) },
  { -invexp2/( 1 + invexp2 + invexp1 ), 0, invexp2/( 1 + invexp2 + invexp1 ) } } ;


  pathIter = pathFirst ;
  
  int count = 0 ;
  while ( pathIter != pathLast && smoothTangentIter != smoothTangentLast ){
   // std::cout << "Count "<< ++count << std::endl ;
    p = *pathIter ;
    n = *smoothTangentIter ;
    
    n /= n.norm( ) ;
    
    if( n[1] > 0.01 ){
      u[0] = 1. ; u[1] = -( n[2] + n[0] ) / n[1] ; u[2] = 1. ;
    } else if( n[0] > 0.01 ){
      u[1] = 1. ; u[0] = -( n[1] + n[2] ) / n[0] ; u[2] = 1. ;
    }else if( n[2] > 0.01 ){
      u[0] = 1. ; u[2] = -( n[0] + n[1] ) / n[2] ; u[1] = 1. ;
    }
    u /= u.norm( ) ;
    v = crossed( u, n ) ;
 
    Point3df current ;
//      std::cout << "Section of point " << p << std::endl 
//  	 << "\tn = " << n <<"\tu = " << u << "\tv = " << v << std::endl 
//  	 << "\tVoxelSize = " << image_->voxelSize() << "\tMinVox = " <<  minVox << std::endl ;
    
    float max = 0. ;
    for( int j = -sectionCenter; j < sectionCenter ; j++ ){
      for ( int i = -sectionCenter ; i < sectionCenter ; i++ ){
	current[0] = p[0] + (i*u[0] + j*v[0]) * minVox / image_->voxelSize()[0] ;
	current[1] = p[1] + (i*u[1] + j*v[1]) * minVox / image_->voxelSize()[1] ;
	current[2] = p[2] + (i*u[2] + j*v[2]) * minVox / image_->voxelSize()[2] ;
	section[ size * (j+sectionCenter) + i + sectionCenter] = image_->interpolation( current ) ;
	if( section[ size * (j+sectionCenter) + i + sectionCenter] > max )
	  max = section[ size * (j+sectionCenter) + i + sectionCenter] ;
      }
    }

   // std::cout << "Section" << std::endl ;

    if( max != 0.){
      float *dataPtr = section ;
      for( int i = 0 ; i < size ; i++ ){
	for( int j = 0 ; j < size ; j++, dataPtr++ ){
	  (*dataPtr) /= max ; 
	 // std::cout << (static_cast<int>( (*dataPtr)*100. ) /*> 50 ? 1 : 0*/ ) << "\t" ;
	}
	//std::cout << std::endl << std::endl ;
      }
    }
   // std::cout << "\tMask Center " << maskCenter << "\tSection [0] = " << section[0] << std::endl ;

    for( int j = 0 ; j < size ; j++ )
      for( int i = 0 ; i < size  ; i++ ){
	current[0] = p[0] + ((i-sectionCenter)*u[0] + (j-sectionCenter)*v[0]) * minVox / image_->voxelSize()[0] ;
	current[1] = p[1] + ((i-sectionCenter)*u[1] + (j-sectionCenter)*v[1]) * minVox / image_->voxelSize()[1] ;
	current[2] = p[2] + ((i-sectionCenter)*u[2] + (j-sectionCenter)*v[2]) * minVox / image_->voxelSize()[2] ;
	//std::cout << "P : " << i << " , " << j << "\t Courant " << current << std::endl ;
      }	
	
    int k2, l2 ;
    for( int j = maskCenter ; j < size - maskCenter - 1 ; j++ )
      for( int i = maskCenter ; i < size - maskCenter - 1 ; i++ ){
	interiorEnergy[i + j*size] = 0. ;
	exteriorEnergy[i + j*size] = 0. ;
	interiorEnergies(i, j, count) = 0. ;
	exteriorEnergies(i, j, count) = 0. ;
	gradEnergy[i + j*size] = 0. ;
	gradEnergies(i, j, count) = 0. ;
	current[0] = p[0] + ((i-sectionCenter)*u[0] + (j-sectionCenter)*v[0]) * minVox / image_->voxelSize()[0] ;
	current[1] = p[1] + ((i-sectionCenter)*u[1] + (j-sectionCenter)*v[1]) * minVox / image_->voxelSize()[1] ;
	current[2] = p[2] + ((i-sectionCenter)*u[2] + (j-sectionCenter)*v[2]) * minVox / image_->voxelSize()[2] ;
	//std::cout << "P : " << i << " , " << j << "\t Courant " << current << std::endl ;
	for ( int l = -maskCenter ; l <= maskCenter ; l++ ){
	  for ( int k = -maskCenter, index = 0 ; k <= maskCenter ; k++, index++ ){
	    
	    k2 = k + maskCenter ; l2 = l + maskCenter ;
	    if( interiorMask[ (l2)*maskSize + k2] ){
	      interiorEnergy[i + j*size] -= section[ (j+l)*size + i+k ] ;
	      interiorEnergies(i, j, count) -= section[ (j+l)*size + i+k ] ;
	    }
	    if( exteriorMask[ (l2)*maskSize + k2] ){
	      exteriorEnergy[i + j*size] += ( section[ (j+l)*size + i+k ] > .5 ? 1. : 0.) ;
	      exteriorEnergies(i, j, count) += ( section[ (j+l)*size + i+k ] > .5 ? 1. : 0.) ;
	    }
	    if( borderMask[ (l2)*maskSize + k2] ){
	      gradient[0] = 0. ; gradient[1] = 0. ; 
	      for( int n = -1 ; n <=1 ; n++ )
		for( int m = -1 ; m <= 1 ; m++ ){
		  gradient[0] += yGradKernel[n+1][m+1] * section[ (j+l+n)*size + i+k+m ] ;
		  gradient[1] += yGradKernel[m+1][n+1] * section[ (j+l+n)*size + i+k+m ] ;
		}
	      //std::cout << "\t( " << int(gradient[0] * 100) << " , " << int(gradient[1] * 100) <<" )" ;
	      position[0] = k ; position[1] = l ;
	      if( position[0] != 0. || position[1] != 0. )
		position /= position.norm( ) ;
	      //std::cout << "\t( " << int(position[0] * 100) << " , " << int(position[1] * 100) <<" )" ;
	      //std::cout << "\t" << -gradient.dot( position ) ;
	      gradEnergy[i + j*size] += gradient.dot( position ) ;
	      gradEnergies(i, j, count) += gradient.dot( position ) ;
	    } //else// std::cout << "\t(out,out)" ;
	  }
	 // std::cout << std::endl ;
	}
	//char cha ; cin >> cha ;	
	smoothEnergy[i + j*size] = ( previousCenter[0] > 0. ? ( (current - previousCenter).norm( ) > 1. ? 
								(current - previousCenter).norm( ) - 1 : 0. ) : 0. ) ;
	smoothEnergies(i, j, count) = ( previousCenter[0] > 0. ? ( (current - previousCenter).norm( ) > 1. ? 
								(current - previousCenter).norm( ) - 1 : 0. ) : 0. ) ;
	if( interiorCardinal ){
	  interiorEnergy[i + j*size] /= interiorCardinal ;
	  interiorEnergies(i, j, count) /= interiorCardinal ;
	}
	if ( exteriorCardinal){
	  exteriorEnergy[i + j*size] /= exteriorCardinal ;
	  exteriorEnergies(i, j, count) /= exteriorCardinal ;
	}
	if ( borderCardinal ){
	  gradEnergy[i + j*size] /= borderCardinal ;
	  gradEnergies(i, j, count) /= borderCardinal ;
	}
	energy[j*size+i] = 
	  interiorWeight * interiorEnergy[i + j*size] +
	  exteriorWeight * exteriorEnergy[i + j*size] +
	  ( previousCenter[0] < 0. ? 0. : smoothWeight) * smoothEnergy[i + j*size] +
	  gradWeight * gradEnergy[i + j*size] ;
	energies(i, j, count) = energy[j*size+i] ;
// 	std::cout << "interiorWeight = " << interiorWeight << "\tinteriorCardinal = " << interiorCardinal << "\tinteriorEnergy" << interiorEnergy[i + j*size]<< std::endl
// 	    << "\texteriorWeight = " << exteriorWeight << "\texteriorCardinal = " << exteriorCardinal<< "\texteriorEnergy" << exteriorEnergy[i + j*size] << std::endl
// 	     <<"\tEnergy = " << energy[j*size+i] 
//  	     << "\tgradWeight = " << gradWeight << "\tgradCardinal = " << borderCardinal<< "\tgradEnergy" << gradEnergy[i + j*size] << std::endl
// 	     <<"\tEnergy = " << energy[j*size+i] ;
     }
    
//      std::cout << std::endl << "InteriorEnergy" << std::endl ;
//      for( int j = 0 ; j < size - 1 ; j++ ){
//        for( int i = 0 ; i < size - 1 ; i++ )
// 	 std::cout << "\t" << int(interiorWeight*interiorEnergy[j*size+i]*100) ;
//       std::cout << std::endl ;
//     }

//     std::cout << std::endl << "ExteriorEnergy" << std::endl ;
//     for( int j = 0 ; j < size - 1 ; j++ ){
//       for( int i = 0 ; i < size - 1 ; i++ )
// 	std::cout << "\t" << int(exteriorWeight * exteriorEnergy[j*size+i]*100) ;
//      std::cout << std::endl ;
//     }

//    std::cout << std::endl << "GradEnergy"<< std::endl ;
//     for( int j = 0 ; j < size - 1 ; j++ ){
//       for( int i = 0 ; i < size - 1 ; i++ )
// 	std::cout << "\t" << int(gradWeight * gradEnergy[j*size+i]*100) ;
//       std::cout << endl ;
//     }

    Point2d energyMinPoint( maskCenter, maskCenter ) ;
    float energyMin = energy[ maskCenter*size + maskCenter ] ;
   // std::cout << std::endl << "Total Energy"<< std::endl ;
   // std::cout << std::endl ;
    for( int j = maskCenter ; j < size - maskCenter - 1 ; j++ ){
      for( int i = maskCenter ; i < size - maskCenter - 1 ; i++ ){
	//std::cout << "\t" << int(energy[j*size+i]*100) ;
	if( energyMin > energy[j*size+i] ){
	  energyMinPoint = Point2d( i , j ) ;
	  energyMin = energy[j*size+i] ;
	}
      }
     // std::cout << std::endl ;
    }
    
    energyMinPoint -= Point2d( sectionCenter, sectionCenter ) ;
    
    Point3df center ;
    center[0] = p[0] + (energyMinPoint[0]*u[0] + energyMinPoint[1]*v[0]) / image_->voxelSize()[0] * minVox ;
    center[1] = p[1] + (energyMinPoint[0]*u[1] + energyMinPoint[1]*v[1]) / image_->voxelSize()[1] * minVox ;
    center[2] = p[2] + (energyMinPoint[0]*u[2] + energyMinPoint[1]*v[2]) / image_->voxelSize()[2] * minVox ;
        
   // std::cout << "Point " << energyMinPoint +  Point2d( sectionCenter, sectionCenter ) << "\talso "<< center << "with min energy = " << energyMin << std::endl ;
    centeredPath_.push_back( center ) ;
    previousCenter = center ;
    //char c ; cin >> c ;
    pointBefore = p;
    ++pathIter ; ++smoothTangentIter ; ++count ;
  }
//   aims::Writer< AimsData<float> > wriIE( "interiorEnergies.ima") ;
//   wriIE.write(interiorEnergies) ;
//   aims::Writer< AimsData<float> > wriEE( "exteriorEnergies.ima") ;
//   wriEE.write(exteriorEnergies) ;
//   aims::Writer< AimsData<float> > wriBE( "gradEnergies.ima") ;
//   wriBE.write(gradEnergies) ;
//   aims::Writer< AimsData<float> > wriSE( "smoothEnergies.ima") ;
//   wriSE.write(smoothEnergies) ;
//   aims::Writer< AimsData<float> > wriE( "energies.ima") ;
//   wriE.write(energies) ;
    
  delete [] section ;
  delete [] interiorMask ; delete [] exteriorMask ; delete [] borderMask ;
  delete [] interiorEnergy ; delete [] exteriorEnergy ; delete [] smoothEnergy ; delete [] energy ;
}

template <typename T>
void 
VeinExtraction<T>::imageConvolution( )
{
  int halfConvolutionSize = 3 ;

  float *kernel = new float[2*halfConvolutionSize+1] ;
  float kernelSum = 0. ;
  for( int u = -halfConvolutionSize ; u < halfConvolutionSize ; u++ ){
    kernel[u + halfConvolutionSize] = exp(-u*u / halfConvolutionSize ) ;
    kernelSum += kernel[u + halfConvolutionSize] ;
  }
  for( int u = 0 ; u < 2*halfConvolutionSize+1 ; u++ )
    kernel[u] /= kernelSum ;
  
  float result ;
  for( int z = 0 ; z < roiSize_[2] ; z++ )
    for( int y = 0 ; y < roiSize_[1] ; y++ )
      for( int x = 0 ; x < roiSize_[0] ; x++ ){
	result = 0. ;
	for( int u = -halfConvolutionSize ; u <= halfConvolutionSize ; u++ ){
	  if( x + u + roiOffset_[0] >= 0 &&  x + u + roiOffset_[0] < image_->size()[0] ) 
	    result += (*image_)( x + u + roiOffset_[0], y + roiOffset_[1], z + roiOffset_[2] )*
	      kernel[u + halfConvolutionSize ] ;
	  else if( x + u + roiOffset_[0] < 0 )
	    result += (*image_)( -(x + u + roiOffset_[0]), y + roiOffset_[1], z + roiOffset_[2] )*
	      kernel[u + halfConvolutionSize ] ;
	  else 
	    result += (*image_)( 2*(image_->size()[0] -1 ) - ( x + u + roiOffset_[0]), 
				 y + roiOffset_[1], 
				 z + roiOffset_[2] )*
	      kernel[u + halfConvolutionSize ] ;
	}
	convolutedImage_->setVoxel( x, y, z, result ) ;
      }
  
  for( int z = 0 ; z < roiSize_[2] ; z++ )
    for( int y = 0 ; y < roiSize_[1] ; y++ )
      for( int x = 0 ; x < roiSize_[0] ; x++ ){
	result = 0. ;
	for( int u = -halfConvolutionSize ; u <= halfConvolutionSize ; u++ ){
	  if( y + u + roiOffset_[1] >= 0 &&  y + u + roiOffset_[1] < image_->size()[1] ) 
	    result += (*image_)( x + roiOffset_[0], y + u + roiOffset_[1], z + roiOffset_[2] )*
	      kernel[u + halfConvolutionSize ] ;
	  else if( y + u + roiOffset_[1] < 0 )
	    result += (*image_)( x + roiOffset_[0], -(y + u + roiOffset_[1]), z + roiOffset_[2] )*
	      kernel[u + halfConvolutionSize ] ;
	  else 
	    result += (*image_)( x + roiOffset_[0], 
				 2*(image_->size()[1] -1 ) - ( y + u + roiOffset_[1]), 
				 z + roiOffset_[2] )*
	      kernel[u + halfConvolutionSize ] ;
	}
	convolutedImage_->setVoxel( x, y, z, result ) ;
      }
  
  for( int z = 0 ; z < roiSize_[2] ; z++ )
    for( int y = 0 ; y < roiSize_[1] ; y++ )
      for( int x = 0 ; x < roiSize_[0] ; x++ ){
	result = 0. ;
	for( int u = -halfConvolutionSize ; u <= halfConvolutionSize ; u++ ){
	  if( z + u + roiOffset_[2] >= 0 &&  z + u + roiOffset_[2] < image_->size()[2] ) 
	    result += (*image_)( x + roiOffset_[0], y + roiOffset_[1], z + u + roiOffset_[2] )*
	      kernel[u + halfConvolutionSize ] ;
	  else if( z + u + roiOffset_[2] < 0 )
	    result += (*image_)( x + roiOffset_[0], y + roiOffset_[1], -(z + u + roiOffset_[2]) )*
	      kernel[u + halfConvolutionSize ] ;
	  else 
	    result += (*image_)( x + roiOffset_[0], 
				 y + roiOffset_[1], 
				 2*(image_->size()[2] -1 ) - ( z + u + roiOffset_[2] ) )*
	      kernel[u + halfConvolutionSize ] ;
	}
	convolutedImage_->setVoxel( x, y, z, result ) ;
      }
}

template <typename T>
const Volume<short>&
VeinExtraction<T>::getExtractedVein( ) 
{

 // std::cout << "RECOMPUTE PATH : " << recomputePath_ << std::endl ;
  if( recomputePath_ ){
    centeredPath_ = getCenteredPath( ) ;
  }

 // std::cout << "Size inside : " << image_->size() ;

  for( int z = 0 ; z < image_->size()[2] ; z++ )
    for( int y = 0 ; y < image_->size()[1] ; y++ )
      for( int x = 0 ; x < image_->size()[0] ; x++ )
	segmentedVein_->setVoxel( x, y, z, 0 ) ;

  float minVox = image_->voxelSize()[0] ;
  for ( int i = 1 ; i < 3 ; i++ )
    if( image_->voxelSize()[i] < minVox && image_->voxelSize()[i] != 0. )
      minVox = image_->voxelSize()[i] ;
  
  
  float ray2 = diameter_/2. ;
  ray2 *= ray2 ;
  

  std::list< Point3df >::iterator iter( centeredPath_.begin( ) ), last( centeredPath_.end( ) ) ;

  while( iter != last ){
    Point3d pint( static_cast<int>( (*iter)[0] + 0.5), static_cast<int>( (*iter)[1] + 0.5 ), static_cast<int>( (*iter)[2] + 0.5 ) ) ;
    for( int k = (int)(-diameter_ /image_->voxelSize()[2]) - 1 ; k < int(diameter_ / image_->voxelSize()[2]) + 1 ; k++ )
      for( int j = int(-diameter_ /image_->voxelSize()[1]) - 1 ; j < int(diameter_ / image_->voxelSize()[1]) + 1 ; j++ )
	for( int i = int(-diameter_ /image_->voxelSize()[0]) - 1 ; i < int(diameter_ / image_->voxelSize()[0]) + 1 ; i++ ){
	  Point3df current ( pint[0] + i, pint[1] + j, pint[2] + k ) ;
	  if( current[0] >= 0. &&  current[1] >= 0. &&  current[2] >= 0. &&
	      current[0] <= image_->size()[0] - 1 && current[1]  <= image_->size()[1] - 1 && current[2] <= image_->size()[2] - 1 )
	    if( distance2( current[0]*image_->voxelSize()[0], 
			   current[1]*image_->voxelSize()[1], 
			   current[2]*image_->voxelSize()[2], 
			   (*iter)[0]*image_->voxelSize()[0],
			   (*iter)[1]*image_->voxelSize()[1],
			   (*iter)[2]*image_->voxelSize()[2] ) < ray2  ){
	      if( current[0] > 0 && current[0] < segmentedVein_->size()[0]&&
		  current[1] > 0 && current[1] < segmentedVein_->size()[1]&&
		  current[2] > 0 && current[2] < segmentedVein_->size()[2] )
		segmentedVein_->setVoxel( (int)current[0], (int)current[1], (int)current[2], 1 ) ;
	    }
	  
	}
      
    ++iter ; 
  }
   return *segmentedVein_ ;
}

inline static float distance2( float x1, float y1, float z1, float x2, float y2, float z2 )
{
  return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2) ;
}

/* template class VeinExtraction<short> ; */
/* template class VeinExtraction<float> ; */

