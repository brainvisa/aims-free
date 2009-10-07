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

#include <cartobase/exception/file.h>
#include <errno.h>
#include <string.h>


using namespace std;
using namespace carto;


//=============================================================================
//	M E T H O D S
//=============================================================================

file_error::file_error( const string& filename_arg )
	   : errno_error( init( filename_arg ) ),
             _filename( filename_arg )
{
}


file_error::file_error( const string&, const string& filename_arg )
	   : errno_error( init( filename_arg ) ),
             _filename( filename_arg )
{
}


file_error::~file_error() throw ()
{
}


const char* file_error::filename() const
{

  return _filename.c_str();

}


string file_error::init( const string& filename_arg )
{

  string s(filename_arg);
  s += ": ";
  s += strerror(errno);
  return s;

}
