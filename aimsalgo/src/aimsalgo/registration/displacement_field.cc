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


#include <aims/registration/displacement_field_d.h>


template class  DisplacementField<int8_t> ;
template class  DisplacementField<uint8_t> ;
template class  DisplacementField<int16_t> ;
template class  DisplacementField<uint16_t> ;
template class  DisplacementField<int32_t> ;
template class  DisplacementField<uint32_t> ;
template class  DisplacementField<float> ;
template class  DisplacementField<double> ;
