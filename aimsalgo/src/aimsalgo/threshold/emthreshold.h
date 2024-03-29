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
                     int level2index = 1, /* Level expressed as index */ 
                     int bins = 0,
                     int binstart = 0, /* First bin considered during thresholding process */
                     int binend = 0 /* Last bin considered during thresholding process */
                   );

    virtual ~AimsEMThreshold() {}

    inline
    std::vector<T> processEMThresholds(const carto::VolumeRef<T> &image);

    /// Return the multi-level thresholded image
    inline carto::VolumeRef<U> operator () (const carto::VolumeRef<T> &image);
  
    /// Return the binary thresholded image
    inline
    carto::VolumeRef<U> bin(const carto::VolumeRef<T> &image);
  
  private :
    int _classes;
    int _levelindex;
    int _level2index;
    int _bins;
    int _binstart;
    int _binend;
};

template <class T, class U> inline
    AimsEMThreshold<T, U>::AimsEMThreshold( threshold_t type,
                                            T backgd, float foregd,
                                            int classes,
                                            int levelindex, int levelindex2,
                                            int bins,
                                            int binstart,
                                            int binend )
  : _classes(classes), 
    _levelindex(levelindex),
    _level2index(levelindex2), 
    _bins(bins),
    _binstart(binstart),
    _binend(binend),
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
//     std::cout << "Bins " << carto::toString(_bins) << " ..." << std::endl << std::flush;
  }
}
// EM
template <class T, class U> inline
    std::vector<T> AimsEMThreshold<T, U>::processEMThresholds(
      const carto::VolumeRef<T> &image )
{
  std::vector<T> thresholds, thresholds_mu;
  int bins = _bins;
  int binend;

  
  T maxi = image.max(), mini = image.min();
  if ( bins <= 0 )
    bins = int(maxi - mini);
  
  if (!_binend)
    binend = bins;
  else
    binend = _binend + 1;
  
  if (carto::verbose)
    std::cout << "Bins " << carto::toString(bins) << " ..." << std::endl << std::flush;
  
  // Histogram
  aims::RegularBinnedHistogram<T> his(bins);
  his.doit(image);

  // Declarations
  carto::VolumeRef<double> th_pi(_classes);
  carto::VolumeRef<double> th_mu(_classes);
  carto::VolumeRef<double> th_sigma(_classes);
  carto::VolumeRef<double> th_tau(bins, _classes);
  carto::VolumeRef<double> aux(_classes);
  int n, i, j;
  
  // Initialisation of the EM algorithm
  double scl = (double) bins / (double)( maxi - mini );
//   std::cout << "scl : " << carto::toString(scl) << std::endl << std::flush;
  double points_count = 0.;
  for (n = _binstart; n < binend; ++n)
    points_count += his.data()(n);

//   std::cout << "points_count : " << carto::toString(points_count) << std::endl << std::flush;
  // Same number of points for each EM class
  double class_points_count = 0.;
  for (n = _binstart; n < binend; ++n)
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
      for (n = _binstart; n < binend; ++n)
      {
        for (i = 0; i < _classes; ++i)
        {
          aux = 0.;
          sum_aux = 0.;
          for (j = 0; j < _classes; ++j)
          {
            aux(j) = th_pi(j) * (1 / sqrt(2 * M_PI * th_sigma(j))) 
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
      for (n = _binstart; n < binend; ++n) {
        sum += th_tau(n, j) * his.data()(n);
        sum1 += th_tau(n, j) * n * his.data()(n);
      }
      th_mu(j) = sum1 / sum;

      for (n = _binstart; n < binend; ++n)
        sum2 += th_tau(n, j) * (n - th_mu(j)) * (n - th_mu(j)) * his.data()(n);
      th_sigma(j) = sum2 / sum;
    
      // Classifying variable pi
      sum1 = 0.;
      sum = 0.;
      for (n = _binstart; n < binend; ++n)
      {
        sum1 += th_tau(n, j) * his.data()(n);
        sum += his.data()(n);  
      }
      th_pi(j) = sum1 / sum;
    }
  }
  
  // Compute thresholds
  T level;
  for (j = 0; j < _classes; ++j)
  {
    level = (T)th_mu(j);
    
    if (level < 0)
      level = 0;
    
    else if (level > (binend - 1))
      level = binend - 1;
    
    thresholds_mu.push_back(level);
  }

  std::sort(thresholds_mu.begin(), thresholds_mu.end());
    
  if (carto::verbose) {
    std::cout << "Found class means : [";
    
    for (i = 0; i < _classes; ++i)
    {
      // Search for last minimum between class means
      std::cout << carto::toString((T)(thresholds_mu[i] / scl) + mini);
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
    
    thresholds.push_back(((T)(level / scl) + mini));
    
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
carto::VolumeRef<U> AimsEMThreshold<T, U>::operator () (
  const carto::VolumeRef<T> &image)
{
  // Threshold
  std::vector<T> thresholds = processEMThresholds(image);
  if (_levelindex < thresholds.size())
    AimsEMThreshold<T, U>::_level = thresholds[_levelindex];
  
  if (_level2index < thresholds.size())
    AimsEMThreshold<T, U>::_level2 = thresholds[_level2index];
  
//   std::cout << "Thresholding values ..." << std::endl << std::flush;
  carto::VolumeRef<U> thresholded = AimsThreshold<T, U>::operator ()( image );
  
  return thresholded;
}

template <class T, class U> inline
carto::VolumeRef<U> AimsEMThreshold<T, U>::bin(
  const carto::VolumeRef<T> &image)
{
  // Threshold
  std::vector<T> thresholds = processEMThresholds(image);
  if (_levelindex < thresholds.size())
    AimsEMThreshold<T, U>::_level = thresholds[_levelindex];
  
  if (_level2index < thresholds.size())
    AimsEMThreshold<T, U>::_level2 = thresholds[_level2index];
  
  carto::VolumeRef<U> thresholded = AimsThreshold<T, U>::bin( image );
  
  return thresholded;
}

#endif
