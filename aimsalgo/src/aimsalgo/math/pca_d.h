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


#include <aims/math/svd.h>
#include <aims/math/pca.h>
#include <aims/io/writer.h>

template <typename T>
void
AimsPCA::doIt( const std::list< Point3d >& selectedPoints, 
               const carto::rc_ptr<carto::Volume<T> > & data )
{
  carto::VolumeRef<T> individuals(
    std::max(int(selectedPoints.size()), 1), data->getSizeT(), 1, 1,
    carto::AllocatorContext::fast() );
  int i = 0 ;
  std::list< Point3d >::const_iterator iter( selectedPoints.begin() ), last( selectedPoints.end() ) ; 
  while( iter != last )
  {
    for(int t = 0 ; t < data->getSizeT() ; ++t )
      individuals( i, t ) = data( (*iter)[0], (*iter)[1], (*iter)[2], t ) ;
    ++i ; ++iter ;
  }
  doIt( individuals ) ;
}

template <typename T>
void 
AimsPCA::doIt( const carto::rc_ptr<carto::Volume<T> > & individuals )
{
  _computed = true ;
  _validPca = true ;
  _matricesComputed = false ;
  int nbFrame = individuals->getSizeY();
  
  carto::VolumeRef<float> centeredIndivMatrix(
    individuals->getSizeX(), individuals->getSizeY(), 1, 1,
    carto::AllocatorContext::fast() );

  if( _center || _normalize )
  {
    _mean = std::vector<float>(individuals->getSizeY(), 0.) ;
    _var = std::vector<float>(individuals->getSizeY(), 0.) ;
    T val ;
    for( int ind = 0 ; ind < individuals->getSizeX() ; ++ind )
      for( int t = 0 ; t < individuals->getSizeY() ; ++t )
      {
        val = individuals->at( ind, t ) ;
        _mean[t] += val ;
        _var[t] += val * val ;
      }
    
    //double meanMean = 0. ;
        
    for( int t = 0 ; t < nbFrame ; ++t )
    {
      _mean[t] /= centeredIndivMatrix->getSizeX() ;
      _var[t] = sqrt( _var[t] / ( centeredIndivMatrix->getSizeX() - 1 )
                      - _mean[t] * _mean[t] );
      for( int ind = 0 ; ind < centeredIndivMatrix->getSizeX() ; ++ind )
      {
        centeredIndivMatrix( ind, t ) = individuals->at( ind, t ) ;
        if( _center )
        {
          centeredIndivMatrix( ind, t ) -= _mean[t] ;
          //meanMean += centeredIndivMatrix( ind, t ) ;
        }
        if( _normalize )
          centeredIndivMatrix( ind, t ) /= _var[t] ;
      }
    }
    //meanMean /= centeredIndivMatrix.getSizeX() * centeredIndivMatrix.getSizeY() ;
    //std::cout << "Centered data mean (should be 0.) = " << meanMean << std::endl ;
  }


  // Matrice des correlations
  carto::VolumeRef<float>matVarCov(
    centeredIndivMatrix->getSizeY(), centeredIndivMatrix->getSizeY(), 1, 1,
    carto::AllocatorContext::fast() );

  int                 x1, y1;
  int dx1 = matVarCov->getSizeX(), dy1 = matVarCov->getSizeY();

  for( y1=0; y1<dy1; ++y1 )
    for( x1=0; x1<dx1; ++x1 )
    {
      for(int k=0;  k < centeredIndivMatrix->getSizeX()  ;++k)
        matVarCov(x1, y1)
          += centeredIndivMatrix(k, x1) * centeredIndivMatrix(k, y1);
      matVarCov(x1, y1) /= centeredIndivMatrix->getSizeX() - 1;
    }
  std::cerr << "var cov filled" << std::endl ;
 
/*   cout << "Centered Indiv Matrix = " << endl  */
/*        << centeredIndivMatrix.transpose() << endl ; */
/*   cout << "Mat Var Cov = " << endl  */
/*        << matVarCov << endl ; */
  
  // Decomposition SVD 
  try
  {
    AimsSVD< float >  svd;
    svd.setReturnType( AimsSVD< float >::VectorOfSingularValues );
    std::cerr << "doing svd !" << std::endl ;
    carto::VolumeRef< float > eigenVal  = svd.doit( matVarCov );
    std::cerr << "svd done !" << std::endl ;
    
    /*   cout << "Mat Var Cov Before sort = " << endl  */
    /*        << matVarCov << endl ; */
    
    svd.sort(matVarCov, eigenVal) ;

    
    /*  cout << "Mat Var Cov After sort = " << endl  */
    /*        << matVarCov << endl ; */
    
    _eigenVectors = matVarCov ;
    
    // Temporaire
    _eigenValues.clear() ;
    _eigenValues.reserve(matVarCov->getSizeX());
    for( int t = 0 ; t < matVarCov->getSizeX() ; ++t )
    {
      _eigenValues.push_back( eigenVal(t) ) ;
    }
  } // catch (user_interruption& e){
//     _validPca = false ;
//   }
  catch( std::exception &e )
  {
    _validPca = false ;
    if( individuals->getSizeX() > 0 && individuals->getSizeY() > 0
        && individuals->getSizeZ() > 0 )
    {
      std::cerr << "invalid pca" << std::endl ;
      
    }
    else
      std::cout << "empty indiv matrix" << std::endl ;
  }
}

