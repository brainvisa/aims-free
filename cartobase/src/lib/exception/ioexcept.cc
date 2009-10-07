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

#include <cartobase/exception/ioexcept.h>
#include <cartobase/exception/parse.h>
#include <cartobase/exception/file.h>
#include <cartobase/exception/format.h>
#include <cartobase/stream/sstream.h>
#include <cerrno>
#include <string.h>

using namespace carto;
using namespace std;


io_error::io_error( const string & arg, const std::string & filename )
  : runtime_error( arg ), _filename ( filename )
{
}


io_error::~io_error() throw()
{
}


const char* io_error::what() const throw()
{
  if( !_filename.empty() )
    {
      _formed_msg = _filename + " : " + runtime_error::what();
      return _formed_msg.c_str();
    }
  else
    return runtime_error::what();
}


open_error::open_error( const string & arg, const std::string & filename )
  : io_error( arg, filename )
{
}


open_error::~open_error() throw()
{
}


file_not_found_error::file_not_found_error( const string & arg, 
					    const std::string & filename )
  : open_error( arg, filename )
{
}


file_not_found_error::file_not_found_error( const std::string & filename )
  : open_error( "file not found", filename )
{
}


file_not_found_error::~file_not_found_error() throw()
{
}


permission_error::permission_error( const string & arg, 
				    const std::string & filename )
  : open_error( arg, filename )
{
}


permission_error::permission_error( const std::string & filename )
  : open_error( "no read/write permission", filename )
{
}


permission_error::~permission_error() throw()
{
}


is_dir_error::is_dir_error( const string & arg, const std::string & filename )
  : open_error( arg, filename )
{
}


is_dir_error::is_dir_error( const std::string & filename )
  : open_error( "attempt to access a directory as a file", filename )
{
}


is_dir_error::~is_dir_error() throw()
{
}


no_dir_error::no_dir_error( const string & arg, const std::string & filename )
  : file_not_found_error( arg, filename )
{
}


no_dir_error::no_dir_error( const std::string & filename )
  : file_not_found_error( "invalid directory", filename )
{
}


no_dir_error::~no_dir_error() throw()
{
}


too_many_descriptors_error::too_many_descriptors_error( const string & arg, 
							const std::string 
							& filename )
  : open_error( arg, filename )
{
}


too_many_descriptors_error::too_many_descriptors_error( const std::string 
							& filename )
  : open_error( "too many open descriptors", filename )
{
}


too_many_descriptors_error::~too_many_descriptors_error() throw()
{
}


read_write_error::read_write_error( const string & arg, 
				    const std::string & filename )
  : io_error( arg, filename )
{
}


read_write_error::~read_write_error() throw()
{
}


bad_medium_error::bad_medium_error( const string & arg, 
				    const std::string & filename )
  : read_write_error( arg, filename )
{
}


bad_medium_error::bad_medium_error( const std::string & filename )
  : read_write_error( "bad medium (disk or network failure)", filename )
{
}


bad_medium_error::~bad_medium_error() throw()
{
}


disk_full_error::disk_full_error( const string & arg, 
				  const std::string & filename )
  : read_write_error( arg, filename )
{
}


disk_full_error::disk_full_error( const std::string & filename )
  : read_write_error( "disk full", filename )
{
}


disk_full_error::~disk_full_error() throw()
{
}


quota_exceeded_error::quota_exceeded_error( const string & arg, 
					    const std::string & filename )
  : disk_full_error( arg, filename )
{
}


quota_exceeded_error::quota_exceeded_error( const std::string & filename )
  : disk_full_error( "disk quota exceeded", filename )
{
}


quota_exceeded_error::~quota_exceeded_error() throw()
{
}


stream_state_error::stream_state_error( const string & arg, 
					const std::string & filename )
  : read_write_error( arg, filename )
{
}


stream_state_error::~stream_state_error() throw()
{
}


eof_error::eof_error( const string & arg, const std::string & filename )
  : stream_state_error( arg, filename )
{
}


eof_error::eof_error( const std::string & filename )
  : stream_state_error( "premature end of file", filename )
{
}


eof_error::~eof_error() throw()
{
}


not_open_error::not_open_error( const string & arg, 
				const std::string & filename )
  : stream_state_error( arg, filename )
{
}


not_open_error::not_open_error( const std::string & filename )
  : stream_state_error( "stream not open", filename )
{
}


not_open_error::~not_open_error() throw()
{
}


stream_fail_error::stream_fail_error( const string & arg, 
				      const std::string & filename )
  : stream_state_error( arg, filename )
{
}


stream_fail_error::stream_fail_error( const std::string & filename )
  : stream_state_error( "stream in fail state", filename )
{
}


stream_fail_error::~stream_fail_error() throw()
{
}


format_error::format_error( const string & arg, const std::string & filename )
  : io_error( arg, filename )
{
}


format_error::format_error( const std::string & filename )
  : io_error( "format error", filename )
{
}


format_error::~format_error() throw()
{
}


wrong_format_error::wrong_format_error( const string & arg, 
					const std::string & filename )
  : format_error( arg, filename )
{
}


wrong_format_error::wrong_format_error( const std::string & filename )
  : format_error( "unrecognized format", filename )
{
}


wrong_format_error::~wrong_format_error() throw()
{
}


format_mismatch_error::format_mismatch_error( const string & arg, 
					      const std::string & filename )
  : format_error( arg, filename )
{
}


format_mismatch_error::format_mismatch_error( const std::string & filename )
  : format_error( "format mismatch", filename )
{
}


format_mismatch_error::~format_mismatch_error() throw()
{
}


corrupt_stream_error::corrupt_stream_error( const string & arg, 
					    const std::string & filename )
  : format_mismatch_error( arg, filename )
{
}


corrupt_stream_error::corrupt_stream_error( const std::string & filename )
  : format_mismatch_error( "corrupt stream", filename )
{
}


corrupt_stream_error::~corrupt_stream_error() throw()
{
}


invalid_format_error::invalid_format_error( const string & arg, 
					    const std::string & filename )
  : format_mismatch_error( arg, filename )
{
}


invalid_format_error::invalid_format_error( const std::string & filename )
  : format_mismatch_error( "invalid format (not handled)", filename )
{
}


invalid_format_error::~invalid_format_error() throw()
{
}


datatype_format_error::datatype_format_error( const string & arg, 
					    const std::string & filename )
  : invalid_format_error( arg, filename )
{
}


datatype_format_error::datatype_format_error( const std::string & filename )
  : invalid_format_error( "unsupported data type", filename )
{
}


datatype_format_error::~datatype_format_error() throw()
{
}


syntax_check_error::syntax_check_error( const string & arg, 
					const std::string & filename )
  : invalid_format_error( arg, filename )
{
}


syntax_check_error::syntax_check_error( const std::string & filename )
  : invalid_format_error( "syntax check error", filename )
{
}


syntax_check_error::~syntax_check_error() throw()
{
}


//

parse_error::parse_error( const string& what_arg,
                          const string& token_arg,
                          const string& filename_arg,
                          int line_arg )
  : format_error( init( what_arg, token_arg, filename_arg, line_arg ), "" ),
    _filename( filename_arg ), _line( line_arg )
{
}


parse_error::parse_error( const string& what_arg, const string& token_arg,
                          int line_arg )
  : format_error( init( what_arg, token_arg, line_arg ), "" ), 
    _line( line_arg )
{
}


parse_error::parse_error( const string& what_arg, int line_arg,
                          const string& filename_arg )
  : format_error( what_arg, "" ), _filename( filename_arg ), _line( line_arg )
{
}


parse_error::~parse_error() throw ()
{
}


int parse_error::line() const
{
  return _line;
}


string parse_error::init( const string& what_arg,
                          const string& token_arg,
                          const string& filename_arg,
                          int line_arg )
{
  string s( filename_arg );
  if( !filename_arg.empty() )
    s += ": ";
  s += "found \"";
  s += token_arg;
  s += "\" while waiting for \"";
  s += what_arg;
  s += "\"";
  if( line_arg > 0 )
    {
      stringstream	ss;
      ss << " (line " << line_arg << ")";
      s += ss.str();
    }
  return s;
}


string parse_error::init( const string& what_arg,
                          const string& token_arg,
                          int line_arg )
{
  string s("found \"");
  s += token_arg;
  s += "\" while waiting for \"";
  s += what_arg;
  s += "\"";
  if( line_arg > 0 )
    {
      stringstream	ss;
      ss << " (line " << line_arg << ")";
      s += ss.str();
    }
  return s;
}


// factory / static things

void io_error::launchErrnoExcept( const string & filename )
{
  switch( errno )
    {
    case ENOENT:
      throw file_not_found_error( strerror( errno ), filename );
    case EIO:
      throw read_write_error( strerror( errno ), filename );
    case EACCES:
      throw permission_error( strerror( errno ), filename );
    case EISDIR:
      throw is_dir_error( strerror( errno ), filename );
    case EMFILE:
      throw too_many_descriptors_error( strerror( errno ), filename );
    case ENOSPC:
      throw disk_full_error( strerror( errno ), filename );
    case EROFS:
      throw permission_error( strerror( errno ), filename );
#ifdef ECOMM
    case ECOMM:
      throw bad_medium_error( strerror( errno ), filename );
#endif
#ifdef EBADFD
    case EBADFD:
      throw stream_state_error( strerror( errno ), filename );
#endif
#ifdef ENETDOWN
    case ENETDOWN:
#endif
#ifdef ENETUNREACH
    case ENETUNREACH:
#endif
#ifdef ENETRESET
    case ENETRESET:
#endif
#ifdef ENOMEDIUM
    case ENOMEDIUM:
#endif
#ifdef EMEDIUMTYPE
    case EMEDIUMTYPE:
#endif
      throw bad_medium_error( strerror( errno ), filename );
    default:
      throw io_error( strerror( errno ), filename );
    }
}


namespace
{

  void copy_except( int p, int n, exception & e, int & prio, 
		    int & type, string & message )
  {
    if( prio < p )
      {
	prio = p;
	type = n;
	message = e.what();
      }
  }

}

void io_error::keepExceptionPriority( exception & e, int & prio, 
				      int & type, string & message, int rp )
{
  //cout << "keep_exception_priority " << e.what() << endl;

  // check several kinds of exceptions
  // (no other way to do it: exception has no copy constructor)
  {
    corrupt_stream_error *te = dynamic_cast<corrupt_stream_error *>( &e );
    if( te )
      {
	copy_except( 10+rp, 1, e, prio, type, message );
	return;
      }
  }
  {
    invalid_format_error *te = dynamic_cast<invalid_format_error *>( &e );
    if( te )
      {
	copy_except( 9+rp, 2, e, prio, type, message );
	return;
      }
  }
  {
    format_mismatch_error *te = dynamic_cast<format_mismatch_error *>( &e );
    if( te )
      {
	copy_except( 8+rp, 3, e, prio, type, message );
	return;
      }
  }
  {
    wrong_format_error *te = dynamic_cast<wrong_format_error *>( &e );
    if( te )
      {
	copy_except( 3+rp, 4, e, prio, type, message );
	return;
      }
  }
  {
    stream_fail_error *te = dynamic_cast<stream_fail_error *>( &e );
    if( te )
      {
	copy_except( 5+rp, 6, e, prio, type, message );
	return;
      }
  }
  {
    not_open_error *te = dynamic_cast<not_open_error *>( &e );
    if( te )
      {
	copy_except( 5+rp, 7, e, prio, type, message );
	return;
      }
  }
  {
    eof_error *te = dynamic_cast<eof_error *>( &e );
    if( te )
      {
	copy_except( 5+rp, 8, e, prio, type, message );
	return;
      }
  }
  {
    stream_state_error *te = dynamic_cast<stream_state_error *>( &e );
    if( te )
      {
	copy_except( 4+rp, 9, e, prio, type, message );
	return;
      }
  }
  {
    quota_exceeded_error *te = dynamic_cast<quota_exceeded_error *>( &e );
    if( te )
      {
	copy_except( 8+rp, 10, e, prio, type, message );
	return;
      }
  }
  {
    disk_full_error *te = dynamic_cast<disk_full_error *>( &e );
    if( te )
      {
	copy_except( 8+rp, 11, e, prio, type, message );
	return;
      }
  }
  {
    bad_medium_error *te = dynamic_cast<bad_medium_error *>( &e );
    if( te )
      {
	copy_except( 9+rp, 12, e, prio, type, message );
	return;
      }
  }
  {
    read_write_error *te = dynamic_cast<read_write_error *>( &e );
    if( te )
      {
	copy_except( 6+rp, 13, e, prio, type, message );
	return;
      }
  }
  {
    too_many_descriptors_error *te 
      = dynamic_cast<too_many_descriptors_error *>( &e );
    if( te )
      {
	copy_except( 7+rp, 14, e, prio, type, message );
	return;
      }
  }
  {
    no_dir_error *te = dynamic_cast<no_dir_error *>( &e );
    if( te )
      {
	copy_except( 8+rp, 15, e, prio, type, message );
	return;
      }
  }
  {
    permission_error *te = dynamic_cast<permission_error *>( &e );
    if( te )
      {
        // La ligne suivante fut commentée dans le doute
        //cout << "perm error\n";
	copy_except( 8+rp, 16, e, prio, type, message );
	return;
      }
  }
  {
    file_not_found_error *te = dynamic_cast<file_not_found_error *>( &e );
    if( te )
      {
	copy_except( 1+rp, 17, e, prio, type, message );
	return;
      }
  }
  {
    open_error *te = dynamic_cast<open_error *>( &e );
    if( te )
      {
	copy_except( 1+rp, 18, e, prio, type, message );
	return;
      }
  }
  {
    datatype_format_error *te = dynamic_cast<datatype_format_error *>( &e );
    if( te )
      {
	copy_except( 9+rp, 20, e, prio, type, message );
	return;
      }
  }
  {
    is_dir_error *te = dynamic_cast<is_dir_error *>( &e );
    if( te )
      {
	copy_except( 7+rp, 21, e, prio, type, message );
	return;
      }
  }
  {
    syntax_check_error *te = dynamic_cast<syntax_check_error *>( &e );
    if( te )
      {
	copy_except( 10+rp, 23, e, prio, type, message );
	return;
      }
  }
  {
    errno_error *te = dynamic_cast<errno_error *>( &e );
    if( te )
      {
	copy_except( 1+rp, 24, e, prio, type, message );
	return;
      }
  }
  {
    file_error *te = dynamic_cast<file_error *>( &e );
    if( te )
      {
	copy_except( 1+rp, 25, e, prio, type, message );
	return;
      }
  }
  {
    invalid_positive_integer *te 
      = dynamic_cast<invalid_positive_integer *>( &e );
    if( te )
      {
	copy_except( 5+rp, 26, e, prio, type, message );
	return;
      }
  }
  {
    invalid_integer *te = dynamic_cast<invalid_integer *>( &e );
    if( te )
      {
	copy_except( 5+rp, 27, e, prio, type, message );
	return;
      }
  }
  {
    invalid_boolean *te = dynamic_cast<invalid_boolean *>( &e );
    if( te )
      {
	copy_except( 5+rp, 31, e, prio, type, message );
	return;
      }
  }
  {
    invalid_number *te = dynamic_cast<invalid_number *>( &e );
    if( te )
      {
	copy_except( 5+rp, 28, e, prio, type, message );
	return;
      }
  }
  {
    out_of_range_error *te = dynamic_cast<out_of_range_error *>( &e );
    if( te )
      {
	copy_except( 5+rp, 30, e, prio, type, message );
	return;
      }
  }
  {
    range_error *te = dynamic_cast<range_error *>( &e );
    if( te )
      {
	copy_except( 5+rp, 29, e, prio, type, message );
	return;
      }
  }
  {
    parse_error *te = dynamic_cast<parse_error *>( &e );
    if( te )
      {
	copy_except( 5+rp, 22, e, prio, type, message );
	return;
      }
  }
  {
    format_error *te = dynamic_cast<format_error *>( &e );
    if( te )
      {
	copy_except( 2+rp, 5, e, prio, type, message );
	return;
      }
  }
  {
    io_error *te = dynamic_cast<io_error *>( &e );
    if( te )
      {
	copy_except( 1+rp, 19, e, prio, type, message );
	return;
      }
  }
  // default non-io exception
  copy_except( rp, -1, e, prio, type, message );
}


void io_error::launchExcept( int code, const string & excm, 
			     const string & defmsg )
{
  switch( code )
    {
    case 1:
      throw corrupt_stream_error( excm, "" );
    case 2:
      throw invalid_format_error( excm, "" );
    case 3:
      throw format_mismatch_error( excm, "" );
    case 4:
      throw wrong_format_error( excm, "" );
    case 5:
      throw format_error( excm, "" );
    case 6:
      throw stream_fail_error( excm, "" );
    case 7:
      throw not_open_error( excm, "" );
    case 8:
      throw eof_error( excm, "" );
    case 9:
      throw stream_state_error( excm, "" );
    case 12:
      throw bad_medium_error( excm, "" );
    case 13:
      throw read_write_error( excm, "" );
    case 14:
      throw too_many_descriptors_error( excm, "" );
    case 15:
      throw no_dir_error( excm, "" );
    case 16:
      throw permission_error( excm, "" );
    case 17:
      throw file_not_found_error( excm, "" );
    case 18:
      throw open_error( excm, "" );
    case 19:
      throw io_error( excm, "" );
    case 20:
      throw datatype_format_error( excm, "" );
    case 21:
      throw is_dir_error( excm, "" );
    case 22:
      throw parse_error( excm, "", 0 );
    case 23:
      throw syntax_check_error( excm, "" );
    case 24:
      throw errno_error();
    case 25:
      throw file_error( excm, "" );
    case 26:
      throw invalid_positive_integer( excm );
    case 27:
      throw invalid_integer( excm );
    case 28:
      throw invalid_number( excm );
    case 29:
      throw range_error( excm );
    case 30:
      throw out_of_range_error( excm );
    case 31:
      throw invalid_boolean( excm );
    default:
      if( defmsg.empty() )
      {
        if( excm.empty() )
          throw runtime_error( "exception (no message)" );
        else
          throw carto::wrong_format_error( excm, "" );
      }
      throw carto::wrong_format_error( defmsg, "" );
    }
}


