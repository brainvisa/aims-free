
#include <aims/math/trieder.h>
#include <aims/math/mathelem.h>
#include <math.h>


AIMSDATA_API Point3df AimsRotationAngle( const Trieder& trieder )
{ 
  // Y axis
#if defined(__GNUC__) && defined (__sun__)
  float theta_y_1 = ::asin( trieder.dirX().item(2) ); 
#else
  float theta_y_1 = asin( trieder.dirX().item(2) ); 
#endif
  float theta_y_2 = PI - theta_y_1;

  // X axis
#if defined(__GNUC__) && defined (__sun__)
  float theta_x_1_1 = ::asin( trieder.dirY().item(2) / cos( theta_y_1 ) );
#else
  float theta_x_1_1 = asin( trieder.dirY().item(2) / cos( theta_y_1 ) );
#endif
  float theta_x_1_2 = PI - theta_x_1_1;

#if defined(__GNUC__) && defined (__sun__)
  float theta_x_2_1 = ::asin( trieder.dirY().item(2) / cos( theta_y_2 ) );
#else
  float theta_x_2_1 = asin( trieder.dirY().item(2) / cos( theta_y_2 ) );
#endif
  float theta_x_2_2 = PI - theta_x_2_1;

  // Z axis
#if defined(__GNUC__) && defined (__sun__)
  float theta_z_1_1 = ::asin( trieder.dirX().item(1) / cos( theta_y_1 ) );
#else
  float theta_z_1_1 = asin( trieder.dirX().item(1) / cos( theta_y_1 ) );
#endif
  float theta_z_1_2 = PI - theta_z_1_1;

#if defined(__GNUC__) && defined (__sun__)
  float theta_z_2_1 = ::asin( trieder.dirX().item(1) / cos( theta_y_2 ) );
#else
  float theta_z_2_1 = asin( trieder.dirX().item(1) / cos( theta_y_2 ) );
#endif
  float theta_z_2_2 = PI - theta_z_2_1;

  Trieder tmp;
  Point3df angle,bestAngle;
  float dotProduct=-1e38,tp=0;

  angle = Point3df( theta_x_1_1, theta_y_1, theta_z_1_1 );
  tmp = Trieder( angle );
  if ( ( tp = tmp.dot( trieder ) ) > dotProduct )
  {
    dotProduct = tp;
    bestAngle = angle;
  }
    
  angle = Point3df( theta_x_1_1, theta_y_1, theta_z_1_2 );
  tmp = Trieder( angle );
  if ( ( tp = tmp.dot( trieder ) ) > dotProduct )
  {
    dotProduct = tp;
    bestAngle = angle;
  }

  angle = Point3df( theta_x_1_2, theta_y_1, theta_z_1_1 );
  tmp = Trieder( angle );
  if ( ( tp = tmp.dot( trieder ) ) > dotProduct )
  {
    dotProduct = tp;
    bestAngle = angle;
  }

  angle = Point3df( theta_x_1_2, theta_y_1, theta_z_1_2 );
  tmp = Trieder( angle );
  if ( ( tp = tmp.dot( trieder ) ) > dotProduct )
  {
    dotProduct = tp;
    bestAngle = angle;
  }

  angle = Point3df( theta_x_2_1, theta_y_2, theta_z_2_1 );
  tmp = Trieder( angle );
  if ( ( tp = tmp.dot( trieder ) ) > dotProduct )
  {
    dotProduct = tp;
    bestAngle = angle;
  }

  angle = Point3df( theta_x_2_1, theta_y_2, theta_z_2_2 );
  tmp = Trieder( angle );
  if ( ( tp = tmp.dot( trieder ) ) > dotProduct )
  {
    dotProduct = tp;
    bestAngle = angle;
  }

  angle = Point3df( theta_x_2_2, theta_y_2, theta_z_2_1 );
  tmp = Trieder( angle );
  if ( ( tp = tmp.dot( trieder ) ) > dotProduct )
  {
    dotProduct = tp;
    bestAngle = angle;
  }

  angle = Point3df( theta_x_2_2, theta_y_2, theta_z_2_2 );
  tmp = Trieder( angle );
  if ( ( tp = tmp.dot( trieder ) ) > dotProduct )
  {
    dotProduct = tp;
    bestAngle = angle;
  }

  return bestAngle;
}
