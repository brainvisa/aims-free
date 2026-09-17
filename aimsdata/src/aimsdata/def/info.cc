// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/def/info.h>
#include <aims/def/path.h>
#include <aims/def/general.h>
#include <aims/def/settings.h>
#include <aims/io/fileFormat.h>
#include <aims/io/argR.h>
#include <cartobase/plugin/plugin.h>
#include <soma-io/writer/pythonwriter.h>
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

  bool dummy __attribute__((unused)) = aimsinfo();

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


void AimsDataInfo::printBuiltins( ostream & /* out */ )
{
#if 0 // builtin plugins is completely obsolete.
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


