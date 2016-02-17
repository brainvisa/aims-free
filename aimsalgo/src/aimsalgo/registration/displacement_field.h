/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */
#ifndef AIMS_REGISTRATION_DISPLACEMENT_FIELD_H
#define AIMS_REGISTRATION_DISPLACEMENT_FIELD_H

#include <cartobase/type/limits.h>
#include <aims/data/data_g.h>
#include <aims/math/math_g.h>
#include <vector>
#include <aims/pyramid/pyramid_g.h>  
#include <aims/registration/scale_control.h>

template <class T>
class DisplacementField
{
 public:
  DisplacementField() {}
  ~DisplacementField() {}
  
  
  void init(AimsData<T>& ref, ScaleControl& scaleControl, T * seuils);
  AimsData< Point3d > getField( AimsData<T>& test);
  std::vector<Point3df> getpointstest() {return  _pointstest;}
  std::vector<Point3df> getpointsref() {return  _pointsref;}
  double* getbarytest() {return _barytest;}
  double* getbaryref() {return _baryref;}
  AimsData< int >* getOffset() { return &_offset;}
  int getLevel() {return _level;}
  Point3d getDimRef() {return _dimRef;}
  Point3df getSizeRef() {return _sizeRef;}  
  AimsData<T> getQuality( AimsData<T>& testtrans, AimsData<T>& ref, 
							  				  int level, T thresh = std::numeric_limits<T>::max() );
  
 private:
  double _cost;
  int    _tx, _ty;
  std::vector<Point3df> _pointstest;
  std::vector<Point3df> _pointsref;
  double _barytest[3];
  double _baryref[3];
  
  
  void selectBlock(const AimsData<T>& image, double cV);
  AimsData< T > _pyrRef;
  AimsData< T > _pyrTest;
  int               _level;
  int               _nx, _ny, _nz, _nz23d;
  AimsData<int>   _offset;
  AimsData<int>   _offset2;
  T _lthr, _hthr, _ltht, _htht;
  
  std::multimap<float, Point3d> _sel;
  std::multimap<float, double>   _moy;
  double  _selThresh;
  double _seuilCorrel;

  Point3d _dimRef;
  Point3df _sizeRef;
};
#endif
