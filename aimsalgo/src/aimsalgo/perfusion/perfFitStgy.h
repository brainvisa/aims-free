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


#ifndef AIMS_PERFUSION_PERFFITSTGY_H
#define AIMS_PERFUSION_PERFFITSTGY_H


#include <aims/data/data.h>
#include <aims/bucket/bucket_g.h>
#include <aims/vector/vector.h>
#include <aims/optimization/lmgamma.h>
#include <map>

namespace aims
{

  struct bestPoint3d
  {
    bool operator()( const Point3d p1, const Point3d p2 ) const
    {
      if ( p1[0] == p2[0] )
	{
	  if ( p1[1] == p2[1] )  return p1[2] < p2[2];
	  else return p1[1] < p2[1];
	}
      else return p1[0] < p2[0];
    }
  }; 

  class PerfusionFitStrategy
  {
  public:

    PerfusionFitStrategy() { }
    virtual ~PerfusionFitStrategy() { }
  
    virtual AimsData< float > 
    doit( AimsData< float >&, aims::BucketMap< Void >&, 
	  int, std::map< Point3d, LMGamma< float >, bestPoint3d >& )
    { return AimsData< float >(); }
  };

}

#endif
