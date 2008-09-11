/* Copyright (c) 1995-2006 CEA
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

#include <aims/def/info.h>
#include <aims/def/path.h>
#include <aims/def/general.h>
#include <aims/def/settings.h>
#include <aims/io/fileFormat.h>
#include <aims/io/argR.h>
#include <cartobase/plugin/plugin.h>
#include <cartobase/object/pythonwriter.h>
#include <algorithm>

using namespace aims;
using namespace carto;
using namespace std;


namespace
{

  bool aimsinfo()
  {
    new AimsDataInfo;
    return true;
  }

  bool dummy = aimsinfo();

}


AimsDataInfo::AimsDataInfo() : Info()
{
}


AimsDataInfo::~AimsDataInfo()
{
}


void AimsDataInfo::printPaths( ostream & out )
{
  const Path	& p = Path::singleton();
  out << "global shared path : " << p.globalShared() << endl;
  out << "aims shared   path : " << p.dependencies() << endl;
  out << "mmap          path : " << p.memmap() << endl;
  out << "nomenclature  path : " << p.nomenclature() << endl;
}


void AimsDataInfo::printBuiltins( ostream & out )
{
  out << "VIDA IO           : ";
#ifdef VIDA_LIB
  out << "yes" << endl;
#else
  out << "no" << endl;
#endif
  out << "JPEG IO           : ";
#ifdef AIMS_JPEG
  out << "yes" << endl;
#else
  out << "no" << endl;
#endif
  out << "DICOM input       : ";
#ifdef DICOM_LIB
  out << "yes" << endl;
#else
  out << "no" << endl;
#endif
  out << "ECAT IO           : ";
#ifdef ECAT_LIB
  out << "yes" << endl;
#else
  out << "no" << endl;
#endif
}


void AimsDataInfo::printVersion( ostream & out )
{
  out << "AIMS version : " << aims::aimsVersion() << endl;
}


void AimsDataInfo::printOtherInfo( ostream & out )
{
  out << "- AIMS IO types and formats (old API):" << endl;
  map<string, map<string, IOObjectTypesDictionary::FormatInfo> > 
    & types = IOObjectTypesDictionary::types();
  if( types.empty() )
    out << "(none, there is obviously a problem !)\n";
  else
    {
      map<string, map<string, IOObjectTypesDictionary::FormatInfo> >::iterator 
	it, et = types.end();
      map<string, IOObjectTypesDictionary::FormatInfo>::iterator 
	ifo, efo;
      set<string>::iterator				ifm, efm;
      for( it=types.begin(); it!=et; ++it )
	for( ifo=it->second.begin(), efo=it->second.end(); ifo!=efo; ++ifo )
	  {
	    out << "Data type: " << it->first << "/" << ifo->first 
		 << " - formats : ";
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

  // graph formats IO
  out << "- Graph sub-formats readers:\n";
  set<string>	argreaders = ArgReader::readers();
  if( argreaders.empty() )
    out << "(none, strange: this shouldn't happen)\n";
  else
    {
      set<string>::iterator	iar, ear = argreaders.end();
      for( iar=argreaders.begin(); iar!=ear; ++iar )
	if( iar->empty() )
	  out << "Generic" << endl;
	else
	  out << *iar << endl;
    }

  // aimsrc settings
  out << "- AIMS settings:" << endl;
  Settings	& sett = Settings::settings();
  PythonWriter	pw;
  pw.attach( out );
  pw.write( sett, false, false );
}


