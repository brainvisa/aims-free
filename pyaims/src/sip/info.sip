
%ModuleCode
#include <cartobase/config/version.h>
%End

SIP_PYTUPLE version();
%Docstring
AIMS / Cartobase version, as a 2-digit tuple
%End

%MethodCode
  std::string v = carto::cartobaseShortVersion();
  std::string::size_type n = v.find( '.' );
  sipRes = PyTuple_New( 2 );
#if PY_VERSION_HEX >= 0x03000000
  PyTuple_SetItem( sipRes, 0, PyLong_FromString(
    const_cast<char *>( v.substr( 0, n ).c_str() ), 0, 10 ) );
  PyTuple_SetItem( sipRes, 1, PyLong_FromString(
    const_cast<char *>( v.substr( n+1, v.length() - n - 1 ).c_str() ), 0,
    10 ) );
#else
  PyTuple_SetItem( sipRes, 0, PyInt_FromString(
    const_cast<char *>( v.substr( 0, n ).c_str() ), 0, 10 ) );
  PyTuple_SetItem( sipRes, 1, PyInt_FromString(
    const_cast<char *>( v.substr( n+1, v.length() - n - 1 ).c_str() ), 0,
    10 ) );
#endif
%End


std::string versionString();
%Docstring
AIMS / Cartobase full version, as a string
%End

%MethodCode
  sipRes = new std::string( carto::cartobaseVersionString() );
%End


namespace carto
{

  class Info
  {
%TypeHeaderCode
#include <cartobase/config/info.h>
#ifndef PYAIMSSIP_VECTOR_Info_DEFINED
#define PYAIMSSIP_VECTOR_Info_DEFINED
typedef std::vector<carto::Info*> vector_InfoPtr;
#endif
%End

%Docstring
Information class, which provides information about the AIMS / Cartobase libraries, such as libraries versions, plugins, data paths, etc.

May be used in 2 ways:
* use static methods: Info.printVersions(), Info.printInfos() to directly output info on the standard output
* get a list of Info objects using Info.infos() then use the specific methods on the Info objects. There may be several Info objects since higher-level libraries and applications may add their own ones.
%End

  public:
    virtual ~Info();
    void printPaths();
%Docstring
Print on the standard output configured data paths.
%End

    void printBuiltins();
%Docstring
Print on the standard output buitin optional features, selected when the library has been built.
%End

    void printOtherInfo();
%Docstring
Print on the standard output additional information (such as IO properties, settings options, etc).
%End

    void printVersion();
%Docstring
Print on the standard output the library version.
%End


    // static vector_InfoPtr & infos();
    static SIP_PYTUPLE infos();
%Docstring
Static method. Get a list of Info objects, each library may provide one (or several).
%End

%MethodCode
    std::vector<carto::Info *> & inf = carto::Info::infos();
    unsigned i, n = inf.size();
    sipRes = PyTuple_New( n );
    for( i=0; i<n; ++i )
      PyTuple_SetItem( sipRes, i, sipConvertFromType( inf[i],
        sipType_carto_Info, 0 ) );
%End

    static void print() /PyName=printInfos/;
%Docstring
Static method. Print all information on the standard output.
%End

    static void printVersions();
%Docstring
Static method. Print versions of libraries
%End

  private:
    Info();
  };

};

