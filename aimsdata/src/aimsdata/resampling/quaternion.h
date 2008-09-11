/* Copyright (c) 1995-2005 CEA
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


#ifndef AIMS_RESAMPLING_QUATERNION_H
#define AIMS_RESAMPLING_QUATERNION_H


#include <aims/vector/vector.h>
#include <aims/data/data.h>

class Motion;

namespace aims
{

  class Quaternion
  {
  public:
    friend Quaternion operator * ( const Quaternion &, const Quaternion & );

    Quaternion();
    Quaternion( const Point4df & q );
    Quaternion( const Quaternion & q );
    Quaternion( float x, float y, float z, float t );
    ~Quaternion();

    Quaternion & operator = ( const Quaternion & q );
    const Point4df & vector() const { return( _vector ); }
    void setVector( const Point4df & vec );
    ///	in OpenGL style: in columns
    AimsVector<float,16> rotationMatrix() const;
    AimsVector<float,16> inverseRotationMatrix() const;
    /// 4x4 matrix in columns (OpenGL-style)
    void buildFromMatrix( const float* m );
    void buildFromMotion( const Motion & m );
    /// Rotates vecotor p
    Point3df apply( const Point3df & p ) const;
    Point3df applyInverse( const Point3df & p ) const;

    Quaternion & operator += ( const Quaternion & q );
    Quaternion & operator *= ( const Quaternion & q );
    void norm();
    Quaternion normalized() const;
    void fromAxis( const Point3df & c, float phi );
    Quaternion inverse() const;
    Point3df axis() const;
    float angle() const;

    static float dot( const Quaternion & a, const Quaternion & b )
    { return( dot( a._vector, b._vector ) ); }
    static float dot( const Point4df & a, const Point4df & b );
    static Point4df cross( const Point4df & a, const Point4df & b );
    static Point3df norm( const Point3df & a );

  protected:
    Point4df	_vector;
  };


  Quaternion operator + ( const Quaternion & a, const Quaternion & b );
  Quaternion operator * ( const Quaternion & a, const Quaternion & b );

}


#endif
