// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/def/settings.h>
#include <aims/getopt/getopt2.h>
#include <aims/def/path.h>
#include <cartobase/stream/fileutil.h>
//#include <soma-io/reader/pythonreader.h>
#include <soma-io/io/reader.h>

//debug
#include <iostream>

using namespace carto;
using namespace std;
using namespace aims;

Settings & Settings::settings()
{
  static Settings	*s = new Settings;
  return *s;
}


Settings::Settings() 
  : AttributedObject( "aims_settings" ), _perso( "aims_settings" )
{
  // read site-wide settings
  try
    {
      carto::Reader<GenericObject>	pr( Path::singleton().dependencies() 
                            + FileUtil::separator() + "aimsrc" );
      pr.read( *this );
    }
  catch( exception & )
    {
      // cerr << "could not read AIMS globl settings: " << e.what() << endl;
    }

  // read personal settings
  try
    {
      carto::Reader<GenericObject> 
        ppr( Path::singleton().home() + FileUtil::separator() + ".aimsrc" );
      ppr.read( _perso );
    }
  catch( exception & )
    {
      // cerr << "could not read AIMS personal settings: " << e.what() << endl;
    }

  // copy personal settings to add / override globals
  copyProperties( Object::reference
                  ( _perso.value<AttributedObject::ContentType>() ) );

  AimsApplication *app = AimsApplication::globalApplication();
  if ( app )
  {
    if ( app->output_4d_volumes >= 0 )
    {
      setProperty( "spm_output_4d_volumes", app->output_4d_volumes );
      setProperty( "nifti_output_4d_volumes", app->output_4d_volumes );
    }
  }
}

