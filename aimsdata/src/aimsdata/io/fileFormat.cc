// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

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


