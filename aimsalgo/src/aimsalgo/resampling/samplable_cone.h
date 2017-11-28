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


#ifndef AIMS_RESAMPLING_SAMPLABLE_CONE_H
#define AIMS_RESAMPLING_SAMPLABLE_CONE_H

#include <aims/resampling/samplable.h>

namespace aims
{

  class ConeSamplable : public aimsalgo::Samplable<float, 3>
  {
  public:
    ConeSamplable( const Point3df & arrow, const Point3df & base, 
                   float radius );
    virtual ~ConeSamplable() {}
    virtual bool contains( AimsVector<float,3> & vector )
    {
      // std::cout << "ConeSamplable::contains " << vector << std::endl;
      float pos = vector.dot( _direction );
      if( pos >= _proj0 and pos <= _proj1 )
      {
        float rmax = (pos - _proj0) / (_proj1 - _proj0) * _radius;
        float r = ( vector - _cax0 - _direction * pos ).norm();
        if( r <= rmax )
          return true;
      }
      return false;
    }

  private:
    Point3df _arrow;
    Point3df _base;
    float    _radius;

    Point3df _direction;
    float    _proj0;
    float    _proj1;
    Point3df _cax0;
  };


  ConeSamplable::ConeSamplable( const Point3df & arrow, 
                                const Point3df & base, float radius )
  : aimsalgo::Samplable<float, 3>(), _arrow( arrow ), _base( base ), 
    _radius( radius )
  {
    _direction = base - arrow;
    _direction /= _direction.norm();
    _proj0 = _direction.dot( arrow );
    _proj1 = _direction.dot( base );
    _cax0 = _arrow - _direction * _proj0;
  }

}

#endif

