/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#include <math.h>
#include <aims/resampling/motion.h>
#include <aims/registration/transformation_d.h>


// using namespace aims;
using namespace std;



template AimsData<int8_t> Transformation::application(const AimsData<int8_t>& test,Point3df p);
template AimsData<uint8_t> Transformation::application(const AimsData<uint8_t>& test,Point3df p);
template AimsData<int16_t> Transformation::application(const AimsData<int16_t>& test,Point3df p);
template AimsData<uint16_t> Transformation::application(const AimsData<uint16_t>& test,Point3df p);
template AimsData<int32_t> Transformation::application(const AimsData<int32_t>& test,Point3df p);
template AimsData<uint32_t> Transformation::application(const AimsData<uint32_t>& test,Point3df p);
template AimsData<float> Transformation::application(const AimsData<float>& test,Point3df p);
template AimsData<double> Transformation::application(const AimsData<double>& test,Point3df p);




bool Transformation::testdelta(const Motion& p, const Motion& q )
{
  bool boule;  

  // Declaration des coins en 2D !!!!!!!!!  ATTENTION, COINS EN MM !!!!!
  Point3df corners[4] ;
  corners[0] = Point3df( 0., 0., 0. ) ;
  corners[1] = Point3df( _XX, 0., 0. ) ;
  corners[2] = Point3df( 0., _YY, 0. ) ;
  corners[3] = Point3df( _XX, _YY, 0. ) ;
  
  float delta=0;
  for ( int i = 0 ; i < 4 ; ++i )
    delta += ( p.transform( corners[i] ) - (q*p).transform(corners[i] ) ).norm() ;
  
  
  if ( (_iter>_iterMax) || (_deltaprev<delta) )
    {
      boule = false;		// On s'arrete la
    }
  else
    {
      _deltaprev=delta;
      _iter++;
      boule = true;		// On continue
    }

  return boule;
} 





Point3df Transformation::composition(Point3df p,Point3df q)
{
  
  //mise a jour de la transfo globale p
  
  Point3df pp;
  pp[2]=p[2]+q[2];
  float angle_rad = q[2] / 180.0 * M_PI;
  float c = cos( angle_rad );
  float s = sin( angle_rad );
  pp[1]=c*p[1]-s*p[0]+q[1];
  pp[0]=s*p[1]+c*p[0]+q[0];
  
  
  return pp;
}
