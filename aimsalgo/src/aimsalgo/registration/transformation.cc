/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/registration/transformation_d.h>
#include <math.h>


using namespace aims;
// using namespace carto; // cause namespace conflict for Transformation
using namespace std;



template carto::VolumeRef<int8_t> Transformation::application(const carto::rc_ptr<carto::Volume<int8_t> > & test,Point3df p);
template carto::VolumeRef<uint8_t> Transformation::application(const carto::rc_ptr<carto::Volume<uint8_t> > & test,Point3df p);
template carto::VolumeRef<int16_t> Transformation::application(const carto::rc_ptr<carto::Volume<int16_t> > & test,Point3df p);
template carto::VolumeRef<uint16_t> Transformation::application(const carto::rc_ptr<carto::Volume<uint16_t> > & test,Point3df p);
template carto::VolumeRef<int32_t> Transformation::application(const carto::rc_ptr<carto::Volume<int32_t> > & test,Point3df p);
template carto::VolumeRef<uint32_t> Transformation::application(const carto::rc_ptr<carto::Volume<uint32_t> > & test,Point3df p);
template carto::VolumeRef<float> Transformation::application(const carto::rc_ptr<carto::Volume<float> > & test,Point3df p);
template carto::VolumeRef<double> Transformation::application(const carto::rc_ptr<carto::Volume<double> > & test,Point3df p);




bool Transformation::testdelta(const AffineTransformation3d & p,
                               const AffineTransformation3d & q )
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





Point3df Transformation::composition(Point3df p, Point3df q)
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
