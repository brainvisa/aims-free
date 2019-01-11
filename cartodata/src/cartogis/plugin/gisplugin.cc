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
#include <cartodata/plugin/gisplugin.h>
#include <soma-io/image/gisimagereader.h>
#include <soma-io/image/gisimagewriter.h>
//--- cartodata --------------------------------------------------------------
#include <cartodata/io/volumeformatreader.h>
#include <cartodata/io/volumeformatwriter.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/io/formatdictionary.h>
#include <soma-io/vector/vector.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/type/types.h>
// #include <cartobase/type/voxelrgb.h>
// #include <cartobase/type/voxelrgba.h>
// #include <cartobase/type/voxelhsv.h>
//--- system -----------------------------------------------------------------
#include <vector>
#include <string>
//----------------------------------------------------------------------------

using namespace cartodata;
using namespace soma;
using namespace carto;
using namespace std;

namespace carto {
  class VoxelRGB;
  class VoxelRGBA;
  class VoxelHSV;
}

namespace {

  bool initgis()
  {
    new GisPlugin;
    return true;
  }

  bool gisinit __attribute__((unused)) = initgis();

}

GisPlugin::GisPlugin() : Plugin()
{
    vector<string>  exts(2);
    exts[0] = "ima";
    exts[1] = "dim";

    typedef AimsVector<float,6> vectorf6;

    ///////////////////////////////////////////////////////////////////////////
    ////                           R E A D E R                             ////
    ///////////////////////////////////////////////////////////////////////////

    //=========================================================================
    //   V O L U M E
    //=========================================================================

    //--- INT -----------------------------------------------------------------

    VolumeFormatReader<int8_t> *vfr8 = new VolumeFormatReader<int8_t>;
    vfr8->attach( rc_ptr<ImageReader<int8_t> >( new GisImageReader<int8_t> ) );
    FormatDictionary<Volume<int8_t> >::registerFormat( "GIS", vfr8, exts );

    VolumeFormatReader<int16_t> *vfr16 = new VolumeFormatReader<int16_t>;
    vfr16->attach( rc_ptr<ImageReader<int16_t> >( new GisImageReader<int16_t> ) );
    FormatDictionary<Volume<int16_t> >::registerFormat( "GIS", vfr16, exts );

    VolumeFormatReader<int32_t> *vfr32 = new VolumeFormatReader<int32_t>;
    vfr32->attach( rc_ptr<ImageReader<int32_t> >( new GisImageReader<int32_t> ) );
    FormatDictionary<Volume<int32_t> >::registerFormat( "GIS", vfr32, exts );

    VolumeFormatReader<int64_t> *vfr64 = new VolumeFormatReader<int64_t>;
    vfr64->attach( rc_ptr<ImageReader<int64_t> >( new GisImageReader<int64_t> ) );
    FormatDictionary<Volume<int64_t> >::registerFormat( "GIS", vfr64, exts );

    //--- UINT ----------------------------------------------------------------

    VolumeFormatReader<uint8_t> *vfru8 = new VolumeFormatReader<uint8_t>;
    vfru8->attach( rc_ptr<ImageReader<uint8_t> >( new GisImageReader<uint8_t> ) );
    FormatDictionary<Volume<uint8_t> >::registerFormat( "GIS", vfru8, exts );

    VolumeFormatReader<uint16_t> *vfru16 = new VolumeFormatReader<uint16_t>;
    vfru16->attach( rc_ptr<ImageReader<uint16_t> >( new GisImageReader<uint16_t> ) );
    FormatDictionary<Volume<uint16_t> >::registerFormat( "GIS", vfru16, exts );

    VolumeFormatReader<uint32_t> *vfru32 = new VolumeFormatReader<uint32_t>;
    vfru32->attach( rc_ptr<ImageReader<uint32_t> >( new GisImageReader<uint32_t> ) );
    FormatDictionary<Volume<uint32_t> >::registerFormat( "GIS", vfru32, exts );

    VolumeFormatReader<uint64_t> *vfru64 = new VolumeFormatReader<uint64_t>;
    vfru64->attach( rc_ptr<ImageReader<uint64_t> >( new GisImageReader<uint64_t> ) );
    FormatDictionary<Volume<uint64_t> >::registerFormat( "GIS", vfru64, exts );

    //--- FLOAT ---------------------------------------------------------------

    VolumeFormatReader<float> *vfrf = new VolumeFormatReader<float>;
    vfrf->attach( rc_ptr<ImageReader<float> >( new GisImageReader<float> ) );
    FormatDictionary<Volume<float> >::registerFormat( "GIS", vfrf, exts );

    VolumeFormatReader<double> *vfrd = new VolumeFormatReader<double>;
    vfrd->attach( rc_ptr<ImageReader<double> >( new GisImageReader<double> ) );
    FormatDictionary<Volume<double> >::registerFormat( "GIS", vfrd, exts );

    //--- RGB -----------------------------------------------------------------

    VolumeFormatReader<VoxelRGB> *vfr_rgb = new VolumeFormatReader<VoxelRGB>;
    vfr_rgb->attach( rc_ptr<ImageReader<VoxelRGB> >( new GisImageReader<VoxelRGB> ) );
    FormatDictionary<Volume<VoxelRGB> >::registerFormat( "GIS", vfr_rgb, exts );

    VolumeFormatReader<VoxelRGBA> *vfr_rgba = new VolumeFormatReader<VoxelRGBA>;
    vfr_rgba->attach( rc_ptr<ImageReader<VoxelRGBA> >( new GisImageReader<VoxelRGBA> ) );
    FormatDictionary<Volume<VoxelRGBA> >::registerFormat( "GIS", vfr_rgba, exts );

    VolumeFormatReader<VoxelHSV> *vfr_hsv = new VolumeFormatReader<VoxelHSV>;
    vfr_hsv->attach( rc_ptr<ImageReader<VoxelHSV> >( new GisImageReader<VoxelHSV> ) );
    FormatDictionary<Volume<VoxelHSV> >::registerFormat( "GIS", vfr_hsv, exts );

    //--- COMPLEX -------------------------------------------------------------

    VolumeFormatReader<std::complex<float> > *vfr_cf = new VolumeFormatReader<std::complex<float> >;
    vfr_cf->attach( rc_ptr<ImageReader<std::complex<float> > >( new GisImageReader<std::complex<float> > ) );
    FormatDictionary<Volume<std::complex<float> > >::registerFormat( "GIS", vfr_cf, exts );

    VolumeFormatReader<std::complex<double> > *vfr_cd = new VolumeFormatReader<std::complex<double> >;
    vfr_cd->attach( rc_ptr<ImageReader<std::complex<double> > >( new GisImageReader<std::complex<double> > ) );
    FormatDictionary<Volume<std::complex<double> > >::registerFormat( "GIS", vfr_cd, exts );

    //--- VECTORS -------------------------------------------------------------

    VolumeFormatReader<Point3df> *vfrv3f = new VolumeFormatReader<Point3df>;
    vfrv3f->attach( rc_ptr<ImageReader<Point3df> >(
      new GisImageReader<Point3df> ) );
    FormatDictionary<Volume<Point3df> >::registerFormat( "GIS", vfrv3f, exts );

    VolumeFormatReader<Point3d> *vfrv3s = new VolumeFormatReader<Point3d>;
    vfrv3s->attach( rc_ptr<ImageReader<Point3d> >(
      new GisImageReader<Point3d> ) );
    FormatDictionary<Volume<Point3d> >::registerFormat( "GIS", vfrv3s, exts );

    VolumeFormatReader<Point2d> *vfrv2s = new VolumeFormatReader<Point2d>;
    vfrv2s->attach( rc_ptr<ImageReader<Point2d> >( new GisImageReader<Point2d> ) );
    FormatDictionary<Volume<Point2d> >::registerFormat( "GIS", vfrv2s, exts );

    typedef AimsVector<float,6> vectorf6;

    VolumeFormatReader<vectorf6> *vfrv6f = new VolumeFormatReader<vectorf6>;
    vfrv6f->attach( rc_ptr<ImageReader<vectorf6> >(
      new GisImageReader<vectorf6> ) );
    FormatDictionary<Volume<vectorf6> >::registerFormat( "GIS", vfrv6f, exts );

    //=========================================================================
    //   V O L U M E   R E F
    //=========================================================================

    //--- INT -----------------------------------------------------------------

    VolumeRefFormatReader<int8_t> *rfr8 = new VolumeRefFormatReader<int8_t>;
    rfr8->attach( rc_ptr<ImageReader<int8_t> >( new GisImageReader<int8_t> ) );
    FormatDictionary<VolumeRef<int8_t> >::registerFormat( "GIS", rfr8, exts );

    VolumeRefFormatReader<int16_t> *rfr16 = new VolumeRefFormatReader<int16_t>;
    rfr16->attach( rc_ptr<ImageReader<int16_t> >( new GisImageReader<int16_t> ) );
    FormatDictionary<VolumeRef<int16_t> >::registerFormat( "GIS", rfr16, exts );

    VolumeRefFormatReader<int32_t> *rfr32 = new VolumeRefFormatReader<int32_t>;
    rfr32->attach( rc_ptr<ImageReader<int32_t> >( new GisImageReader<int32_t> ) );
    FormatDictionary<VolumeRef<int32_t> >::registerFormat( "GIS", rfr32, exts );

    VolumeRefFormatReader<int64_t> *rfr64 = new VolumeRefFormatReader<int64_t>;
    rfr64->attach( rc_ptr<ImageReader<int64_t> >( new GisImageReader<int64_t> ) );
    FormatDictionary<VolumeRef<int64_t> >::registerFormat( "GIS", rfr64, exts );

    //--- UINT ----------------------------------------------------------------

    VolumeRefFormatReader<uint8_t> *rfru8 = new VolumeRefFormatReader<uint8_t>;
    rfru8->attach( rc_ptr<ImageReader<uint8_t> >( new GisImageReader<uint8_t> ) );
    FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "GIS", rfru8, exts );

    VolumeRefFormatReader<uint16_t> *rfru16 = new VolumeRefFormatReader<uint16_t>;
    rfru16->attach( rc_ptr<ImageReader<uint16_t> >( new GisImageReader<uint16_t> ) );
    FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "GIS", rfru16, exts );

    VolumeRefFormatReader<uint32_t> *rfru32 = new VolumeRefFormatReader<uint32_t>;
    rfru32->attach( rc_ptr<ImageReader<uint32_t> >( new GisImageReader<uint32_t> ) );
    FormatDictionary<VolumeRef<uint32_t> >::registerFormat( "GIS", rfru32, exts );

    VolumeRefFormatReader<uint64_t> *rfru64 = new VolumeRefFormatReader<uint64_t>;
    rfru64->attach( rc_ptr<ImageReader<uint64_t> >( new GisImageReader<uint64_t> ) );
    FormatDictionary<VolumeRef<uint64_t> >::registerFormat( "GIS", rfru64, exts );

    //--- FLOAT ---------------------------------------------------------------

    VolumeRefFormatReader<float> *rfrf = new VolumeRefFormatReader<float>;
    rfrf->attach( rc_ptr<ImageReader<float> >( new GisImageReader<float> ) );
    FormatDictionary<VolumeRef<float> >::registerFormat( "GIS", rfrf, exts );

    VolumeRefFormatReader<double> *rfrd = new VolumeRefFormatReader<double>;
    rfrd->attach( rc_ptr<ImageReader<double> >( new GisImageReader<double> ) );
    FormatDictionary<VolumeRef<double> >::registerFormat( "GIS", rfrd, exts );

    //--- RGB -----------------------------------------------------------------

    VolumeRefFormatReader<VoxelRGB> *rfr_rgb = new VolumeRefFormatReader<VoxelRGB>;
    rfr_rgb->attach( rc_ptr<ImageReader<VoxelRGB> >( new GisImageReader<VoxelRGB> ) );
    FormatDictionary<VolumeRef<VoxelRGB> >::registerFormat( "GIS", rfr_rgb, exts );

    VolumeRefFormatReader<VoxelRGBA> *rfr_rgba = new VolumeRefFormatReader<VoxelRGBA>;
    rfr_rgba->attach( rc_ptr<ImageReader<VoxelRGBA> >( new GisImageReader<VoxelRGBA> ) );
    FormatDictionary<VolumeRef<VoxelRGBA> >::registerFormat( "GIS", rfr_rgba, exts );

    VolumeRefFormatReader<VoxelHSV> *rfr_hsv = new VolumeRefFormatReader<VoxelHSV>;
    rfr_hsv->attach( rc_ptr<ImageReader<VoxelHSV> >( new GisImageReader<VoxelHSV> ) );
    FormatDictionary<VolumeRef<VoxelHSV> >::registerFormat( "GIS", rfr_hsv, exts );

    //--- COMPLEX ------------------------------------------------------------

    VolumeRefFormatReader<std::complex<float> > *rfr_cf = new VolumeRefFormatReader<std::complex<float> >;
    rfr_cf->attach( rc_ptr<ImageReader<std::complex<float> > >( new GisImageReader<std::complex<float> > ) );
    FormatDictionary<VolumeRef<std::complex<float> > >::registerFormat( "GIS", rfr_cf, exts );

    VolumeRefFormatReader<std::complex<double> > *rfr_cd = new VolumeRefFormatReader<std::complex<double> >;
    rfr_cd->attach( rc_ptr<ImageReader<std::complex<double> > >( new GisImageReader<std::complex<double> > ) );
    FormatDictionary<VolumeRef<std::complex<double> > >::registerFormat( "GIS", rfr_cd, exts );

    //--- VECTORS -------------------------------------------------------------

    VolumeRefFormatReader<Point3df> *rfrv3f = new VolumeRefFormatReader<Point3df>;
    rfrv3f->attach( rc_ptr<ImageReader<Point3df> >(
      new GisImageReader<Point3df> ) );
    FormatDictionary<VolumeRef<Point3df> >::registerFormat( "GIS", rfrv3f,
                                                            exts );

    VolumeRefFormatReader<Point3d> *rfrv3s
      = new VolumeRefFormatReader<Point3d>;
    rfrv3s->attach( rc_ptr<ImageReader<Point3d> >(
      new GisImageReader<Point3d> ) );
    FormatDictionary<VolumeRef<Point3d> >::registerFormat( "GIS", rfrv3s,
                                                           exts );

    VolumeRefFormatReader<Point2d> *rfrv2s
      = new VolumeRefFormatReader<Point2d>;
    rfrv2s->attach( rc_ptr<ImageReader<Point2d> >(
      new GisImageReader<Point2d> ) );
    FormatDictionary<VolumeRef<Point2d> >::registerFormat( "GIS", rfrv2s,
                                                           exts );

    VolumeRefFormatReader<vectorf6> *rfrv6f
      = new VolumeRefFormatReader<vectorf6>;
    rfrv6f->attach( rc_ptr<ImageReader<vectorf6> >(
      new GisImageReader<vectorf6> ) );
    FormatDictionary<VolumeRef<vectorf6> >::registerFormat( "GIS", rfrv6f,
                                                            exts );


    ///////////////////////////////////////////////////////////////////////////
    ////                           W R I T E R                             ////
    ///////////////////////////////////////////////////////////////////////////

    //=========================================================================
    //   V O L U M E
    //=========================================================================

    //--- INT -----------------------------------------------------------------

    VolumeFormatWriter<int8_t> *vfw8 = new VolumeFormatWriter<int8_t>;
    vfw8->attach( rc_ptr<ImageWriter<int8_t> >( new GisImageWriter<int8_t> ) );
    FormatDictionary<Volume<int8_t> >::registerFormat( "GIS", vfw8, exts );

    VolumeFormatWriter<int16_t> *vfw16 = new VolumeFormatWriter<int16_t>;
    vfw16->attach( rc_ptr<ImageWriter<int16_t> >( new GisImageWriter<int16_t> ) );
    FormatDictionary<Volume<int16_t> >::registerFormat( "GIS", vfw16, exts );

    VolumeFormatWriter<int32_t> *vfw32 = new VolumeFormatWriter<int32_t>;
    vfw32->attach( rc_ptr<ImageWriter<int32_t> >( new GisImageWriter<int32_t> ) );
    FormatDictionary<Volume<int32_t> >::registerFormat( "GIS", vfw32, exts );

    VolumeFormatWriter<int64_t> *vfw64 = new VolumeFormatWriter<int64_t>;
    vfw64->attach( rc_ptr<ImageWriter<int64_t> >( new GisImageWriter<int64_t> ) );
    FormatDictionary<Volume<int64_t> >::registerFormat( "GIS", vfw64, exts );

    //--- UINT ----------------------------------------------------------------

    VolumeFormatWriter<uint8_t> *vfwu8 = new VolumeFormatWriter<uint8_t>;
    vfwu8->attach( rc_ptr<ImageWriter<uint8_t> >( new GisImageWriter<uint8_t> ) );
    FormatDictionary<Volume<uint8_t> >::registerFormat( "GIS", vfwu8, exts );

    VolumeFormatWriter<uint16_t> *vfwu16 = new VolumeFormatWriter<uint16_t>;
    vfwu16->attach( rc_ptr<ImageWriter<uint16_t> >( new GisImageWriter<uint16_t> ) );
    FormatDictionary<Volume<uint16_t> >::registerFormat( "GIS", vfwu16, exts );

    VolumeFormatWriter<uint32_t> *vfwu32 = new VolumeFormatWriter<uint32_t>;
    vfwu32->attach( rc_ptr<ImageWriter<uint32_t> >( new GisImageWriter<uint32_t> ) );
    FormatDictionary<Volume<uint32_t> >::registerFormat( "GIS", vfwu32, exts );

    VolumeFormatWriter<uint64_t> *vfwu64 = new VolumeFormatWriter<uint64_t>;
    vfwu64->attach( rc_ptr<ImageWriter<uint64_t> >( new GisImageWriter<uint64_t> ) );
    FormatDictionary<Volume<uint64_t> >::registerFormat( "GIS", vfwu64, exts );

    //--- FLOAT ---------------------------------------------------------------

    VolumeFormatWriter<float> *vfwf = new VolumeFormatWriter<float>;
    vfwf->attach( rc_ptr<ImageWriter<float> >( new GisImageWriter<float> ) );
    FormatDictionary<Volume<float> >::registerFormat( "GIS", vfwf, exts );

    VolumeFormatWriter<double> *vfwd = new VolumeFormatWriter<double>;
    vfwd->attach( rc_ptr<ImageWriter<double> >( new GisImageWriter<double> ) );
    FormatDictionary<Volume<double> >::registerFormat( "GIS", vfwd, exts );

    //--- RGB -----------------------------------------------------------------

    VolumeFormatWriter<VoxelRGB> *vfw_rgb = new VolumeFormatWriter<VoxelRGB>;
    vfw_rgb->attach( rc_ptr<ImageWriter<VoxelRGB> >( new GisImageWriter<VoxelRGB> ) );
    FormatDictionary<Volume<VoxelRGB> >::registerFormat( "GIS", vfw_rgb, exts );

    VolumeFormatWriter<VoxelRGBA> *vfw_rgba = new VolumeFormatWriter<VoxelRGBA>;
    vfw_rgba->attach( rc_ptr<ImageWriter<VoxelRGBA> >( new GisImageWriter<VoxelRGBA> ) );
    FormatDictionary<Volume<VoxelRGBA> >::registerFormat( "GIS", vfw_rgba, exts );

    VolumeFormatWriter<VoxelHSV> *vfw_hsv = new VolumeFormatWriter<VoxelHSV>;
    vfw_hsv->attach( rc_ptr<ImageWriter<VoxelHSV> >( new GisImageWriter<VoxelHSV> ) );
    FormatDictionary<Volume<VoxelHSV> >::registerFormat( "GIS", vfw_hsv, exts );

    //--- COMPLEX -------------------------------------------------------------

    VolumeFormatWriter<std::complex<float> > *vfw_cf = new VolumeFormatWriter<std::complex<float> >;
    vfw_cf->attach( rc_ptr<ImageWriter<std::complex<float> > >( new GisImageWriter<std::complex<float> > ) );
    FormatDictionary<Volume<std::complex<float> > >::registerFormat( "GIS", vfw_cf, exts );

    VolumeFormatWriter<std::complex<double> > *vfw_cd = new VolumeFormatWriter<std::complex<double> >;
    vfw_cd->attach( rc_ptr<ImageWriter<std::complex<double> > >( new GisImageWriter<std::complex<double> > ) );
    FormatDictionary<Volume<std::complex<double> > >::registerFormat( "GIS", vfw_cd, exts );

    //--- VECTORS -------------------------------------------------------------

    VolumeFormatWriter<Point3df> *vfwv3f = new VolumeFormatWriter<Point3df>;
    vfwv3f->attach( rc_ptr<ImageWriter<Point3df> >(
      new GisImageWriter<Point3df> ) );
    FormatDictionary<Volume<Point3df> >::registerFormat( "GIS", vfwv3f, exts );

    VolumeFormatWriter<Point3d> *vfwv3s = new VolumeFormatWriter<Point3d>;
    vfwv3s->attach( rc_ptr<ImageWriter<Point3d> >(
      new GisImageWriter<Point3d> ) );
    FormatDictionary<Volume<Point3d> >::registerFormat( "GIS", vfwv3s, exts );

    VolumeFormatWriter<Point2d> *vfwc2s = new VolumeFormatWriter<Point2d>;
    vfwc2s->attach( rc_ptr<ImageWriter<Point2d> >(
      new GisImageWriter<Point2d> ) );
    FormatDictionary<Volume<Point2d> >::registerFormat( "GIS", vfwc2s, exts );

    VolumeFormatWriter<vectorf6> *vfwv6f = new VolumeFormatWriter<vectorf6>;
    vfwv6f->attach( rc_ptr<ImageWriter<vectorf6> >(
      new GisImageWriter<vectorf6> ) );
    FormatDictionary<Volume<vectorf6> >::registerFormat( "GIS", vfwv6f, exts );


    //=========================================================================
    //   V O L U M E   R E F
    //=========================================================================

    //--- INT -----------------------------------------------------------------

    VolumeRefFormatWriter<int8_t> *rfw8 = new VolumeRefFormatWriter<int8_t>;
    rfw8->attach( rc_ptr<ImageWriter<int8_t> >( new GisImageWriter<int8_t> ) );
    FormatDictionary<VolumeRef<int8_t> >::registerFormat( "GIS", rfw8, exts );

    VolumeRefFormatWriter<int16_t> *rfw16 = new VolumeRefFormatWriter<int16_t>;
    rfw16->attach( rc_ptr<ImageWriter<int16_t> >( new GisImageWriter<int16_t> ) );
    FormatDictionary<VolumeRef<int16_t> >::registerFormat( "GIS", rfw16, exts );

    VolumeRefFormatWriter<int32_t> *rfw32 = new VolumeRefFormatWriter<int32_t>;
    rfw32->attach( rc_ptr<ImageWriter<int32_t> >( new GisImageWriter<int32_t> ) );
    FormatDictionary<VolumeRef<int32_t> >::registerFormat( "GIS", rfw32, exts );

    VolumeRefFormatWriter<int64_t> *rfw64 = new VolumeRefFormatWriter<int64_t>;
    rfw64->attach( rc_ptr<ImageWriter<int64_t> >( new GisImageWriter<int64_t> ) );
    FormatDictionary<VolumeRef<int64_t> >::registerFormat( "GIS", rfw64, exts );

    //--- UINT ----------------------------------------------------------------

    VolumeRefFormatWriter<uint8_t> *rfwu8 = new VolumeRefFormatWriter<uint8_t>;
    rfwu8->attach( rc_ptr<ImageWriter<uint8_t> >( new GisImageWriter<uint8_t> ) );
    FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "GIS", rfwu8, exts );

    VolumeRefFormatWriter<uint16_t> *rfwu16 = new VolumeRefFormatWriter<uint16_t>;
    rfwu16->attach( rc_ptr<ImageWriter<uint16_t> >( new GisImageWriter<uint16_t> ) );
    FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "GIS", rfwu16, exts );

    VolumeRefFormatWriter<uint32_t> *rfwu32 = new VolumeRefFormatWriter<uint32_t>;
    rfwu32->attach( rc_ptr<ImageWriter<uint32_t> >( new GisImageWriter<uint32_t> ) );
    FormatDictionary<VolumeRef<uint32_t> >::registerFormat( "GIS", rfwu32, exts );

    VolumeRefFormatWriter<uint64_t> *rfwu64 = new VolumeRefFormatWriter<uint64_t>;
    rfwu64->attach( rc_ptr<ImageWriter<uint64_t> >( new GisImageWriter<uint64_t> ) );
    FormatDictionary<VolumeRef<uint64_t> >::registerFormat( "GIS", rfwu64, exts );

    //--- FLOAT ---------------------------------------------------------------

    VolumeRefFormatWriter<float> *rfwf = new VolumeRefFormatWriter<float>;
    rfwf->attach( rc_ptr<ImageWriter<float> >( new GisImageWriter<float> ) );
    FormatDictionary<VolumeRef<float> >::registerFormat( "GIS", rfwf, exts );

    VolumeRefFormatWriter<double> *rfwd = new VolumeRefFormatWriter<double>;
    rfwd->attach( rc_ptr<ImageWriter<double> >( new GisImageWriter<double> ) );
    FormatDictionary<VolumeRef<double> >::registerFormat( "GIS", rfwd, exts );

    //--- RGB -----------------------------------------------------------------

    VolumeRefFormatWriter<VoxelRGB> *rfw_rgb = new VolumeRefFormatWriter<VoxelRGB>;
    rfw_rgb->attach( rc_ptr<ImageWriter<VoxelRGB> >( new GisImageWriter<VoxelRGB> ) );
    FormatDictionary<VolumeRef<VoxelRGB> >::registerFormat( "GIS", rfw_rgb, exts );

    VolumeRefFormatWriter<VoxelRGBA> *rfw_rgba = new VolumeRefFormatWriter<VoxelRGBA>;
    rfw_rgba->attach( rc_ptr<ImageWriter<VoxelRGBA> >( new GisImageWriter<VoxelRGBA> ) );
    FormatDictionary<VolumeRef<VoxelRGBA> >::registerFormat( "GIS", rfw_rgba, exts );

    VolumeRefFormatWriter<VoxelHSV> *rfw_hsv = new VolumeRefFormatWriter<VoxelHSV>;
    rfw_hsv->attach( rc_ptr<ImageWriter<VoxelHSV> >( new GisImageWriter<VoxelHSV> ) );
    FormatDictionary<VolumeRef<VoxelHSV> >::registerFormat( "GIS", rfw_hsv, exts );

    //--- COMPLEX -------------------------------------------------------------

    VolumeRefFormatWriter<std::complex<float> > *rfw_cf = new VolumeRefFormatWriter<std::complex<float> >;
    rfw_cf->attach( rc_ptr<ImageWriter<std::complex<float> > >( new GisImageWriter<std::complex<float> > ) );
    FormatDictionary<VolumeRef<std::complex<float> > >::registerFormat( "GIS", rfw_cf, exts );

    VolumeRefFormatWriter<std::complex<double> > *rfw_cd = new VolumeRefFormatWriter<std::complex<double> >;
    rfw_cd->attach( rc_ptr<ImageWriter<std::complex<double> > >( new GisImageWriter<std::complex<double> > ) );
    FormatDictionary<VolumeRef<std::complex<double> > >::registerFormat( "GIS", rfw_cd, exts );

    //--- VECTORS -------------------------------------------------------------

    VolumeRefFormatWriter<Point3df> *rfwv3f
      = new VolumeRefFormatWriter<Point3df>;
    rfwv3f->attach( rc_ptr<ImageWriter<Point3df> >(
      new GisImageWriter<Point3df> ) );
    FormatDictionary<VolumeRef<Point3df> >::registerFormat( "GIS", rfwv3f,
                                                            exts );

    VolumeRefFormatWriter<Point3d> *rfwv3s
      = new VolumeRefFormatWriter<Point3d>;
    rfwv3s->attach( rc_ptr<ImageWriter<Point3d> >(
      new GisImageWriter<Point3d> ) );
    FormatDictionary<VolumeRef<Point3d> >::registerFormat( "GIS", rfwv3s,
                                                           exts );

    VolumeRefFormatWriter<Point2d> *rfwv2s
      = new VolumeRefFormatWriter<Point2d>;
    rfwv2s->attach( rc_ptr<ImageWriter<Point2d> >(
      new GisImageWriter<Point2d> ) );
    FormatDictionary<VolumeRef<Point2d> >::registerFormat( "GIS", rfwv2s,
                                                           exts );

    VolumeRefFormatWriter<vectorf6> *rfwv6f
      = new VolumeRefFormatWriter<vectorf6>;
    rfwv6f->attach( rc_ptr<ImageWriter<vectorf6> >(
      new GisImageWriter<vectorf6> ) );
    FormatDictionary<VolumeRef<vectorf6> >::registerFormat( "GIS", rfwv6f,
                                                            exts );

}


GisPlugin::~GisPlugin()
{
}


string GisPlugin::name() const
{
  return string("GIS CARTO");
}


bool GisPlugin::noop()
{
  return true;
}


#include <soma-io/image/gisimagereader_d.h>
#include <soma-io/image/gisimagewriter_d.h>
#include <aims/io/asciidatasourcetraits_aims.h>

namespace carto
{
  template class GisImageReader<Point3df>;
  template class GisImageReader<Point3d>;
  template class GisImageReader<Point2d>;
  template class GisImageReader<AimsVector<float, 6> >;

  template class GisImageWriter<Point3df>;
  template class GisImageWriter<Point3d>;
  template class GisImageWriter<Point2d>;
  template class GisImageWriter<AimsVector<float, 6> >;
}

