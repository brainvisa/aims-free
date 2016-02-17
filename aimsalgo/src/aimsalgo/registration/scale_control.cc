
/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#include <aims/registration/scale_control_d.h>

template void ScaleControl::init(const AimsData< int8_t >& ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const AimsData< uint8_t >& ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const AimsData< int16_t >& ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const AimsData< uint16_t >& ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const AimsData< int32_t >& ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const AimsData< uint32_t >& ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const AimsData< float >& ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );
template void ScaleControl::init(const AimsData< double >& ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc );



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
