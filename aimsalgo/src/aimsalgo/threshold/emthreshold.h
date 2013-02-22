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

#include <cartobase/config/verbose.h>
#include <aims/data/data_g.h>
#include <aims/math/math_g.h>
#include <aims/io/writer.h>
#include <aims/io/reader.h>
#include <aims/histogram/histogram_g.h>
#include <aims/histogram/regularBinnedHisto.h>
#include <aims/utility/threshold.h>


template <class T, class U>
    class AimsEMThreshold: AimsThreshold<T, U>
{
  public:
    inline
    AimsEMThreshold( threshold_t type, 
                     T backgd = 0, 
                     float foregd = 32767,
                     int classes = 2,
                     int levelindex = 0, /* Level expressed as index */
                     int level2index = 1 /* Level expressed as index */ );

    virtual ~AimsEMThreshold() {}

    inline
    std::vector<T> processEMThresholds(const AimsData<T> &image,
                                       int bins = 0);

    /// Return the multi-level thresholded image
    inline AimsData<T> operator () (const AimsData<T> &image);
  
    /// Return the binary thresholded image
    inline
    AimsData<U> bin(const AimsData<T> &image);
  
  private :
    int _classes;
    int _levelindex;
    int _level2index;
};

template <class T, class U> inline
    AimsEMThreshold<T, U>::AimsEMThreshold( threshold_t type,
                                            T backgd, float foregd,
                                            int classes,
                                            int levelindex, int levelindex2 )
  : _classes(classes), 
    _levelindex(levelindex),
    _level2index(levelindex2), 
    AimsThreshold<T, U>( type, 0, 0, backgd, foregd )
{
  if (carto::verbose)
  {
    std::cout << "Type " << carto::toString(this->_type) << " ..." << std::endl << std::flush;
    std::cout << "Backgd " << carto::toString(this->_backgd) << " ..." << std::endl << std::flush;
    std::cout << "Foregd " << carto::toString(this->_foregd) << " ..." << std::endl << std::flush;
    std::cout << "Classes " << carto::toString(_classes) << " ..." << std::endl << std::flush;
    std::cout << "Level index " << carto::toString(_levelindex) << " ..." << std::endl << std::flush;
    std::cout << "Level 2 index " << carto::toString(_level2index) << " ..." << std::endl << std::flush;
  }
}
// EM
template <class T, class U> inline
    std::vector<T> AimsEMThreshold<T, U>::processEMThresholds(const AimsData<T> &image,
                                                              int bins)
{
  std::vector<T> thresholds, thresholds_mu;
  
  if ( bins <= 0 )
    bins = int(image.maximum() - image.minimum());
  
  if (carto::verbose)
    std::cout << "Bins " << carto::toString(bins) << " ..." << std::endl << std::flush;
  
  // Histogram
  aims::RegularBinnedHistogram<T> his(bins);
  his.doit(image);

  // Declarations
  AimsData<double> th_pi(_classes);
  AimsData<double> th_mu(_classes);
  AimsData<double> th_sigma(_classes);
  AimsData<double> th_tau(bins, _classes);
  AimsData<double> aux(_classes);
  int n, i, j;
  
  // Initialisation of the EM algorithm
  double points_count = 0.;
  for (n = 0; n < bins; ++n)
    points_count += his.data()(n);
  
  // Same number of points for each EM class
  double class_points_count = 0.;
  for (n = 0; n < bins; ++n)
  {
    class_points_count += his.data()(n);
    for (i = 0; i < _classes; ++i)
    {
      if ((i <= (_classes * class_points_count / points_count)) 
           && (i >= (_classes * class_points_count / points_count) - 1))
        th_tau(n, i) = 1; 
      else
        th_tau(n, i) = 0;
    }
  }
  
  // MAIN interation
  int count = 0;
  double sum1 = 0, sum2 = 0, sum = 0, sum_aux;
  while ( count <= (_classes + 1) )
  {
    count++;

    // EM STEP E : calculate expect classification variable tau for each class 
    // the first time jump this step to initialize mu, sigma and pi.
    if (count > 1)
    {
      th_tau = 0.;
      for (n = 0; n < bins; ++n)
      {
        for (i = 0; i < _classes; ++i)
        {
          aux = 0.;
          sum_aux = 0.;
          for (j = 0; j < _classes; ++j)
          {
            aux(j) = th_pi(j) * (1 / sqrt(2 * 3.1415926 * th_sigma(j))) 
                     * exp(-(1 / (2 * th_sigma(j))) * (n - th_mu(j)) * (n - th_mu(j)));
            sum_aux += aux(j);
          }
          th_tau(n, i) = aux(i) / sum_aux;
        }
      }
    }

    // EM STEP M : calculate new threshold means and variances for classified data
    th_mu = 0.; th_sigma = 0.; th_pi = 0.;

    for (j = 0; j < _classes; ++j)
    {
      // Mean and covariance
      sum1 = 0.;
      sum2 = 0.;
      sum = 0.;
      for (n = 0; n < bins; ++n) {
        sum += th_tau(n, j) * his.data()(n);
        sum1 += th_tau(n, j) * n * his.data()(n);
      }
      th_mu(j) = sum1 / sum;

      for (n = 0; n < bins; ++n)
        sum2 += th_tau(n, j) * (n - th_mu(j)) * (n - th_mu(j)) * his.data()(n);
      th_sigma(j) = sum2 / sum;
    
      // Classifying variable pi
      sum1 = 0.;
      sum = 0.;
      for (n = 0; n < bins; ++n)
      {
        sum1 += th_tau(n, j) * his.data()(n);
        sum += his.data()(n);  
      }
      th_pi(j) = sum1 / sum;
    }
  }
  
  // Compute thresholds
  int level;
  for (j = 0; j < _classes; ++j)
  {
    level = (int)th_mu(j);
    
    if (level < 0)
      level = 0;
    
    else if (level > (bins - 1))
      level = bins - 1;
    
    thresholds_mu.push_back(level);
  }

  std::sort(thresholds_mu.begin(), thresholds_mu.end());
    
  if (carto::verbose) {
    std::cout << "Found class means : [";
    
    for (i = 0; i < _classes; ++i)
    {
      // Search for last minimum between class means
      std::cout << carto::toString((thresholds_mu[i] * (his.maxDataValue() - his.minDataValue() + 1) / bins) + his.minDataValue());
      if (i <  (_classes - 1))
        std::cout << ", ";
    }
    std::cout << "]" << std::endl << std::flush;
  }
  
  if (carto::verbose)
    std::cout << "Found thresholds : [";
  
  for (i = 0; i < (_classes - 1); ++i)
  {
    // Search for last minimum between class means
    level = thresholds_mu[i];
    for(int j = thresholds_mu[i]; j <= thresholds_mu[i + 1]; j++)
    {
      if (his.data()(j) < his.data()(level))
        level = j;
    }
    
    thresholds.push_back((level * (his.maxDataValue() - his.minDataValue() + 1) / bins) + his.minDataValue());
    
    if (carto::verbose) {
      std::cout << carto::toString(thresholds[i]);
      if (i <  (_classes - 2))
        std::cout << ", ";
    }
  }
  
  if (carto::verbose)
    std::cout << "]" << std::endl << std::flush;
  
  return thresholds;
}

template <class T, class U> inline
AimsData<T> AimsEMThreshold<T, U>::operator () (const AimsData<T> &image)
{
  // Threshold
  std::vector<T> thresholds = processEMThresholds(image);
  if (_levelindex < thresholds.size())
    AimsEMThreshold<T, U>::_level = thresholds[_levelindex];
  
  if (_level2index < thresholds.size())
    AimsEMThreshold<T, U>::_level2 = thresholds[_level2index];
  
//   std::cout << "Thresholding values ..." << std::endl << std::flush;
  AimsData<U> thresholded = AimsThreshold<T, U>::operator ()( image );
  
  return thresholded;
}

template <class T, class U> inline
AimsData<U> AimsEMThreshold<T, U>::bin(const AimsData<T> &image)
{
  // Threshold
  std::vector<T> thresholds = processEMThresholds(image);
  if (_levelindex < thresholds.size())
    AimsEMThreshold<T, U>::_level = thresholds[_levelindex];
  
  if (_level2index < thresholds.size())
    AimsEMThreshold<T, U>::_level2 = thresholds[_level2index];
  
  AimsData<U> thresholded = AimsThreshold<T, U>::bin( image );
  
  return thresholded;
}

#endif
