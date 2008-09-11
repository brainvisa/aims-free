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

/*
 *  Get the 3 angles defined by a trieder.
 */
#include <aims/math/trieder.h>
#include <aims/math/mathelem.h>
#include <math.h>


AIMSDATA_API Point3df AimsRotationAngle( const Trieder& trieder )
{ 
  // Y axis
  float theta_y_1 = asin( trieder.dirX().item(2) ); 
  float theta_y_2 = M_PI - theta_y_1;

  // X axis
  float theta_x_1_1 = asin( trieder.dirY().item(2) / cos( theta_y_1 ) );
  float theta_x_1_2 = M_PI - theta_x_1_1;

  float theta_x_2_1 = asin( trieder.dirY().item(2) / cos( theta_y_2 ) );
  float theta_x_2_2 = M_PI - theta_x_2_1;

  // Z axis
  float theta_z_1_1 = asin( trieder.dirX().item(1) / cos( theta_y_1 ) );
  float theta_z_1_2 = M_PI - theta_z_1_1;

  float theta_z_2_1 = asin( trieder.dirX().item(1) / cos( theta_y_2 ) );
  float theta_z_2_2 = M_PI - theta_z_2_1;

  Trieder tmp;
  Point3df angle,bestAngle;
  float dotProduct=-1e38,tp;

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
    //dotProduct = tp;
    bestAngle = angle;
  }

  return bestAngle;
}
