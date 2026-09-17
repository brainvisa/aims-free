// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  Allocator class
 */
#include <aims/memmap/allocator.h>

// COMPATIBILITY CODE : TEMPORARY

const carto::AllocatorStrategy::MappingMode 
MEM = carto::AllocatorStrategy::Memory;
const carto::AllocatorStrategy::MappingMode 
MAP = carto::AllocatorStrategy::MAP;
const carto::AllocatorStrategy::MappingMode 
MAP_COPY = carto::AllocatorStrategy::CopyMap;
const carto::AllocatorStrategy::MappingMode 
MAP_RO = carto::AllocatorStrategy::ReadOnlyMap;
const carto::AllocatorStrategy::MappingMode 
MAP_RW = carto::AllocatorStrategy::ReadWriteMap;


