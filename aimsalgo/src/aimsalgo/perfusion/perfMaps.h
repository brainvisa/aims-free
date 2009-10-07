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



#ifndef AIMS_PERFUSION_PERFMAPS_H
#define AIMS_PERFUSION_PERFMAPS_H

#include <aims/perfusion/perfProcessing.h>
#include <aims/perfusion/perfMapBase.h>

#include <aims/data/data.h>
#include <aims/bucket/bucket.h>

namespace aims
{
  //! Class PerfusionMaps
  /*!
    This class provides functions to compute the perfusion maps
    (i.e. CBV, CBF, MTT and so on).
  */
  class PerfusionMaps : public PerfusionProcessing
  {
  public:

    PerfusionMaps();

    AimsData< float >& mapCBF();
    AimsData< float >& mapCBV();
    AimsData< float >& mapMTT();
    AimsData< float >& mapTTP();
    AimsData< float >& mapDelay();
    AimsData< float >& maph();
    AimsData< float >& mapBBB();

    PerfusionMapBase *mapBase( int i ) { return mCalc[ i ]; }

    void setState( int, bool );

    void saveMaps( const std::string& );

    bool doit( AimsData< float >& q, AimsData< float >& d, 
	       AimsData< float >& aif, AimsData< float >& r, 
	       aims::BucketMap< Void >& mask, LMGamma< float >&, 
	       std::map< Point3d, LMGamma< float >, bestPoint3d >&,
	       PerfusionParameters& pp );

  private:

    std::map< int, PerfusionMapBase * > mCalc;
  };

}

#endif
