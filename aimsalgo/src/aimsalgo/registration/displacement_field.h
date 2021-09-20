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

#include <cartodata/volume/volume.h>
#include <aims/registration/scale_control.h>
#include <cartobase/type/limits.h>
#include <vector>

template <class T>
class DisplacementField
{
 public:
  DisplacementField() {}
  ~DisplacementField() {}
  
  
  void init(carto::rc_ptr<carto::Volume<T> >& ref, ScaleControl& scaleControl,
            T * seuils);
  carto::VolumeRef< Point3d > getField(
    carto::rc_ptr<carto::Volume<T> >& test );
  std::vector<Point3df> getpointstest() {return  _pointstest;}
  std::vector<Point3df> getpointsref() {return  _pointsref;}
  double* getbarytest() {return _barytest;}
  double* getbaryref() {return _baryref;}
  carto::VolumeRef< int >* getOffset() { return &_offset;}
  int getLevel() {return _level;}
  Point3d getDimRef() {return _dimRef;}
  Point3df getSizeRef() {return _sizeRef;}  
  carto::VolumeRef<T> getQuality( carto::rc_ptr<carto::Volume<T> >& testtrans,
                                  carto::rc_ptr<carto::Volume<T> >& ref,
                                  int level,
                                  T thresh = std::numeric_limits<T>::max() );
  
 private:
  double _cost;
  int    _tx, _ty;
  std::vector<Point3df> _pointstest;
  std::vector<Point3df> _pointsref;
  double _barytest[3];
  double _baryref[3];
  
  
  void selectBlock(const carto::rc_ptr<carto::Volume<T> >& image, double cV);
  carto::VolumeRef< T > _pyrRef;
  carto::VolumeRef< T > _pyrTest;
  int               _level;
  int               _nx, _ny, _nz, _nz23d;
  carto::VolumeRef<int>   _offset;
  carto::VolumeRef<int>   _offset2;
  T _lthr, _hthr, _ltht, _htht;
  
  std::multimap<float, Point3d> _sel;
  std::multimap<float, double>   _moy;
  double  _selThresh;
  double _seuilCorrel;

  Point3d _dimRef;
  Point3df _sizeRef;
};
#endif
