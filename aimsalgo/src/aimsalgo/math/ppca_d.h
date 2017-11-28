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



#ifndef AIMS_MATH_PPCA_D_H
#define AIMS_MATH_PPCA_D_H

#include <aims/data/fastAllocationData.h>
#include <aims/data/data.h>
#include <aims/math/svd.h>
#include <aims/math/ppca.h>
#include <aims/math/mathelem.h>
#include <aims/math/gausslu.h>
#include <aims/io/writer.h>
#include <cartobase/type/string_conversion.h>


namespace aims
{

  template <typename T>
  void
  ProbabilisticPcaElement::doIt( const std::list< Point3d >& selectedPoints, 
                                 const AimsData<T>& data, double distanceRef )
  {
    if( selectedPoints.size() <= (unsigned) data.dimT() )
      {
        _valid = false ;
        _computed = true ;
        return ;
      }
    _valid = true ;
    AimsFastAllocationData<T> individuals( std::max(int(selectedPoints.size()), 1), 
                             data.dimT() );
    int i = 0 ;
    std::list< Point3d >::const_iterator iter( selectedPoints.begin() ), 
      last( selectedPoints.end() ) ; 
    Point3df mean ;
  
    while( iter != last )
      {
        for(int t = 0 ; t < data.dimT() ; ++t )
          individuals( i, t ) = T( data( (*iter)[0], (*iter)[1], (*iter)[2], 
                                         t ) ) ;
        ++i ; ++iter ;
        //std::cout << "Indiv " << i << " = " << *iter << std::endl ;
        mean += Point3df( (*iter)[0], (*iter)[1], (*iter)[2] ) ;
      }
    //std::cout << "Center = " << mean / float( individuals.dimX() ) 
    // << std::endl ;
    doIt( individuals, distanceRef ) ;
  }

  template <typename T>
  void 
  ProbabilisticPcaElement::doIt( const AimsData<T>& matriceIndiv, 
                                 double distanceRef )
  {
    _distanceRef = distanceRef ;
//     AimsData<double> CiT(matriceIndiv.dimY(), matriceIndiv.dimY()) ;
//     AimsData<double> invCiT(matriceIndiv.dimY(), matriceIndiv.dimY()) ;
//     AimsData<double> invCi2T(matriceIndiv.dimY(), matriceIndiv.dimY()) ;
//     AimsData<double> Wi2T(matriceIndiv.dimY(), matriceIndiv.dimY()) ;
//     AimsData<double> matVarCovT(matriceIndiv.dimY(), matriceIndiv.dimY()) ;
//     AimsData<double> CiD(matriceIndiv.dimY(), matriceIndiv.dimY()) ;
//     AimsData<double> invCiD(matriceIndiv.dimY(), matriceIndiv.dimY()) ;
//     AimsData<double> invCi2D(matriceIndiv.dimY(), matriceIndiv.dimY()) ;
//     AimsData<double> Wi2D(matriceIndiv.dimY(), matriceIndiv.dimY()) ;
//     AimsData<double> matVarCovD(matriceIndiv.dimY(), matriceIndiv.dimY()) ;
    
    _x = AimsData<double>( matriceIndiv.dimY() ) ;
    _xT = AimsData<double>( 1, matriceIndiv.dimY() ) ;
    /*   int ignoreVPs = 0 ; */
    std::cout << "toto" << std::endl << std::endl ;
    
    if( matriceIndiv.dimX() <= matriceIndiv.dimY() ){
      _valid = false ;
      _computed = true ;
      return ;
    }
    _valid = true ;
  
    //----------------- Just for testing to separate kidney core and cortex
    int ignoreVPs = 0 ;

    _computed = true ;
    int nbFrame = matriceIndiv.dimY() ;
  
    if( matriceIndiv.dimY() < nbFrame )
      std::cerr << "Beware : not enough data to evaluate svd" << std::endl ;
  
    AimsFastAllocationData<double> centeredIndivMatrix( matriceIndiv.dimX(), matriceIndiv. 
                                          dimY() ) ;
    _mean = AimsFastAllocationData<double>(matriceIndiv.dimY()) ;
    _mean = 0. ;
    T val ;
    for( int ind = 0 ; ind < matriceIndiv.dimX() ; ++ind )
      for( int t = 0 ; t < matriceIndiv.dimY() ; ++t )
        {
          val = matriceIndiv( ind, t ) ;
          _mean[t] += val ;
        }

    //TEMP
    double meanMean = 0. ; 
    //END TEMP

    for( int t = 0 ; t < nbFrame ; ++t )
      {
        _mean[t] /= centeredIndivMatrix.dimX() ;
        for( int ind = 0 ; ind < centeredIndivMatrix.dimX() ; ++ind ){
          centeredIndivMatrix( ind, t ) = matriceIndiv(ind, t) - _mean[t] ;
	  meanMean += centeredIndivMatrix( ind, t ) ;
        }
      }
    std::cout << "Mean centered mean = " << meanMean << " (should be 0) " << std::endl ;

    /*   std::cout << "Mean : " ; */
    /*   for( int i = 0 ; i < _mean.dimX() ; ++ i ){ */
    /*     std::cout << _mean(i) << " " ; */
    /*   }   */
    /*   std::cout << std::endl << _meanMean << std::endl ; */
  
       Writer< AimsData< double> > wri02( "centeredIndiv.ima" ) ;  
       wri02.write(centeredIndivMatrix) ; 
  
    // Matrice des correlations
    AimsFastAllocationData<double> matVarCov(nbFrame, nbFrame) ;
    int                 x, y ;
    ForEach2d( matVarCov, x, y )
    {
      for(int k=0;  k < centeredIndivMatrix.dimX()  ;++k)
	matVarCov(x, y) += centeredIndivMatrix(k, x) 
          * centeredIndivMatrix(k, y);
      matVarCov(x, y) /= centeredIndivMatrix.dimX() - 1 ;
    }
    
    Writer< AimsData< double> > wriMVC( "matVarCov.ima" ) ; 
    wriMVC.write(matVarCov) ;
    
    // Decomposition SVD 
    AimsSVD< double >  svd;
    svd.setReturnType( AimsSVD< double >::MatrixOfSingularValues );
    _EValues  = svd.doit( matVarCov );
//       Writer< AimsData< double> > wri04( "eValAvant.ima" ) ; 
//      wri04.write(_EValues) ;
  
    Writer< AimsData< double> > wriMVC2( "matVarCov2.ima" ) ; 
    wriMVC2.write((matVarCov.clone().transpose()).cross( _EValues.cross(matVarCov) ) ) ;
    
    Writer< AimsData< double> > wriMVC3( "matVarCov3.ima" ) ; 
    wriMVC3.write((matVarCov).cross( _EValues.cross(matVarCov.clone().transpose()) ) ) ;

    svd.sort(matVarCov, _EValues) ;
//        Writer< AimsData< double> > wri05( "eValApres.ima" ) ; 
//       wri05.write(_EValues) ; 
  
    _EVect = AimsFastAllocationData<double>( nbFrame, _nbOfSignificantEV ) ;
    for( int i = 0 ; i < nbFrame ; ++i )
      for( int j = 0 ; j < _nbOfSignificantEV ; ++j )
        _EVect(i, j) = matVarCov(i, j) ;  
  
    _Sigma2 = 0. ;
    for( int i = _nbOfSignificantEV + ignoreVPs ; i < nbFrame ; ++i )
      _Sigma2 += _EValues(i, i) ;
  
    _Sigma2 /= double(nbFrame - ( _nbOfSignificantEV + ignoreVPs ) ) ;
  
    for( int c = 0 ; c < _EValues.dimX() ; ++c )
      std::cout << "\tLamda " << c+1 << "EValue = " << _EValues(c, c) ;
    std::cout <<"\tSigma2 = " << _Sigma2 << std::endl ;
    
//     std::cout << "Lamda 1 = " << _EValues(0, 0) << "Lamda 2 = " 
//               << _EValues(1, 1) 
//               <<"\tSigma2 = " << _Sigma2 << std::endl ;
  
    _Wi = AimsFastAllocationData<double>( _nbOfSignificantEV, _nbOfSignificantEV ) ;
    for( int i = 0; i < _nbOfSignificantEV ; ++i )
      _Wi(i, i) = sqrt( _EValues(i) - _Sigma2 ) ;
  
    std::cout << "_Wi.dimZ = " << _Wi.dimZ()
	 << "\t_Wi.dimT = " << _Wi.dimT() << std::endl ;

    std::cout << "_EVect.dimZ = " << _EVect.dimZ()
	 << "\t_EVect.dimT = " << _EVect.dimT() << std::endl ;

    _Wi = _EVect.cross( _Wi ) ;
//       Writer< AimsData< double> > wri0( "Wi.ima" ) ;  
//      wri0.write(_Wi) ; 

    AimsFastAllocationData<double> Ci = _Wi.cross( _Wi.clone().transpose() ) ;
//     Writer< AimsData< double > > wri1( "Wi2.ima" ) ; 
//     wri1.write(Ci) ; 
    
    ForEach2d( matVarCov, x, y )
      if( x < y ){
	double val = 0.5 * ( Ci(x, y) + Ci(y, x) ) ;
	Ci(x, y) = Ci(y, x) = val ;
      }
    
    for( int i = 0; i < nbFrame ; ++i )
      Ci(i, i) += _Sigma2 ;
  
//     Writer< AimsData< double> > wri2( "Ci.ima" ) ;  
//     wri2.write(Ci) ; 
    
    //-------------------------------------------------------------------------
    double s = 0.00000001 ;
    AimsFastAllocationData< double > u = Ci.clone(); // because it is replaced on SVD output
    AimsFastAllocationData< double > v( Ci.dimY(), Ci.dimY() );

    AimsSVD< double > svd2;
    AimsFastAllocationData< double > w = svd2.doit( u, &v );
  
    // Set the singular values lower than threshold to zero
    double ts = s * w.maximum();
    int i, n = w.dimX();
    _detCi = 1. ;
    short unkept = 0 ;
    for ( i=0; i<n; i++ )
      {
        _detCi *= w( i, i ) / _Sigma2 ;
        if ( w( i, i ) > ts )  w( i, i ) = 1.0f / w( i, i );
        else {
	  w( i, i ) = 0.0f;
	  ++unkept ;
	}
      }
    
    std::cout << "Unkept values = " << unkept << std::endl ;
    
    _invCi = AimsFastAllocationData<double>(1,1,1,1) ;
    _invCi = v.cross( w.cross(  u.transpose() ) ) ;

//     Writer< AimsData< double> > wriI( "invCi.ima" ) ;  
//     wriI.write(_invCi) ; 
    
    //TEMP
    AimsData<float> ciFloat( Ci.dimX(), Ci.dimY() ) ;
    ForEach2d(ciFloat, x, y)
      ciFloat(x, y) = float( Ci(x, y) ) ;
    
//     Writer< AimsData< float> > wriI2( "invCi2.ima" ) ;  
//     wriI2.write( AimsInversionLU(ciFloat) ) ; 

//     Writer< AimsData< float> > wriI3( "invCi3.ima" ) ;  
//     wriI3.write( AimsInversionLU( AimsInversionLU(ciFloat) ) ) ; 
    // END TEMP

    std::cout << "_invCiInside.dimZ = " << _invCi.dimZ()
	 << "\t_invCiInside.dimT = " << _invCi.dimT() << std::endl ;

//     _normFactor = 1. / ( pow( 2*M_PI, nbFrame/2. ) * pow( _detCi, 0.5 ) 
//                          * pow( _Sigma2, nbFrame/2. ) ) ;
    _normFactor = 1. /  ( pow(_detCi, 0.5 ) * pow( _Sigma2, nbFrame/2. ) ) ;
  
    std::cout << "-0.5 * log( _detCi ) = " << -0.5 *  log( _detCi ) << "\t log(_PIj) = " << log(_PIj) << std::endl ;
    //x	      << "- (nbFrame/2) * log(_Sigma2) = " << - (nbFrame/2) * log(_Sigma2) << std::endl ;
    
    _lnAddFactor = log( 1. / sqrt(_detCi) / pow( _Sigma2, nbFrame/2. ) ) ;
    //_lnAddFactor = -0.5 * log( _detCi ) /*+ log(_PIj)*/ - (nbFrame/2) * log(_Sigma2) ;
      

    //_lnAddFactor = log( _detCi / ( _PIj * _PIj ) ) + nbFrame * log( _Sigma2 ) ;
    
    std::cout << "Det Ci = " << _detCi << "\tNorm Factor = " << _normFactor 
              << "\tLn Add Factor = " << _lnAddFactor << "\tCorresp factor = " << exp(_lnAddFactor)<< std::endl ;

//     AimsData<float> invCi2( AimsInversionLU(ciFloat) ) ;
//     ForEach2d(matVarCov, x, y){
//       CiT(y, x) = Ci(x, y) ;
//       CiD(x, y) = Ci(x, y) - Ci(y, x) ;
//       invCiT(y, x) = _invCi(x, y) ;
//       invCiD(x, y) = _invCi(x, y) - _invCi(y, x) ;
//       invCi2T(y, x) = invCi2(x, y) ;
//       invCi2D(x, y) = invCi2(x, y) - invCi2(y, x) ;
// //       Wi2T(y, x) = Wi2(x, y) ;
// //       Wi2D(x, y) = Wi2(x, y) - Wi2(y, x) ;
//     }
//     Writer< AimsData<double> > w1("matVarCovT.ima") ;
//     w1.write(matVarCovT) ;
//     Writer< AimsData<double> > w2("matVarCovD.ima") ;
//     w2.write(matVarCovD) ;
//     Writer< AimsData<double> > w3("CiT.ima") ;
//     w3.write(CiT) ;
//     Writer< AimsData<double> > w4("CiD.ima") ;
//     w4.write(CiD) ;
//     Writer< AimsData<double> > w5("invCiT.ima") ;
//     w5.write(invCiT) ;
//     Writer< AimsData<double> > w6("invCiD.ima") ;
//     w6.write(invCiD) ;
//     Writer< AimsData<double> > w7("invCi2T.ima") ;
//     w7.write(invCi2T) ;
//     Writer< AimsData<double> > w8("invCi2D.ima") ;
//     w8.write(invCi2D) ;
    
    ForEach2d( matVarCov, x, y )
      if( x < y ){
	double val = 0.5 * ( _invCi(x, y) + _invCi(y, x) ) ;
	_invCi(x, y) = _invCi(y, x) = val ;
      }
//     ForEach2d( matVarCov, x, y )
//       invCiD(x, y) = _invCi(x, y) - _invCi(y, x) ;
    
//     Writer< AimsData<double> > w9("invCiSymD.ima") ;
//     w9.write(invCiD) ;

  }

  template <class T>
  double 
  ProbabilisticPcaElement::noiseVariance( const AimsData<T>& matriceIndiv, 
                                          double & meanNorm ) const
  {
    int ignoreVPs = 0 ;

    int nbFrame = matriceIndiv.dimY() ;
  
    if( matriceIndiv.dimY() < nbFrame )
      std::cerr << "Beware : not enough data to evaluate svd" << std::endl ;
  
    //-------------------TMP-------------------------------
    //AimsData<T> indMatCopy(matriceIndiv.clone()) ;
    //Writer< AimsData<T> > indWri("indiv.ima") ;
    //indWri.write(indMatCopy) ;
  
    //-------------------TMP-------------------------------
  
    AimsFastAllocationData<double> centeredIndivMatrix( matriceIndiv.dimX(), 
                                          matriceIndiv.dimY() ) ;
    AimsFastAllocationData<double> mean(matriceIndiv.dimY()) ;

    for( int ind = 0 ; ind < matriceIndiv.dimX() ; ++ind )
      for( int t = 0 ; t < matriceIndiv.dimY() ; ++t )
        mean[t] += matriceIndiv( ind, t ) ;
    
    
    meanNorm = 0. ;
    for( int t = 0 ; t < nbFrame ; ++t ){
      mean[t] /= centeredIndivMatrix.dimX() ;
      meanNorm += mean[t] * mean[t] ;
      for( int ind = 0 ; ind < centeredIndivMatrix.dimX() ; ++ind ){
        centeredIndivMatrix( ind, t ) = matriceIndiv(ind, t) - mean[t] ;
      }
    }
    meanNorm /= mean.dimX() ;
  

    // Matrice des correlations
    AimsFastAllocationData<double> matVarCov(nbFrame, nbFrame) ;
    int                 x, y ;
    ForEach2d( matVarCov, x, y )
    {
      for(int k=0;  k < centeredIndivMatrix.dimX()  ;++k)
	matVarCov(x, y) += centeredIndivMatrix(k, x) 
          * centeredIndivMatrix(k, y);	
      matVarCov(x, y) /= centeredIndivMatrix.dimX() - 1 ;
    }
  
    //Writer< AimsFastAllocationData<T> > indWri("indiv.ima") ;
    //indWri.write(matriceIndiv) ;
  
    //Writer< AimsData<double> > matVarWri("matVarCov.ima") ;
    //matVarWri.write(matVarCov) ;
  
    // Decomposition SVD 
    AimsSVD< double >  svd;
    svd.setReturnType( AimsSVD< double >::MatrixOfSingularValues );
    AimsFastAllocationData<double> EValues  = svd.doit( matVarCov );
  
    svd.sort(matVarCov, EValues) ;
  
  
    double Sigma2 = 0. ;
    for( int i = _nbOfSignificantEV + ignoreVPs ; i < nbFrame ; ++i )
      Sigma2 += EValues(i, i) ;
  
    Sigma2 /= double(nbFrame - ( _nbOfSignificantEV + ignoreVPs ) ) ;

    return Sigma2 ;
  }


  template <class T>
  ProbabilisticPca<T>::ProbabilisticPca( const AimsData<T>& data, 
                                         const std::vector< 
                                         std::list< Point3d > >& 
					 classes, 
					 int nbOfSignificantEigenValues, 
					 const std::vector<double>& PIj ) :
    _classes(classes), _data(data), _PIj(PIj), 
    _nbOfSignificantEigenValues(nbOfSignificantEigenValues)
  {
    //std::cout << "ProbabilisticPca<T>::ProbabilisticPca classes size " 
    // << _classes.size() << std::endl ;
    if( PIj.size() != 0 )
      if( PIj.size() == classes.size() )
        _PIj = PIj ;
      else
        throw std::runtime_error( "Inconsistant data : number of classes and " 
                             "number of classes prior probabilities are " 
                             "different !") ;
    else
      {
        _PIj.reserve( _classes.size() ) ;
        for( unsigned int c = 0 ; c < _classes.size() ; ++c )
          _PIj.push_back( 1./_classes.size() ) ;
      }
  
    _discrElements.reserve( _classes.size() ) ;
    _distanceRef = 0. ;

    // ------------------ TEST ---------------------
    AimsFastAllocationData<double> eigenValues( _data.dimT(), _classes.size() ) ;
    // ------------------ TEST ---------------------


    for( unsigned int c = 0 ; c < _classes.size() ; ++c ){
      if( int(_classes[c].size()) > _data.dimT() ) {
        ProbabilisticPcaElement el( _nbOfSignificantEigenValues, _PIj[c] ) ;
        if( c == 0 ){
          el.doIt( _classes[c], _data ) ;
          _distanceRef = log( el.normFactor() ) ;
          std::cout << "Noise Ref = " << _distanceRef << std::endl ;
        }
        el.doIt( _classes[c], _data, _distanceRef ) ;
      
        // ------------------ TEST ---------------------
        for( int t = 0 ; t < _data.dimT() ; ++t )
          eigenValues( t, c ) = el.eigenValues()(t) ;
        // ------------------ TEST ---------------------
      
        std::cout << "Class " << c << " done !" << std::endl ;
        _discrElements.push_back( el ) ;
      } else {
        std::cerr << "Not enough data for element " << c << std::endl ;
      }
    }
  
    // ------------------ TEST ---------------------
    Writer< AimsData<double> > eigenValuesWri( "classesEigenValues.ima") ;
    eigenValuesWri.write(eigenValues) ;
    std::cout << "Probabilistic Pca initization completed" << std::endl ;
    //std::cout << "ProbabilisticPca<T>::ProbabilisticPca end classes size " 
    // << _classes.size() << std::endl ;

  }

  template <class T>
  double
  ProbabilisticPca<T>::weight( double norm2, int c, float tolerance )
  {  if( tolerance * norm2 > _discrElements[c].meanNorm() )
      return 1. ;
    else
      return 2 * pow( 0.5, _discrElements[c].meanNorm() 
                      / (tolerance * tolerance * norm2 ) ) ;
  }

  template <class T>
  double
  ProbabilisticPca<T>::wienerFilter( double sigma2, double norm2, 
                                     double factor )
  {
    double res = 1 - factor * factor * sigma2 / norm2 ;
    if( res < 0. )
      return 0. ;
    else 
      return res ;
  }

  template <class T>
  std::vector<double> 
  ProbabilisticPca<T>::andersonScores( const AimsData<double>& x, double, 
                                       std::vector<double>& maxProbaByClass )
  {
    //std::cout << "ProbabilisticPca<T>::andersonScores classes size " 
    // << _classes.size() << std::endl ;

    std::vector<double> res( _discrElements.size() ) ;
    //std::cout << "Nb of classes "<< _discrElements.size()<< std::endl  ;
    double scoreSum = 0. ;
    for( unsigned int c = 0 ; c < _discrElements.size() ; ++c )
      {
        //std::cout << "Class " << c << std::endl ;

        res[c] = _discrElements[c].posteriorProbability( x, 1 ) * _PIj[c] ;
        if( res[c] > maxProbaByClass[c] )
          maxProbaByClass[c] = res[c] ;
        scoreSum += res[c] ;
      }

    for( unsigned int c = 0 ; c < _discrElements.size() ; ++c )
      {
        res[c] /= scoreSum ;
        //std::cout << "\t" << res[c] ;
      }
    //std::cout << std::endl ;

    return res ;
  }

  template <class T>
  std::vector<double> 
  ProbabilisticPca<T>::posteriorProbabilities( const AimsData<double>& x, 
                                               double, 
                                               std::vector<double>& 
                                               maxProbaByClass )
  {
    std::vector<double> res( _discrElements.size() ) ;
    /*   std::vector<double> normFactor ( _classes.size() ) ; */
    /*   for( int i = 0 ; i < _classes.size() ; ++i ) */
    /*     normFactor[i] = pow( _discrElements[c].noiseVariance() 
           / _discrElements[0].noiseVariance(), x.dimX() / 2. ) ; */

    for( unsigned int c = 0 ; c < _discrElements.size() ; ++c )
      {
        res[c] = _discrElements[c].posteriorProbability( x, 1 ) ;
        if( res[c] > maxProbaByClass[c] )
          maxProbaByClass[c] = res[c] ;
        /*       *  *//* wienerFilter( _discrElements[c].noiseVariance(), 
                         norm2, 5. ) ; */  
        /* _discrElements[c].normalizedPosteriorProbability( x, 1, norm2 )
         *//*  * weight(norm2, c, 3) */ ;
      }
    return res ;
  }

  template <class T>
  int 
  ProbabilisticPca<T>::affectedTo( const AimsData<double>& x )
  {
    double lnP ;
    double lnPMin = _discrElements[0].lnPosteriorProbability( x ) ;
    int indMin = 0 ;
    for( unsigned int c = 0 ; c < _discrElements.size() ; ++c )
      {
        lnP = _discrElements[c].lnPosteriorProbability( x ) ;
        //std::cout << "Classe " << c << " : lnP = "<< lnP << std::endl ;
        if( lnP < lnPMin ){
          lnPMin = lnP ;
          indMin = c ;
        }
      }

    //std::cout << "lnPMin = "<< lnPMin << std::endl ;
    return indMin ;
  }


  template <class T>
  bool
  ProbabilisticPca<T>::classification( const AimsData<T>& dynamicImage, 
                                       const AimsData<byte>& mask,
                                       AimsData<short>& segmented )
  {
    int x, y, z ;
    if( dynamicImage.dimT() != _data.dimT() ) 
      return false ;
    if( segmented.dimX() != _data.dimX() ||  segmented.dimY() != _data.dimY() 
        || segmented.dimZ() != _data.dimZ() )
      {
        segmented = AimsFastAllocationData<short> ( _data.dimX(), _data.dimY(), 
                                      _data.dimZ() ) ;
        segmented.setSizeXYZT( _data.sizeX(), _data.sizeY(), _data.sizeZ(), 
                               1.0 ) ;
      }

    AimsFastAllocationData<double> indiv( dynamicImage.dimT() ) ;
    ForEach3d( dynamicImage, x, y, z )
    {
      if( mask(x, y, z) )
        {
          //std::cout << "x : " << x << ", y : " << y << ", z : " << z 
          // << std::endl ;
          for( int t = 0 ; t < _data.dimT() ; ++t )
            indiv(t) = dynamicImage(x, y, z, t ) ;
          segmented(x, y, z) = affectedTo( indiv ) ;
          //std::cout << " affected To " << segmented(x, y, z) << std::endl ;
        }
    }

    std::cout << "Probabilistic Pca classification completed" << std::endl ;

    return true ;
  }

  template <class T>
  bool
  ProbabilisticPca<T>::fuzzyClassification( const AimsData<T>& dynamicImage, 
                                            const AimsData<byte>& mask,
                                            AimsData<float>& fuzzySegmented,
                                            double /* probaThreshold */, 
                                            double /* andersonScoreThreshold*/,
                                            const AimsData<double>& 
                                            indivPriorProbabilities )
  {
    bool noPriorProba =
      ( indivPriorProbabilities.dimX( ) == 1
      && indivPriorProbabilities.dimY( ) == 1
      && indivPriorProbabilities.dimZ( ) == 1 ) ;
    int count = 0 ; 
    double uniformPriorProba ; 
    int x, y, z ;
    if( noPriorProba )
      {
        ForEach3d( mask, x, y, z )
          if( mask( x, y, z ) )
            ++count ;
        uniformPriorProba = 1. / count ;
      }
    if( dynamicImage.dimT() != _data.dimT() ) 
      return false ;
    if( fuzzySegmented.dimX() != _data.dimX() 
        || fuzzySegmented.dimY() != _data.dimY() || 
        fuzzySegmented.dimZ() != _data.dimZ() 
        || fuzzySegmented.dimT() != int(_classes.size() ) )
      {
        fuzzySegmented = AimsFastAllocationData<float> ( _data.dimX(), _data.dimY(), 
                                           _data.dimZ(), _classes.size() ) ;
        fuzzySegmented.setSizeXYZT( _data.sizeX(), _data.sizeY(), 
                                    _data.sizeZ(), 1.0 ) ;
      }

    std::vector<double> probabilityRepartition ;
    std::vector<double> maxProbaByClass( _discrElements.size(), 0. ) ;
    std::vector<double> max( _discrElements.size(), 0. ) ;
    AimsFastAllocationData<double> indiv( dynamicImage.dimT() ) ;
    AimsFastAllocationData<double> maxOnClass( dynamicImage.dimX(), dynamicImage.dimY(), 
                                 dynamicImage.dimZ() ) ;

    ForEach3d( dynamicImage, x, y, z )
    {
      if( mask(x, y, z) )
        {
          double norm = 0. ;
          /* std::cout << "x : " << x << ", y : " << y << ", z : " << z 
             << std::endl ; */
          for( int t = 0 ; t < _data.dimT() ; ++t )
            {
              indiv(t) = dynamicImage(x, y, z, t ) ;
              norm += indiv(t) * indiv(t);
            }
          /* probabilityRepartition = posteriorProbabilities( indiv, norm, 
             maxProbaByClass ) ; */
          probabilityRepartition = andersonScores(indiv, norm, 
                                                  maxProbaByClass) ;
          for( int c = 0 ; c < fuzzySegmented.dimT() ; ++c )
            {
              fuzzySegmented(x, y, z, c) = probabilityRepartition[c] ;
              if( probabilityRepartition[c] > max[c] )
                max[c] = probabilityRepartition[c] ;
              if( probabilityRepartition[c] > maxOnClass(x, y, z) )
                maxOnClass(x, y, z) = probabilityRepartition[c] ;
            }
        }
    }
  
    std::cout << "Max on c = " ;
    for( int c = 0 ; c < fuzzySegmented.dimT() ; ++c )
      std::cout << max[c] << " " << std::endl ;
  
    Writer< AimsData< float> > fuzzySegmentedW( "fuzzy.ima" ) ;
    fuzzySegmentedW.write( fuzzySegmented ) ;
  
    // float f = 0.000001 ;
  
    /*   ForEach3d(fuzzySegmented, x, y, z ) */
    /*     if( mask(x, y, z) ) */
    /*       for( int c = 0 ; c < fuzzySegmented.dimT() ; ++c ){  */
    /* 	if( fuzzySegmented(x, y, z, c) < f * max[c] ) */
    /* 	  fuzzySegmented(x, y, z, c) = 0. ; */
	
    /* 	fuzzySegmented(x, y, z, c) /= max[c] ; */
    /* 	//std::cout << fuzzySegmented(x, y, z, c) << " " << std::endl ; */
       /*       } */
       //std::cout << std::endl ;

  
       Writer< AimsData< float> > pbByClassW( "probaByClass.ima" ) ;
    pbByClassW.write( fuzzySegmented ) ;
  
    std::cout << "Probabilistic Pca : fuzzy classification completed" 
              << std::endl ;

    return true ;
  }


  template <class T>
  double
  ProbabilisticPca<T>::pX(const AimsData<double>& x )
  {
    std::vector<double> res( _discrElements.size() ) ;
    //cout << "Nb of classes "<< _discrElements.size()<< endl  ;
    double scoreSum = 0. ;
    for( unsigned int c = 0 ; c < _discrElements.size() ; ++c )
      scoreSum = _discrElements[c].posteriorProbability( x, 1 ) * _PIj[c] ;
  
    return scoreSum ;
  }

}

#endif




