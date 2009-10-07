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

/*
 *  3 axis trieder class
 */
#ifndef AIMS_MATH_TRIEDER_H
#define AIMS_MATH_TRIEDER_H

#include <aims/config/aimsdata_config.h>
#include <aims/vector/vector.h>
#include <fstream>
#include <math.h>

class Trieder;

AIMSDATA_API int operator == (const Trieder& thing1, const Trieder& thing2);
AIMSDATA_API std::ostream& operator << (std::ostream& os, 
					const Trieder& thing);
AIMSDATA_API Point3df AimsRotationAngle( const Trieder& trieder );


class AIMSDATA_API Trieder
{
  public:
    Trieder()  { }
    Trieder( const Point3df& dirX, const Point3df& dirY, const Point3df& dirZ )
    { 
      _dir[ 0 ] = dirX;
      _dir[ 1 ] = dirY;
      _dir[ 2 ] = dirZ;
    }
    Trieder( const Point3df& angle );
    ~Trieder() { }

    const Point3df& dirX() const { return _dir[ 0 ]; }
    const Point3df& dirY() const { return _dir[ 1 ]; }
    const Point3df& dirZ() const { return _dir[ 2 ]; }
    const Point3df& operator []( int i ) const { return _dir[ i ]; }

    Point3df& dirX() { return _dir[ 0 ]; }
    Point3df& dirY() { return _dir[ 1 ]; }
    Point3df& dirZ() { return _dir[ 2 ]; }
    Point3df& operator []( int i ) { return _dir[ i ]; }

    float dot(const Trieder& other) const;

    friend
    int operator == (const Trieder& thing1, const Trieder& thing2);

    friend
    std::ostream& operator << (std::ostream& os, const Trieder& thing);

  protected:
    Point3df _dir[ 3 ];
};


//
//   ->   ->   ->
// [ e1 ; e2 ; e3 ] = Rz * Ry * Rx
//
//      [ 1       0           0       ]
// Rx = [ 0  cos(thetax) -sin(thetax) ]
//      [ 0  sin(thetax)  cos(thetax) ]
//
//      [ cos(thetay) 0  -sin(thetay) ]
// Ry = [      0      1       0       ]
//      [ sin(thetay) 0   cos(thetay) ]
//
//      [ cos(thetaz) -sin(thetaz)  0 ]
// Rz = [ sin(thetaz)  cos(thetaz)  0 ]
//      [      0            0       1 ]
//

inline
Trieder::Trieder( const Point3df& angle )
{
  _dir[ 0 ].item(0) = cos( angle.item(1) ) * cos( angle.item(2) );
  _dir[ 0 ].item(1) = cos( angle.item(1) ) * sin( angle.item(2) );
  _dir[ 0 ].item(2) = sin( angle.item(1) );

  _dir[ 1 ].item(0) = - sin( angle.item(0) ) * sin( angle.item(1) )
                  * cos( angle.item(2) ) 
                  - cos( angle.item(0) ) * sin( angle.item(2) );
  _dir[ 1 ].item(1) = - sin( angle.item(0) ) * sin( angle.item(1) )
                  * sin( angle.item(2) )
                  + cos( angle.item(0) ) * cos( angle.item(2) );
  _dir[ 1 ].item(2) = sin( angle.item(0) ) * cos( angle.item(1) );

  _dir[ 2 ].item(0) = - cos( angle.item(0) ) * sin( angle.item(1) )
                  * cos( angle.item(2) )
                  + sin( angle.item(0) ) * sin( angle.item(2) );
  _dir[ 2 ].item(1) = - cos( angle.item(0) ) * sin( angle.item(1) )
                  * sin( angle.item(2) )
                  - sin( angle.item(0) ) * cos( angle.item(2) );
  _dir[ 2 ].item(2) = cos( angle.item(0) ) * cos( angle.item(1) );
}


inline
float Trieder::dot(const Trieder& other) const
{
  return _dir[ 0 ].dot( other.dirX() ) +
         _dir[ 1 ].dot( other.dirY() ) +
         _dir[ 2 ].dot( other.dirZ() ) ;
}


inline
int operator == (const Trieder& thing1, const Trieder& thing2)
{
  return thing1.dirX() == thing2.dirX() &&
         thing1.dirY() == thing2.dirY() &&
         thing1.dirZ() == thing2.dirZ();
}


inline
std::ostream& operator << (std::ostream& os, const Trieder& thing)
{
  os << "{dirX=" << thing.dirX()
     << ",dirY=" << thing.dirY()
     << ",dirZ=" << thing.dirZ()
     << "}";
  return os;
}



#endif
