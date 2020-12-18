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

  PyRun_SimpleString( "import sip" );
  PyRun_SimpleString( "for name in ['QString', 'QVariant', 'QDate', 'QDateTime', 'QTextStream', 'QTime', 'QUrl']: sip.setapi(name, 2)" );
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

