/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef AIMS_REGISTRATION_SCALE_CONTROL_D_H
#define AIMS_REGISTRATION_SCALE_CONTROL_D_H


#include <aims/registration/scale_control.h>
#include <math.h>


template <class T>
void ScaleControl::init(const carto::rc_ptr<carto::Volume< T > >& ref, int level_start, int level_stop, double cutVar, double stopVar, double seuilCorrel, const Point3d& tailleBloc )
{
  // Calcul du niveau de pyramide adequat si non precise...
  _currentScale = level_start;
  if(level_start==-1)
  {
  		int dimoy = int( (ref->getSizeX() + ref->getSizeY())/2. + 0.5);
  		_currentScale = 0;
  		for(int i = 0;i<15;i++)
  		{
 			if(dimoy<64)	break;
			dimoy = int(dimoy/2. + 0.5);
			_currentScale = i;
  		}
  }

  _level_stop = level_stop;
  if(level_stop==-1)	_level_stop = (_currentScale>2) ? (_currentScale-2) : 0;

  _cutVar = cutVar;
  if(cutVar==-1)	_cutVar = 0.2 + _currentScale*0.2;
  _cutVar = (_cutVar <= 1) ? _cutVar : 1;

  _stopVar = stopVar;
  if(stopVar==-1)	 _stopVar = 0.2 + _level_stop*0.2;
  _stopVar = (_stopVar<=_cutVar) ? _stopVar : _cutVar;



 // Toutes les valeurs suivantes sont en coordonnees voxels
  _NN = ( (ref->getSizeX() < ref->getSizeY()) ? ref->getSizeX() : ref->getSizeY())/8;
  _delta1 = _NN/4;
  _delta2 = 4;
  _derniere = false;
  _seuilCorrel = seuilCorrel;
  _tailleBloc = tailleBloc;
  if(ref->getSizeZ()==1)	_tailleBloc[2] = 1;
}


#endif

