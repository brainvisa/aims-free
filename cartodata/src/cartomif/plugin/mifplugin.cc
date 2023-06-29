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
#include <cartodata/plugin/mifplugin.h>
#include <soma-io/image/mifimagereader.h>
//--- cartodata --------------------------------------------------------------
#include <cartodata/io/volumeformatreader.h>
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

namespace {

  bool initmif()
  {
    new MifPlugin;
    return true;
  }

  bool mifinit __attribute__((unused)) = initmif();

}

MifPlugin::MifPlugin() : Plugin()
{
    vector<string>  exts(7);
    exts[0] = "mif";
    exts[1] = "mif.gz";
    exts[4] = "mih";
    exts[6] = "gz";

    ////////////////////////////////////////////////////////////////////////////
    ////                           R E A D E R                              ////
    ////////////////////////////////////////////////////////////////////////////

    //==========================================================================
    //   V O L U M E
    //==========================================================================

    //--- INT ------------------------------------------------------------------

    VolumeFormatReader<int8_t> *vfr8 = new VolumeFormatReader<int8_t>;
    vfr8->attach( rc_ptr<ImageReader<int8_t> >( new MifImageReader<int8_t> ) );
    FormatDictionary<Volume<int8_t> >::registerFormat( "MIF", vfr8, exts );

    VolumeFormatReader<int16_t> *vfr16 = new VolumeFormatReader<int16_t>;
    vfr16->attach( rc_ptr<ImageReader<int16_t> >( new MifImageReader<int16_t> ) );
    FormatDictionary<Volume<int16_t> >::registerFormat( "MIF", vfr16, exts );

    VolumeFormatReader<int32_t> *vfr32 = new VolumeFormatReader<int32_t>;
    vfr32->attach( rc_ptr<ImageReader<int32_t> >( new MifImageReader<int32_t> ) );
    FormatDictionary<Volume<int32_t> >::registerFormat( "MIF", vfr32, exts );


    //--- UINT -----------------------------------------------------------------

    VolumeFormatReader<uint8_t> *vfru8 = new VolumeFormatReader<uint8_t>;
    vfru8->attach( rc_ptr<ImageReader<uint8_t> >( new MifImageReader<uint8_t> ) );
    FormatDictionary<Volume<uint8_t> >::registerFormat( "MIF", vfru8, exts );

    VolumeFormatReader<uint16_t> *vfru16 = new VolumeFormatReader<uint16_t>;
    vfru16->attach( rc_ptr<ImageReader<uint16_t> >( new MifImageReader<uint16_t> ) );
    FormatDictionary<Volume<uint16_t> >::registerFormat( "MIF", vfru16, exts );

    VolumeFormatReader<uint32_t> *vfru32 = new VolumeFormatReader<uint32_t>;
    vfru32->attach( rc_ptr<ImageReader<uint32_t> >( new MifImageReader<uint32_t> ) );
    FormatDictionary<Volume<uint32_t> >::registerFormat( "MIF", vfru32, exts );

    //--- FLOAT ----------------------------------------------------------------

    VolumeFormatReader<float> *vfrf = new VolumeFormatReader<float>;
    vfrf->attach( rc_ptr<ImageReader<float> >( new MifImageReader<float> ) );
    FormatDictionary<Volume<float> >::registerFormat( "MIF", vfrf, exts );

    VolumeFormatReader<double> *vfrd = new VolumeFormatReader<double>;
    vfrd->attach( rc_ptr<ImageReader<double> >( new MifImageReader<double> ) );
    FormatDictionary<Volume<double> >::registerFormat( "MIF", vfrd, exts );

    //--- RGB ------------------------------------------------------------------

    // TODO: RGB is not a proper datatype in MIF, see if we can detect what
    // images should be interpreted as RGB

/*
    VolumeFormatReader<VoxelRGB> *vfr_rgb = new VolumeFormatReader<VoxelRGB>;
    vfr_rgb->attach( rc_ptr<ImageReader<VoxelRGB> >( new MifImageReader<VoxelRGB> ) );
    FormatDictionary<Volume<VoxelRGB> >::registerFormat( "MIF", vfr_rgb, exts );

    VolumeFormatReader<VoxelRGBA> *vfr_rgba = new VolumeFormatReader<VoxelRGBA>;
    vfr_rgba->attach( rc_ptr<ImageReader<VoxelRGBA> >( new MifImageReader<VoxelRGBA> ) );
    FormatDictionary<Volume<VoxelRGBA> >::registerFormat( "MIF", vfr_rgba, exts );

    VolumeFormatReader<VoxelHSV> *vfr_hsv = new VolumeFormatReader<VoxelHSV>;
    vfr_hsv->attach( rc_ptr<ImageReader<VoxelHSV> >( new MifImageReader<VoxelHSV> ) );
    FormatDictionary<Volume<VoxelHSV> >::registerFormat( "MIF", vfr_hsv, exts );
*/
    //--- COMPLEX -------------------------------------------------------------

    VolumeFormatReader<std::complex<float> > *vfr_cf = new VolumeFormatReader<std::complex<float> >;
    vfr_cf->attach( rc_ptr<ImageReader<std::complex<float> > >( new MifImageReader<std::complex<float> > ) );
    FormatDictionary<Volume<std::complex<float> > >::registerFormat( "MIF", vfr_cf, exts );

    VolumeFormatReader<std::complex<double> > *vfr_cd = new VolumeFormatReader<std::complex<double> >;
    vfr_cd->attach( rc_ptr<ImageReader<std::complex<double> > >( new MifImageReader<std::complex<double> > ) );
    FormatDictionary<Volume<std::complex<double> > >::registerFormat( "MIF", vfr_cd, exts );

    //==========================================================================
    //   V O L U M E   R E F
    //==========================================================================

    //--- INT ------------------------------------------------------------------

    VolumeRefFormatReader<int8_t> *rfr8 = new VolumeRefFormatReader<int8_t>;
    rfr8->attach( rc_ptr<ImageReader<int8_t> >( new MifImageReader<int8_t> ) );
    FormatDictionary<VolumeRef<int8_t> >::registerFormat( "MIF", rfr8, exts );

    VolumeRefFormatReader<int16_t> *rfr16 = new VolumeRefFormatReader<int16_t>;
    rfr16->attach( rc_ptr<ImageReader<int16_t> >( new MifImageReader<int16_t> ) );
    FormatDictionary<VolumeRef<int16_t> >::registerFormat( "MIF", rfr16, exts );

    VolumeRefFormatReader<int32_t> *rfr32 = new VolumeRefFormatReader<int32_t>;
    rfr32->attach( rc_ptr<ImageReader<int32_t> >( new MifImageReader<int32_t> ) );
    FormatDictionary<VolumeRef<int32_t> >::registerFormat( "MIF", rfr32, exts );

    //--- UINT -----------------------------------------------------------------

    VolumeRefFormatReader<uint8_t> *rfru8 = new VolumeRefFormatReader<uint8_t>;
    rfru8->attach( rc_ptr<ImageReader<uint8_t> >( new MifImageReader<uint8_t> ) );
    FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "MIF", rfru8, exts );

    VolumeRefFormatReader<uint16_t> *rfru16 = new VolumeRefFormatReader<uint16_t>;
    rfru16->attach( rc_ptr<ImageReader<uint16_t> >( new MifImageReader<uint16_t> ) );
    FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "MIF", rfru16, exts );

    VolumeRefFormatReader<uint32_t> *rfru32 = new VolumeRefFormatReader<uint32_t>;
    rfru32->attach( rc_ptr<ImageReader<uint32_t> >( new MifImageReader<uint32_t> ) );
    FormatDictionary<VolumeRef<uint32_t> >::registerFormat( "MIF", rfru32, exts );

    //--- FLOAT ----------------------------------------------------------------

    VolumeRefFormatReader<float> *rfrf = new VolumeRefFormatReader<float>;
    rfrf->attach( rc_ptr<ImageReader<float> >( new MifImageReader<float> ) );
    FormatDictionary<VolumeRef<float> >::registerFormat( "MIF", rfrf, exts );

    VolumeRefFormatReader<double> *rfrd = new VolumeRefFormatReader<double>;
    rfrd->attach( rc_ptr<ImageReader<double> >( new MifImageReader<double> ) );
    FormatDictionary<VolumeRef<double> >::registerFormat( "MIF", rfrd, exts );

    //--- RGB ------------------------------------------------------------------

    // TODO: RGB is not a proper datatype in MIF, see if we can detect what
    // images should be interpreted as RGB

/*
    VolumeRefFormatReader<VoxelRGB> *rfr_rgb = new VolumeRefFormatReader<VoxelRGB>;
    rfr_rgb->attach( rc_ptr<ImageReader<VoxelRGB> >( new MifImageReader<VoxelRGB> ) );
    FormatDictionary<VolumeRef<VoxelRGB> >::registerFormat( "MIF", rfr_rgb, exts );

    VolumeRefFormatReader<VoxelRGBA> *rfr_rgba = new VolumeRefFormatReader<VoxelRGBA>;
    rfr_rgba->attach( rc_ptr<ImageReader<VoxelRGBA> >( new MifImageReader<VoxelRGBA> ) );
    FormatDictionary<VolumeRef<VoxelRGBA> >::registerFormat( "MIF", rfr_rgba, exts );

    VolumeRefFormatReader<VoxelHSV> *rfr_hsv = new VolumeRefFormatReader<VoxelHSV>;
    rfr_hsv->attach( rc_ptr<ImageReader<VoxelHSV> >( new MifImageReader<VoxelHSV> ) );
    FormatDictionary<VolumeRef<VoxelHSV> >::registerFormat( "MIF", rfr_hsv, exts );
*/
    //--- COMPLEX -------------------------------------------------------------

    VolumeRefFormatReader<std::complex<float> > *rfr_cf = new VolumeRefFormatReader<std::complex<float> >;
    rfr_cf->attach( rc_ptr<ImageReader<std::complex<float> > >( new MifImageReader<std::complex<float> > ) );
    FormatDictionary<VolumeRef<std::complex<float> > >::registerFormat(
      "MIF", rfr_cf, exts );

    VolumeRefFormatReader<std::complex<double> > *rfr_cd = new VolumeRefFormatReader<std::complex<double> >;
    rfr_cd->attach( rc_ptr<ImageReader<std::complex<double> > >( new MifImageReader<std::complex<double> > ) );
    FormatDictionary<VolumeRef<std::complex<double> > >::registerFormat(
      "MIF", rfr_cd, exts );
}


MifPlugin::~MifPlugin()
{
}


string MifPlugin::name() const
{
  return string("MIF CARTO");
}


bool MifPlugin::noop()
{
  return true;
}
