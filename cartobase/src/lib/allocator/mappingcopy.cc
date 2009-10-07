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

#include <cartobase/allocator/mappingcopy.h>
#include <cartobase/exception/assert.h>
#include <cartobase/stream/sstream.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <errno.h>

#ifndef _WIN32

#undef _XOPEN_SOURCE
#include <sys/mman.h>
#include <unistd.h>

#endif

#include <sys/types.h>
#include <sys/stat.h>
#define _XOPEN_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <cartobase/config/paths.h>

using namespace carto;
using namespace std;

MappingCopyAllocator*& MappingCopyAllocator::_allocptr()
{
  static MappingCopyAllocator  *allocptr = 0;

  return allocptr;

}



MappingCopyAllocator::~MappingCopyAllocator()
{
  _allocptr() = 0;
}


char* MappingCopyAllocator::allocate( size_t n, size_t sz, DataSource* ) const
{
  cout << "Memory mapping with copy mode used - please be patient...\n";

#ifdef _WIN32

  char* buffer = 0;
  /*string fileName = string( mktemp( "cartoXXXXXX" ) );

  fileName = carto::Paths::memmap() + '\\' + fileName; */

  HANDLE	fm = CreateFileMapping( INVALID_HANDLE_VALUE, 0, 
                                        PAGE_READWRITE, 0, 
                                        n*sz, 0 );

  if( fm != INVALID_HANDLE_VALUE )
    {
      buffer = (char *) MapViewOfFile( fm, FILE_MAP_WRITE, 0, 0, 0 );
      _mapDesc[ buffer ] = fm;
#ifdef _ALLOCATOR_VERBOSE_
      cout << singleton() << endl;
#endif
      return buffer;
    }
  return 0;

#else // _WIN32

  char* buffer = 0;
  string	fileName = Paths::memmap() + "/cartoXXXXXX";

  if ( fileName.length() )
    {
      int	fildest = mkstemp( (char *) fileName.c_str() );
      if ( fildest != -1 )
	{
          // unlink file to ensure it will be deleted after usage
          unlink( fileName.c_str() );
	  if ( fchmod( fildest, S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH ) != -1 )
	    {
	      if ( ftruncate( fildest, n * sz ) != -1 )
		{
		  buffer = (char *) mmap( 0, n * sz, 
					  PROT_READ | PROT_WRITE, MAP_SHARED,
					  fildest, 0 );
		  if( buffer != MAP_FAILED )
                    {
		      _mapname[ buffer ] = fileName;
		      _mapDesc[ buffer ] = fildest;
#ifdef _ALLOCATOR_VERBOSE_
		      cout << singleton() << endl;
#endif
		      return buffer;
		    }
                  int err = errno;
                  ostringstream	ss;
                  ss << "memory mapping failed for " << (long long) n * sz 
                     << " bytes";
                  perror( ss.str().c_str() );
                  if( err == ENOMEM )
                    cerr << "Not enough memory left (or addressable)" << endl;
                  return 0;
		}
	    }
	  close( fildest );
	  /* Ici on ne peut faire un throw car il est trappé comme une erreur 
             de format. */
	  cerr << "truncate failed on  " << fileName << " !" << endl;
	  return 0; // this error is not managed by caller... !!
	}
      else 
	{
	  /* Ici on ne peut faire un throw car il est trappé comme une erreur 
             de format. */
	  cerr << "unable to open " << fileName << " !" << endl;    
	}
    }

  return 0;

#endif
}


void MappingCopyAllocator::deallocate( 
#ifdef _WIN32
char* ptr, size_t, size_t 
#else
char* ptr, size_t n, size_t sz 
#endif
) const
{
#ifdef _WIN32

  if( ptr )
  {
    UnmapViewOfFile( ptr );
    CloseHandle( _mapDesc[ ptr ] ); 
    _mapDesc.erase( ptr );
  }

#else // _WIN32

  if( ptr )
  {
    std::map< char*, std::string >::iterator it = _mapname.find( ptr );
    ASSERT( it != _mapname.end() );
    std::string name = it->second;

    if ( name.length() )
    {
      munmap( (char*)ptr, n * sz );
      close( _mapDesc[ ptr ] ); 
      unlink( name.c_str() );
      _mapname.erase( ptr );
      _mapDesc.erase( ptr );
#ifdef _ALLOCATOR_VERBOSE_
      cout << singleton() << endl;
#endif
    }
  }
#endif
}


const MappingCopyAllocator & MappingCopyAllocator::singleton()
{
  if( !_allocptr() )
    _allocptr() = new MappingCopyAllocator;
  return *_allocptr();
}


std::ostream& carto::operator << ( ostream& os, 
				  const MappingCopyAllocator & thing )
{
  os << "{ MAP_COPY, this=" << &thing << ", ";
#ifndef _WIN32
  std::map< char*, std::string>::const_iterator it;
  for( it = thing._mapname.begin(); it != thing._mapname.end(); it++ )
    os << "{ " << it->first << ", " << it->second << " }, ";
#endif
  os << "NULL }" << flush;

  return os;
}


