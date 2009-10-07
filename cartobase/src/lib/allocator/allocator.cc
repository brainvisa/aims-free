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

#include <cartobase/allocator/allocator.h>
#include <cartobase/allocator/mappingro.h>
#include <cartobase/allocator/mappingrw.h>
#include <cartobase/allocator/mappingcopy.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/datasource/filedatasource.h>
#include <cartobase/datasource/bufferdatasource.h>
#include <iostream>
#include <cstdlib>


using namespace carto;
using namespace std;


LowLevelAllocator::~LowLevelAllocator()
{
}

// ---

AllocatorContext::AllocatorContext( const AllocatorContext & al ) 
  : _alloc( al._alloc ), 
    _datasource( al._datasource ? al._datasource->clone() : 0 ), 
    _access( al._access ), _diskcompat( al._diskcompat ), 
    _usefact( al._usefact ), _allocated( false ), _forced( al._forced )
{
  /*
  cout << "AllocatorContext copy " << this << ", DS: " << al._datasource 
       << " -> " << _datasource.get() << endl;
  cout << "type: " << allocatorType() << ", access: " << _access 
       << ", forced: " << _forced << endl;
  */
}


void AllocatorContext::setDataSource( rc_ptr<DataSource> ds )
{
  _datasource = ds;
}


void AllocatorContext::setAccessMode( DataAccess mode )
{
  _access = mode;
}


bool AllocatorContext::canDuplicate() const
{
  if( _alloc )
    return _alloc->canDuplicate();
  return false;
}


void AllocatorContext::setUseFactor( float x )
{
  _usefact = x;
}


float AllocatorContext::useFactor() const
{
  return _usefact;
}


AllocatorStrategy::MappingMode AllocatorContext::allocatorType() const
{
  if( _alloc == &MemoryAllocator::singleton() )
    return AllocatorStrategy::Memory;
  if( _alloc == &MappingROAllocator::singleton() )
    return AllocatorStrategy::ReadOnlyMap;
  if( _alloc == &MappingRWAllocator::singleton() )
    return AllocatorStrategy::ReadWriteMap;
  if( _alloc == &NullAllocator::singleton() )
    return AllocatorStrategy::Unallocated;
  if( _alloc == &MappingCopyAllocator::singleton() )
    return AllocatorStrategy::CopyMap;
  cerr << "Unknown allocation method\n";
  return AllocatorStrategy::CopyMap;
}


AllocatorContext::AllocatorContext( AllocatorStrategy::DataAccess mode,
                                    rc_ptr<DataSource> datasource, 
                                    bool isDiskformatOK, 
                                    float usefactor )
  : _alloc( 0 ), _datasource( datasource ), _access( mode ), 
    _diskcompat( isDiskformatOK ), _usefact( usefactor ), _allocated( false ), 
    _forced( false )
{
  // cout << "AllocatorContext " << this << " with DS: " << datasource.get() 
  // << endl;
}


AllocatorContext::AllocatorContext( AllocatorStrategy::DataAccess mode,
                                    const string & url, offset_t offset, 
                                    bool isDiskformatOK, float usefactor )
  : _alloc( 0 ), _datasource( new FileDataSource( url, offset ) ), 
    _access( mode ), _diskcompat( isDiskformatOK ), _usefact( usefactor ), 
    _allocated( false ), _forced( false )
{
}


AllocatorContext::AllocatorContext( const LowLevelAllocator* alloc )
  : _alloc( alloc ), _datasource(), 
    _access( AllocatorStrategy::InternalModif ),
    _diskcompat( false ), _usefact( 1 ), _allocated( false ), _forced( true )
{
  // cout << "AllocatorContext " << this << " hardcoded\n";
  if( alloc != &MemoryAllocator::singleton() )
    _diskcompat = true;
}


AllocatorContext::~AllocatorContext()
{
  // cout << "~AllocatorContext " << this <<  ", DS: " << _datasource.get() 
  // << endl;
}


AllocatorContext & 
AllocatorContext::operator = ( const AllocatorContext & other )
{
  if( this ==  &other )
    return *this;

  _alloc = other._alloc;
  _datasource = other._datasource;
  _access = other._access;
  _diskcompat = other._diskcompat;
  _usefact = other._usefact;
  _allocated = false;
  _forced = other._forced;
  return *this;
}


// ---

const MemoryAllocator & MemoryAllocator::singleton()
{
  if( !_allocptr() )
    _allocptr() = new MemoryAllocator;
  return *_allocptr();
}


MemoryAllocator *& MemoryAllocator::_allocptr()
{
  static MemoryAllocator	*allocptr = 0;

  return allocptr;
}


MemoryAllocator::~MemoryAllocator()
{
  _allocptr() = 0;
}


char * MemoryAllocator::allocate( size_t n, size_t sz, DataSource* ) const
{
  if ( n == 0 )
    return 0;

  char	*buf = (char *) malloc( n * sz );
  if( !buf )
    {
      cerr << "Can't allocate memory - no memory left." << endl;
      throw bad_alloc();
    }
  return buf;
}


void MemoryAllocator::deallocate( char* ptr, size_t n, size_t ) const
{
  if ( n != 0 )
    free( ptr );
}


std::ostream& operator << ( std::ostream& os, const MemoryAllocator& thing )
{
  os << "{ MEM, this=" << &thing << " }" << flush;
  return os;
}

// ----------

const NullAllocator & NullAllocator::singleton()
{
  if( !_allocptr() )
    _allocptr() = new NullAllocator;
  return *_allocptr();
}


NullAllocator *& NullAllocator::_allocptr()
{
  static NullAllocator	*allocptr = 0;

  return allocptr;
}


NullAllocator::~NullAllocator()
{
  _allocptr() = 0;
}


char * NullAllocator::allocate( size_t, size_t, DataSource* ds ) const
{
  if( !ds )
    return 0;
  return static_cast<BufferDataSource *>( ds )->buffer();
}


void NullAllocator::deallocate( char*, size_t, size_t ) const
{
}


std::ostream& operator << ( std::ostream& os, const NullAllocator& thing )
{
  os << "{ Unallocated, this=" << &thing << " }" << flush;
  return os;
}

// ----------

namespace
{

  void _memmapThresholds( carto::offset_t & ro, carto::offset_t & rw, 
                          carto::offset_t & cp, float usefactor = 1 )
  {
    carto::offset_t	ram, freeram, swap;
    AllocatorStrategy::memSizes( ram, freeram, swap );
    if( freeram == 0 )
      freeram = 0x10000000;	// 256 Mb
    if( swap == 0 )
      swap = 0x40000000;	// 1Gb
    /*cout << "freeram: " << freeram << ", ram: " << ram << ", swap: " << swap 
      << endl;*/
    ro = (carto::offset_t ) ( freeram * 0.5 * usefactor );
    if( ro > swap * 0.8 )
      ro = (carto::offset_t ) ( swap * 0.8 );
    cp = (carto::offset_t ) ( freeram * 0.9 );
    if( cp > swap * 0.8 )
      cp = (carto::offset_t ) ( swap * 0.8 );
    rw = (carto::offset_t ) ( freeram * 0.5 * usefactor );
    if( rw > swap * 0.8 )
      rw = (carto::offset_t ) ( swap * 0.8 );
  }

}


bool AllocatorStrategy::isMMapCompatible( bool ascii, int byteorder, 
                                          bool scalefactored, int border )
{
  return !ascii && !scalefactored && border == 0 
    && byteorder ==  0x41424344;
}


/*
bool AllocatorStrategy::isMMapCompatible( const AttributedObject & hdr )
{
  int	ascii = 1;
  int	bswap = 1;
  float	sfac = 1;
  hdr.getAttribute( "ascii", ascii );
  hdr.getAttribute( "byteswapping", bswap );
  hdr.getAttribute( "scale_factor", sfac );

  return !ascii && !bswap && sfac == 1.;
}
*/


AllocatorStrategy::MappingMode 
AllocatorStrategy::mappingMode( DataAccess mode, 
                                carto::offset_t buflen, 
                                const DataSource* datasource, 
                                bool isDiskformatOK, 
                                float usefactor )
{
  if( mode == NotOwner && datasource 
      && dynamic_cast<const BufferDataSource *>( datasource ) )
    return Unallocated;

  carto::offset_t	ro, rw, cp;
  _memmapThresholds( ro, rw, cp, usefactor );
  /* cout << "mmap thresholds: ro: " << ro << ", rw: " << rw << ", cp: " << cp 
     << endl;*/
  bool mmappable 
    = isDiskformatOK && datasource && datasource->allowsMemoryMapping();
  /*cout << "usefactor: " << usefactor << ", mmap comp: " << mmappable << endl;
    cout << "buff size: " << buflen << endl;*/

  switch( mode )
    {
    case ReadOnly:
      if( mmappable )
      {
        if( buflen <= ro )
          return MEM;
        else
          return MAP_RO;
      }
      // no break here

    case InternalModif:
      if( buflen <= cp )
        return MEM;
      else
        return MAP_COPY;

    case ReadWrite:
      if( mmappable )
      {
        if( buflen <= ro )
          return MEM;
        else if( datasource && ( datasource->mode() & DataSource::Write ) )
          return MAP_RW;
      }
      if( buflen <= cp )
        return MEM;
      else
        return MAP_COPY;

    case NotOwner:
      return Unallocated;

    default:
      break;
    }

  if( buflen <= cp )
    return MEM;
  else
    return MAP_COPY;
}


AllocatorContext AllocatorStrategy::allocator( MappingMode mode, 
                                               rc_ptr<DataSource> ds )
{
  /*
  cout << "AllocatorStrategy::allocator mode " << mode << ", fname: " 
       << filename << "\n";
  */
  switch( mode )
    {
    case MAP:
    case MAP_COPY:
      {
        AllocatorContext	a( &MappingCopyAllocator::singleton() );
        a.setDataSource( ds );
        return a;
      }
    case MAP_RO:
      {
        AllocatorContext	a( &MappingROAllocator::singleton() );
        a.setDataSource( ds );
        return a;
      }
    case MAP_RW:
      {
        AllocatorContext	a( &MappingRWAllocator::singleton() );
        a.setDataSource( ds );
        return a;
      }
    case Unallocated:
      if( dynamic_cast<BufferDataSource *>( ds.get() ) )
        {
          AllocatorContext	a( &NullAllocator::singleton() );
          a.setDataSource( ds );
          return a;
        }
    default:
      {
        AllocatorContext	a( &MemoryAllocator::singleton() );
        a.setDataSource( ds );
        return a;
      }
    }
}


const LowLevelAllocator & 
AllocatorStrategy::lowLevelAllocator( MappingMode m )
{
  switch( m )
    {
    case Memory:
      return MemoryAllocator::singleton();
    case ReadOnlyMap:
      return MappingROAllocator::singleton();
    case ReadWriteMap:
      return MappingRWAllocator::singleton();
    case MAP:
    case CopyMap:
      break;
    case Unallocated:
      return NullAllocator::singleton();
      break;
    }
  return MappingCopyAllocator::singleton();
}


