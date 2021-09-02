/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#ifndef AIMS_REGISTRATION_TRANSFORMATION_H
#define AIMS_REGISTRATION_TRANSFORMATION_H

#include <cartodata/volume/volume.h>

namespace aims
{
  class AffineTransformation3d;
}


class Transformation
{
 public:
  Transformation() {};
  ~Transformation() {};
  
  template <class T>
  carto::VolumeRef<T> application( const carto::rc_ptr<carto::Volume<T> > &,
                                   Point3df );
  
  
  float getcx() {return _cx;}
  float getcy() {return _cy;}
  float getcz() {return _cz;}
  int getiter() {return _iter;}
  
  
  void setcx(float cx) { _cx=cx; }
  void setcy(float cy) { _cy=cy; }
  void setcz(float cz) { _cz=cz; }
  void setX(int X) { _XX=X; }
  void setY(int Y) { _YY=Y; }
  void setZ(int Z) { _ZZ=Z; }
  void raziter() { _iter=1; }
  void setiterMax(int max) {_iterMax = max;}
  void setdeltaprev(float d) { _deltaprev=d; }
    
  
  bool testdelta(const aims::AffineTransformation3d&,
                 const aims::AffineTransformation3d& );
  
  Point3df composition(Point3df,Point3df);
  
  
 private:
  
  float _cx;
  float _cy;
  float _cz;
  float _deltaprev;
  int _XX;			// taille en X de l'image en mm
  int _YY;			// taille en Y de l'image en mm
  int _ZZ;			// taille en Z de l'image en mm
  int _iter;
  int _iterMax;
  
};

#endif

