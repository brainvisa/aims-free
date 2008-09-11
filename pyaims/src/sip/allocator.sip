
namespace carto
{


  class AllocatorStrategy
  {
%TypeHeaderCode
#include <cartobase/allocator/allocator.h>
%End

  public:
    enum DataAccess
      {
        InternalModif,
        ReadOnly,
        ReadWrite,
        NotOwner,
      };

    enum MappingMode
      {
        Memory, 
        MEM = Memory,
        MAP,
        CopyMap, 
        MAP_COPY = CopyMap,
        ReadOnlyMap, 
        MAP_RO = ReadOnlyMap,
        ReadWriteMap, 
        MAP_RW = ReadWriteMap, 
        Unallocated,
      };

    static void memSizes( unsigned long long & ram,
                          unsigned long long & freeram,
                          unsigned long long & swap );

    static carto::AllocatorContext
      allocator( carto::AllocatorStrategy::MappingMode );
%MethodCode
    sipRes = new carto::AllocatorContext
      ( &carto::AllocatorStrategy::lowLevelAllocator( a0 ) );
%End

  };


  class AllocatorContext
  {
%TypeHeaderCode
#include <cartobase/allocator/allocator.h>
%End

  public:
    typedef carto::AllocatorStrategy::DataAccess DataAccess;
    typedef carto::AllocatorStrategy::MappingMode MappingMode;

    AllocatorContext( carto::AllocatorContext::DataAccess mode,
                      const std::string & filename = "", 
                      unsigned long long offset = 0,
                      bool isDiskformatOK = false, 
                      float usefactor = 1 );
    AllocatorContext( const carto::AllocatorContext & );
    ~AllocatorContext();

    carto::AllocatorContext::MappingMode allocatorType() const;
    carto::AllocatorContext::DataAccess accessMode() const;
    void setAccessMode( carto::AllocatorContext::DataAccess mode );
    bool isAllocated() const;
    float useFactor() const;
    void setUseFactor( float x );
    bool allowsMemoryMapping() const;
    void setAllowsMemoryMapping( bool x );
  };

};

