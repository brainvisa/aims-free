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

#include <aims/math/bspline3.h>
#include "scaleControl.h"


ScaleControl::~ScaleControl()
{
}


ScaleControl::ScaleControl(const int s, const int e)
{
  _start = s;
  _end =   e;
  _status = s;
}

int ScaleControl::getScale() const
{
  return( _status );
}

void ScaleControl::init()
{
  _status = _start;
}

bool ScaleControl::end()
{
  return( _status > _end );
}

void ScaleControl::incr()
{
  _status--;
}
