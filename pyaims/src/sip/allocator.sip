
namespace carto
{


  class AllocatorStrategy
  {
%TypeHeaderCode
#include <cartobase/allocator/allocator.h>
%End

%Docstring
Memory management strategy helper. Contains mainly constants and a memory query function.
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
%Docstring
Check the current system memory and return the amount of RAM, free RAM, and swap.
%End

    static carto::AllocatorContext
      allocator( carto::AllocatorStrategy::MappingMode );
%Docstring
Create an AllocatorContext object with the required mapping mode
%End
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

%Docstring
Allocation context specifications.
This object is used when memory must be allocated for large objects (volumes, arrays...), especially for data which will be read from disk. It allows to specify how the memory will be used (read-only, read-write, how much...) to allow to decide whether any memory mapping techniques should be used or not.
%End

  public:
    typedef carto::AllocatorStrategy::DataAccess DataAccess;
    typedef carto::AllocatorStrategy::MappingMode MappingMode;

    AllocatorContext( carto::AllocatorContext::DataAccess mode
                        = carto::AllocatorStrategy::InternalModif,
                      const std::string & filename = "", 
                      unsigned long long offset = 0,
                      bool isDiskformatOK = false, 
                      float usefactor = 1 );
    AllocatorContext( const carto::AllocatorContext & );
    ~AllocatorContext();

    carto::AllocatorContext::MappingMode allocatorType() const;
    carto::AllocatorContext::DataAccess accessMode() const;
    void setAccessMode( carto::AllocatorContext::DataAccess mode );

    void setDataSource( rc_ptr_DataSource datasource );
    // const rc_ptr_DataSource dataSource() const;
    rc_ptr_DataSource dataSource();

    void setDataSourceInfo( rc_ptr_DataSourceInfo dsi );
    // const rc_ptr_DataSourceInfo dataSourceInfo() const;
    rc_ptr_DataSourceInfo dataSourceInfo();

    bool isAllocated() const;
    float useFactor() const;
    void setUseFactor( float x );
    bool allowsMemoryMapping() const;
    void setAllowsMemoryMapping( bool x );
  };

};

