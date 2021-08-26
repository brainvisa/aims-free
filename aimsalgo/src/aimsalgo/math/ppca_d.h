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
                                 const carto::rc_ptr<carto::Volume<T> > & data,
                                 double distanceRef )
  {
    if( selectedPoints.size() <= (unsigned) data->getSizeT() )
      {
        _valid = false ;
        _computed = true ;
        return ;
      }
    _valid = true ;
    carto::VolumeRef<T> individuals(
      std::max(int(selectedPoints.size()), 1), data->getSizeT(), 1, 1,
      carto::AllocatorContext::fast() );
    int i = 0 ;
    std::list< Point3d >::const_iterator iter( selectedPoints.begin() ), 
      last( selectedPoints.end() ) ; 
    Point3df mean ;
  
    while( iter != last )
      {
        for(int t = 0 ; t < data->getSizeT() ; ++t )
          individuals( i, t ) = T( data->at( (*iter)[0], (*iter)[1], (*iter)[2],
                                         t ) ) ;
        ++i ; ++iter ;
        //std::cout << "Indiv " << i << " = " << *iter << std::endl ;
        mean += Point3df( (*iter)[0], (*iter)[1], (*iter)[2] ) ;
      }
    //std::cout << "Center = " << mean / float( individuals.getSizeX() )
    // << std::endl ;
    doIt( individuals, distanceRef ) ;
  }

  template <typename T>
  void 
  ProbabilisticPcaElement::doIt(
    const carto::rc_ptr<carto::Volume<T> > & matriceIndiv, double distanceRef )
  {
    _distanceRef = distanceRef ;
    
    _x = carto::VolumeRef<double>(
      matriceIndiv->getSizeY(), 1, 1, 1, carto::AllocatorContext::fast() ) ;
    _xT = carto::VolumeRef<double>( 1, matriceIndiv->getSizeY(), 1, 1,
                                    carto::AllocatorContext::fast() );
    /*   int ignoreVPs = 0 ; */
    std::cout << "toto" << std::endl << std::endl ;
    
    if( matriceIndiv->getSizeX() <= matriceIndiv->getSizeY() )
    {
      _valid = false ;
      _computed = true ;
      return ;
    }
    _valid = true ;
  
    //----------------- Just for testing to separate kidney core and cortex
    int ignoreVPs = 0 ;

    _computed = true ;
    int nbFrame = matriceIndiv->getSizeY() ;
  
    if( matriceIndiv->getSizeY() < nbFrame )
      std::cerr << "Beware : not enough data to evaluate svd" << std::endl ;
  
    carto::VolumeRef<double> centeredIndivMatrix(
      matriceIndiv->getSizeX(), matriceIndiv->getSizeY(), 1, 1,
      carto::AllocatorContext::fast() );
    _mean = carto::VolumeRef<double>( matriceIndiv->getSizeY(), 1, 1, 1,
                                      carto::AllocatorContext::fast() );
    _mean = 0. ;
    T val ;
    for( int ind = 0 ; ind < matriceIndiv->getSizeX() ; ++ind )
      for( int t = 0 ; t < matriceIndiv->getSizeY() ; ++t )
        {
          val = matriceIndiv->at( ind, t ) ;
          _mean(t) += val ;
        }

    //TEMP
    double meanMean = 0. ; 
    //END TEMP

    for( int t = 0 ; t < nbFrame ; ++t )
    {
      _mean.at(t) /= centeredIndivMatrix->getSizeX() ;
      for( int ind = 0 ; ind < centeredIndivMatrix->getSizeX() ; ++ind )
      {
        centeredIndivMatrix( ind, t ) = matriceIndiv->at(ind, t) - _mean(t) ;
        meanMean += centeredIndivMatrix( ind, t ) ;
      }
    }
    std::cout << "Mean centered mean = " << meanMean << " (should be 0) " << std::endl ;

    /*   std::cout << "Mean : " ; */
    /*   for( int i = 0 ; i < _mean->getSizeX() ; ++ i ){ */
    /*     std::cout << _mean(i) << " " ; */
    /*   }   */
    /*   std::cout << std::endl << _meanMean << std::endl ; */
  
//        Writer< carto::VolumeRef< double> > wri02( "centeredIndiv.ima" ) ;
//        wri02.write(centeredIndivMatrix) ;
  
    // Matrice des correlations
    carto::VolumeRef<double> matVarCov( nbFrame, nbFrame, 1, 1,
                                        carto::AllocatorContext::fast() ) ;
    int x, y, dx = matVarCov->getSizeX(), dy = matVarCov->getSizeY();

    for( y=0; y<dy; ++y )
      for( x=0; x<dx; ++x )
      {
        for(int k=0;  k < centeredIndivMatrix->getSizeX()  ;++k)
          matVarCov(x, y) += centeredIndivMatrix(k, x)
            * centeredIndivMatrix(k, y);
        matVarCov(x, y) /= centeredIndivMatrix->getSizeX() - 1 ;
      }
    
//     Writer< carto::VolumeRef< double> > wriMVC( "matVarCov.ima" ) ;
//     wriMVC.write(matVarCov) ;

    // Decomposition SVD 
    AimsSVD< double >  svd;
    svd.setReturnType( AimsSVD< double >::MatrixOfSingularValues );
    _EValues  = svd.doit( matVarCov );
//       Writer< carto::VolumeRef< double> > wri04( "eValAvant.ima" ) ;
//      wri04.write(_EValues) ;
  
//     Writer< carto::VolumeRef< double> > wriMVC2( "matVarCov2.ima" ) ;
//     wriMVC2.write((matVarCov.clone().transpose()).cross( _EValues.cross(matVarCov) ) ) ;
//
//     Writer< carto::VolumeRef< double> > wriMVC3( "matVarCov3.ima" ) ;
//     wriMVC3.write((matVarCov).cross( _EValues.cross(matVarCov.clone().transpose()) ) ) ;

    svd.sort(matVarCov, _EValues) ;
//        Writer< carto::VolumeRef< double> > wri05( "eValApres.ima" ) ;
//       wri05.write(_EValues) ; 
  
    _EVect = carto::VolumeRef<double>( nbFrame, _nbOfSignificantEV, 1, 1,
                                       carto::AllocatorContext::fast() ) ;
    for( int i = 0 ; i < nbFrame ; ++i )
      for( int j = 0 ; j < _nbOfSignificantEV ; ++j )
        _EVect(i, j) = matVarCov(i, j) ;  
  
    _Sigma2 = 0. ;
    for( int i = _nbOfSignificantEV + ignoreVPs ; i < nbFrame ; ++i )
      _Sigma2 += _EValues(i, i) ;
  
    _Sigma2 /= double(nbFrame - ( _nbOfSignificantEV + ignoreVPs ) ) ;
  
    for( int c = 0 ; c < _EValues->getSizeX() ; ++c )
      std::cout << "\tLamda " << c+1 << "EValue = " << _EValues(c, c) ;
    std::cout <<"\tSigma2 = " << _Sigma2 << std::endl ;
    
//     std::cout << "Lamda 1 = " << _EValues(0, 0) << "Lamda 2 = " 
//               << _EValues(1, 1) 
//               <<"\tSigma2 = " << _Sigma2 << std::endl ;
  
    _Wi = carto::VolumeRef<double>(
      _nbOfSignificantEV, _nbOfSignificantEV, 1, 1,
      carto::AllocatorContext::fast() );

    for( int i = 0; i < _nbOfSignificantEV ; ++i )
      _Wi(i, i) = sqrt( _EValues(i) - _Sigma2 ) ;
  
    std::cout << "_Wi.dimZ = " << _Wi->getSizeZ()
	 << "\t_Wi.dimT = " << _Wi->getSizeT() << std::endl ;

    std::cout << "_EVect.dimZ = " << _EVect->getSizeZ()
	 << "\t_EVect.dimT = " << _EVect->getSizeT() << std::endl ;

    _Wi = matrix_product( _EVect, _Wi );
//       Writer< carto::VolumeRef< double> > wri0( "Wi.ima" ) ;
//      wri0.write(_Wi) ; 

    carto::VolumeRef<double> Ci = matrix_product( _Wi, transpose( _Wi ) );
//     Writer< carto::VolumeRef< double > > wri1( "Wi2.ima" ) ;
//     wri1.write(Ci) ; 
    
    for( y=0; y<dy; ++y )
      for( x=0; x<dx; ++x )
        if( x < y )
        {
          double val = 0.5 * ( Ci(x, y) + Ci(y, x) ) ;
          Ci(x, y) = Ci(y, x) = val ;
        }

    for( int i = 0; i < nbFrame ; ++i )
      Ci(i, i) += _Sigma2 ;
  
//     Writer< carto::VolumeRef< double> > wri2( "Ci.ima" ) ;
//     wri2.write(Ci) ; 
    
    //-------------------------------------------------------------------------
    double s = 0.00000001 ;
    carto::VolumeRef< double > u = Ci.deepcopy(); // because it is replaced on SVD output
    carto::VolumeRef< double > v( Ci->getSizeY(), Ci->getSizeY(), 1, 1,
                                  carto::AllocatorContext::fast() );

    AimsSVD< double > svd2;
    carto::VolumeRef< double > w = svd2.doit( u, &v );
  
    // Set the singular values lower than threshold to zero
    double ts = s * w.max();
    int i, n = w->getSizeX();
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
    
    _invCi = matrix_product( v, matrix_product( w, transpose( u ) ) );

//     Writer< carto::VolumeRef< double> > wriI( "invCi.ima" ) ;
//     wriI.write(_invCi) ; 
    
    //TEMP
    carto::VolumeRef<float> ciFloat = Ci.deepcopy();
    
//     Writer< carto::VolumeRef< float> > wriI2( "invCi2.ima" ) ;
//     wriI2.write( AimsInversionLU(ciFloat) ) ; 

//     Writer< carto::VolumeRef< float> > wriI3( "invCi3.ima" ) ;
//     wriI3.write( AimsInversionLU( AimsInversionLU(ciFloat) ) ) ; 
    // END TEMP

    std::cout << "_invCiInside.dimZ = " << _invCi->getSizeZ()
	 << "\t_invCiInside.dimT = " << _invCi->getSizeT() << std::endl ;

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

//     carto::VolumeRef<float> invCi2( AimsInversionLU(ciFloat) ) ;
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
//     Writer< carto::VolumeRef<double> > w1("matVarCovT.ima") ;
//     w1.write(matVarCovT) ;
//     Writer< carto::VolumeRef<double> > w2("matVarCovD.ima") ;
//     w2.write(matVarCovD) ;
//     Writer< carto::VolumeRef<double> > w3("CiT.ima") ;
//     w3.write(CiT) ;
//     Writer< carto::VolumeRef<double> > w4("CiD.ima") ;
//     w4.write(CiD) ;
//     Writer< carto::VolumeRef<double> > w5("invCiT.ima") ;
//     w5.write(invCiT) ;
//     Writer< carto::VolumeRef<double> > w6("invCiD.ima") ;
//     w6.write(invCiD) ;
//     Writer< carto::VolumeRef<double> > w7("invCi2T.ima") ;
//     w7.write(invCi2T) ;
//     Writer< carto::VolumeRef<double> > w8("invCi2D.ima") ;
//     w8.write(invCi2D) ;
    
    for( y=0; y<dy; ++y )
      for( x=0; x<dx; ++x )
        if( x < y )
        {
          double val = 0.5 * ( _invCi(x, y) + _invCi(y, x) ) ;
          _invCi(x, y) = _invCi(y, x) = val ;
        }
//     ForEach2d( matVarCov, x, y )
//       invCiD(x, y) = _invCi(x, y) - _invCi(y, x) ;
    
//     Writer< carto::VolumeRef<double> > w9("invCiSymD.ima") ;
//     w9.write(invCiD) ;

  }

  template <class T>
  double 
  ProbabilisticPcaElement::noiseVariance(
    const carto::rc_ptr<carto::Volume<T> > & matriceIndiv,
    double & meanNorm ) const
  {
    int ignoreVPs = 0 ;

    int nbFrame = matriceIndiv->getSizeY() ;
  
    if( matriceIndiv->getSizeY() < nbFrame )
      std::cerr << "Beware : not enough data to evaluate svd" << std::endl ;
  
    //-------------------TMP-------------------------------
    //carto::VolumeRef<T> indMatCopy(matriceIndiv.clone()) ;
    //Writer< carto::VolumeRef<T> > indWri("indiv.ima") ;
    //indWri.write(indMatCopy) ;
  
    //-------------------TMP-------------------------------
  
    carto::VolumeRef<double> centeredIndivMatrix(
      matriceIndiv->getSizeX(), matriceIndiv->getSizeY(), 1, 1,
      carto::AllocatorContext::fast() ) ;
    carto::VolumeRef<double> mean( matriceIndiv->getSizeY(), 1, 1, 1,
                                   carto::AllocatorContext::fast() ) ;

    for( int ind = 0 ; ind < matriceIndiv->getSizeX() ; ++ind )
      for( int t = 0 ; t < matriceIndiv->getSizeY() ; ++t )
        mean(t) += matriceIndiv->at( ind, t ) ;
    
    
    meanNorm = 0. ;
    for( int t = 0 ; t < nbFrame ; ++t )
    {
      mean(t) /= centeredIndivMatrix->getSizeX() ;
      meanNorm += mean(t) * mean(t);
      for( int ind = 0 ; ind < centeredIndivMatrix->getSizeX() ; ++ind )
      {
        centeredIndivMatrix( ind, t ) = matriceIndiv->at(ind, t) - mean(t);
      }
    }
    meanNorm /= mean->getSizeX();
  

    // Matrice des correlations
    carto::VolumeRef<double> matVarCov( nbFrame, nbFrame, 1, 1,
                                        carto::AllocatorContext::fast() );
    int                 x, y ;
    for( y=0; y<nbFrame; ++y )
      for( x=0; x<nbFrame; ++x )
      {
        for(int k=0;  k < centeredIndivMatrix->getSizeX()  ;++k)
          matVarCov(x, y) += centeredIndivMatrix(k, x)
            * centeredIndivMatrix(k, y);
        matVarCov(x, y) /= centeredIndivMatrix->getSizeX() - 1 ;
      }
  
    //Writer< carto::VolumeRef<T> > indWri("indiv.ima") ;
    //indWri.write(matriceIndiv) ;
  
    //Writer< carto::VolumeRef<double> > matVarWri("matVarCov.ima") ;
    //matVarWri.write(matVarCov) ;
  
    // Decomposition SVD 
    AimsSVD< double >  svd;
    svd.setReturnType( AimsSVD< double >::MatrixOfSingularValues );
    carto::VolumeRef<double> EValues  = svd.doit( matVarCov );
  
    svd.sort(matVarCov, EValues) ;
  
  
    double Sigma2 = 0. ;
    for( int i = _nbOfSignificantEV + ignoreVPs ; i < nbFrame ; ++i )
      Sigma2 += EValues(i, i) ;
  
    Sigma2 /= double(nbFrame - ( _nbOfSignificantEV + ignoreVPs ) ) ;

    return Sigma2 ;
  }


  template <class T>
  ProbabilisticPca<T>::ProbabilisticPca(
    const carto::rc_ptr<carto::Volume<T> > & data,
    const std::vector<
    std::list< Point3d > >& classes,
    int nbOfSignificantEigenValues,
    const std::vector<double>& PIj )
    :
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
    carto::VolumeRef<double> eigenValues(
      _data->getSizeT(), _classes.size(), 1, 1,
      carto::AllocatorContext::fast() ) ;
    // ------------------ TEST ---------------------


    for( unsigned int c = 0 ; c < _classes.size() ; ++c )
    {
      if( int(_classes[c].size()) > _data->getSizeT() )
      {
        ProbabilisticPcaElement el( _nbOfSignificantEigenValues, _PIj[c] ) ;
        if( c == 0 ){
          el.doIt( _classes[c], _data ) ;
          _distanceRef = log( el.normFactor() ) ;
          std::cout << "Noise Ref = " << _distanceRef << std::endl ;
        }
        el.doIt( _classes[c], _data, _distanceRef ) ;
      
        // ------------------ TEST ---------------------
        for( int t = 0 ; t < _data->getSizeT() ; ++t )
          eigenValues( t, c ) = el.eigenValues()(t) ;
        // ------------------ TEST ---------------------
      
        std::cout << "Class " << c << " done !" << std::endl ;
        _discrElements.push_back( el ) ;
      } else {
        std::cerr << "Not enough data for element " << c << std::endl ;
      }
    }
  
    // ------------------ TEST ---------------------
    Writer< carto::VolumeRef<double> > eigenValuesWri( "classesEigenValues.ima") ;
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
  ProbabilisticPca<T>::andersonScores(
    const carto::rc_ptr<carto::Volume<double> > & x, double,
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
  ProbabilisticPca<T>::posteriorProbabilities(
    const carto::rc_ptr<carto::Volume<double> > & x,
    double,
    std::vector<double>&
    maxProbaByClass )
  {
    std::vector<double> res( _discrElements.size() ) ;
    /*   std::vector<double> normFactor ( _classes.size() ) ; */
    /*   for( int i = 0 ; i < _classes.size() ; ++i ) */
    /*     normFactor[i] = pow( _discrElements[c].noiseVariance() 
           / _discrElements[0].noiseVariance(), x->getSizeX() / 2. ) ; */

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
  ProbabilisticPca<T>::affectedTo(
    const carto::rc_ptr<carto::Volume<double> > & x )
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
  ProbabilisticPca<T>::classification(
    const carto::rc_ptr<carto::Volume<T> > & dynamicImage,
    const carto::rc_ptr<carto::Volume<byte> > & mask,
    carto::rc_ptr<carto::Volume<short> > & segmented )
  {
    int x, y, z ;
    if( dynamicImage->getSizeT() != _data->getSizeT() )
      return false ;
    if( segmented->getSizeX() != _data->getSizeX() ||  segmented->getSizeY() != _data->getSizeY()
        || segmented->getSizeZ() != _data->getSizeZ() )
      {
        segmented = carto::VolumeRef<short> ( _data->getSizeX(), _data->getSizeY(),
                                      _data->getSizeZ() ) ;
        segmented->setVoxelSize( _data->getVoxelSize() ) ;
      }

    carto::VolumeRef<double> indiv( dynamicImage->getSizeT(), 1, 1, 1,
                                    carto::AllocatorContext::fast() ) ;
    std::vector<int> dim = dynamicImage->getSize();
    for( z=0; z<dim[2]; ++z )
      for( y=0; y<dim[1]; ++y )
        for( x=0; x<dim[0]; ++x )
        {
          if( mask->at(x, y, z) )
          {
            //std::cout << "x : " << x << ", y : " << y << ", z : " << z
            // << std::endl ;
            for( int t = 0 ; t < _data->getSizeT() ; ++t )
              indiv(t) = dynamicImage->at(x, y, z, t ) ;
            segmented->at(x, y, z) = affectedTo( indiv ) ;
            //std::cout << " affected To " << segmented(x, y, z) << std::endl ;
          }
        }

    std::cout << "Probabilistic Pca classification completed" << std::endl ;

    return true ;
  }

  template <class T>
  bool
  ProbabilisticPca<T>::fuzzyClassification(
    const carto::rc_ptr<carto::Volume<T> > & dynamicImage,
    const carto::rc_ptr<carto::Volume<byte> > & mask,
    carto::rc_ptr<carto::Volume<float> > & fuzzySegmented,
    double /* probaThreshold */,
    double /* andersonScoreThreshold*/,
    const carto::rc_ptr<carto::Volume<double> > & indivPriorProbabilities )
  {
    bool noPriorProba =
      ( indivPriorProbabilities->getSizeX( ) == 1
      && indivPriorProbabilities->getSizeY( ) == 1
      && indivPriorProbabilities->getSizeZ( ) == 1 ) ;
    int count = 0 ; 
    //double uniformPriorProba ; 
    int x, y, z ;
    if( noPriorProba )
    {
      std::vector<int> mdim = mask->getSize();
      for( z=0; z<mdim[2]; ++z )
        for( y=0; y<mdim[1]; ++y )
          for( x=0; x<mdim[0]; ++x )
            if( mask->at( x, y, z ) )
              ++count ;
      //uniformPriorProba = 1. / count ;
    }
    if( dynamicImage->getSizeT() != _data->getSizeT() )
      return false ;
    if( fuzzySegmented->getSizeX() != _data->getSizeX()
        || fuzzySegmented->getSizeY() != _data->getSizeY() ||
        fuzzySegmented->getSizeZ() != _data->getSizeZ()
        || fuzzySegmented->getSizeT() != int(_classes.size() ) )
    {
      fuzzySegmented.reset( new carto::Volume<float>(
        _data->getSizeX(), _data->getSizeY(),
        _data->getSizeZ(), _classes.size() ) );
      fuzzySegmented->setVoxelSize( _data->getVoxelSize() );
    }

    std::vector<double> probabilityRepartition ;
    std::vector<double> maxProbaByClass( _discrElements.size(), 0. ) ;
    std::vector<double> max( _discrElements.size(), 0. ) ;
    carto::VolumeRef<double> indiv( dynamicImage->getSizeT(), 1, 1, 1,
                                    carto::AllocatorContext::fast() ) ;
    carto::VolumeRef<double> maxOnClass( dynamicImage->getSizeX(),
                                         dynamicImage->getSizeY(),
                                         dynamicImage->getSizeZ() );

    std::vector<int> ddim = dynamicImage->getSize();
    for( z=0; z<ddim[2]; ++z )
      for( y=0; y<ddim[1]; ++y )
        for( x=0; x<ddim[0]; ++x )
        {
          if( mask->at(x, y, z) )
          {
            double norm = 0. ;
            /* std::cout << "x : " << x << ", y : " << y << ", z : " << z
              << std::endl ; */
            for( int t = 0 ; t < _data->getSizeT() ; ++t )
              {
                indiv(t) = dynamicImage->at(x, y, z, t ) ;
                norm += indiv(t) * indiv(t);
              }
            /* probabilityRepartition = posteriorProbabilities( indiv, norm,
              maxProbaByClass ) ; */
            probabilityRepartition = andersonScores(indiv, norm,
                                                    maxProbaByClass) ;
            for( int c = 0 ; c < fuzzySegmented->getSizeT() ; ++c )
              {
                fuzzySegmented->at(x, y, z, c) = probabilityRepartition[c] ;
                if( probabilityRepartition[c] > max[c] )
                  max[c] = probabilityRepartition[c] ;
                if( probabilityRepartition[c] > maxOnClass(x, y, z) )
                  maxOnClass(x, y, z) = probabilityRepartition[c] ;
              }
          }
        }
  
    std::cout << "Max on c = " ;
    for( int c = 0 ; c < fuzzySegmented->getSizeT() ; ++c )
      std::cout << max[c] << " " << std::endl ;
  
//     Writer< carto::VolumeRef< float> > fuzzySegmentedW( "fuzzy.ima" ) ;
//     fuzzySegmentedW.write( fuzzySegmented ) ;

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

  
//        Writer< carto::VolumeRef< float> > pbByClassW( "probaByClass.ima" ) ;
//     pbByClassW.write( fuzzySegmented ) ;
  
    std::cout << "Probabilistic Pca : fuzzy classification completed" 
              << std::endl ;

    return true ;
  }


  template <class T>
  double
  ProbabilisticPca<T>::pX( const carto::rc_ptr<carto::Volume<double> > & x )
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




