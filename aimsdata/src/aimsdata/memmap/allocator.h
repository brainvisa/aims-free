/*
 *  Allocator class
 */
#ifndef AIMS_ALLOCATOR_ALLOCATOR_H
#define AIMS_ALLOCATOR_ALLOCATOR_H

#include <soma-io/allocator/allocator.h>

// COMPATIBILITY CODE : TEMPORARY

typedef carto::AllocatorStrategy::MappingMode FileOpenMode;
extern const carto::AllocatorStrategy::MappingMode 
MEM, MAP, MAP_COPY, MAP_RO, MAP_RW;

namespace aims
{
  /// compatibility typedef - NEVER USE IT
  typedef carto::AllocatorContext Allocator;
}


#endif
