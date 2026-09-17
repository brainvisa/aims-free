// we don't want to issue a warning
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/plugin/ecatplugin.h>
#include <aims/io/ecatformat_d.h>
#include <aims/io/ecatfinderformat.h>
#include <aims/io/finder.h>
#include <aims/io/baseFormats_cartovolume.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  bool initecat()
  {
    new EcatPlugin;
    return true;
  }

  bool ecatinit __attribute__((unused)) = initecat();

}

EcatPlugin::EcatPlugin() : Plugin()
{
  vector<string>	ext;
  ext.push_back( "v" );
  ext.push_back( "i" );
  ext.push_back( "p" );
  EcatFormat<int16_t>	*df3 = new EcatFormat<int16_t>;
  FileFormatDictionary<AimsData<int16_t> >::registerFormat( "ECAT", df3, ext );
  VolumeFormat<int16_t>	*vf3 = new VolumeFormat<int16_t>( "ECAT" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "ECAT", vf3, ext );

  EcatFormat<float>	*df7 = new EcatFormat<float>;
  FileFormatDictionary<AimsData<float> >::registerFormat( "ECAT", df7, ext );
  VolumeFormat<float>	*vf7 = new VolumeFormat<float>( "ECAT" );
  FileFormatDictionary<Volume<float> >::registerFormat( "ECAT", vf7, ext );

  Finder::registerFormat( "ECAT", new FinderEcatFormat, ext );
}


EcatPlugin::~EcatPlugin()
{
}


string EcatPlugin::name() const
{
  return "ECAT IO";
}


bool EcatPlugin::noop()
{
  return true;
}
