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
 *  Data reader class
 */
#include <aims/io/fileFormat.h>

using namespace aims;
using namespace std;

void IOObjectTypesDictionary::registerType( const std::string & objtype, 
					    const std::string & datatype, 
					    FormatInfo info )
{
  /*cout << "registerType: " << objtype << ", " << datatype << " : ";
  set<string>	ss = info();
  set<string>::iterator	is, es = ss.end();
  for( is=ss.begin(); is!=es; ++is )
    cout << *is << " ";
    cout << endl;*/
  types()[ objtype ][ datatype ] = info;
}


map<string, map<string, IOObjectTypesDictionary::FormatInfo> > 
& IOObjectTypesDictionary::types()
{
  static map<string, map<string, FormatInfo> >	t;
  return( t );
}


bool IOObjectTypesDictionary::hasType( const std::string & objtype, 
				       const std::string & datatype )
{
  map<string, map<string, IOObjectTypesDictionary::FormatInfo> > & t = types();
  map<string, map<string, IOObjectTypesDictionary::FormatInfo> >::iterator 
    i = t.find( objtype );
  if( i == t.end() )
    return( false );
  map<string, IOObjectTypesDictionary::FormatInfo>::iterator 
    j = i->second.find( datatype );
  return( j != i->second.end() );
}


FinderFormat::~FinderFormat()
{
}


