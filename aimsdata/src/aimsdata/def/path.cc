// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  Paths required for A.I.M.S.
 */
#include <cstdlib>
#include <aims/def/path.h>
#include <aims/def/general.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/directory.h>
#include <cartobase/config/paths.h>
#include <cartobase/config/version.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef USE_SHARE_CONFIG
#include <brainvisa-share/config.h>
#endif


using namespace std;
using namespace aims;
using namespace carto;

Path::Path()
{
  const char	*aims_path = getenv( "AIMS_PATH" );
  char		s = FileUtil::separator();
  Directory	d( "/" );
  const string	& _shared = Paths::globalShared();

  if( aims_path )
    {
      _aims = aims_path;
      d.chdir( _aims );
      if( !d.isValid() )
        _aims = "";
    }
  if( _aims.empty() )
    {
      _aims = _shared + s + "aims-" + carto::cartobaseShortVersion();
      d.chdir( _aims );
      if( !d.isValid() )
        {
#ifdef _WIN32
          const string	hardpath = "c:\\share\\aims-";
#else
          const string	hardpath = "/home/appli/aimsdata-";
#endif
          _aims = hardpath + carto::cartobaseShortVersion();
          d.chdir( _aims );
          if( !d.isValid() )
            {
              _aims = _shared + s + "aims-main";
              d.chdir( _aims );
              if( !d.isValid() )
                {
                  _aims = _shared + s + "aims";
                  d.chdir( _aims );
                  if( !d.isValid() )
                    {
                      _aims = hardpath + "main";
                      d.chdir( _aims );
                      if( !d.isValid() )
                        _aims = _shared;
                    }
                }
            }
        }
    }

  _dependencies = _aims + s + "shared";
  d.chdir( _dependencies );
  if( !d.isValid() )
    _dependencies = _aims;

#if USE_SHARE_CONFIG
  _nomenclature = _shared + s + BRAINVISA_SHARE_DIRECTORY + s + "nomenclature";
  static bool firsttime = true;
  if( firsttime )
  {
    firsttime = false;
    Paths::addResourceSearchPath( Paths::globalShared() + FileUtil::separator()
      + BRAINVISA_SHARE_DIRECTORY );
  }
#else
  _nomenclature = Paths::shfjShared() + s + "nomenclature";
#endif
  Directory	dir( _nomenclature );
  if( !dir.isValid() )
    _nomenclature = _shared + s + "nomenclature";
  _syntax = _nomenclature + s + "syntax";
  _hierarchy = _nomenclature + s + "hierarchy";
}


const string & Path::home() const
{
  // use carto::Path
  return Paths::home();
}


const string & Path::memmap() const
{
  // use carto::Path
  return Paths::memmap();
}


const string & Path::globalShared() const
{
  // use carto::Path
  return Paths::globalShared();
}


const string & Path::shfjShared() const
{
  // use carto::Path
  return Paths::shfjShared();
}


const Path& Path::singleton()
{
  static Path* path = new Path;
  return *path;
}
