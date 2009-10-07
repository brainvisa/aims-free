/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

#ifndef CARTOBASE_ALLOCATOR_ALLOCATOR_H
#define CARTOBASE_ALLOCATOR_ALLOCATOR_H

#include <cartobase/config/cartobase_config.h>
#include <cartobase/datasource/datasource.h>
#include <cartobase/smart/rcptr.h>
#include <memory>
#include <string>


namespace carto
{

  /** Abstract base class for actual allocators types

  All subclasses of LowLevelAllocator are singletons, which each provide 
  a singleton() static method to access their unique instance.
  */
  class LowLevelAllocator
  {
  public:
    virtual ~LowLevelAllocator();

    virtual char *allocate( size_t n, size_t elemsize, 
                            DataSource* datasource ) const = 0;
    virtual void deallocate( char *ptr, size_t n, size_t elemsize ) const = 0;

    virtual bool canDuplicate() const { return true; }

  protected:
    LowLevelAllocator() {}
  };


  /** Normal allocation mode: allocation in main memory 
   */
  class MemoryAllocator : public LowLevelAllocator
  {
  public:
    virtual ~MemoryAllocator();

    virtual char *allocate( size_t n, size_t elemsize, 
                            DataSource* datasource ) const;
    virtual void deallocate( char *ptr, size_t n, size_t sz ) const;

    static const MemoryAllocator & singleton();

  private:
    /// constructor is private: access can only be done via singleton()
    MemoryAllocator() : LowLevelAllocator() { _allocptr() = this; }

    static MemoryAllocator	*& _allocptr();
  };


  /** This allocator doesn't allocate anything

  NullAllocator is used to "fake" allocation and use data already allocated 
  (and owned) by other structures
  */
  class NullAllocator : public LowLevelAllocator
  {
  public:
    virtual ~NullAllocator();

    virtual char *allocate( size_t n, size_t elemsize, 
                            DataSource* datasource ) const;
    virtual void deallocate( char *ptr, size_t n, size_t sz ) const;

    static const NullAllocator & singleton();

  private:
    /// constructor is private: access can only be done via singleton()
    NullAllocator() : LowLevelAllocator() { _allocptr() = this; }

    static NullAllocator	*& _allocptr();
  };


  std::ostream& operator << ( std::ostream& os, 
                              const MemoryAllocator& thing );

  class AllocatorContext;


  /** Determination of the allocation type depending of the buffer size 
      to allocate and the disk format of data
     
      The optimal allocation mode depends on: 
      - what you intend to do with the data: read only, read / modify / 
        rewrite to disk, read / modify internally but leave it unchanged on 
        disk
      - if data on disk is stored in the same format as the memory format 
        (binary data, no byteswapping, no alignment mismatch, no 
        scalefactor...)
      - the size of data
      - possibly the size of the system memory (total or currently available), 
        but as it cannot be determined in a portable way, this information 
        is likely not to be accessible on every system, so we use absolute 
        threshold instead
  */
  class AllocatorStrategy
  {
  public:
    /** Data access modes, they describe what you intend to do with data to be 
        read
    */
    enum DataAccess
      {
        /** Mode for internal modification. If memory mapping is enabled, it 
            will be on a copy of the input file (MAP_COPY) because it needs to 
            be writeable but must not modify the original file. It is the 
            safest mode (but not the more efficient), so if you hesitate, 
            take this one.
        */
        InternalModif,
        /// Mode for read-only access
        ReadOnly, 
        /** Mode for read/write access, the input file will be modified and 
            needs read/write permissions */
        ReadWrite, 
        /** This value is used for sub-volumes or fake volumes using data 
            that are already allocated by another structure */
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

    /** Determines optimal allocator for a data file
        \param mode what you intend to do tith the data: read only, read/write 
        or read/internally modify it. read/write mode is dangerous: it will 
        allow modifying the original file.
        \param buflen size in bytes of the data to allocate
        \param datasource data source to map (file, buffer, stream, ...)
        \param isDiskformatOK true if the disk format is exactly the same 
        as the internal memory format (allowing direct memory mapping)
        \param usefactor rate of usage you intend to do with the data, from 
        0 if you will only use a few bytes of it, to 1 if you intend to make 
        intensive usage of all data. A small value will allow memory mapping
        on smaller files, and a higher value will avoid it.
     */
    static MappingMode mappingMode( DataAccess mode, 
                                    offset_t buflen, 
                                    const DataSource* datasource, 
                                    bool isDiskformatOK, 
                                    float usefactor = 1 );

    /// helper function, tells if memory mapping is directly possible
    static bool isMMapCompatible( bool ascii, int byteorder, 
                                  bool scalefactored = false, int border = 0 );
    /// same as above but uses a Header attributed object
    //static bool isMMapCompatible( const carto::AttributedObject & hdr );

    /** Try to detect memory amounts and fills referenced params. Params are 
        set to 0 if thezy cannor be determined.

        \param ram physical RAM amount
        \param freeram free RAM  amount
        \param swap free RAM+swap amount
    */
    static void memSizes( offset_t & ram, 
                          offset_t & freeram, 
                          offset_t & swap );

    static AllocatorContext allocator( MappingMode mode, 
                                       rc_ptr<DataSource> ds );

    static const LowLevelAllocator & lowLevelAllocator( MappingMode m );
  };


  /** Allocation context

  The allocators system is globally described on a separate \ref allocators 
  page.

  More specifically, AllocatorContext is the entry point to this allocation 
  system: only this class has generally to be used by programmers. It 
  handles a switch to the appropriate low-level allocator (LowLevelAllocator) 
  according to the amount of memory available, the size of the data block to 
  be allocated, and to what is intended to be done with it.

  The switch mechanism is itself in large part deported to another class: 
  AllocatorStrategy which also provides some constant enums used by 
  AllocatorContext. This is perhaps a bad idea and both classes might be 
  merged in the future.

  In order to accelerate allocation, or to be sure that allocation is done in 
  the intended mode (which may be important in some cases), this swiching 
  mechanism can be bypassed by directly providing a LowLevelAllocator. Such 
  low-level allocators are singletons and should never be created by hand: 
  use their singleton() static methods.
  */
  class AllocatorContext
  {
  public:
    typedef AllocatorStrategy::DataAccess DataAccess;
    typedef AllocatorStrategy::MappingMode MappingMode;

    AllocatorContext( DataAccess mode 
		      = AllocatorStrategy::InternalModif, 
                      rc_ptr<DataSource> datasource = DataSource::none(), 
                      bool isDiskformatOK = false, 
                      float usefactor = 1 );
    AllocatorContext( DataAccess mode, 
                      const std::string & filename, 
                      offset_t offset = 0, 
                      bool isDiskformatOK = false, 
                      float usefactor = 1 );
    AllocatorContext( const LowLevelAllocator* alloc );
    AllocatorContext( const AllocatorContext & );
    ~AllocatorContext();
    AllocatorContext & operator = ( const AllocatorContext & );

    /// return value is the same as modifiable input param ptr
    template<typename T> T* allocate( T*& ptr, size_t n ) const;
    template<typename T> void deallocate( T* ptr, size_t n ) const;
    bool canDuplicate() const;
    MappingMode allocatorType() const;
    DataAccess accessMode() const
    { return _access; }
    void setAccessMode( DataAccess mode );
    /// AllocatorContext becomes owner of \c datasource
    void setDataSource( rc_ptr<DataSource> datasource );
    const rc_ptr<DataSource> dataSource() const { return _datasource; }
    rc_ptr<DataSource> dataSource() { return _datasource; }
    bool isAllocated() const { return _allocated; }
    float useFactor() const;
    void setUseFactor( float x );
    bool allowsMemoryMapping() const;
    void setAllowsMemoryMapping( bool x );

  private:
    mutable const LowLevelAllocator	*_alloc;
    mutable rc_ptr<DataSource>          _datasource;
    DataAccess				_access;
    bool				_diskcompat;
    float				_usefact;
    mutable bool			_allocated;
    mutable bool			_forced;
  };


  template<typename T>
  inline T* AllocatorContext::allocate( T*& ptr, size_t n ) const
  {
    if( _allocated )
      return ptr; // maybe throw an exception ?
    _allocated = true;

    if( !_alloc || !_forced )
      {
        _forced = false;
        AllocatorStrategy::MappingMode	m 
          = AllocatorStrategy::mappingMode( _access, 
                                            offset_t( n ) * sizeof( T ), 
                                            _datasource.get(), _diskcompat, 
                                            _usefact );
        _alloc = &AllocatorStrategy::lowLevelAllocator( m );
      }

    ptr = (T*) _alloc->allocate( n, sizeof( T ), _datasource.get() );
#ifdef _MSC_VER
#warning uninitialized_fill_n not present in Microsoft standard includes
#else
    // Don't initialize in mem'mapped allocators!
    if( _alloc == &MemoryAllocator::singleton() )
      std::uninitialized_fill_n( ptr, n, T() );
#endif
    return ptr;
  }


  template<typename T>
  inline void AllocatorContext::deallocate( T* ptr, size_t n ) const
  {
    if( !_allocated )
      return;
    _allocated = false;
    if( _alloc == &MemoryAllocator::singleton() )
      std::_Destroy( ptr, ptr + n );
    _alloc->deallocate( (char *) ptr, n, sizeof( T ) );
  }


  inline bool AllocatorContext::allowsMemoryMapping() const
  {
    return _diskcompat;
  }


  inline void AllocatorContext::setAllowsMemoryMapping( bool x )
  {
    _diskcompat = x;
  }

}


#endif
