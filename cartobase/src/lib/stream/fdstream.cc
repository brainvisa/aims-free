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

#include <cartobase/stream/fdstream.h>

/* The following code declares classes to read from and write to
 * file descriptore or file handles.
 *
 * See
 *      http://www.josuttis.com/cppcode
 * for details and the latest version.
 *
 * - open:
 *      - integrating BUFSIZ on some systems?
 *      - optimized reading of multiple characters
 *      - stream for reading AND writing
 *      - i18n
 *
 * (C) Copyright Nicolai M. Josuttis 2001.
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 * Version: Jul 28, 2002
 * History:
 *  Jul 28, 2002: bugfix memcpy() => memmove()
 *                fdinbuf::underflow(): cast for return statements
 *  Aug 05, 2001: first public version
 */

// for EOF:
#include <cstdio>
// for memmove():
#include <cstring>

// low-level read and write functions
#ifdef _MSC_VER
# include <io.h>
#else
# include <unistd.h>
//extern "C" {
//    int write (int fd, const char* buf, int num);
//    int read (int fd, char* buf, int num);
//}
#endif

#ifdef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <winsock.h>
#endif

//debug
#include <iostream>
using namespace std;

using namespace boost;


// constructor
fdoutbuf::fdoutbuf (int _fd ) : fd(_fd)
{
#ifdef _WIN32
  struct stat _s_;
  if( fstat( _fd, &_s_ ) == 0 )
    _sock = false;
  else
    _sock = true;
#endif
}


// write one character
fdoutbuf::int_type fdoutbuf::overflow (fdoutbuf::int_type c)
{
  if (c != EOF) {
    char z = c;
#ifdef _WIN32
    if( _sock )
      {
        if (send (fd, &z, 1, 0) != 1)
          return EOF;
      }
    else
#endif
      if (write (fd, &z, 1) != 1)
        {
          return EOF;
        }
  }
  return c;
}


// write multiple characters
std::streamsize fdoutbuf::xsputn (const char* s,
                                  std::streamsize num)
{
#ifdef _WIN32
  if( _sock )
    return send (fd, s, num, 0);
  else
#endif
    return write(fd,s,num);
}


fdinbuf::fdinbuf (int _fd) : fd(_fd)
{
  setg (buffer+pbSize,     // beginning of putback area
        buffer+pbSize,     // read position
        buffer+pbSize);    // end position
#ifdef _WIN32
  struct stat _s_;
  if( fstat( _fd, &_s_ ) == 0 )
    _sock = false;
  else
    _sock = true;
#endif
}


fdinbuf::int_type fdinbuf::underflow ()
{
#ifndef _MSC_VER
  using std::memmove;
#endif

  // is read position before end of buffer?
  if (gptr() < egptr()) {
#if defined( __GNUC__ ) && __GNUC__-0 < 3
    return *gptr();
#else
    return traits_type::to_int_type(*gptr());
#endif
  }
  /* process size of putback area
   * - use number of characters read
   * - but at most size of putback area
   */
  int numPutback;
  numPutback = gptr() - eback();
  if (numPutback > pbSize) {
    numPutback = pbSize;
  }

  /* copy up to pbSize characters previously read into
   * the putback area
   */
  memmove (buffer+(pbSize-numPutback), gptr()-numPutback,
           numPutback);

  // read at most bufSize new characters
  int num;
#ifdef _WIN32
  if( _sock )
    {
      errno = 0;
      // set blocking
      unsigned long	blk = 0;
      int x = ioctlsocket( fd, FIONBIO, &blk );
      /* if( x != 0 )
        {
          cerr << "socket error on ioctl: " << errno << ", sock: " << fd 
               << ", returned: " << x << endl << flush;
          x = WSAGetLastError();
          cerr << "WSAGetLastError(): " << x << endl << flush;
          } */
      fd_set	fds;
      FD_ZERO( &fds );
      FD_SET( fd, &fds );
      // wait
      errno = 0;
      x = select( fd + 1, &fds, 0, 0, 0 );
      if( x != 1 )
        cerr << "socket error on select: " << errno << ", returned: " << x 
             << endl << flush;
      //cout << "fdistream: recv'ing on socket\n" << flush;
      errno = 0;
      num = recv( fd, buffer+pbSize, bufSize, 0 );
      //cout << "fdistream: received " << num << " bytes\n" << flush;
      if( num < 0 )
        {
          cout << "recv error: " << errno << endl << flush;
        }
    }
  else
#endif
    num = read (fd, buffer+pbSize, bufSize);
  if (num <= 0) {
    // ERROR or EOF
    return EOF;
  }

  // reset buffer pointers
  setg (buffer+(pbSize-numPutback),   // beginning of putback area
        buffer+pbSize,                // read position
        buffer+pbSize+num);           // end of buffer

  // return next character
#if defined( __GNUC__ ) && __GNUC__-0 < 3
  return *gptr();
#else
  return traits_type::to_int_type(*gptr());
#endif
}


