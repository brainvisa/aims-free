/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#ifndef AIMS_REGISTRATION_MINIMISATION_H
#define AIMS_REGISTRATION_MINIMISATION_H


#include <aims/registration/displacement_field.h>
#include <aims/resampling/motion.h>


class Minimisation
{
 public:
  Minimisation() {_Pkept=0.5; _transfo = 1;};
  Minimisation(double Pkept, int transfo) {_Pkept=Pkept; _transfo = transfo;};
  ~ Minimisation(){};

  
  template <class T>
  Motion quaternion(DisplacementField<T>& displacementField);
  

 private:
	double _Pkept;
	int _transfo;
  
};

#endif
