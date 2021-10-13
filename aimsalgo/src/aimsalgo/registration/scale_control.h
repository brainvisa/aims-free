/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */
#ifndef AIMS_REGISTRATION_SCALE_CONTROL_H
#define AIMS_REGISTRATION_SCALE_CONTROL_H

#include <cartodata/volume/volume.h>


class ScaleControl 
{
 public:
  ScaleControl() {};
  ~ScaleControl() {};

  template <class T>
  void init(const carto::rc_ptr<carto::Volume<T> > ref, int level_start, int level_stop, double cutVar, double stopVar, double
seuilCorrel, const Point3d& tailleBloc = Point3d(4, 4, 4) );
  bool minReached();

  int getN() {return _NN;}
  int getDelta1() {return _delta1;}
  int getDelta2() {return _delta2;}
  int getScale()  { return _currentScale;}
  int getlevel_stop()  { return _level_stop;}
  void nextScale();
  void setcutVar(double cV) {_cutVar = cV;}
  double getcutVar() {return _cutVar;}
  double getstopVar() {return _stopVar;}
  double getSeuilCorrel() {return _seuilCorrel;}
  Point3d getTailleBloc() {return _tailleBloc;}
  bool goOn() {return ( (_currentScale>_level_stop - 1) ? true : false ) ;}

 private:
  int _currentScale;
  int _level_stop;
  int _delta1;
  int _delta2;
  int _NN;
  bool _derniere;
  double	_cutVar;
  double _stopVar;
  double _seuilCorrel;
  Point3d _tailleBloc;
};

#endif
