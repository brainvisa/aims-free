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

//--- aims ---------------------------------------------------------------------
#include <aims/plugin/osplugin.h>
#include <aims/io/osimagereader_d.h>
#include <aims/io/osimagewriter_d.h>
#include <aims/io/osformatchecker.h>
#include <aims/io/baseFormats_cartovolume.h>
#include <aims/rgb/rgb.h>
//--- cartodata ----------------------------------------------------------------
#include <cartodata/io/volumeformatreader_d.h>
#include <cartodata/io/volumeformatwriter_d.h>
//--- soma-io ------------------------------------------------------------------
#include <soma-io/io/formatdictionary_d.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/type/types.h>
//--- system -------------------------------------------------------------------
#include <vector>
#include <string>
//------------------------------------------------------------------------------

using namespace aims;
using namespace soma;
using namespace carto;
using namespace std;

namespace
{

  bool initgis()
  {
    new OSPlugin;
    return true;
  }

  bool osinit = initos();

}

OSPlugin:OSPlugin() : Plugin()
{
    vector<string>  exts(2);
    exts[0] = "tif";    // Aperio, Trestle, Generic TIFF
    exts[1] = "tiff";   // Aperio, Trestle, Generic TIFF
    exts[2] = "svs";    // Aperio
    exts[3] = "vms";    // Hamamatsu
    exts[4] = "vmu";    // Hamamatsu
    exts[5] = "scn";    // Leica
    exts[6] = "mrxs";   // MIRAX
    
    ////////////////////////////////////////////////////////////////////////////
    ////                          C H E C K E R                             ////
    ////////////////////////////////////////////////////////////////////////////
    
    DataSourceInfoLoader::registerFormat( "OpenSlide", new OSFormatChecker, exts );
    
    ////////////////////////////////////////////////////////////////////////////
    ////                           R E A D E R                              ////
    ////////////////////////////////////////////////////////////////////////////
    
    //==========================================================================
    //   V O L U M E
    //==========================================================================
    
    //--- RGB ------------------------------------------------------------------
    
    //VolumeFormatReader<AimsRGB> *vfr_rgb = new VolumeFormatReader<AimsRGB>;
    //vfr_rgb->attach( rc_ptr<ImageReader<AimsRGB> >( new GisImageReader<AimsRGB> ) );
    //FormatDictionary<Volume<AimsRGB> >::registerFormat( "GIS", vfr_rgb, exts );
    
    VolumeFormatReader<AimsRGBA> *vfr_rgba = new VolumeFormatReader<AimsRGBA>;
    vfr_rgba->attach( rc_ptr<ImageReader<AimsRGBA> >( new OSImageReader<AimsRGBA> ) );
    FormatDictionary<Volume<AimsRGBA> >::registerFormat( "OpenSlide", vfr_rgba, exts );
    
    //VolumeFormatReader<AimsHSV> *vfr_hsv = new VolumeFormatReader<AimsHSV>;
    //vfr_hsv->attach( rc_ptr<ImageReader<AimsHSV> >( new GisImageReader<AimsHSV> ) );
    //FormatDictionary<Volume<AimsHSV> >::registerFormat( "GIS", vfr_hsv, exts );
    
    //==========================================================================
    //   V O L U M E   R E F
    //==========================================================================
    
    //--- RGB ------------------------------------------------------------------
    
    //VolumeRefFormatReader<AimsRGB> *rfr_rgb = new VolumeRefFormatReader<AimsRGB>;
    //rfr_rgb->attach( rc_ptr<ImageReader<AimsRGB> >( new GisImageReader<AimsRGB> ) );
    //FormatDictionary<VolumeRef<AimsRGB> >::registerFormat( "GIS", rfr_rgb, exts );
    
    VolumeRefFormatReader<AimsRGBA> *rfr_rgba = new VolumeRefFormatReader<AimsRGBA>;
    rfr_rgba->attach( rc_ptr<ImageReader<AimsRGBA> >( new OSImageReader<AimsRGBA> ) );
    FormatDictionary<VolumeRef<AimsRGBA> >::registerFormat( "OpenSlide", rfr_rgba, exts );
    
    //VolumeRefFormatReader<AimsHSV> *rfr_hsv = new VolumeRefFormatReader<AimsHSV>;
    //rfr_hsv->attach( rc_ptr<ImageReader<AimsHSV> >( new GisImageReader<AimsHSV> ) );
    //FormatDictionary<VolumeRef<AimsHSV> >::registerFormat( "GIS", rfr_hsv, exts );
    
    ////////////////////////////////////////////////////////////////////////////
    ////                           W R I T E R                              ////
    ////////////////////////////////////////////////////////////////////////////
    
    //==========================================================================
    //   V O L U M E
    //==========================================================================
    
    //--- RGB ------------------------------------------------------------------
    
    //VolumeFormatWriter<AimsRGB> *vfwf = new VolumeFormatWriter<AimsRGB>;
    //vfwf->attach( rc_ptr<ImageWriter<AimsRGB> >( new GisImageWriter<AimsRGB> ) );
    //FormatDictionary<Volume<AimsRGB> >::registerFormat( "GIS", vfwf, exts );
    
    VolumeFormatWriter<AimsRGBA> *vfwf = new VolumeFormatWriter<AimsRGBA>;
    vfwf->attach( rc_ptr<ImageWriter<AimsRGBA> >( new OSImageWriter<AimsRGBA> ) );
    FormatDictionary<Volume<AimsRGBA> >::registerFormat( "OpenSlide", vfwf, exts );
    
    //VolumeFormatWriter<AimsHSV> *vfwf = new VolumeFormatWriter<AimsHSV>;
    //vfwf->attach( rc_ptr<ImageWriter<AimsHSV> >( new GisImageWriter<AimsHSV> ) );
    //FormatDictionary<Volume<AimsHSV> >::registerFormat( "GIS", vfwf, exts );
    
    //==========================================================================
    //   V O L U M E   R E F
    //==========================================================================
    
    //--- RGB ----------------------------------------------------------------
    
    //VolumeRefFormatWriter<AimsRGB> *rfwf = new VolumeRefFormatWriter<AimsRGB>;
    //rfwf->attach( rc_ptr<ImageWriter<AimsRGB> >( new GisImageWriter<AimsRGB> ) );
    //FormatDictionary<VolumeRef<AimsRGB> >::registerFormat( "GIS", rfwf, exts );
    
    VolumeRefFormatWriter<AimsRGBA> *rfwd = new VolumeRefFormatWriter<AimsRGBA>;
    rfwd->attach( rc_ptr<ImageWriter<AimsRGBA> >( new OSImageWriter<AimsRGBA> ) );
    FormatDictionary<VolumeRef<AimsRGBA> >::registerFormat( "OpenSlide", rfwd, exts );
    
    //VolumeRefFormatWriter<AimsHSV> *rfwd = new VolumeRefFormatWriter<AimsHSV>;
    //rfwd->attach( rc_ptr<ImageWriter<AimsHSV> >( new GisImageWriter<AimsHSV> ) );
    //FormatDictionary<VolumeRef<AimsHSV> >::registerFormat( "GIS", rfwd, exts );
}


OSPlugin::~OSPlugin()
{
}


string GisPlugin::name() const
{
  return string("GIS IO");
}


bool GisPlugin::noop()
{
  return true;
}

