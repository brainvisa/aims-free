/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#include <aims/resampling/motion.h>
#include <aims/registration/block_matching_d.h>


template class  BlockMatching<int8_t> ;
template class  BlockMatching<uint8_t> ;
template class  BlockMatching<int16_t> ;
template class  BlockMatching<uint16_t> ;
template class  BlockMatching<int32_t> ;
template class  BlockMatching<uint32_t> ;
template class  BlockMatching<float> ;
template class  BlockMatching<double> ;
