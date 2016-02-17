/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#ifndef BRAINRAT_MPI_TYPES_H
#define BRAINRAT_MPI_TYPES_H

#include <string>
#include <mpi.h>
using namespace MPI;

/* Conversion of AIMS "T" to MPI Datatype (programmer seems to always have to do it himself with C++ templates). */
    //typedefs from cartobase/type/limits.h from stdint.h .

/** This class is just a hint to convert an actual mpi data type to an 
identifier string used in input/output stuff */
template<class T> class MpiDataTypeCode
{
public:
  MpiDataTypeCode() {};

public:
  static inline Datatype mpiDataType();
};


template <>
Datatype MpiDataTypeCode<int8_t>::mpiDataType()
{
  return SIGNED_CHAR;
}

template <>
Datatype MpiDataTypeCode<uint8_t>::mpiDataType()
{
  return UNSIGNED_CHAR;
}

template <>
Datatype MpiDataTypeCode<int16_t>::mpiDataType()
{
  return SHORT;
}

template <>
Datatype MpiDataTypeCode<uint16_t>::mpiDataType()
{
  return UNSIGNED_SHORT;
}

template <>
Datatype MpiDataTypeCode<int32_t>::mpiDataType()
{
  return INT;
}

template <>
Datatype MpiDataTypeCode<uint32_t>::mpiDataType()
{
  return UNSIGNED;
}

template <>
Datatype MpiDataTypeCode<float>::mpiDataType()
{
  return FLOAT;
}

template <>
Datatype MpiDataTypeCode<double>::mpiDataType()
{
  return DOUBLE;
}

#endif
