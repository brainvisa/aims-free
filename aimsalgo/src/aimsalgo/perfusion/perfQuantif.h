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



#ifndef AIMS_PERFUSION_PERFQUANTIF_H
#define AIMS_PERFUSION_PERFQUANTIF_H

#include <aims/perfusion/perfProcessing.h>
#include <aims/perfusion/perfParam.h>

#include <aims/data/data.h>
#include <aims/bucket/bucket_g.h>

namespace aims
{

  //! Class PerfusionQuantification
  /*!
    This class performes the MR image quantification according to the MR
    information by the formula:

    d(t) = -ln( S(t) / S0 ) / Te.

    The quantification is only performed on the brain mask.
  */
  class PerfusionQuantification : public PerfusionProcessing
  {
  public:

    //! Constructor
    /*!
      The constructor does nothing.
    */
    PerfusionQuantification() : PerfusionProcessing( 5 )  { }

    //! Compute the quantification
    /*!
      \param d is the input volume sequence
      \param mask is the brain mask computed by PerfusionMask
      \param te is the echo time
      \param preinj is the preinjection time determine by PerfusionPreInjection
      \return is the quantified volume sequence
    */
    AimsData< float > doit( AimsData< short >& d, 
			    aims::BucketMap< Void >& mask,
			    PerfusionParameters& pp );
  };

}

#endif
