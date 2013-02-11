/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

/*
 *  EM Threshold operators
 */
#ifndef AIMS_UTILITY_EMTHRESHOLD_H
#define AIMS_UTILITY_EMTHRESHOLD_H

#include <aims/data/data_g.h>
#include <aims/math/math_g.h>
#include <aims/io/writer.h>
#include <aims/io/reader.h>
#include <aims/utility/threshold.h>
#include <aims/histogram/histogram_g.h>
#include <aims/histogram/regularBinnedHisto.h>
#include <aims/connectivity/component.h>

template <class T,class U>
    class AimsEMThreshold: AimsThreshold<T,U>
{
  public:
    inline AimsEMThreshold( threshold_t type, T level = 0, 
                            T backgd = 0, int bins = 256 );

    virtual ~AimsEMThreshold() {}

  
  /// Return the binary thresholded image
    inline AimsData<U> bin(const AimsData<T> &image);
  
  private :
    int _bins;
};


template <class T,class U> inline
    AimsEMThreshold<T,U>::AimsEMThreshold( threshold_t type, T level,
                                           T backgd, int bins )
  : _bins(bins), AimsThreshold<T,U>( type, level, 0, backgd )
{
}

//   EM
template <class T,class U> inline
    AimsData<U> AimsEMThreshold<T,U>::bin(const AimsData<T> &image)
{
  // histogram
  aims::RegularBinnedHistogram<T> his;
  
  his.doit(image, 0, _bins - 1);

//   aims::Writer<AimsData<int32_t> > w("/tmp/test_histo.ima");
//   w << his.data();
//   
//   his.rebin(256);
  
//   aims::Writer<AimsData<int32_t> > w1("/tmp/test_histo_rebinned.ima");
//   w1 << his.data();  

  
  //declarations
  const int nb_th_classes=2;
  
  AimsData< double > th_pi(nb_th_classes);
  AimsData< double > th_mu(nb_th_classes);
  AimsData< double > th_sigma(nb_th_classes);
  AimsData< float > th_tau(_bins,nb_th_classes);
  int n,i,j;
  
  //initialisation of the EM algorithm
  double nb_points =0;
  for (n=0; n<_bins;++n) nb_points += his.data()(n);
  
  // the same number of points for each EM class
  double nb_points_thresh=0;
  for (n=0; n<_bins;++n)
  {
    nb_points_thresh += his.data()(n);
    for (i=0; i< nb_th_classes ; ++i)
    {
      if ((i<=(nb_th_classes*nb_points_thresh/nb_points)) 
           && (i>=(nb_th_classes*nb_points_thresh/nb_points)-1))
        th_tau(n,i)=1; 
      else th_tau(n,i)=0;
//       cout <<"th_tau(n,i)= "<< th_tau(n,i)<<endl; // tau devient 'nan' quand trop d'itérations (proba trop faible)
    }
  }
  
  // MAIN interation
  int count=0;
  while (count<= 3)
  {
    count ++;

      //   EM STEP E : calculate expect classification variable tau for each class 
      //   the first time jump this step to initialize mu, sigma and pi.
      
    if (count>1)
    {
      th_tau=0.;
      for (n=0; n<_bins; ++n)
      {
        for (i=0 ; i<nb_th_classes; ++i)
        {
          AimsData<double> aux(nb_th_classes);
          aux=0.;
          double sum_aux=0;
          for (j=0; j<nb_th_classes; ++j)
          {
            aux(j) = th_pi(j)*(1/sqrt(2*3.14*th_sigma(j))) 
                * exp(-(1/(2*th_sigma(j)))*(n-th_mu(j))*(n-th_mu(j)));
            sum_aux += aux(j);
          }
          th_tau(n,i)=aux(i)/sum_aux;
          //DEBUG
//           if (isnan (th_tau(n,i)))
//             th_tau(n,i) = 0;
//           th_tau(n,i) = (isnan (th_tau(n,i))) ? 0 : th_tau(n,i);
          //FIN DEBUG
        }
      }
    }

      //   EM STEP M : calculate new threshold means and variances for classified dada
    th_mu=0.; th_sigma=0.; th_pi=0.;

    for (j=0; j<nb_th_classes; ++j)
    {

	  //   Mean and covariance
      double sum1=0,sum2=0,sum=0;;
      for (n=0 ; n<_bins ; ++n) sum += th_tau(n,j)*his.data()(n);
      for (n=0 ; n<_bins; ++n) sum1 += th_tau(n,j)*n*his.data()(n);
      th_mu(j)=sum1/sum;

	  // for (n=0 ; n< 255; ++n) sum2 += th_tau(n,j)*(n-th_mu(j))*(n-th_mu(j))*his(n);
      for (n=0 ; n< _bins; ++n) sum2 += th_tau(n,j)*(n-th_mu(j))*(n-th_mu(j))*his.data()(n);
      th_sigma(j)=sum2/sum;
	  
	  //   Classifying variable pi
      sum1=0,sum=0;
      for (n=0 ; n< _bins; ++n)
      {
        sum1 += th_tau(n,j)*his.data()(n);
//         cout <<"th_tau(n,j)= "<< th_tau(n,j)<<endl;
        sum += his.data()(n);  
      }
      th_pi(j)=sum1/sum;

    }
  }
//   cout << "isnan th_mu(0) :" << isnan(th_mu(0))<<endl;
//   cout << "isnan 1 :" << isnan(1)<<endl;
//   cout <<"th_mu(0) = "<< th_mu(0) <<", th_mu(1) = "<< th_mu(1)<<endl;
//   cout<<"th_sigma(0) = "<<th_sigma(0)<<endl;

  //   Compute thresholds
  int seuil1 = (int) (th_mu(0)+sqrt(th_sigma(0)));
  int th_mu_max = (th_mu(0) < th_mu(1)) ? (int)th_mu(1) : (int)th_mu(0);
  int th_mu_min = (th_mu(0) < th_mu(1)) ? (int)th_mu(0) : (int)th_mu(1);
  int seuil2 = (int)th_mu(0);
  
  
  for(int i=th_mu_min; i<=th_mu_max; i++)
  {
    if (his.data()(i)<his.data()(seuil2)) seuil2 = i;
  }
  
  //  Threshold
  //AimsThreshold< T, U > thresholder( AIMS_BETWEEN, 0, seuil2);
  //   AimsThreshold< uint8_t, int16_t > thresholder( AIMS_BETWEEN, 0, 240);
  AimsEMThreshold<T,U>::_level2 = seuil2;
  AimsData<U>  imageThresh = AimsThreshold<T,U>::bin( image );
//  cout<<"seuil variance = "<<seuil1 <<", seuil min = "<<seuil2<<endl;
  
  return imageThresh;
  
}

#endif
