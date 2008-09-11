/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


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

