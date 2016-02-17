/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef _AIMSSCALECONTROL_H_
#define _AIMSSCALECONTROL_H_
#include <aims/vector/vector.h> 


class ScaleControl
{
 public: 


  ScaleControl(const int s, const int e);
  virtual  ~ScaleControl();
  int      getScale() const ;
  int      getDataIncrement() const ;
  void     init();
  bool     end();
  void     incr();

 private:
  int _start, _end, _status;
  int _dataIncr ;
     
};



#endif 
