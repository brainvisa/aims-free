
#ifndef AIMS_IO_BYTESWAP_H
#define AIMS_IO_BYTESWAP_H

#include <aims/config/aimsdata_config.h>
#include <aims/def/assert.h>

namespace aims
{

  template <typename T> inline AIMSDATA_API T byteswap( const T & x )
  {
    ASSERT( sizeof( T ) == 4 || sizeof( T ) == 2 || sizeof( T ) == 8 );
    if( sizeof( T ) == 4 )
      return byteswap32( x );
    else if( sizeof( T ) == 2 )
      return byteswap16( x );
    else
      return byteswap64( x );
  }

  template<typename T> inline AIMSDATA_API T byteswap16( const T & x )
  {
    unsigned char	*tmp = (unsigned char *) &x;
    union {
      unsigned short t2;
      T              t1;
    }   t;
    *( (unsigned char *) &t.t2) = *(tmp+1);
    *( ( (unsigned char *) &t.t2) + 1 ) = *tmp;
    return t.t1;
  }

  template<typename T> inline AIMSDATA_API T byteswap32( const T & x )
  {
    unsigned char	*tmp = (unsigned char *) &x;
    union {
      unsigned  t2;
      T         t1;
    }   t;
    *( (unsigned char *) &t.t2) = *(tmp+3);
    *( ( (unsigned char *) &t.t2) + 1 ) = *(tmp+2);
    *( ( (unsigned char *) &t.t2) + 2 ) = *(tmp+1);
    *( ( (unsigned char *) &t.t2) + 3 ) = *tmp;
    return t.t1;
  }

  template<typename T> inline AIMSDATA_API T byteswap64( const T & x )
  {
    unsigned char	*tmp = (unsigned char *) &x;
    union {
      double    t2;
      T         t1;
    }   t;
    *( (unsigned char *) &t.t2) = *(tmp+7);
    *( ( (unsigned char *) &t.t2) + 1 ) = *(tmp+6);
    *( ( (unsigned char *) &t.t2) + 2 ) = *(tmp+5);
    *( ( (unsigned char *) &t.t2) + 3 ) = *(tmp+4);
    *( ( (unsigned char *) &t.t2) + 4 ) = *(tmp+3);
    *( ( (unsigned char *) &t.t2) + 5 ) = *(tmp+2);
    *( ( (unsigned char *) &t.t2) + 6 ) = *(tmp+1);
    *( ( (unsigned char *) &t.t2) + 7 ) = *tmp;
    return t.t1;
  }


  // specializations

  inline AIMSDATA_API short byteswap( const short & x )
  {
    return( byteswap16( x ) );
  }

  inline AIMSDATA_API unsigned short byteswap( const unsigned short & x )
  {
    return( byteswap16( x ) );
  }

  inline AIMSDATA_API int byteswap( const int & x )
  {
    return( byteswap32( x ) );
  }

  inline AIMSDATA_API unsigned int byteswap( unsigned int x )
  {
    return( byteswap32( x ) );
  }

  inline AIMSDATA_API float byteswap( const float & x )
  {
    return( byteswap32( x ) );
  }

}


#endif

