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

/*
 *  Header class
 */
#include <aims/data/header.h>
#include <cartobase/stream/fileutil.h>

using aims::Header;
using namespace carto;
using namespace std;

/*!
  Return the type of the class as a character string.  This base class returns
  "#".  Derived classes _must_ reimplement this function to return a string
  different from all other derived classes.
*/
const char* Header::id() const
{
  static const char dummy_type[] = "#";
  return dummy_type;
}


/*!
  Pure virtual destructors must be implemented.  It's mandatory.
*/
Header::~Header()
{
}


string Header::extension() const
{
  return( "" );
}


set<string> Header::extensions() const
{
  set<string>	exts;
  return( exts );
}


string Header::extension( const string & name ) const
{
  string		base = FileUtil::basename( name );
  string::size_type	pos = base.rfind( '.' );

  if( pos == string::npos )
    return( "" );
  return( base.substr( pos, base.length() - pos ) );
}


string Header::removeExtension( const string & name ) const
{
  string	ext = extension( name );
  set<string>	exs = extensions();
  if( exs.find( ext ) == exs.end() )
    return name;
  return name.substr( 0, name.length() - ext.length() );
}


bool Header::readMinf( const string & )
{
  return( false );
}


bool Header::writeMinf( const string & file )
{
  return( ((const Header *) this)->writeMinf( file ) );
}


bool Header::writeMinf( const string & ) const
{
  return( false );
}
