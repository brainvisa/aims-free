//--- plugin -------------------------------------------------------------------
#include <cartodata/plugin/osplugin.h>
#include <soma-io/image/osimagereader.h>
//--- cartodata ----------------------------------------------------------------
#include <cartodata/io/volumeformatreader.h>
//--- soma-io ------------------------------------------------------------------
#include <soma-io/io/formatdictionary.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/type/types.h>
// #include <cartobase/type/voxelrgba.h>
//--- system -------------------------------------------------------------------
#include <vector>
#include <string>
//------------------------------------------------------------------------------

using namespace cartodata;
using namespace soma;
using namespace carto;
using namespace std;


namespace carto {
  class VoxelRGBA;
}

namespace
{

  bool initos()
  {
    new OSPlugin;
    return true;
  }

  bool osinit __attribute__((unused)) = initos();

}

OSPlugin::OSPlugin() : Plugin()
{
    vector<string>  exts(9);
    exts[0] = "tif";    // Aperio, Trestle, Generic TIFF
    exts[1] = "tiff";   // Aperio, Trestle, Generic TIFF
    exts[2] = "svs";    // Aperio
    exts[3] = "vms";    // Hamamatsu
    exts[4] = "vmu";    // Hamamatsu
    exts[5] = "ndpi";   // Hamamatsu
    exts[6] = "scn";    // Leica
    exts[7] = "mrxs";   // MIRAX
    exts[8] = "czi";    // Zeiss

    ////////////////////////////////////////////////////////////////////////////
    ////                           R E A D E R                              ////
    ////////////////////////////////////////////////////////////////////////////

    //==========================================================================
    //   V O L U M E
    //==========================================================================

    //--- RGB ------------------------------------------------------------------

    VolumeFormatReader<VoxelRGBA> *vfr_rgba = new VolumeFormatReader<VoxelRGBA>;
    vfr_rgba->attach( rc_ptr<ImageReader<VoxelRGBA> >( new OSImageReader<VoxelRGBA> ) );
    FormatDictionary<Volume<VoxelRGBA> >::registerFormat( "OpenSlide", vfr_rgba, exts );

    //==========================================================================
    //   V O L U M E   R E F
    //==========================================================================

    //--- RGB ------------------------------------------------------------------

    VolumeRefFormatReader<VoxelRGBA> *rfr_rgba = new VolumeRefFormatReader<VoxelRGBA>;
    rfr_rgba->attach( rc_ptr<ImageReader<VoxelRGBA> >( new OSImageReader<VoxelRGBA> ) );
    FormatDictionary<VolumeRef<VoxelRGBA> >::registerFormat( "OpenSlide", rfr_rgba, exts );

}


OSPlugin::~OSPlugin()
{
}


string OSPlugin::name() const
{
  return string("OpenSlide CARTO");
}


bool OSPlugin::noop()
{
  return true;
}

