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

  bool osinit = initos();

}

OSPlugin::OSPlugin() : Plugin()
{
    vector<string>  exts(8);
    exts[0] = "tif";    // Aperio, Trestle, Generic TIFF
    exts[1] = "tiff";   // Aperio, Trestle, Generic TIFF
    exts[2] = "svs";    // Aperio
    exts[3] = "vms";    // Hamamatsu
    exts[4] = "vmu";    // Hamamatsu
    exts[5] = "ndpi";   // Hamamatsu
    exts[6] = "scn";    // Leica
    exts[7] = "mrxs";   // MIRAX
    
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

