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
#include <cartobase/uuid/uuid.h>
#include <stdlib.h>
#include <stdio.h>

#if defined( _WIN32 )

#  include <windows.h>

#elif defined( __MACOSX__ )

#  include <CoreFoundation/CoreFoundation.h>

#else

#  if defined(__osf__)
#    define _XOPEN_SOURCE_EXTENDED
#  endif
#  include <unistd.h>
#  include <stdio.h>
#  include <string.h>
#  include <sys/time.h>
#  include <sys/socket.h>
#  include <net/if.h>
#  if defined(__linux) || defined(__osf__)
#    include <sys/ioctl.h>
#  endif
#  include <assert.h>

#endif

typedef unsigned int unsigned32;
typedef unsigned short unsigned16;
typedef unsigned char unsigned8;


namespace carto
{

UUID::UUID()
{

  zero();

}


UUID::UUID( const std::string& uuid )
{

  unsigned32 time_low;
  unsigned16 time_mid;
  unsigned16 time_hi_and_version;
  unsigned16 clock_seq_hi_and_reserved;
  unsigned16 clock_seq_low;
  unsigned16 node[6];

  // why, oh why, can't scanf read integer values into chars?
  int n = sscanf( uuid.c_str(),
                  "%8x-%4hx-%4hx-%2hx%2hx-%2hx%2hx%2hx%2hx%2hx%2hx",
                  &time_low,
                  &time_mid,
                  &time_hi_and_version,
                  &clock_seq_hi_and_reserved,
                  &clock_seq_low,
                  &node[ 0 ], &node[ 1 ], &node[ 2 ],
                  &node[ 3 ], &node[ 4 ], &node[ 5 ] );
  if ( n == 11 )
  {

    memcpy( &data[ 0 ], &time_low, 4 );
    memcpy( &data[ 4 ], &time_mid, 2 );
    memcpy( &data[ 6 ], &time_hi_and_version, 2 );
    data[  8 ] = clock_seq_hi_and_reserved;
    data[  9 ] = clock_seq_low;
    data[ 10 ] = node[ 0 ];
    data[ 11 ] = node[ 1 ];
    data[ 12 ] = node[ 2 ];
    data[ 13 ] = node[ 3 ];
    data[ 14 ] = node[ 4 ];
    data[ 15 ] = node[ 5 ];

  }
  else
  {

    zero();

  }

}


void UUID::zero()
{

  memset( data, 0, sizeof( uuid_type ) );

}


UUID::UUID( const UUID& u )
{

  memcpy( data, u.data, sizeof( uuid_type ) );

}


bool UUID::operator==( const UUID& u ) const
{

  return memcmp( data, u.data, sizeof( uuid_type ) ) == 0;

}


std::string UUID::toString() const
{

  char buffer[ 37 ];
  unsigned32 time_low;
  memcpy( &time_low, &data[ 0 ], 4 );
  unsigned16 time_mid;
  memcpy( &time_mid, &data[ 4 ], 2 );
  unsigned16 time_hi_and_version;
  memcpy( &time_hi_and_version, &data[ 6 ], 2 );
  sprintf( buffer, "%08x-%04hx-%04hx-%02hx%02hx-%02x%02x%02x%02x%02x%02x",
           time_low, time_mid, time_hi_and_version,
           data[  8 ], data[  9 ], data[ 10 ], data[ 11 ],
           data[ 12 ], data[ 13 ], data[ 14 ], data[ 15 ] );
  return std::string( buffer );

}


bool UUID::isNull() const
{
  return data[  0 ] == 0 && data[  1 ] == 0 && data[  2 ] == 0 &&
         data[  3 ] == 0 && data[  4 ] == 0 && data[  5 ] == 0 &&
         data[  6 ] == 0 && data[  7 ] == 0 && data[  8 ] == 0 &&
         data[  9 ] == 0 && data[ 10 ] == 0 && data[ 11 ] == 0 &&
         data[ 12 ] == 0 && data[ 13 ] == 0;

}


#if defined( _WIN32 )

void UUID::generate()
{
  GUID uuid;
  CoCreateGuid( &uuid );
  memcpy( &data[ 0 ], &uuid.Data1, sizeof( unsigned int ) );
  memcpy( &data[ 4 ], &uuid.Data2, sizeof( unsigned short ) );
  memcpy( &data[ 6 ], &uuid.Data3, sizeof( unsigned short ) );
  memcpy( &data[ 8 ], &uuid.Data4, 8 );

}

#elif defined( __MACOSX__ )

void UUID::generate()
{
  CFUUIDRef uuid = CFUUIDCreate(0);
  CFUUIDBytes bytes = CFUUIDGetUUIDBytes(uuid);
  CFRelease(cfuuid);
  data[ 0 ]  = bytes.byteO;
  data[ 1 ]  = bytes.byte1;
  data[ 2 ]  = bytes.byte2;
  data[ 3 ]  = bytes.byte3;
  data[ 4 ]  = bytes.byte4;
  data[ 5 ]  = bytes.byte5;
  data[ 6 ]  = bytes.byte6;
  data[ 7 ]  = bytes.byte7;
  data[ 8 ]  = bytes.byte8;
  data[ 9 ]  = bytes.byte9;
  data[ 10 ] = bytes.byte10;
  data[ 11 ] = bytes.byte11;
  data[ 12 ] = bytes.byte12;
  data[ 13 ] = bytes.byte13;
  data[ 14 ] = bytes.byte14;
  data[ 15 ] = bytes.byte15;
}

#else

// retrieve MAC / IEEE 802 address

static int getieee802( unsigned char* node )
{

  memset( node, 0, 6 );

#if defined(__linux)

  int sock = socket( AF_INET, SOCK_DGRAM, 0 );
  if ( sock == -1 )
  {

    return -1;

  }

  struct ifconf ifc;
  static const int IFC_LEN_MAX = 1024;
  char buf[ IFC_LEN_MAX ];
  ifc.ifc_len = IFC_LEN_MAX;
  ifc.ifc_buf = buf;
  if ( ioctl( sock, SIOCGIFCONF, &ifc ) < 0 )
  {

    close( sock );
    return -1;

  }

  struct ifreq* ifrp = reinterpret_cast< ifreq* >( ifc.ifc_buf );
  for ( unsigned int i = 0; i < ifc.ifc_len / sizeof( ifreq ); ++i, ++ifrp )
  {

    struct ifreq ifr;
    strcpy( ifr.ifr_name, ifrp->ifr_name );
    if ( ioctl( sock, SIOCGIFHWADDR, &ifr ) < 0 )
      continue;
    unsigned8* mac =
      reinterpret_cast< unsigned8* >( &ifr.ifr_hwaddr.sa_data );
    if ( mac[ 0 ] == 0 && mac[ 1 ] == 0 && mac[ 2 ] == 0 &&
         mac[ 3 ] == 0 && mac[ 4 ] == 0 && mac[ 5 ] == 0 )
      continue;
    memcpy( node, mac, 6 );
    close( sock );
    return 0;

  }

  close( sock );

#elif defined(__osf__)

  int sock = socket( AF_INET, SOCK_DGRAM, 0 );
  if ( sock == -1 )
  {

    return -1;

  }

  // enough room for total number of possible interfaces,
  // including aliases for LAT and DECnet
  const int IFREQCNT = 64;
  struct ifreq ifreqs[ IFREQCNT ];
  struct ifconf ifc;
  ifc.ifc_req = ifreqs;
  ifc.ifc_len = IFREQCNT;
  if ( ioctl( sock, SIOCGIFCONF, &ifc ) < 0 )
  {

    close( sock );
    return -1;

  }

  for ( int i = 0; i < IFREQCNT && strlen( ifreqs[ i ].ifr_name ) != 0; ++i )
  {

    if ( ioctl( sock, SIOCGIFFLAGS, &ifreqs[ i ] ) < 0 )
    {

      close( sock );
      return -1;

    }
    if ( ifreqs[ i ].ifr_flags & IFF_POINTOPOINT == IFF_POINTOPOINT )
      continue; // skip point to point
    if ( ifreqs[ i ].ifr_flags & ( IFF_MULTICAST | IFF_BROADCAST ) != 
         ( IFF_MULTICAST | IFF_BROADCAST ) )
      continue; // skip CI and other such interfaces
    struct ifdevea ifrp;
    memset( &ifrp, 0, sizeof( ifrp ) );
    strcpy( ifrp.ifr_name, ifreqs[ i ].ifr_name );
    if ( ioctl( sock, SIOCRPHYSADDR, &ifrp ) < 0 )
      continue;
    assert( ifrp.default_pa[ 0 ] != 0 && ifrp.default_pa[ 1 ] != 0 &&
            ifrp.default_pa[ 2 ] != 0 && ifrp.default_pa[ 3 ] != 0 &&
            ifrp.default_pa[ 4 ] != 0 && ifrp.default_pa[ 5 ] != 0 );
    memcpy(node, ifrp.default_pa, 6);
    close( sock );
    return 0;

  }
  close( sock );

#endif

  return -1;

}


// multiply two 32-bit integers into a 64-bit integer

static void multiply32x32to64( unsigned32 a, unsigned32 b,
                               unsigned32* hi, unsigned32* lo )
{

  unsigned32 a_lo = a & 0xffff;
  unsigned32 a_hi = a >> 16;
  unsigned32 b_lo = b & 0xffff;
  unsigned32 b_hi = b >> 16;

  unsigned32 tmp = a_lo * b_lo;
  *lo = tmp & 0xffff;
  tmp = a_hi * b_lo + ( tmp >> 16 );
  *hi = tmp >> 16;
  tmp = a_lo * b_hi + ( tmp & 0xffff );
  *lo += ( tmp & 0xffff ) << 16;
  *hi += a_hi * b_hi + ( tmp >> 16 );

}


// compare two time values a bit like strcmp()

static inline int timecmp( struct timeval& t1, struct timeval& t2 )
{

  int sec = static_cast< int >( t1.tv_sec - t2.tv_sec );
  return sec ? sec : static_cast< int >( t1.tv_usec - t2.tv_usec );

}


// http://www.opengroup.org/dce/info/draft-leach-uuids-guids-01.txt
//
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                          time_low                             |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |       time_mid                |         time_hi_and_version   |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |clk_seq_hi_res |  clk_seq_low  |         node (0-1)            |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                         node (2-5)                            |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void UUID::generate()
{

  static unsigned8 node_id[ 6 ];
  static struct timeval last;
  static unsigned16 clk_seq;
  static bool initialized = false;
  if (!initialized)
  {

    memset( &node_id[ 0 ], 0, 6 );
    // Get the MAC address.
    if ( getieee802( &node_id[ 0 ] ) == -1 )
    {

      // If the above doesn't work simulate it using the
      // host ID instead. Yes, I know, this is bad...
      // The host ID is a unique 32-bit identifier for
      // the current machine. It is intended to be unique
      // among all UNIX systems in existence.
#ifdef __CYGWIN__

#warning "gethostid() doesn't exist on this system"

      int hostid = 0x01020304;

#else

      int hostid = (int)gethostid(); // Single Unix Specification defect

#endif
      memcpy( &node_id[ 2 ], &hostid, 4 );
      // Set multicast bit, to prevent conflicts with
      // genuine MAC addresses.
      node_id[ 0 ] |= 0x80;

    }
    // Initialize time.
    gettimeofday( &last, 0 );
    // The clock sequence value must be changed whenever:
    // - [...]
    // - The UUID generator has lost its state of the last value
    //   of UTC used, indicating that time may have gone backward;
    //   this is typically the case on reboot.
    // Here the state has been lost if initialized is not true.
    unsigned int seed = last.tv_usec;
    srand( seed );
    clk_seq = rand();
    clk_seq &= 0x1fff;
    if ( clk_seq == 0 )
      ++clk_seq;
    // The UUID generator has been initialized.
    initialized = true;

  }

  // The timestamp is a 60 bit value. For UUID version 1, this is
  // represented by Coordinated Universal Time (UTC) as a count of
  // 100-nanosecond intervals since 00:00:00.00, 15 October 1582
  // (the date of Gregorian reform to the Christian calendar).
  struct timeval time;
  static int adjustment = 0;
  for (;;)
  {

    gettimeofday( &time, 0 );
    if ( timecmp( time, last ) < 0 )
    {

      // The clock sequence value must be changed whenever:
      // - The UUID generator detects that the local value
      //   of UTC has gone backward; this may be due to
      //   normal functioning of the DCE Time Service.
      // - [...]
      ++clk_seq;
      clk_seq &= 0x1fff;
      if ( clk_seq == 0 )
        ++clk_seq;
      adjustment = 0;
      last = time;

    }
    else if ( timecmp( time, last ) == 0 )
    {

      // UUIDs may be created at a rate greater than the
      // system clock resolution. Therefore, the system
      // must also maintain an adjustment value to be added
      // to the lower-order bits of the time. Logically,
      // each time the system clock ticks, the adjustment
      // value is cleared. Every time a UUID is generated,
      // the current adjustment value is read and incremented
      // atomically, then added to the UTC time field of the
      // UUID.
      // 
      // On UNIX gettimeofday() returns system time as a count
      // of microseconds which leaves room for adjustment in
      // the range [0,10[.
      if ( adjustment < 10 )
        ++adjustment;
      else
        continue;

    }
    else
    {

      adjustment = 0;
      last = time;

    }
    break;

  }

  // Set the reserved bits.
  clk_seq &= 0x1fff;
  clk_seq |= 0x8000;

  // Code time since Epoch using 100 ns units.
  unsigned32 time_low;
  unsigned32 time_mhv;
  // Handle seconds. Convert second units to 100 ns units.
  multiply32x32to64( time.tv_sec, 10000000, &time_mhv, &time_low );
  // Handle microseconds. Convert microsecond units to 100 ns units.
  // We are assured that time.tv_usec is less than 999999 so we do
  // not need to handle overflows. Add adjustment.
  unsigned32 tmp = time.tv_usec * 10 + adjustment;
  if ( !( time_low & 0x80000000 ) )
  {

    time_low += tmp;

  }
  else
  {

    time_low += tmp;
    if ( !( time_low & 0x80000000 ) )
      ++time_mhv;

  }
  // Add lap between 15 October 1582 and 1 January 1970 in 100 ns units.
  //  1 January 1970 is Julian Day Number 244 0587.500
  // 15 October 1582 is Julian Day Number 229 9160.500
  // The difference is 141427 days or 12219292800 seconds,
  // that is 122192928000000000 100-nanoseconds, that is:
  // 	0x01B21DD2 << 32 + 0x13814000
  if ( !( time_low & 0x80000000 ) )
  {

    time_low += 0x13814000;

  }
  else
  {

    time_low += 0x13814000;
    if ( !( time_low & 0x80000000 ) )
      ++time_mhv;

  }
  time_mhv += 0x01B21DD2;

  // The high field of the timestamp is multiplexed with the version
  // number which is 1 for the time-based version herein.
  time_mhv &= 0x0fffffff;
  time_mhv |= 0x10000000;

  memcpy( &data[  0 ], &time_low, 4 );
  unsigned16 time_mid = time_mhv & 0xffff;
  memcpy( &data[  4 ], &time_mid, 2 );
  unsigned16 time_hi_and_version = time_mhv >> 16;
  memcpy( &data[  6 ], &time_hi_and_version, 2 );
  unsigned8 clk_seq_hi_res = clk_seq >> 8;
  memcpy( &data[  8 ], &clk_seq_hi_res, 1 );
  unsigned8 clk_seq_low = clk_seq & 0xff;
  memcpy( &data[  9 ], &clk_seq_low, 1 );
  memcpy( &data[ 10 ], node_id, 6 );

}

#endif

}

