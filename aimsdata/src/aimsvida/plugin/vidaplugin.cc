#include <aims/plugin/vidaplugin.h>
#include <aims/io/vidaformat_d.h>
#include <aims/io/vidafinderformat.h>
#include <aims/io/finder.h>
#include <aims/rgb/rgb.h>
#include <aims/io/baseFormats_cartovolume.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  bool initvida()
  {
    new VidaPlugin;
    return true;
  }

  bool vidainit __attribute__((unused)) = initvida();

}

VidaPlugin::VidaPlugin() : Plugin()
{
  vector<string>	ext;
  ext.push_back( "vimg" );
  ext.push_back( "vhdr" );
  ext.push_back( "vinfo" );
  VidaFormat<int16_t>	*df3 = new VidaFormat<int16_t>;
  FileFormatDictionary<AimsData<int16_t> >::registerFormat( "VIDA", df3, ext );
  VolumeFormat<int16_t>	*vf3 = new VolumeFormat<int16_t>( "VIDA" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "VIDA", vf3, ext );

  VidaFormat<float>	*df7 = new VidaFormat<float>;
  FileFormatDictionary<AimsData<float> >::registerFormat( "VIDA", df7, ext );
  VolumeFormat<float>	*vf7 = new VolumeFormat<float>( "VIDA" );
  FileFormatDictionary<Volume<float> >::registerFormat( "VIDA", vf7, ext );

  Finder::registerFormat( "VIDA", new FinderVidaFormat, ext );
}


VidaPlugin::~VidaPlugin()
{
}


string VidaPlugin::name() const
{
  return "VIDA IO";
}


bool VidaPlugin::noop()
{
  return true;
}
