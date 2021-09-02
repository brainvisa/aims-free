/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef AIMS_REGISTRATION_TRANSFORMATION_D_H
#define AIMS_REGISTRATION_TRANSFORMATION_D_H


#include <aims/transformation/affinetransformation3d.h>
#include <aims/registration/transformation.h>
#include <iostream>
#include <math.h>


template <class T>
carto::VolumeRef<T> Transformation::application(
  const carto::rc_ptr<carto::Volume<T> > & test, Point3df p )
{
  
  float angle_rad = p[2] /180.0 * M_PI;
  float c = cos( angle_rad );
  float s = sin( angle_rad );
  carto::VolumeRef<T>    testtrans( test->getSizeX(), test->getSizeY() );

  int x,y, nx = testtrans->getSizeX(), ny = testtrans->getSizeY();

  for( y=0; y<ny; ++y )
    for( x=0; x<nx; ++x )
    {
      testtrans(x,y)      = T( 195 );

      if( ((_cx + c*(x-_cx) + s*(y-_cy) - p[1]) < nx)
          && ((_cx + c*(x-_cx) + s*(y-_cy) - p[1]) >= 0)
          && ((_cy - s*(x-_cx) + c*(y-_cy) - p[0]) < ny)
          && ((_cy - s*(x-_cx) + c*(y-_cy) - p[0]) >= 0) )
        testtrans(x,y)	= test->at(
          unsigned( _cx + c*(x-_cx) + s*(y-_cy) - p[1]),
          unsigned( _cy - s*(x-_cx) + c*(y-_cy)-p[0]) ) ;
    }

  return testtrans;
} 


#endif

