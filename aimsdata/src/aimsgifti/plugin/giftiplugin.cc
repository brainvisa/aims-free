// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/plugin/giftiplugin.h>
#include <aims/io/giftimeshformat_d.h>
#include <aims/io/giftitextureformat_d.h>
#include <aims/io/giftifinderformat.h>
#include <aims/io/finder.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  bool initgifti()
  {
    new GiftiPlugin;
    return true;
  }

  bool giftiinit __attribute__((unused)) = initgifti();

}

GiftiPlugin::GiftiPlugin() : Plugin()
{
  vector<string> ext;
  ext.push_back( "gii" );

  GiftiMeshFormat<3, Void>   *df1 = new GiftiMeshFormat<3, Void>;
  FileFormatDictionary<AimsTimeSurface<3, Void> >::registerFormat( "GIFTI",
                                                                   df1, ext );

  GiftiTextureFormat<float> *df2 = new GiftiTextureFormat<float>;
  FileFormatDictionary<TimeTexture<float> >::registerFormat( "GIFTI",
                                                             df2, ext );

  GiftiTextureFormat<double> *df13 = new GiftiTextureFormat<double>;
  FileFormatDictionary<TimeTexture<double> >::registerFormat( "GIFTI",
                                                              df13, ext );

  GiftiTextureFormat<int8_t> *df9 = new GiftiTextureFormat<int8_t>;
  FileFormatDictionary<TimeTexture<int8_t> >::registerFormat( "GIFTI",
                                                              df9, ext );

  GiftiTextureFormat<uint8_t> *df10 = new GiftiTextureFormat<uint8_t>;
  FileFormatDictionary<TimeTexture<uint8_t> >::registerFormat( "GIFTI",
                                                               df10, ext );

  GiftiTextureFormat<int16_t> *df3 = new GiftiTextureFormat<int16_t>;
  FileFormatDictionary<TimeTexture<int16_t> >::registerFormat( "GIFTI",
                                                               df3, ext );

  GiftiTextureFormat<uint16_t> *df8 = new GiftiTextureFormat<uint16_t>;
  FileFormatDictionary<TimeTexture<uint16_t> >::registerFormat( "GIFTI",
                                                                df8, ext );

  GiftiTextureFormat<int32_t> *df4 = new GiftiTextureFormat<int32_t>;
  FileFormatDictionary<TimeTexture<int32_t> >::registerFormat( "GIFTI",
                                                               df4, ext );

  GiftiTextureFormat<uint32_t> *df6 = new GiftiTextureFormat<uint32_t>;
  FileFormatDictionary<TimeTexture<uint32_t> >::registerFormat( "GIFTI",
                                                                df6, ext );

  GiftiTextureFormat<int64_t> *df11 = new GiftiTextureFormat<int64_t>;
  FileFormatDictionary<TimeTexture<int64_t> >::registerFormat( "GIFTI",
                                                               df11, ext );

  GiftiTextureFormat<uint64_t> *df12 = new GiftiTextureFormat<uint64_t>;
  FileFormatDictionary<TimeTexture<uint64_t> >::registerFormat( "GIFTI",
                                                                df12, ext );

  GiftiTextureFormat<Point2df> *df5 = new GiftiTextureFormat<Point2df>;
  FileFormatDictionary<TimeTexture<Point2df> >::registerFormat( "GIFTI",
                                                               df5, ext );

  GiftiTextureFormat<Point2d> *df7 = new GiftiTextureFormat<Point2d>;
  FileFormatDictionary<TimeTexture<Point2d> >::registerFormat( "GIFTI",
                                                               df7, ext );

  Finder::registerFormat( "GIFTI", new FinderGiftiFormat, ext );
  GiftiHeader::giftiMutex(); // force creating it now.
}


GiftiPlugin::~GiftiPlugin()
{
}


string GiftiPlugin::name() const
{
  return "GIFTI IO";
}


bool GiftiPlugin::noop()
{
  return true;
}
