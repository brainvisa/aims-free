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


#ifndef AIMS_PERFUSION_PERFMAPBASE_H
#define AIMS_PERFUSION_PERFMAPBASE_H

#include <aims/perfusion/perfFitStgy.h>
#include <aims/perfusion/perfParam.h>

#include <list>
#include <string>

namespace aims
{

  class PerfusionMapBase
  {
  public:

    enum MapType
      {
	cbv,
	cbf,
	mtt,
	ttp,
	delay,
	h,
	bbb
      };

    PerfusionMapBase( MapType id=(MapType)0 ) : _id( id ), m_state( false ) { }
    virtual ~PerfusionMapBase() { }

    void setState( bool st ) { m_state = st;  }

    std::list< int >& dependencies() { return _deps; }
    std::list< int >& backDependencies() { return _backdeps; }

    int id() { return (int)_id; }
    bool isChecked() { return m_state; }

    AimsData< float >& map() { return m_map; }

    virtual void saveMap( const std::string& ) { }

    virtual 
    AimsData< float >& doit( AimsData< float >&, 
			     AimsData< float >&,
			     AimsData< float >&,
			     AimsData< float >&,
			     aims::BucketMap< Void >&, 
			     LMGamma< float >&,
			     std::map< Point3d, LMGamma< float >, 
			     bestPoint3d >&,
			     PerfusionParameters&,
			     AimsData< float > *,
			     AimsData< float > * ) 
    { return m_map = AimsData< float >(); }

  protected:

    std::list< int > _deps;
    std::list< int > _backdeps;
    AimsData< float > m_map;

    void doSave( const std::string& );

  private:

    MapType _id;
    bool m_state;
  };

}

#endif
