
#ifndef AIMS_MATH_GEOMETRY_H
#define AIMS_MATH_GEOMETRY_H

#include <stdlib.h>

template <class T,int D> class AimsVector;
class Trieder;


/**@name 2D geometry on lines
*/
//@{

/**@name Angle between 2 lines in radians.
         Get the angle between two lines which you give one direction vector.
         The returned value is expressed in radians and is between 0 and PI/2.
         @param v1 the direction of the first line
         @param v2 the direction of the second line
*/
//@{
///
float Aims2VectorsAngle( const AimsVector<float,3>& v1,
                         const AimsVector<float,3>& v2 );
//@}

/**@name Angle between 2 vectors in radians.
         Get the angle between two vectors.
         The returned value is expressed in radians and is between 0 and PI.
         @param v1 the first vector
         @param v2 the second vector
*/
//@{
///
float Aims2LinesAngle( const AimsVector<float,3>& v1,
                       const AimsVector<float,3>& v2 );
//@}

//@}


/**@name 3D geometrical objects.
*/
//@{

/**@name Generating trieder.
         Get a random orthonormal trieder from one of its axis.
         @param v1 one axis of the trieder
*/
//@{
///
Trieder AimsOrthonormalTrieder(const AimsVector<float,3>& v1);
//@}


/**@name Cylinder.
         Return true if a point is inside a cylinder.
         @param p the point to test
         @param p0 a central point of one of both bases of the cylinder
         @param dir unit direction or axis of the cylinder 
                    (direction is important)
         @param radius radius of the cylinder
         @param length length of the cylinder
*/
//@{
///
bool AimsIsInCylinder(const AimsVector<float,3>& p,
                      const AimsVector<float,3>& p0,
                      const AimsVector<float,3>& dir,
                      float radius,float length);
//@}


/**@name Torus.
         Return true if a point is inside a torus.
         @param p the point to test
         @param p0 center of the torus
         @param axe main axis of the torus
         @param Radius greatest radius of the torus
         @param radius smallest radius of the torus
         @param dir pointer where the unit vector of the direction at point p is
                stored
*/
//@{
///
bool AimsIsInTorus(const AimsVector<float,3>& p,
                   const AimsVector<float,3>& p0,
                   const AimsVector<float,3>& axe,
                   float Radius,float radius,
                   AimsVector<float,3>* dir=NULL);
//@}


/**@name Cube.
         Return true if a point is inside a cube.
         @param p the point to test
         @param p0 the inferior vertex
         @param p1 the superior vertex
*/
//@{
///
bool AimsIsInCube(const AimsVector<float,3>& p,
                  const AimsVector<float,3>& p0,
                  const AimsVector<float,3>& p1);
//@}


/**@name Parallelepipede.
         Return true if a point is inside a parallelepipede.
         @param p the point to test
         @param p0 one of the different verteces of the parallelepipede
         @param v1 one unit vector that is an edge of the parallelepipede
         @param v2 another perpendicular unit vector that is an edge of the 
                   parallelepipede
         @param L1 length of the first edge associated to v1
         @param L2 length of the second edge associated to v2
         @param L3 length of the third edge associated to v1^v2
*/
//@{
///
bool AimsIsInParallelepipede(const AimsVector<float,3>& p,
                             const AimsVector<float,3>& p0,
                             const AimsVector<float,3>& v1,
                             const AimsVector<float,3>& v2,
                             float L1,float L2,float L3);
//@}

//@}

#endif
