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

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <cartobase/config/cartobase-config.h>
#include <cartobase/exception/parse.h>
#include <stdio.h>
#include <errno.h>


using namespace std;
using namespace carto;


//=============================================================================
//	M E T H O D S
//=============================================================================

parse_error::parse_error( const string& what_arg,
                          const string& token_arg,
                          const string& filename_arg,
                          int line_arg )
            : io_error( init( what_arg,
			      token_arg,
			      filename_arg,
			      line_arg ), "" ),
              _filename( filename_arg ),
              _line( line_arg )
{
}


parse_error::parse_error( const string& what_arg,
                          const string& token_arg,
                          int line_arg )
            : io_error( init( what_arg, token_arg, line_arg ), "" ),
              _line( line_arg )
{
}


parse_error::parse_error( const string& what_arg, int line_arg,
                          const string& filename_arg )
            : io_error( what_arg, "" ),
              _filename( filename_arg ),
              _line( line_arg )
{
}


parse_error::~parse_error() throw ()
{
}


const char* parse_error::filename() const
{

  return _filename.c_str();

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
  s += ": found \"";
  s += token_arg;
  s += "\" while waiting for \"";
  s += what_arg;
  s += "\" (line ";
  char buffer[ 10 ];
  sprintf(buffer, "%d", line_arg);
  s += buffer;
  s += ")";
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
  s += "\" (line ";
  char buffer[ 10 ];
  sprintf(buffer, "%d", line_arg);
  s += buffer;
  s += ")";
  return s;

}
