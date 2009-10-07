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

#include <cstdlib>
#include <cartobase/allocator/allocator.h>
#include <iostream>
#include <cstdlib>
#ifdef linux
#include <cstring>
#include <fstream>
#endif
#ifdef __sun
// on Solaris, swapctl is incompatible with large files
#undef _FILE_OFFSET_BITS
#undef _LARGEFILE_SOURCE
#undef _LARGE_FILES
#include <unistd.h>
#include <sys/stat.h>
#include <sys/swap.h>
#endif
#ifdef __sgi
#include <sys/stat.h>
#include <sys/swap.h>
#endif
#ifdef _WIN32
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0500
#include <windows.h>
#endif

using namespace carto;
using namespace std;


void AllocatorStrategy::memSizes( carto::offset_t & ram, 
                                  carto::offset_t & freeram, 
                                  carto::offset_t & swap )
{
  ram = 0;
  freeram = 0;
  swap = 0;

#ifdef __linux

  /*
  struct sysinfo  s;
  if( sysinfo( &s ) != 0 )
    return;

  ram = s.totalram;
  freeram = s.freeram + s.bufferram;
  swap = s.freeswap;
  */

  ifstream	proc( "/proc/meminfo" );
  if( !proc )
    return;

  int		det = 0;
  char	buf[ 200 ];
  long	free0 = 0, buff = 0, cach = 0;

  while( det < 5 && !proc.eof() )
    {
      proc.getline( buf, 200 );
      if( strncmp( buf, "MemTotal:", 9 ) == 0 )
        {
          ram = atol( buf + 9 );
          ++det;
        }
      else if( strncmp( buf, "MemFree:", 8 ) == 0 )
        {
          free0 = atol( buf + 8 );
          ++det;
        }
      else if( strncmp( buf, "Buffers:", 8 ) == 0 )
        {
          buff = atol( buf + 8 );
          ++det;
        }
      else if( strncmp( buf, "Cached:", 7 ) == 0 )
        {
          cach = atol( buf + 7 );
          ++det;
        }
      else if( strncmp( buf, "SwapFree:", 9 ) == 0 )
        {
          swap = atol( buf + 9 );
          ++det;
        }
    }
  proc.close();

  freeram = free0 + buff + cach;
  swap += freeram;
  ram <<= 10;
  freeram <<= 10;
  swap <<= 10;

#else
#ifdef __sun

  ram = sysconf( _SC_PHYS_PAGES );
  freeram = sysconf( _SC_AVPHYS_PAGES );
  long psize = sysconf( _SC_PAGE_SIZE );
  ram *= psize;
  freeram *= psize;

  swaptbl_t		*st;
  int			nswap, i, n;
  char			*sws, *buf;
  const unsigned	MAXSTRSIZE = 256;

  nswap = swapctl( SC_GETNSWP, 0 );
  buf = new char[ (nswap + 1) * sizeof( swapent_t ) 
                  + sizeof( struct swaptable ) ];
  st = (swaptbl_t *) buf;
  sws = new char[ (nswap+1) * MAXSTRSIZE ];
  for( i=0; i<nswap+1; ++i )
    st->swt_ent[i].ste_path = sws + ( i * MAXSTRSIZE );
  st->swt_n = nswap + 1;
  n = swapctl( SC_LIST, st );
  for( i=0; i<n; ++i )
    if( !(st->swt_ent[i].ste_flags & ST_INDEL) )
      swap += st->swt_ent[i].ste_free;
  swap *= psize;
  swap += freeram;
  delete sws;
  delete buf;

#else
#ifdef __sgi

  off_t swapt = 0, swapf = 0, swapt2 = 0, swapr = 0, swapv = 0;
  swapctl( SC_GETFREESWAP, &swapf );
  swapctl( SC_GETLSWAPTOT, &swapt );	// RAM + swap total
  swapctl( SC_GETSWAPTOT, &swapt2 );	// swap total
  swapctl( SC_GETRESVSWAP, &swapr );	// used RAM ? (swappable if needed)
  // swapctl( SC_GETSWAPVIRT, &swapv );	// always 0
  ram = swapt - swapt2;
  freeram = ram - swapr;
  swap = swapf + ram;

  ram *= 512;
  freeram *= 512;
  swap *= 512;

#else
#ifdef _WIN32

  // Thanks Roro for this code...
  MEMORYSTATUSEX memory;
  memory.dwLength = sizeof(memory);
  if( !::GlobalMemoryStatusEx(&memory) )
    return;

  //memory.dwMemoryLoad;
  ram = memory.ullTotalPhys;
  freeram = memory.ullAvailPhys;
  //memory.ullTotalPageFile;
  swap = memory.ullAvailPageFile;

#endif
#endif
#endif
#endif

  /*
  cout << "RAM: " << ram << ", free: " << freeram << ", swap: " << swap 
       << endl;
  */
}
