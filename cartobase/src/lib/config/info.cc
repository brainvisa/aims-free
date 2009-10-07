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

#include <cartobase/config/info.h>
#include <cartobase/plugin/plugin.h>
#include <cartobase/wip/io/formatdictionary.h>
#include <cartobase/config/version.h>
#include <algorithm>

using namespace carto;
using namespace std;


vector<Info*> & Info::infos()
{
  static bool initialized = false;
  static vector<Info*>	inf;
  if( !initialized )
    {
      initialized = true;
      new CartoBaseInfo;
    }
  return inf;
}


Info::Info()
{
  infos().push_back( this );
}


Info::~Info()
{
  vector<Info*>	& inf = infos();
  vector<Info*>::iterator 
    i = find( inf.begin(), inf.end(), this );
  if( i != inf.end() )
    inf.erase( i );
}


void Info::printPaths( ostream & )
{
}


void Info::printBuiltins( ostream & )
{
}


void Info::printOtherInfo( ostream & )
{
}


void Info::printVersion( ostream & )
{
}


void Info::printVersions( ostream & out )
{
  vector<Info*>::iterator	i, n = infos().end();
  for( i=infos().begin(); i!=n; ++i )
    (*i)->printVersion( out );
}


void Info::print( ostream & out )
{
  printVersions( out );
  out << "General information:" << endl;
  vector<Info*>::iterator	i, n = infos().end();
  for( i=infos().begin(); i!=n; ++i )
    (*i)->printPaths( out );
  out << "- Compiled with the following built-in options:" << endl;
  for( i=infos().begin(); i!=n; ++i )
    (*i)->printBuiltins( out );
  out << "Plugin support    : ";
#ifndef CARTO_NO_DLOPEN
  out << "yes" << endl;
#else
  out << "no" << endl;
#endif
  out << "Plugins:" << endl;
  set<Plugin*>		pl = PluginManager::singleton().plugins();
  if( pl.empty() )
    out << "(none)" << endl;
  else
    {
      set<Plugin*>::iterator	ip, ep = pl.end();
      unsigned			i = 0;
      for( ip=pl.begin(); ip!=ep; ++ip, ++i )
	out << i << "\t" << (*ip)->name() << endl;
    }
  out << "- IO types and formats:" << endl;
  out << "  Input formats:" << endl;
  map<string, IOObjectTypesDictionary::FormatInfo> 
    & types = IOObjectTypesDictionary::readTypes();
  if( types.empty() )
    out << "(none, there is obviously a problem !)\n";
  else
    {
      map<string, IOObjectTypesDictionary::FormatInfo>::iterator 
	ifo, efo = types.end();
      set<string>::iterator				ifm, efm;
      for( ifo=types.begin(); ifo!=efo; ++ifo )
        {
          out << "Data type: " << ifo->first << " - formats : ";
          set<string>	formats = ifo->second();
          if( formats.empty() )
            out << "(none)" << endl;
          else
            {
              for( ifm=formats.begin(), efm=formats.end(); ifm!=efm; ++ifm )
                out << *ifm << " ";
              out << endl;
            }
        }
    }

  out << endl;
  out << "  Output formats:" << endl;
  map<string, IOObjectTypesDictionary::FormatInfo> 
    & otypes = IOObjectTypesDictionary::writeTypes();
  if( otypes.empty() )
    out << "(none, there is obviously a problem !)\n";
  else
    {
      map<string, IOObjectTypesDictionary::FormatInfo>::iterator 
	ifo, efo = otypes.end();
      set<string>::iterator				ifm, efm;
      for( ifo=otypes.begin(); ifo!=efo; ++ifo )
	  {
	    out << "Data type: " << ifo->first << " - formats : ";
	    set<string>	formats = ifo->second();
	    if( formats.empty() )
	      out << "(none)" << endl;
	    else
	      {
		for( ifm=formats.begin(), efm=formats.end(); ifm!=efm; ++ifm )
		  out << *ifm << " ";
		out << endl;
	      }
	  }
    }
  out << endl;

  for( i=infos().begin(); i!=n; ++i )
    (*i)->printOtherInfo( out );
}


CartoBaseInfo::CartoBaseInfo()
  : Info()
{
}


CartoBaseInfo::~CartoBaseInfo()
{
}


void CartoBaseInfo::printVersion( ostream & out )
{
  out << "CartoBase version : " << cartobaseVersionString() << endl;
}


