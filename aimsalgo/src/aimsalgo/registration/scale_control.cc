
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


#include <aims/registration/scale_control_d.h>

using namespace carto;

template void ScaleControl::init(const rc_ptr<Volume< int8_t > > ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const rc_ptr<Volume< uint8_t > > ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const rc_ptr<Volume< int16_t > > ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const rc_ptr<Volume< uint16_t > > ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const rc_ptr<Volume< int32_t > > ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const rc_ptr<Volume< uint32_t > > ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const rc_ptr<Volume< float > > ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const rc_ptr<Volume< double > > ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );



// fonction inutile
bool ScaleControl::minReached()
{	
  if( _currentScale )
    return false ;
  
  return true;
  //   return (local);
  
}


void ScaleControl::nextScale()
{
  _currentScale -= 1;
  if(_cutVar>=(_stopVar + 0.2) )	_cutVar -= 0.2;
  else _cutVar = _stopVar;  
}
