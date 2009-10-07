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

#ifndef AIMS_TRANSFORM_RIGIDESTIMATION_H
#define AIMS_TRANSFORM_RIGIDESTIMATION_H

#include <aims/vector/vector.h>
#include <aims/data/data.h>
#include <vector>

class Motion ;

namespace aims {
  
  class RigidTransformEstimation {
  public:
    RigidTransformEstimation() ;
    RigidTransformEstimation( std::vector<Point3df> from, std::vector<Point3df> to, 
			      bool looseCondition = true ) ;
    
    ~RigidTransformEstimation() ;
    
    void setAppariatedPoints( std::vector<Point3df> from, std::vector<Point3df> to ) ;
    void setLooseCondition( bool looseCondition ) ;
    
    bool motion(Motion&) ;
    
    static Point3df cross( const Point3df& u, const Point3df& v ) ;
    void rotationEstimation( const Point3df& u1, const Point3df& u2,
			     const Point3df& n1, const Point3df& n2,
			     Point3df& axis, float& ang ) ;

    static AimsData<float> criterionItem( const Point3df& p1, const Point3df& p2, 
					  const Point3df& gc1, const Point3df& gc2, float weight ) ;
  private:
    std::vector<Point3df> _pointsFrom ;
    std::vector<Point3df> _pointsTo ;
    bool _looseCondition ;
    
    
    Motion * _motion ;
    bool _motionCalculated ;
	 bool _is2D ;
    
    void looseEstimation() ;
    void pointToPointEstimation() ;
  } ;
}

#endif
