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



#ifndef AIMS_PERFUSION_PERFAIFPOINTS_H
#define AIMS_PERFUSION_PERFAIFPOINTS_H

#include <aims/perfusion/perfProcessing.h>
#include <aims/perfusion/perfParam.h>

#include <aims/data/data.h>
#include <aims/bucket/bucket_g.h>

#include <list>


namespace aims
{

  //! Class PerfusionAifPoints
  /*!
    This class provides functions to automatically determine some Arterial 
    Input Functions (AIF).

    It processes as follows:

    -# Compute a volume whose points are the maximum value found during their
    time evolution
    -# Create a sorted list of points that have the maximum amplitude 
    variations with the earliest occurrence
    -# Keep the best values
  */
  class PerfusionAifPoints : public PerfusionProcessing
  {
  public:

    //! Constructor
    /*!
      The constructor initializes the skip value for the volume sequence
      and the number of AIF possible points to keep
      \param s is the skip value determine by PerfusionSkip
      \param na is the desired number of value to keep
    */
    PerfusionAifPoints() : PerfusionProcessing( 3 ) {  }

    //! Look for the best AIF possible points
    /*!
      \param d is the input volume sequence
      \param mask is the brain mask computed by PerfusionMask
      \return the list of possible AIF points
    */
    std::list< Point4dl > search( AimsData< short >& d, 
				 aims::BucketMap< Void >& mask,
				 PerfusionParameters& );
  };

}

#endif
