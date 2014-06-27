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

//--- plugin -----------------------------------------------------------------
#include <cartodata/plugin/tiffplugin.h>
#include <soma-io/image/tiffimagereader.h>
//#include <soma-io/image/tiffimagewriter.h>
//--- cartodata --------------------------------------------------------------
#include <cartodata/io/volumeformatreader.h>
#include <cartodata/io/volumeformatwriter.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/io/formatdictionary.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/type/types.h>
//--- system -----------------------------------------------------------------
#include <vector>
#include <string>
//----------------------------------------------------------------------------

using namespace cartodata;
using namespace soma;
using namespace carto;
using namespace std;

namespace carto
{
  class VoxelRGB;
  class VoxelRGBA;
  class VoxelHSV;
}

namespace {

  bool inittiff()
  {
    new TiffPlugin;
    return true;
  }

  bool tiffinit = inittiff();

}

TiffPlugin::TiffPlugin() : Plugin()
{
    vector<string>  exts(2);
    exts[0] = "tif";
    exts[1] = "tiff";

    ////////////////////////////////////////////////////////////////////////////
    ////                           R E A D E R                              ////
    ////////////////////////////////////////////////////////////////////////////
    
    //==========================================================================
    //   V O L U M E
    //==========================================================================
    
    //--- INT ------------------------------------------------------------------

    VolumeFormatReader<int8_t> *vfr8 = new VolumeFormatReader<int8_t>;
    vfr8->attach( rc_ptr<ImageReader<int8_t> >( new TiffImageReader<int8_t> ) );
    FormatDictionary<Volume<int8_t> >::registerFormat( "TIFF", vfr8, exts );

    VolumeFormatReader<int16_t> *vfr16 = new VolumeFormatReader<int16_t>;
    vfr16->attach( rc_ptr<ImageReader<int16_t> >( new TiffImageReader<int16_t> ) );
    FormatDictionary<Volume<int16_t> >::registerFormat( "TIFF", vfr16, exts );

    VolumeFormatReader<int32_t> *vfr32 = new VolumeFormatReader<int32_t>;
    vfr32->attach( rc_ptr<ImageReader<int32_t> >( new TiffImageReader<int32_t> ) );
    FormatDictionary<Volume<int32_t> >::registerFormat( "TIFF", vfr32, exts );

    VolumeFormatReader<int64_t> *vfr64 = new VolumeFormatReader<int64_t>;
    vfr64->attach( rc_ptr<ImageReader<int64_t> >( new TiffImageReader<int64_t> ) );
    FormatDictionary<Volume<int64_t> >::registerFormat( "TIFF", vfr64, exts );

    //--- UINT -----------------------------------------------------------------

    VolumeFormatReader<uint8_t> *vfru8 = new VolumeFormatReader<uint8_t>;
    vfru8->attach( rc_ptr<ImageReader<uint8_t> >( new TiffImageReader<uint8_t> ) );
    FormatDictionary<Volume<uint8_t> >::registerFormat( "TIFF", vfru8, exts );

    VolumeFormatReader<uint16_t> *vfru16 = new VolumeFormatReader<uint16_t>;
    vfru16->attach( rc_ptr<ImageReader<uint16_t> >( new TiffImageReader<uint16_t> ) );
    FormatDictionary<Volume<uint16_t> >::registerFormat( "TIFF", vfru16, exts );

    VolumeFormatReader<uint32_t> *vfru32 = new VolumeFormatReader<uint32_t>;
    vfru32->attach( rc_ptr<ImageReader<uint32_t> >( new TiffImageReader<uint32_t> ) );
    FormatDictionary<Volume<uint32_t> >::registerFormat( "TIFF", vfru32, exts );

    VolumeFormatReader<uint64_t> *vfru64 = new VolumeFormatReader<uint64_t>;
    vfru64->attach( rc_ptr<ImageReader<uint64_t> >( new TiffImageReader<uint64_t> ) );
    FormatDictionary<Volume<uint64_t> >::registerFormat( "TIFF", vfru64, exts );

    //--- FLOAT ----------------------------------------------------------------
    
    VolumeFormatReader<float> *vfrf = new VolumeFormatReader<float>;
    vfrf->attach( rc_ptr<ImageReader<float> >( new TiffImageReader<float> ) );
    FormatDictionary<Volume<float> >::registerFormat( "TIFF", vfrf, exts );
    
    VolumeFormatReader<double> *vfrd = new VolumeFormatReader<double>;
    vfrd->attach( rc_ptr<ImageReader<double> >( new TiffImageReader<double> ) );
    FormatDictionary<Volume<double> >::registerFormat( "TIFF", vfrd, exts );
    
    //--- RGB ------------------------------------------------------------------
    
    VolumeFormatReader<VoxelRGB> *vfr_rgb = new VolumeFormatReader<VoxelRGB>;
    vfr_rgb->attach( rc_ptr<ImageReader<VoxelRGB> >( new TiffImageReader<VoxelRGB> ) );
    FormatDictionary<Volume<VoxelRGB> >::registerFormat( "TIFF", vfr_rgb, exts );
    
    VolumeFormatReader<VoxelRGBA> *vfr_rgba = new VolumeFormatReader<VoxelRGBA>;
    vfr_rgba->attach( rc_ptr<ImageReader<VoxelRGBA> >( new TiffImageReader<VoxelRGBA> ) );
    FormatDictionary<Volume<VoxelRGBA> >::registerFormat( "TIFF", vfr_rgba, exts );
    
    VolumeFormatReader<VoxelHSV> *vfr_hsv = new VolumeFormatReader<VoxelHSV>;
    vfr_hsv->attach( rc_ptr<ImageReader<VoxelHSV> >( new TiffImageReader<VoxelHSV> ) );
    FormatDictionary<Volume<VoxelHSV> >::registerFormat( "TIFF", vfr_hsv, exts );
    
    //--- COMPLEX -------------------------------------------------------------
/* complex needs AsciiDataSourceTraits to be defined on complex types

    VolumeFormatReader<std::complex<float> > *vfr_cf = new VolumeFormatReader<std::complex<float> >;
    vfr_cf->attach( rc_ptr<ImageReader<std::complex<float> > >( new TiffImageReader<std::complex<float> > ) );
    FormatDictionary<Volume<std::complex<float> > >::registerFormat( "TIFF", vfr_cf, exts );

    VolumeFormatReader<std::complex<double> > *vfr_cd = new VolumeFormatReader<std::complex<double> >;
    vfr_cd->attach( rc_ptr<ImageReader<std::complex<double> > >( new TiffImageReader<std::complex<double> > ) );
    FormatDictionary<Volume<std::complex<double> > >::registerFormat( "TIFF", vfr_cd, exts );
*/
    //==========================================================================
    //   V O L U M E   R E F
    //==========================================================================
    
    //--- INT ------------------------------------------------------------------

    VolumeRefFormatReader<int8_t> *rfr8 = new VolumeRefFormatReader<int8_t>;
    rfr8->attach( rc_ptr<ImageReader<int8_t> >( new TiffImageReader<int8_t> ) );
    FormatDictionary<VolumeRef<int8_t> >::registerFormat( "TIFF", rfr8, exts );

    VolumeRefFormatReader<int16_t> *rfr16 = new VolumeRefFormatReader<int16_t>;
    rfr16->attach( rc_ptr<ImageReader<int16_t> >( new TiffImageReader<int16_t> ) );
    FormatDictionary<VolumeRef<int16_t> >::registerFormat( "TIFF", rfr16, exts );

    VolumeRefFormatReader<int32_t> *rfr32 = new VolumeRefFormatReader<int32_t>;
    rfr32->attach( rc_ptr<ImageReader<int32_t> >( new TiffImageReader<int32_t> ) );
    FormatDictionary<VolumeRef<int32_t> >::registerFormat( "TIFF", rfr32, exts );

    VolumeRefFormatReader<int64_t> *rfr64 = new VolumeRefFormatReader<int64_t>;
    rfr64->attach( rc_ptr<ImageReader<int64_t> >( new TiffImageReader<int64_t> ) );
    FormatDictionary<VolumeRef<int64_t> >::registerFormat( "TIFF", rfr64, exts );

    //--- UINT -----------------------------------------------------------------

    VolumeRefFormatReader<uint8_t> *rfru8 = new VolumeRefFormatReader<uint8_t>;
    rfru8->attach( rc_ptr<ImageReader<uint8_t> >( new TiffImageReader<uint8_t> ) );
    FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "TIFF", rfru8, exts );

    VolumeRefFormatReader<uint16_t> *rfru16 = new VolumeRefFormatReader<uint16_t>;
    rfru16->attach( rc_ptr<ImageReader<uint16_t> >( new TiffImageReader<uint16_t> ) );
    FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "TIFF", rfru16, exts );

    VolumeRefFormatReader<uint32_t> *rfru32 = new VolumeRefFormatReader<uint32_t>;
    rfru32->attach( rc_ptr<ImageReader<uint32_t> >( new TiffImageReader<uint32_t> ) );
    FormatDictionary<VolumeRef<uint32_t> >::registerFormat( "TIFF", rfru32, exts );

    VolumeRefFormatReader<uint64_t> *rfru64 = new VolumeRefFormatReader<uint64_t>;
    rfru64->attach( rc_ptr<ImageReader<uint64_t> >( new TiffImageReader<uint64_t> ) );
    FormatDictionary<VolumeRef<uint64_t> >::registerFormat( "TIFF", rfru64, exts );

    //--- FLOAT ----------------------------------------------------------------
    
    VolumeRefFormatReader<float> *rfrf = new VolumeRefFormatReader<float>;
    rfrf->attach( rc_ptr<ImageReader<float> >( new TiffImageReader<float> ) );
    FormatDictionary<VolumeRef<float> >::registerFormat( "TIFF", rfrf, exts );
    
    VolumeRefFormatReader<double> *rfrd = new VolumeRefFormatReader<double>;
    rfrd->attach( rc_ptr<ImageReader<double> >( new TiffImageReader<double> ) );
    FormatDictionary<VolumeRef<double> >::registerFormat( "TIFF", rfrd, exts );
    
    //--- RGB ------------------------------------------------------------------
    
    VolumeRefFormatReader<VoxelRGB> *rfr_rgb = new VolumeRefFormatReader<VoxelRGB>;
    rfr_rgb->attach( rc_ptr<ImageReader<VoxelRGB> >( new TiffImageReader<VoxelRGB> ) );
    FormatDictionary<VolumeRef<VoxelRGB> >::registerFormat( "TIFF", rfr_rgb, exts );
    
    VolumeRefFormatReader<VoxelRGBA> *rfr_rgba = new VolumeRefFormatReader<VoxelRGBA>;
    rfr_rgba->attach( rc_ptr<ImageReader<VoxelRGBA> >( new TiffImageReader<VoxelRGBA> ) );
    FormatDictionary<VolumeRef<VoxelRGBA> >::registerFormat( "TIFF", rfr_rgba, exts );
    
    VolumeRefFormatReader<VoxelHSV> *rfr_hsv = new VolumeRefFormatReader<VoxelHSV>;
    rfr_hsv->attach( rc_ptr<ImageReader<VoxelHSV> >( new TiffImageReader<VoxelHSV> ) );
    FormatDictionary<VolumeRef<VoxelHSV> >::registerFormat( "TIFF", rfr_hsv, exts );
    
    //--- COMPLEX -------------------------------------------------------------
/*
    VolumeRefFormatReader<std::complex<float> > *rfr_cf = new VolumeRefFormatReader<std::complex<float> >;
    rfr_cf->attach( rc_ptr<ImageReader<std::complex<float> > >( new TiffImageReader<std::complex<float> > ) );
    FormatDictionary<VolumeRef<std::complex<float> > >::registerFormat( "TIFF", rfr_cf, exts );

    VolumeRefFormatReader<std::complex<double> > *rfr_cd = new VolumeRefFormatReader<std::complex<double> >;
    rfr_cd->attach( rc_ptr<ImageReader<std::complex<double> > >( new TiffImageReader<std::complex<double> > ) );
    FormatDictionary<VolumeRef<std::complex<double> > >::registerFormat( "TIFF", rfr_cd, exts );
*/

    ////////////////////////////////////////////////////////////////////////////
    ////                           W R I T E R                              ////
    ////////////////////////////////////////////////////////////////////////////

    //==========================================================================
    //   V O L U M E
    //==========================================================================
    
    //--- INT ------------------------------------------------------------------

//     VolumeFormatWriter<int8_t> *vfw8 = new VolumeFormatWriter<int8_t>;
//     rc_ptr<ImageWriter<int8_t> > toto;
//     toto.reset( new TiffImageWriter<int8_t> );
//     vfw8->attach( rc_ptr<ImageWriter<int8_t> >( new TiffImageWriter<int8_t> ) );
//     FormatDictionary<Volume<int8_t> >::registerFormat( "TIFF", vfw8, exts );
// 
//     VolumeFormatWriter<int16_t> *vfw16 = new VolumeFormatWriter<int16_t>;
//     vfw16->attach( rc_ptr<ImageWriter<int16_t> >( new TiffImageWriter<int16_t> ) );
//     FormatDictionary<Volume<int16_t> >::registerFormat( "TIFF", vfw16, exts );
// 
//     VolumeFormatWriter<int32_t> *vfw32 = new VolumeFormatWriter<int32_t>;
//     vfw32->attach( rc_ptr<ImageWriter<int32_t> >( new TiffImageWriter<int32_t> ) );
//     FormatDictionary<Volume<int32_t> >::registerFormat( "TIFF", vfw32, exts );
// 
//     VolumeFormatWriter<int64_t> *vfw64 = new VolumeFormatWriter<int64_t>;
//     vfw64->attach( rc_ptr<ImageWriter<int64_t> >( new TiffImageWriter<int64_t> ) );
//     FormatDictionary<Volume<int64_t> >::registerFormat( "TIFF", vfw64, exts );
// 
//     //--- UINT -----------------------------------------------------------------
// 
//     VolumeFormatWriter<uint8_t> *vfwu8 = new VolumeFormatWriter<uint8_t>;
//     vfwu8->attach( rc_ptr<ImageWriter<uint8_t> >( new TiffImageWriter<uint8_t> ) );
//     FormatDictionary<Volume<uint8_t> >::registerFormat( "TIFF", vfwu8, exts );
// 
//     VolumeFormatWriter<uint16_t> *vfwu16 = new VolumeFormatWriter<uint16_t>;
//     vfwu16->attach( rc_ptr<ImageWriter<uint16_t> >( new TiffImageWriter<uint16_t> ) );
//     FormatDictionary<Volume<uint16_t> >::registerFormat( "TIFF", vfwu16, exts );
// 
//     VolumeFormatWriter<uint32_t> *vfwu32 = new VolumeFormatWriter<uint32_t>;
//     vfwu32->attach( rc_ptr<ImageWriter<uint32_t> >( new TiffImageWriter<uint32_t> ) );
//     FormatDictionary<Volume<uint32_t> >::registerFormat( "TIFF", vfwu32, exts );
// 
//     VolumeFormatWriter<uint64_t> *vfwu64 = new VolumeFormatWriter<uint64_t>;
//     vfwu64->attach( rc_ptr<ImageWriter<uint64_t> >( new TiffImageWriter<uint64_t> ) );
//     FormatDictionary<Volume<uint64_t> >::registerFormat( "TIFF", vfwu64, exts );
// 
//     //--- FLOAT ----------------------------------------------------------------
//     
//     VolumeFormatWriter<float> *vfwf = new VolumeFormatWriter<float>;
//     vfwf->attach( rc_ptr<ImageWriter<float> >( new TiffImageWriter<float> ) );
//     FormatDictionary<Volume<float> >::registerFormat( "TIFF", vfwf, exts );
//     
//     VolumeFormatWriter<double> *vfwd = new VolumeFormatWriter<double>;
//     vfwd->attach( rc_ptr<ImageWriter<double> >( new TiffImageWriter<double> ) );
//     FormatDictionary<Volume<double> >::registerFormat( "TIFF", vfwd, exts );
//     
//     //--- RGB ------------------------------------------------------------------
//     
//     VolumeFormatWriter<VoxelRGB> *vfw_rgb = new VolumeFormatWriter<VoxelRGB>;
//     vfw_rgb->attach( rc_ptr<ImageWriter<VoxelRGB> >( new TiffImageWriter<VoxelRGB> ) );
//     FormatDictionary<Volume<VoxelRGB> >::registerFormat( "TIFF", vfw_rgb, exts );
//     
//     VolumeFormatWriter<VoxelRGBA> *vfw_rgba = new VolumeFormatWriter<VoxelRGBA>;
//     vfw_rgba->attach( rc_ptr<ImageWriter<VoxelRGBA> >( new TiffImageWriter<VoxelRGBA> ) );
//     FormatDictionary<Volume<VoxelRGBA> >::registerFormat( "TIFF", vfw_rgba, exts );
//     
//     VolumeFormatWriter<VoxelHSV> *vfw_hsv = new VolumeFormatWriter<VoxelHSV>;
//     vfw_hsv->attach( rc_ptr<ImageWriter<VoxelHSV> >( new TiffImageWriter<VoxelHSV> ) );
//     FormatDictionary<Volume<VoxelHSV> >::registerFormat( "TIFF", vfw_hsv, exts );
//     
//     //--- COMPLEX -------------------------------------------------------------
// /*
//     VolumeFormatWriter<std::complex<float> > *vfw_cf = new VolumeFormatWriter<std::complex<float> >;
//     vfw_cf->attach( rc_ptr<ImageWriter<std::complex<float> > >( new TiffImageWriter<std::complex<float> > ) );
//     FormatDictionary<Volume<std::complex<float> > >::registerFormat( "TIFF", vfw_cf, exts );
// 
//     VolumeFormatWriter<std::complex<double> > *vfw_cd = new VolumeFormatWriter<std::complex<double> >;
//     vfw_cd->attach( rc_ptr<ImageWriter<std::complex<double> > >( new TiffImageWriter<std::complex<double> > ) );
//     FormatDictionary<Volume<std::complex<double> > >::registerFormat( "TIFF", vfw_cd, exts );
// */
//     //==========================================================================
//     //   V O L U M E   R E F
//     //==========================================================================
//     
//     //--- INT ------------------------------------------------------------------
// 
//     VolumeRefFormatWriter<int8_t> *rfw8 = new VolumeRefFormatWriter<int8_t>;
//     rfw8->attach( rc_ptr<ImageWriter<int8_t> >( new TiffImageWriter<int8_t> ) );
//     FormatDictionary<VolumeRef<int8_t> >::registerFormat( "TIFF", rfw8, exts );
// 
//     VolumeRefFormatWriter<int16_t> *rfw16 = new VolumeRefFormatWriter<int16_t>;
//     rfw16->attach( rc_ptr<ImageWriter<int16_t> >( new TiffImageWriter<int16_t> ) );
//     FormatDictionary<VolumeRef<int16_t> >::registerFormat( "TIFF", rfw16, exts );
// 
//     VolumeRefFormatWriter<int32_t> *rfw32 = new VolumeRefFormatWriter<int32_t>;
//     rfw32->attach( rc_ptr<ImageWriter<int32_t> >( new TiffImageWriter<int32_t> ) );
//     FormatDictionary<VolumeRef<int32_t> >::registerFormat( "TIFF", rfw32, exts );
// 
//     VolumeRefFormatWriter<int64_t> *rfw64 = new VolumeRefFormatWriter<int64_t>;
//     rfw64->attach( rc_ptr<ImageWriter<int64_t> >( new TiffImageWriter<int64_t> ) );
//     FormatDictionary<VolumeRef<int64_t> >::registerFormat( "TIFF", rfw64, exts );
// 
//     //--- UINT -----------------------------------------------------------------
// 
//     VolumeRefFormatWriter<uint8_t> *rfwu8 = new VolumeRefFormatWriter<uint8_t>;
//     rfwu8->attach( rc_ptr<ImageWriter<uint8_t> >( new TiffImageWriter<uint8_t> ) );
//     FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "TIFF", rfwu8, exts );
// 
//     VolumeRefFormatWriter<uint16_t> *rfwu16 = new VolumeRefFormatWriter<uint16_t>;
//     rfwu16->attach( rc_ptr<ImageWriter<uint16_t> >( new TiffImageWriter<uint16_t> ) );
//     FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "TIFF", rfwu16, exts );
// 
//     VolumeRefFormatWriter<uint32_t> *rfwu32 = new VolumeRefFormatWriter<uint32_t>;
//     rfwu32->attach( rc_ptr<ImageWriter<uint32_t> >( new TiffImageWriter<uint32_t> ) );
//     FormatDictionary<VolumeRef<uint32_t> >::registerFormat( "TIFF", rfwu32, exts );
// 
//     VolumeRefFormatWriter<uint64_t> *rfwu64 = new VolumeRefFormatWriter<uint64_t>;
//     rfwu64->attach( rc_ptr<ImageWriter<uint64_t> >( new TiffImageWriter<uint64_t> ) );
//     FormatDictionary<VolumeRef<uint64_t> >::registerFormat( "TIFF", rfwu64, exts );
// 
//     //--- FLOAT ----------------------------------------------------------------
//     
//     VolumeRefFormatWriter<float> *rfwf = new VolumeRefFormatWriter<float>;
//     rfwf->attach( rc_ptr<ImageWriter<float> >( new TiffImageWriter<float> ) );
//     FormatDictionary<VolumeRef<float> >::registerFormat( "TIFF", rfwf, exts );
//     
//     VolumeRefFormatWriter<double> *rfwd = new VolumeRefFormatWriter<double>;
//     rfwd->attach( rc_ptr<ImageWriter<double> >( new TiffImageWriter<double> ) );
//     FormatDictionary<VolumeRef<double> >::registerFormat( "TIFF", rfwd, exts );
//     
//     //--- RGB ----------------------------------------------------------------
//     
//     VolumeRefFormatWriter<VoxelRGB> *rfw_rgb = new VolumeRefFormatWriter<VoxelRGB>;
//     rfw_rgb->attach( rc_ptr<ImageWriter<VoxelRGB> >( new TiffImageWriter<VoxelRGB> ) );
//     FormatDictionary<VolumeRef<VoxelRGB> >::registerFormat( "TIFF", rfw_rgb, exts );
//     
//     VolumeRefFormatWriter<VoxelRGBA> *rfw_rgba = new VolumeRefFormatWriter<VoxelRGBA>;
//     rfw_rgba->attach( rc_ptr<ImageWriter<VoxelRGBA> >( new TiffImageWriter<VoxelRGBA> ) );
//     FormatDictionary<VolumeRef<VoxelRGBA> >::registerFormat( "TIFF", rfw_rgba, exts );
//     
//     VolumeRefFormatWriter<VoxelHSV> *rfw_hsv = new VolumeRefFormatWriter<VoxelHSV>;
//     rfw_hsv->attach( rc_ptr<ImageWriter<VoxelHSV> >( new TiffImageWriter<VoxelHSV> ) );
//     FormatDictionary<VolumeRef<VoxelHSV> >::registerFormat( "TIFF", rfw_hsv, exts );

    //--- COMPLEX -------------------------------------------------------------
/*
    VolumeRefFormatWriter<std::complex<float> > *rfw_cf = new VolumeRefFormatWriter<std::complex<float> >;
    rfw_cf->attach( rc_ptr<ImageWriter<std::complex<float> > >( new TiffImageWriter<std::complex<float> > ) );
    FormatDictionary<VolumeRef<std::complex<float> > >::registerFormat( "TIFF", rfw_cf, exts );

    VolumeRefFormatWriter<std::complex<double> > *rfw_cd = new VolumeRefFormatWriter<std::complex<double> >;
    rfw_cd->attach( rc_ptr<ImageWriter<std::complex<double> > >( new TiffImageWriter<std::complex<double> > ) );
    FormatDictionary<VolumeRef<std::complex<double> > >::registerFormat( "TIFF", rfw_cd, exts );
*/
}


TiffPlugin::~TiffPlugin()
{
}


string TiffPlugin::name() const
{
  return string("TIFF CARTO");
}


bool TiffPlugin::noop()
{
  return true;
}

