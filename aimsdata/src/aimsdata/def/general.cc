// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  General variables and Group/Type returning string
 */
#include <cstdlib>
#include <aims/def/general.h>
#include <cartobase/plugin/plugin.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/config/version.h>
#include <aims/def/path.h>
#include <cstdlib>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;


string aims::aimsVersion()
{
  return AIMSDATA_VERSION;
}


void AimsClear()
{
  if( system("clear") != 0 )
    // error, what do we do exactly ?
    return;
}

void AimsPrintVersion(void)
{
  cout << "A.I.M.S. Library " << aims::aimsVersion() 
       << " - (c)1995-2024, CEA/NeuroSpin (France)"
       << "http://www.brainvisa.info" << std::endl;
}

string AimsStringTypeOf(long type)
{
  static string _StrType[10] = { "Real signed 8 bits\0"      ,
				 "Real unsigned 8 bits\0"    ,
				 "Real signed 16 bits\0"     ,
				 "Real unsigned 16 bits\0"   ,
				 "Real signed 32 bits\0"     , 
				 "Real unsigned 32 bits\0"   ,
				 "Real floating 32 bits\0"   ,
				 "Real floating 64 bits\0"   ,
				 "Complex floating 32 bits\0",
				 "Complex floating 64 bits\0"};
  return _StrType[type];
}

long AimsNCompOf(long type)
{
  static long _NComp[10] = { 1,1,1,1,1,1,1,1,2,2 };
  return _NComp[type];
}

double AimsMinValOf(long type)
{
  static double _MinVal[10] = {  -128.0,
                                    0.0,
                               -32768.0,
                                    0.0,
                          -2147483648.0,
                                    0.0,
                                -3.4e38,
                              -1.79e308,
                                -3.4e38,
                              -1.79e308};
  return _MinVal[type];
}

double AimsMaxValOf(long type)
{
  static double _MaxVal[10] = {   127.0,
                                  255.0,
                                32767.0,
                                65535.0,
                           2147483647.0,
                           4294967295.0,
                                 3.4e38,
                               1.79e308,
                                 3.4e38,
                               1.79e308};
  return _MaxVal[type];
}

bool AimsIsSignedType(long type)
{
  return AimsMinValOf( type ) < 0 ? true : false;
}

bool AimsIsComplexType(long type)
{
  return type<8 ? false : true;
}


namespace
{

  bool addAimsPlugins()
  {
    list<PluginLoader::PluginFile> 
      & plugins = PluginLoader::pluginFiles();
    char	s = FileUtil::separator();
    plugins.push_back( PluginLoader::PluginFile
                       ( Path::singleton().home() + s + ".aims" 
                         + s + "plugins" + s + "aimsdata.plugins", 
                         aimsVersion() ) );
    plugins.push_back( PluginLoader::PluginFile
                       ( Path::singleton().dependencies() 
                         + s + "plugins" + s + "aimsdata.plugins", 
                         aimsVersion() ) );
    return true;
  }

  bool dummy __attribute__((unused)) = addAimsPlugins();

}

