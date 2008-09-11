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



#ifndef AIMS_PERFUSION_PERFMASK_H
#define AIMS_PERFUSION_PERFMASK_H

#include <aims/perfusion/perfProcessing.h>
#include <aims/perfusion/perfParam.h>

#include <aims/data/data.h>
#include <aims/bucket/bucket_g.h>

namespace aims
{

  //! class PerfusionMask
  /*!
    This class provides functions to determine a brain mask from the
    MRI perfusion images. 

    The mask is obtained as follows:

    -# 3D gaussian filtering of the last volume of the sequence with
    a standard deviation sigma
    -# Binarization with a threshold value equal to (100*thres)% of the maximum
    amplitude of the filtered volume
    -# Conversion of the result to a bucket form (i.e. a list of 3D points)
  */
  class PerfusionMask : public PerfusionProcessing
  {
  public:

    //! Constructor
    /*!
      The constructor initializes the gaussian standard deviation and the
      threshold value
      \param s is the floating sigma value
      \param t is the floating threshold value given as a multiplicative
      factor of the maximum amplitude of the filtered volume
    */
    PerfusionMask() : PerfusionProcessing( 2 ) { }

    //! Compute the brain mask
    /*!
      \param d is the input volume sequence
      \return the list of points contained in the mask
    */
    aims::BucketMap< Void > doit( AimsData< short >& d, PerfusionParameters& );
  };

}

#endif
