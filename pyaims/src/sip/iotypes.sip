
namespace aims
{

class IOObjectTypesDictionary
{
%TypeHeaderCode
#include <aims/io/fileFormat.h>
%End

%TypeCode
#include <cartobase/plugin/plugin.h>
%End

public:
  // typedef std::set<std::string> (*FormatInfo)();
/* static void registerType( const std::string & objtype,
                            const std::string & datatype, FormatInfo info ); */
  static bool hasType( const std::string & objtype,
                        const std::string & datatype );
  static SIP_PYDICT objectsTypes();
%MethodCode
  carto::PluginLoader::load();
  sipRes = PyDict_New();
  std::map<std::string, std::map<std::string,
    aims::IOObjectTypesDictionary::FormatInfo> >::const_iterator i, e \
    = aims::IOObjectTypesDictionary::types().end();
  std::map<std::string,
    aims::IOObjectTypesDictionary::FormatInfo>::const_iterator
    iot, eot;
  int u = 0;
  for( i=aims::IOObjectTypesDictionary::types().begin(); i!=e; ++i )
  {
    PyObject *p = PyTuple_New( i->second.size() );
    for( u=0, iot=i->second.begin(), eot=i->second.end(); iot!=eot; ++iot, ++u )
      PyTuple_SetItem( p, u, PyString_FromString( iot->first.c_str() ) );
    PyDict_SetItem( sipRes, PyString_FromString( i->first.c_str() ), p );
  }
%End

  static set_STRING formats( const std::string & objectType, const std::string & datatype );
%MethodCode
  carto::PluginLoader::load();
  std::map<std::string, std::map<std::string,
    aims::IOObjectTypesDictionary::FormatInfo> >::const_iterator i
      = aims::IOObjectTypesDictionary::types().find( *a0 );
  if( i != aims::IOObjectTypesDictionary::types().end() )
  {
    std::map<std::string,
      aims::IOObjectTypesDictionary::FormatInfo>::const_iterator iot
      = i->second.find( *a1 );
    if( iot != i->second.end() )
      sipRes = new set_STRING( iot->second() );
  }
%End

};

};

// --- SOMA IO ---

namespace carto // FIXME: will become soma
{

  class IOObjectTypesDictionary
  {
%TypeHeaderCode
#include <soma-io/io/formatdictionary.h>
%End

%TypeCode
#include <cartobase/plugin/plugin.h>
%End

  public:
    static bool hasReadType( const std::string & datatype );
    static bool hasWriteType( const std::string & datatype );

    static SIP_PYDICT readTypes();
%MethodCode
  carto::PluginLoader::load();
  sipRes = PyDict_New();
  std::map<std::string, soma::IOObjectTypesDictionary::FormatInfo>::const_iterator i, e \
    = soma::IOObjectTypesDictionary::readTypes().end();
  std::set<std::string>::const_iterator iot, eot;
  int u = 0;
  for( i=soma::IOObjectTypesDictionary::readTypes().begin(); i!=e; ++i )
  {
    const std::set<std::string> & iset = i->second();
    PyObject *p = PyTuple_New( iset.size() );
    for( u=0, iot=iset.begin(), eot=iset.end(); iot!=eot; ++iot, ++u )
      PyTuple_SetItem( p, u, PyString_FromString( iot->c_str() ) );
    PyDict_SetItem( sipRes, PyString_FromString( i->first.c_str() ), p );
  }
%End

    static SIP_PYDICT writeTypes();
%MethodCode
  carto::PluginLoader::load();
  sipRes = PyDict_New();
  std::map<std::string, soma::IOObjectTypesDictionary::FormatInfo>::const_iterator i, e \
    = soma::IOObjectTypesDictionary::writeTypes().end();
  std::set<std::string>::const_iterator iot, eot;
  int u = 0;
  for( i=soma::IOObjectTypesDictionary::writeTypes().begin(); i!=e; ++i )
  {
    const std::set<std::string> & iset = i->second();
    PyObject *p = PyTuple_New( iset.size() );
    for( u=0, iot=iset.begin(), eot=iset.end(); iot!=eot; ++iot, ++u )
      PyTuple_SetItem( p, u, PyString_FromString( iot->c_str() ) );
    PyDict_SetItem( sipRes, PyString_FromString( i->first.c_str() ), p );
  }
%End

  };

};

