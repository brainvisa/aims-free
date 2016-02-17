/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#include <aims/registration/minimisation_d.h>



template Motion Minimisation::quaternion(DisplacementField<int8_t>& displacementField) ;
template Motion Minimisation::quaternion(DisplacementField<uint8_t>& displacementField) ;
template Motion Minimisation::quaternion(DisplacementField<int16_t>& displacementField) ;
template Motion Minimisation::quaternion(DisplacementField<uint16_t>& displacementField) ;
template Motion Minimisation::quaternion(DisplacementField<int32_t>& displacementField) ;
template Motion Minimisation::quaternion(DisplacementField<uint32_t>& displacementField) ;
template Motion Minimisation::quaternion(DisplacementField<float>& displacementField) ;
template Motion Minimisation::quaternion(DisplacementField<double>& displacementField) ;
