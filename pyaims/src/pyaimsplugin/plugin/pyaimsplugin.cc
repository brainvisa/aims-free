#include "pyaimsplugin.h"
#include <cartobase/stream/fileutil.h>
extern "C"
{
#include <Python.h>
}
// #include <iostream>
#ifdef __linux__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include <link.h>
#include <dlfcn.h>
#endif

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  bool initpyaims()
  {
    new PyaimsPlugin;
    return true;
  }

  bool pyaimsinit = initpyaims();

}

PyaimsPlugin::PyaimsPlugin() : Plugin()
{
#ifdef __linux__
  /* avoids missing symbols (why ??) see
     https://stackoverflow.com/questions/29880931/importerror-and-pyexc-systemerror-while-embedding-python-script-within-c-for-pam

     without this libpython reload, importing the sip module results in an
     error: undefined symbol: PyExc_SystemError
  */
  void *self = dlopen(0, RTLD_LAZY | RTLD_GLOBAL);
  struct link_map *info;

  if( dlinfo( self, RTLD_DI_LINKMAP, &info ) == 0 )
  {
    struct link_map *next = info->l_next;
    string bname;
    while( next )
    {
      bname = FileUtil::basename( next->l_name );
      if( bname.length() > 9 && bname.substr( 0, 9 ) == "libpython" )
      {
        // force reopening libpython in RT_GLOBAL mode
        dlopen( next->l_name, RTLD_LAZY | RTLD_GLOBAL);
      }
      next = next->l_next;
    }
  }
#endif

  if( Py_IsInitialized() )
  {
    // cout << "Python is already running." << endl;
    return;
  }
  else
    Py_Initialize();

  PyRun_SimpleString( "import soma.aims" );
}


PyaimsPlugin::~PyaimsPlugin()
{
}


string PyaimsPlugin::name() const
{
  return "PyAIMS plugin";
}


bool PyaimsPlugin::noop()
{
  return true;
}

