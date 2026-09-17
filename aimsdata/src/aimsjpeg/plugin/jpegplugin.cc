#include <aims/plugin/jpegplugin.h>
#include <aims/io/jpegformat_d.h>
#include <aims/io/jpegfinderformat.h>
#include <aims/io/finder.h>
#include <aims/rgb/rgb.h>
#include <aims/io/baseFormats_cartovolume.h>
#ifdef __APPLE__
#include <aims/io/macosxbugs.h>
#endif

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  bool initjpeg()
  {
    new JpegPlugin;
    return true;
  }

  bool jpeginit __attribute__((unused)) = initjpeg();

}

JpegPlugin::JpegPlugin() : Plugin()
{
  vector<string>	ext;
  ext.push_back( "jpg" );
  ext.push_back( "JPG" );
  ext.push_back( "jpeg" );
  ext.push_back( "JPEG" );
  JpegFormat<uint8_t>	*df2 = new JpegFormat<uint8_t>;
  FileFormatDictionary<AimsData<uint8_t> >::registerFormat( "JPEG", df2, ext );
  VolumeFormat<uint8_t>	*vf2 = new VolumeFormat<uint8_t>( "JPEG" );
  FileFormatDictionary<Volume<uint8_t> >::registerFormat( "JPEG", vf2, ext );

  JpegFormat<AimsRGB>	*df8 = new JpegFormat<AimsRGB>;
#ifdef AIMS_APPLE_GCC33BUG
  macosxbugs::fileFormatDictionary_dataRGB_registerFormat( "JPEG", df8, ext );
#else
  FileFormatDictionary<AimsData<AimsRGB> >::registerFormat( "JPEG", df8, ext );
#endif
  VolumeFormat<AimsRGB>	*vf8 = new VolumeFormat<AimsRGB>( "JPEG" );
  FileFormatDictionary<Volume<AimsRGB> >::registerFormat( "JPEG", vf8, ext );
  Finder::registerFormat( "JPEG", new FinderJpegFormat, ext );
}


JpegPlugin::~JpegPlugin()
{
}


string JpegPlugin::name() const
{
  return "JPEG IO";
}


bool JpegPlugin::noop()
{
  return true;
}

