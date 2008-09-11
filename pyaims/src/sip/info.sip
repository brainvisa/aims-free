
%ModuleCode
#include <cartobase/config/version.h>
%End

SIP_PYTUPLE version();
%MethodCode
  std::string v = carto::cartobaseShortVersion();
  std::string::size_type n = v.find( '.' );
  sipRes = PyTuple_New( 2 );
  PyTuple_SetItem( sipRes, 0, PyInt_FromString(
    const_cast<char *>( v.substr( 0, n ).c_str() ), 0, 10 ) );
  PyTuple_SetItem( sipRes, 1, PyInt_FromString(
    const_cast<char *>( v.substr( n+1, v.length() - n - 1 ).c_str() ), 0,
    10 ) );
%End


std::string versionString();
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

  public:
    virtual ~Info();
    virtual void printPaths();
    virtual void printBuiltins();
    virtual void printOtherInfo();
    virtual void printVersion();

    // static vector_InfoPtr & infos();
    static SIP_PYTUPLE infos();
%MethodCode
    std::vector<carto::Info *> & inf = carto::Info::infos();
    unsigned i, n = inf.size();
    sipRes = PyTuple_New( n );
    for( i=0; i<n; ++i )
      PyTuple_SetItem( sipRes, i, sipConvertFromInstance( inf[i],
        sipClass_carto_Info, 0 ) );
%End
    static void print() /PyName=printInfos/;
    static void printVersions();

  private:
    Info();
  };

};

