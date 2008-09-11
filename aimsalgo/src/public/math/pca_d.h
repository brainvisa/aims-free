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


#include <aims/data/fastAllocationData.h>
#include <aims/math/svd.h>
#include <aims/math/pca.h>
#include <aims/io/writer.h>

template <typename T>
void
AimsPCA::doIt( const std::list< Point3d >& selectedPoints, 
	       const AimsData<T>& data )
{
  aims::AimsFastAllocationData<T> individuals( std::max(int(selectedPoints.size()), 1), data.dimT() ) ;
  int i = 0 ;
  std::list< Point3d >::const_iterator iter( selectedPoints.begin() ), last( selectedPoints.end() ) ; 
  while( iter != last )
    {
      for(int t = 0 ; t < data.dimT() ; ++t )
	individuals( i, t ) = data( (*iter)[0], (*iter)[1], (*iter)[2], t ) ;
      ++i ; ++iter ;
    }
  doIt( individuals ) ;
}

template <typename T>
void 
AimsPCA::doIt( const AimsData<T>& individuals )
{
  _computed = true ;
  _validPca = true ;
  _matricesComputed = false ;
  int nbFrame = individuals.dimY() ;
  
  aims::AimsFastAllocationData<float> centeredIndivMatrix( individuals.dimX(), individuals.dimY() ) ;
  if( _center || _normalize ){
    _mean = std::vector<float>(individuals.dimY(), 0.) ;
    _var = std::vector<float>(individuals.dimY(), 0.) ;
    T val ;
    for( int ind = 0 ; ind < individuals.dimX() ; ++ind )
      for( int t = 0 ; t < individuals.dimY() ; ++t ){
	val = individuals( ind, t ) ;
	_mean[t] += val ;
	_var[t] += val * val ;
      }
    
    //double meanMean = 0. ;
        
    for( int t = 0 ; t < nbFrame ; ++t ){
      _mean[t] /= centeredIndivMatrix.dimX() ;
      _var[t] = sqrt(_var[t] / ( centeredIndivMatrix.dimX() - 1 ) - _mean[t] * _mean[t]) ;
      for( int ind = 0 ; ind < centeredIndivMatrix.dimX() ; ++ind ){
	centeredIndivMatrix( ind, t ) = individuals( ind, t ) ;
	if( _center ){
	  centeredIndivMatrix( ind, t ) -= _mean[t] ;
	  //meanMean += centeredIndivMatrix( ind, t ) ; 
	}
	if( _normalize )
	  centeredIndivMatrix( ind, t ) /= _var[t] ;
      }
    }
    //meanMean /= centeredIndivMatrix.dimX() * centeredIndivMatrix.dimY() ;
    //std::cout << "Centered data mean (should be 0.) = " << meanMean << std::endl ;
  }


  // Matrice des correlations
  aims::AimsFastAllocationData<float>matVarCov(centeredIndivMatrix.dimY(), centeredIndivMatrix.dimY() ) ;
  int                 x1, y1;
  ForEach2d( matVarCov, x1, y1 )
    {
      for(int k=0;  k < centeredIndivMatrix.dimX()  ;++k)
	matVarCov(x1, y1) += centeredIndivMatrix(k, x1) * centeredIndivMatrix(k, y1);	
      matVarCov(x1, y1) /= centeredIndivMatrix.dimX() - 1 ;
    }
  std::cerr << "var cov filled" << std::endl ;
 
/*   cout << "Centered Indiv Matrix = " << endl  */
/*        << centeredIndivMatrix.transpose() << endl ; */
/*   cout << "Mat Var Cov = " << endl  */
/*        << matVarCov << endl ; */
  
  // Decomposition SVD 
  try{
    AimsSVD< float >  svd;
    svd.setReturnType( AimsSVD< float >::VectorOfSingularValues );
    std::cerr << "doing svd !" << std::endl ;
    aims::AimsFastAllocationData< float > eigenVal  = svd.doit( matVarCov );
    std::cerr << "svd done !" << std::endl ;
    
    /*   cout << "Mat Var Cov Before sort = " << endl  */
    /*        << matVarCov << endl ; */
    
    svd.sort(matVarCov, eigenVal) ;

    
    /*  cout << "Mat Var Cov After sort = " << endl  */
    /*        << matVarCov << endl ; */
    
    _eigenVectors = matVarCov ;
    
    // Temporaire
    _eigenValues.clear() ;
    _eigenValues.reserve(matVarCov.dimX()) ;
    for( int t = 0 ; t < matVarCov.dimX() ; ++t ){
      _eigenValues.push_back( eigenVal(t) ) ;
    }
  } // catch (user_interruption& e){
//     _validPca = false ;
//   }
  catch( std::exception &e ){
    _validPca = false ;
    if( individuals.dimX() > 0 && individuals.dimY() > 0 && individuals.dimZ() > 0 ){
      std::cerr << "invalid pca" << std::endl ;
//       aims::Writer< AimsData<T> > wriIM( "indivMatrix.ima") ;
//       wriIM.write(individuals) ;
//       aims::Writer< AimsData<float> > wriCIM( "centeredIndivMatrix.ima") ;
//       wriCIM.write(centeredIndivMatrix) ;
//       aims::Writer< AimsData<float> > wriVarCov( "matVarCov.ima") ;
//       wriVarCov.write(matVarCov) ;
//       char c ;
//       std::cin >> c ;
      
    } else
      std::cout << "empty indiv matrix" << std::endl ;
  }
}

