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
 *  ...
 */
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <cartobase/exception/format.h>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;


Process::Process()
{
}


Process::~Process()
{
}


void Process::registerProcessType( const string & objType, 
				   const string & dataType, ProcFunc procFunc )
{
  _execs[ objType ][ dataType ] = procFunc;
}


bool Process::execute( const string & filename )
{
  //cout << "Process::execute( " << filename << " )\n";
  Finder	f;
  if( !f.check( filename ) )
    {
      f.launchException();
      throw format_error( filename );
      return( false );
    }
  return( execute( f, filename ) );
}


bool Process::execute( Finder & f, const string & filename )
{
  string	otype = f.objectType();
  string	dtype = f.dataType();

  map<string, map<string, ProcFunc> >::const_iterator	
    ip = _execs.find( otype );
  if( ip == _execs.end() )
    {
      throw datatype_format_error( string( "unsupported data type " ) 
					  + otype + " / " + dtype, filename );
      return( false );
    }
  map<string, ProcFunc>::const_iterator  ip2 = (*ip).second.find( dtype );
  if( ip2 == (*ip).second.end() )
    {
      // Try alternate data types
      vector<string>	posstypes = f.possibleDataTypes();
      unsigned		i, n = posstypes.size();

      for( i=0; i<n; ++i )
	if( posstypes[i] != dtype )
	  {
	    ip2 = (*ip).second.find( posstypes[i] );
	    if( ip2 != (*ip).second.end() )
	      {
		// force new datatype into finder
		f.setDataType( posstypes[i] );
		break;
	      }
	  }
      if( i == n )
	{
	  throw datatype_format_error( string( "unsupported data type " ) 
				       + otype + " / " + dtype, filename );
	  return( false );
	}
    }

  //	execute process function
  return( (*ip2).second( *this, filename, f ) );
}


