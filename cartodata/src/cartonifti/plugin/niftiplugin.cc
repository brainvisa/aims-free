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
#include <cartodata/plugin/niftiplugin.h>
#include <soma-io/image/nifti1imagereader.h>
#include <soma-io/image/nifti1imagewriter.h>
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

  bool initnifti()
  {
    new NiftiPlugin;
    return true;
  }

  bool niftiinit = initnifti();

}

NiftiPlugin::NiftiPlugin() : Plugin()
{
    vector<string>  exts(6);
    exts[0] = "nii";
    exts[1] = "nii.gz";
    exts[2] = "img";
    exts[3] = "img.gz";
    exts[4] = "hdr";
    exts[5] = "gz";

    ////////////////////////////////////////////////////////////////////////////
    ////                           R E A D E R                              ////
    ////////////////////////////////////////////////////////////////////////////

    //==========================================================================
    //   V O L U M E
    //==========================================================================

    //--- INT ------------------------------------------------------------------

    VolumeFormatReader<int8_t> *vfr8 = new VolumeFormatReader<int8_t>;
    vfr8->attach( rc_ptr<ImageReader<int8_t> >( new Nifti1ImageReader<int8_t> ) );
    FormatDictionary<Volume<int8_t> >::registerFormat( "NIFTI-1", vfr8, exts );
    vfr8 = new VolumeFormatReader<int8_t>;
    vfr8->attach( rc_ptr<ImageReader<int8_t> >( new Nifti1ImageReader<int8_t> ) );
    FormatDictionary<Volume<int8_t> >::registerFormat( "NIFTI-2", vfr8, exts );

    VolumeFormatReader<int16_t> *vfr16 = new VolumeFormatReader<int16_t>;
    vfr16->attach( rc_ptr<ImageReader<int16_t> >( new Nifti1ImageReader<int16_t> ) );
    FormatDictionary<Volume<int16_t> >::registerFormat( "NIFTI-1", vfr16, exts );
    vfr16 = new VolumeFormatReader<int16_t>;
    vfr16->attach( rc_ptr<ImageReader<int16_t> >( new Nifti1ImageReader<int16_t> ) );
    FormatDictionary<Volume<int16_t> >::registerFormat( "NIFTI-2", vfr16, exts );

    VolumeFormatReader<int32_t> *vfr32 = new VolumeFormatReader<int32_t>;
    vfr32->attach( rc_ptr<ImageReader<int32_t> >( new Nifti1ImageReader<int32_t> ) );
    FormatDictionary<Volume<int32_t> >::registerFormat( "NIFTI-1", vfr32, exts );
    vfr32 = new VolumeFormatReader<int32_t>;
    vfr32->attach( rc_ptr<ImageReader<int32_t> >( new Nifti1ImageReader<int32_t> ) );
    FormatDictionary<Volume<int32_t> >::registerFormat( "NIFTI-2", vfr32, exts );

    VolumeFormatReader<int64_t> *vfr64 = new VolumeFormatReader<int64_t>;
    vfr64->attach( rc_ptr<ImageReader<int64_t> >( new Nifti1ImageReader<int64_t> ) );
    FormatDictionary<Volume<int64_t> >::registerFormat( "NIFTI-1", vfr64, exts );
    vfr64 = new VolumeFormatReader<int64_t>;
    vfr64->attach( rc_ptr<ImageReader<int64_t> >( new Nifti1ImageReader<int64_t> ) );
    FormatDictionary<Volume<int64_t> >::registerFormat( "NIFTI-2", vfr64, exts );

    //--- UINT -----------------------------------------------------------------

    VolumeFormatReader<uint8_t> *vfru8 = new VolumeFormatReader<uint8_t>;
    vfru8->attach( rc_ptr<ImageReader<uint8_t> >( new Nifti1ImageReader<uint8_t> ) );
    FormatDictionary<Volume<uint8_t> >::registerFormat( "NIFTI-1", vfru8, exts );
    vfru8 = new VolumeFormatReader<uint8_t>;
    vfru8->attach( rc_ptr<ImageReader<uint8_t> >( new Nifti1ImageReader<uint8_t> ) );
    FormatDictionary<Volume<uint8_t> >::registerFormat( "NIFTI-2", vfru8, exts );

    VolumeFormatReader<uint16_t> *vfru16 = new VolumeFormatReader<uint16_t>;
    vfru16->attach( rc_ptr<ImageReader<uint16_t> >( new Nifti1ImageReader<uint16_t> ) );
    FormatDictionary<Volume<uint16_t> >::registerFormat( "NIFTI-1", vfru16, exts );
    vfru16 = new VolumeFormatReader<uint16_t>;
    vfru16->attach( rc_ptr<ImageReader<uint16_t> >( new Nifti1ImageReader<uint16_t> ) );
    FormatDictionary<Volume<uint16_t> >::registerFormat( "NIFTI-2", vfru16, exts );

    VolumeFormatReader<uint32_t> *vfru32 = new VolumeFormatReader<uint32_t>;
    vfru32->attach( rc_ptr<ImageReader<uint32_t> >( new Nifti1ImageReader<uint32_t> ) );
    FormatDictionary<Volume<uint32_t> >::registerFormat( "NIFTI-1", vfru32, exts );
    vfru32 = new VolumeFormatReader<uint32_t>;
    vfru32->attach( rc_ptr<ImageReader<uint32_t> >( new Nifti1ImageReader<uint32_t> ) );
    FormatDictionary<Volume<uint32_t> >::registerFormat( "NIFTI-2", vfru32, exts );

    VolumeFormatReader<uint64_t> *vfru64 = new VolumeFormatReader<uint64_t>;
    vfru64->attach( rc_ptr<ImageReader<uint64_t> >( new Nifti1ImageReader<uint64_t> ) );
    FormatDictionary<Volume<uint64_t> >::registerFormat( "NIFTI-1", vfru64, exts );
    vfru64 = new VolumeFormatReader<uint64_t>;
    vfru64->attach( rc_ptr<ImageReader<uint64_t> >( new Nifti1ImageReader<uint64_t> ) );
    FormatDictionary<Volume<uint64_t> >::registerFormat( "NIFTI-2", vfru64, exts );

    //--- FLOAT ----------------------------------------------------------------

    VolumeFormatReader<float> *vfrf = new VolumeFormatReader<float>;
    vfrf->attach( rc_ptr<ImageReader<float> >( new Nifti1ImageReader<float> ) );
    FormatDictionary<Volume<float> >::registerFormat( "NIFTI-1", vfrf, exts );
    vfrf = new VolumeFormatReader<float>;
    vfrf->attach( rc_ptr<ImageReader<float> >( new Nifti1ImageReader<float> ) );
    FormatDictionary<Volume<float> >::registerFormat( "NIFTI-2", vfrf, exts );

    VolumeFormatReader<double> *vfrd = new VolumeFormatReader<double>;
    vfrd->attach( rc_ptr<ImageReader<double> >( new Nifti1ImageReader<double> ) );
    FormatDictionary<Volume<double> >::registerFormat( "NIFTI-1", vfrd, exts );
    vfrd = new VolumeFormatReader<double>;
    vfrd->attach( rc_ptr<ImageReader<double> >( new Nifti1ImageReader<double> ) );
    FormatDictionary<Volume<double> >::registerFormat( "NIFTI-2", vfrd, exts );

    //--- RGB ------------------------------------------------------------------

    VolumeFormatReader<VoxelRGB> *vfr_rgb = new VolumeFormatReader<VoxelRGB>;
    vfr_rgb->attach( rc_ptr<ImageReader<VoxelRGB> >( new Nifti1ImageReader<VoxelRGB> ) );
    FormatDictionary<Volume<VoxelRGB> >::registerFormat( "NIFTI-1", vfr_rgb, exts );
    vfr_rgb = new VolumeFormatReader<VoxelRGB>;
    vfr_rgb->attach( rc_ptr<ImageReader<VoxelRGB> >( new Nifti1ImageReader<VoxelRGB> ) );
    FormatDictionary<Volume<VoxelRGB> >::registerFormat( "NIFTI-2", vfr_rgb, exts );

    VolumeFormatReader<VoxelRGBA> *vfr_rgba = new VolumeFormatReader<VoxelRGBA>;
    vfr_rgba->attach( rc_ptr<ImageReader<VoxelRGBA> >( new Nifti1ImageReader<VoxelRGBA> ) );
    FormatDictionary<Volume<VoxelRGBA> >::registerFormat( "NIFTI-1", vfr_rgba, exts );
    vfr_rgba = new VolumeFormatReader<VoxelRGBA>;
    vfr_rgba->attach( rc_ptr<ImageReader<VoxelRGBA> >( new Nifti1ImageReader<VoxelRGBA> ) );
    FormatDictionary<Volume<VoxelRGBA> >::registerFormat( "NIFTI-2", vfr_rgba, exts );

    VolumeFormatReader<VoxelHSV> *vfr_hsv = new VolumeFormatReader<VoxelHSV>;
    vfr_hsv->attach( rc_ptr<ImageReader<VoxelHSV> >( new Nifti1ImageReader<VoxelHSV> ) );
    FormatDictionary<Volume<VoxelHSV> >::registerFormat( "NIFTI-1", vfr_hsv, exts );
    vfr_hsv = new VolumeFormatReader<VoxelHSV>;
    vfr_hsv->attach( rc_ptr<ImageReader<VoxelHSV> >( new Nifti1ImageReader<VoxelHSV> ) );
    FormatDictionary<Volume<VoxelHSV> >::registerFormat( "NIFTI-2", vfr_hsv, exts );

    //--- COMPLEX -------------------------------------------------------------

    VolumeFormatReader<std::complex<float> > *vfr_cf = new VolumeFormatReader<std::complex<float> >;
    vfr_cf->attach( rc_ptr<ImageReader<std::complex<float> > >( new Nifti1ImageReader<std::complex<float> > ) );
    FormatDictionary<Volume<std::complex<float> > >::registerFormat( "NIFTI-1", vfr_cf, exts );
    vfr_cf = new VolumeFormatReader<std::complex<float> >;
    vfr_cf->attach( rc_ptr<ImageReader<std::complex<float> > >( new Nifti1ImageReader<std::complex<float> > ) );
    FormatDictionary<Volume<std::complex<float> > >::registerFormat( "NIFTI-2", vfr_cf, exts );

    VolumeFormatReader<std::complex<double> > *vfr_cd = new VolumeFormatReader<std::complex<double> >;
    vfr_cd->attach( rc_ptr<ImageReader<std::complex<double> > >( new Nifti1ImageReader<std::complex<double> > ) );
    FormatDictionary<Volume<std::complex<double> > >::registerFormat( "NIFTI-1", vfr_cd, exts );
    vfr_cd = new VolumeFormatReader<std::complex<double> >;
    vfr_cd->attach( rc_ptr<ImageReader<std::complex<double> > >( new Nifti1ImageReader<std::complex<double> > ) );
    FormatDictionary<Volume<std::complex<double> > >::registerFormat( "NIFTI-2", vfr_cd, exts );

    //==========================================================================
    //   V O L U M E   R E F
    //==========================================================================

    //--- INT ------------------------------------------------------------------

    VolumeRefFormatReader<int8_t> *rfr8 = new VolumeRefFormatReader<int8_t>;
    rfr8->attach( rc_ptr<ImageReader<int8_t> >( new Nifti1ImageReader<int8_t> ) );
    FormatDictionary<VolumeRef<int8_t> >::registerFormat( "NIFTI-1", rfr8, exts );
    rfr8 = new VolumeRefFormatReader<int8_t>;
    rfr8->attach( rc_ptr<ImageReader<int8_t> >( new Nifti1ImageReader<int8_t> ) );
    FormatDictionary<VolumeRef<int8_t> >::registerFormat( "NIFTI-2", rfr8, exts );

    VolumeRefFormatReader<int16_t> *rfr16 = new VolumeRefFormatReader<int16_t>;
    rfr16->attach( rc_ptr<ImageReader<int16_t> >( new Nifti1ImageReader<int16_t> ) );
    FormatDictionary<VolumeRef<int16_t> >::registerFormat( "NIFTI-1", rfr16, exts );
    rfr16 = new VolumeRefFormatReader<int16_t>;
    rfr16->attach( rc_ptr<ImageReader<int16_t> >( new Nifti1ImageReader<int16_t> ) );
    FormatDictionary<VolumeRef<int16_t> >::registerFormat( "NIFTI-2", rfr16, exts );

    VolumeRefFormatReader<int32_t> *rfr32 = new VolumeRefFormatReader<int32_t>;
    rfr32->attach( rc_ptr<ImageReader<int32_t> >( new Nifti1ImageReader<int32_t> ) );
    FormatDictionary<VolumeRef<int32_t> >::registerFormat( "NIFTI-1", rfr32, exts );
    rfr32 = new VolumeRefFormatReader<int32_t>;
    rfr32->attach( rc_ptr<ImageReader<int32_t> >( new Nifti1ImageReader<int32_t> ) );
    FormatDictionary<VolumeRef<int32_t> >::registerFormat( "NIFTI-2", rfr32, exts );

    VolumeRefFormatReader<int64_t> *rfr64 = new VolumeRefFormatReader<int64_t>;
    rfr64->attach( rc_ptr<ImageReader<int64_t> >( new Nifti1ImageReader<int64_t> ) );
    FormatDictionary<VolumeRef<int64_t> >::registerFormat( "NIFTI-1", rfr64, exts );
    rfr64 = new VolumeRefFormatReader<int64_t>;
    rfr64->attach( rc_ptr<ImageReader<int64_t> >( new Nifti1ImageReader<int64_t> ) );
    FormatDictionary<VolumeRef<int64_t> >::registerFormat( "NIFTI-2", rfr64, exts );

    //--- UINT -----------------------------------------------------------------

    VolumeRefFormatReader<uint8_t> *rfru8 = new VolumeRefFormatReader<uint8_t>;
    rfru8->attach( rc_ptr<ImageReader<uint8_t> >( new Nifti1ImageReader<uint8_t> ) );
    FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "NIFTI-1", rfru8, exts );
    rfru8 = new VolumeRefFormatReader<uint8_t>;
    rfru8->attach( rc_ptr<ImageReader<uint8_t> >( new Nifti1ImageReader<uint8_t> ) );
    FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "NIFTI-2", rfru8, exts );

    VolumeRefFormatReader<uint16_t> *rfru16 = new VolumeRefFormatReader<uint16_t>;
    rfru16->attach( rc_ptr<ImageReader<uint16_t> >( new Nifti1ImageReader<uint16_t> ) );
    FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "NIFTI-1", rfru16, exts );
    rfru16 = new VolumeRefFormatReader<uint16_t>;
    rfru16->attach( rc_ptr<ImageReader<uint16_t> >( new Nifti1ImageReader<uint16_t> ) );
    FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "NIFTI-2", rfru16, exts );

    VolumeRefFormatReader<uint32_t> *rfru32 = new VolumeRefFormatReader<uint32_t>;
    rfru32->attach( rc_ptr<ImageReader<uint32_t> >( new Nifti1ImageReader<uint32_t> ) );
    FormatDictionary<VolumeRef<uint32_t> >::registerFormat( "NIFTI-1", rfru32, exts );
    rfru32 = new VolumeRefFormatReader<uint32_t>;
    rfru32->attach( rc_ptr<ImageReader<uint32_t> >( new Nifti1ImageReader<uint32_t> ) );
    FormatDictionary<VolumeRef<uint32_t> >::registerFormat( "NIFTI-2", rfru32, exts );

    VolumeRefFormatReader<uint64_t> *rfru64 = new VolumeRefFormatReader<uint64_t>;
    rfru64->attach( rc_ptr<ImageReader<uint64_t> >( new Nifti1ImageReader<uint64_t> ) );
    FormatDictionary<VolumeRef<uint64_t> >::registerFormat( "NIFTI-1", rfru64, exts );
    rfru64 = new VolumeRefFormatReader<uint64_t>;
    rfru64->attach( rc_ptr<ImageReader<uint64_t> >( new Nifti1ImageReader<uint64_t> ) );
    FormatDictionary<VolumeRef<uint64_t> >::registerFormat( "NIFTI-2", rfru64, exts );

    //--- FLOAT ----------------------------------------------------------------

    VolumeRefFormatReader<float> *rfrf = new VolumeRefFormatReader<float>;
    rfrf->attach( rc_ptr<ImageReader<float> >( new Nifti1ImageReader<float> ) );
    FormatDictionary<VolumeRef<float> >::registerFormat( "NIFTI-1", rfrf, exts );
    rfrf = new VolumeRefFormatReader<float>;
    rfrf->attach( rc_ptr<ImageReader<float> >( new Nifti1ImageReader<float> ) );
    FormatDictionary<VolumeRef<float> >::registerFormat( "NIFTI-2", rfrf, exts );

    VolumeRefFormatReader<double> *rfrd = new VolumeRefFormatReader<double>;
    rfrd->attach( rc_ptr<ImageReader<double> >( new Nifti1ImageReader<double> ) );
    FormatDictionary<VolumeRef<double> >::registerFormat( "NIFTI-1", rfrd, exts );
    rfrd = new VolumeRefFormatReader<double>;
    rfrd->attach( rc_ptr<ImageReader<double> >( new Nifti1ImageReader<double> ) );
    FormatDictionary<VolumeRef<double> >::registerFormat( "NIFTI-2", rfrd, exts );

    //--- RGB ------------------------------------------------------------------

    VolumeRefFormatReader<VoxelRGB> *rfr_rgb = new VolumeRefFormatReader<VoxelRGB>;
    rfr_rgb->attach( rc_ptr<ImageReader<VoxelRGB> >( new Nifti1ImageReader<VoxelRGB> ) );
    FormatDictionary<VolumeRef<VoxelRGB> >::registerFormat( "NIFTI-1", rfr_rgb, exts );
    rfr_rgb = new VolumeRefFormatReader<VoxelRGB>;
    rfr_rgb->attach( rc_ptr<ImageReader<VoxelRGB> >( new Nifti1ImageReader<VoxelRGB> ) );
    FormatDictionary<VolumeRef<VoxelRGB> >::registerFormat( "NIFTI-2", rfr_rgb, exts );

    VolumeRefFormatReader<VoxelRGBA> *rfr_rgba = new VolumeRefFormatReader<VoxelRGBA>;
    rfr_rgba->attach( rc_ptr<ImageReader<VoxelRGBA> >( new Nifti1ImageReader<VoxelRGBA> ) );
    FormatDictionary<VolumeRef<VoxelRGBA> >::registerFormat( "NIFTI-1", rfr_rgba, exts );
    rfr_rgba = new VolumeRefFormatReader<VoxelRGBA>;
    rfr_rgba->attach( rc_ptr<ImageReader<VoxelRGBA> >( new Nifti1ImageReader<VoxelRGBA> ) );
    FormatDictionary<VolumeRef<VoxelRGBA> >::registerFormat( "NIFTI-2", rfr_rgba, exts );

    VolumeRefFormatReader<VoxelHSV> *rfr_hsv = new VolumeRefFormatReader<VoxelHSV>;
    rfr_hsv->attach( rc_ptr<ImageReader<VoxelHSV> >( new Nifti1ImageReader<VoxelHSV> ) );
    FormatDictionary<VolumeRef<VoxelHSV> >::registerFormat( "NIFTI-1", rfr_hsv, exts );
    rfr_hsv = new VolumeRefFormatReader<VoxelHSV>;
    rfr_hsv->attach( rc_ptr<ImageReader<VoxelHSV> >( new Nifti1ImageReader<VoxelHSV> ) );
    FormatDictionary<VolumeRef<VoxelHSV> >::registerFormat( "NIFTI-2", rfr_hsv, exts );

    //--- COMPLEX -------------------------------------------------------------

    VolumeRefFormatReader<std::complex<float> > *rfr_cf = new VolumeRefFormatReader<std::complex<float> >;
    rfr_cf->attach( rc_ptr<ImageReader<std::complex<float> > >( new Nifti1ImageReader<std::complex<float> > ) );
    FormatDictionary<VolumeRef<std::complex<float> > >::registerFormat( "NIFTI-1", rfr_cf, exts );
    rfr_cf = new VolumeRefFormatReader<std::complex<float> >;
    rfr_cf->attach( rc_ptr<ImageReader<std::complex<float> > >( new Nifti1ImageReader<std::complex<float> > ) );
    FormatDictionary<VolumeRef<std::complex<float> > >::registerFormat( "NIFTI-2", rfr_cf, exts );

    VolumeRefFormatReader<std::complex<double> > *rfr_cd = new VolumeRefFormatReader<std::complex<double> >;
    rfr_cd->attach( rc_ptr<ImageReader<std::complex<double> > >( new Nifti1ImageReader<std::complex<double> > ) );
    FormatDictionary<VolumeRef<std::complex<double> > >::registerFormat( "NIFTI-1", rfr_cd, exts );
    rfr_cd = new VolumeRefFormatReader<std::complex<double> >;
    rfr_cd->attach( rc_ptr<ImageReader<std::complex<double> > >( new Nifti1ImageReader<std::complex<double> > ) );
    FormatDictionary<VolumeRef<std::complex<double> > >::registerFormat( "NIFTI-2", rfr_cd, exts );


    ////////////////////////////////////////////////////////////////////////////
    ////                           W R I T E R                              ////
    ////////////////////////////////////////////////////////////////////////////

    //==========================================================================
    //   V O L U M E
    //==========================================================================

    //--- INT ------------------------------------------------------------------

    VolumeFormatWriter<int8_t> *vfw8 = new VolumeFormatWriter<int8_t>;
    vfw8->attach( rc_ptr<ImageWriter<int8_t> >( new Nifti1ImageWriter<int8_t> ) );
    FormatDictionary<Volume<int8_t> >::registerFormat( "NIFTI-1", vfw8, exts );

    VolumeFormatWriter<int16_t> *vfw16 = new VolumeFormatWriter<int16_t>;
    vfw16->attach( rc_ptr<ImageWriter<int16_t> >( new Nifti1ImageWriter<int16_t> ) );
    FormatDictionary<Volume<int16_t> >::registerFormat( "NIFTI-1", vfw16, exts );

    VolumeFormatWriter<int32_t> *vfw32 = new VolumeFormatWriter<int32_t>;
    vfw32->attach( rc_ptr<ImageWriter<int32_t> >( new Nifti1ImageWriter<int32_t> ) );
    FormatDictionary<Volume<int32_t> >::registerFormat( "NIFTI-1", vfw32, exts );

    VolumeFormatWriter<int64_t> *vfw64 = new VolumeFormatWriter<int64_t>;
    vfw64->attach( rc_ptr<ImageWriter<int64_t> >( new Nifti1ImageWriter<int64_t> ) );
    FormatDictionary<Volume<int64_t> >::registerFormat( "NIFTI-1", vfw64, exts );

    //--- UINT -----------------------------------------------------------------

    VolumeFormatWriter<uint8_t> *vfwu8 = new VolumeFormatWriter<uint8_t>;
    vfwu8->attach( rc_ptr<ImageWriter<uint8_t> >( new Nifti1ImageWriter<uint8_t> ) );
    FormatDictionary<Volume<uint8_t> >::registerFormat( "NIFTI-1", vfwu8, exts );

    VolumeFormatWriter<uint16_t> *vfwu16 = new VolumeFormatWriter<uint16_t>;
    vfwu16->attach( rc_ptr<ImageWriter<uint16_t> >( new Nifti1ImageWriter<uint16_t> ) );
    FormatDictionary<Volume<uint16_t> >::registerFormat( "NIFTI-1", vfwu16, exts );

    VolumeFormatWriter<uint32_t> *vfwu32 = new VolumeFormatWriter<uint32_t>;
    vfwu32->attach( rc_ptr<ImageWriter<uint32_t> >( new Nifti1ImageWriter<uint32_t> ) );
    FormatDictionary<Volume<uint32_t> >::registerFormat( "NIFTI-1", vfwu32, exts );

    VolumeFormatWriter<uint64_t> *vfwu64 = new VolumeFormatWriter<uint64_t>;
    vfwu64->attach( rc_ptr<ImageWriter<uint64_t> >( new Nifti1ImageWriter<uint64_t> ) );
    FormatDictionary<Volume<uint64_t> >::registerFormat( "NIFTI-1", vfwu64, exts );

    //--- FLOAT ----------------------------------------------------------------

    VolumeFormatWriter<float> *vfwf = new VolumeFormatWriter<float>;
    vfwf->attach( rc_ptr<ImageWriter<float> >( new Nifti1ImageWriter<float> ) );
    FormatDictionary<Volume<float> >::registerFormat( "NIFTI-1", vfwf, exts );

    VolumeFormatWriter<double> *vfwd = new VolumeFormatWriter<double>;
    vfwd->attach( rc_ptr<ImageWriter<double> >( new Nifti1ImageWriter<double> ) );
    FormatDictionary<Volume<double> >::registerFormat( "NIFTI-1", vfwd, exts );

    //--- RGB ------------------------------------------------------------------

    VolumeFormatWriter<VoxelRGB> *vfw_rgb = new VolumeFormatWriter<VoxelRGB>;
    vfw_rgb->attach( rc_ptr<ImageWriter<VoxelRGB> >( new Nifti1ImageWriter<VoxelRGB> ) );
    FormatDictionary<Volume<VoxelRGB> >::registerFormat( "NIFTI-1", vfw_rgb, exts );

    VolumeFormatWriter<VoxelRGBA> *vfw_rgba = new VolumeFormatWriter<VoxelRGBA>;
    vfw_rgba->attach( rc_ptr<ImageWriter<VoxelRGBA> >( new Nifti1ImageWriter<VoxelRGBA> ) );
    FormatDictionary<Volume<VoxelRGBA> >::registerFormat( "NIFTI-1", vfw_rgba, exts );

    VolumeFormatWriter<VoxelHSV> *vfw_hsv = new VolumeFormatWriter<VoxelHSV>;
    vfw_hsv->attach( rc_ptr<ImageWriter<VoxelHSV> >( new Nifti1ImageWriter<VoxelHSV> ) );
    FormatDictionary<Volume<VoxelHSV> >::registerFormat( "NIFTI-1", vfw_hsv, exts );

    //--- COMPLEX -------------------------------------------------------------

    VolumeFormatWriter<std::complex<float> > *vfw_cf = new VolumeFormatWriter<std::complex<float> >;
    vfw_cf->attach( rc_ptr<ImageWriter<std::complex<float> > >( new Nifti1ImageWriter<std::complex<float> > ) );
    FormatDictionary<Volume<std::complex<float> > >::registerFormat( "NIFTI-1", vfw_cf, exts );

    VolumeFormatWriter<std::complex<double> > *vfw_cd = new VolumeFormatWriter<std::complex<double> >;
    vfw_cd->attach( rc_ptr<ImageWriter<std::complex<double> > >( new Nifti1ImageWriter<std::complex<double> > ) );
    FormatDictionary<Volume<std::complex<double> > >::registerFormat( "NIFTI-1", vfw_cd, exts );

    //==========================================================================
    //   V O L U M E   R E F
    //==========================================================================

    //--- INT ------------------------------------------------------------------

    VolumeRefFormatWriter<int8_t> *rfw8 = new VolumeRefFormatWriter<int8_t>;
    rfw8->attach( rc_ptr<ImageWriter<int8_t> >( new Nifti1ImageWriter<int8_t> ) );
    FormatDictionary<VolumeRef<int8_t> >::registerFormat( "NIFTI-1", rfw8, exts );

    VolumeRefFormatWriter<int16_t> *rfw16 = new VolumeRefFormatWriter<int16_t>;
    rfw16->attach( rc_ptr<ImageWriter<int16_t> >( new Nifti1ImageWriter<int16_t> ) );
    FormatDictionary<VolumeRef<int16_t> >::registerFormat( "NIFTI-1", rfw16, exts );

    VolumeRefFormatWriter<int32_t> *rfw32 = new VolumeRefFormatWriter<int32_t>;
    rfw32->attach( rc_ptr<ImageWriter<int32_t> >( new Nifti1ImageWriter<int32_t> ) );
    FormatDictionary<VolumeRef<int32_t> >::registerFormat( "NIFTI-1", rfw32, exts );

    VolumeRefFormatWriter<int64_t> *rfw64 = new VolumeRefFormatWriter<int64_t>;
    rfw64->attach( rc_ptr<ImageWriter<int64_t> >( new Nifti1ImageWriter<int64_t> ) );
    FormatDictionary<VolumeRef<int64_t> >::registerFormat( "NIFTI-1", rfw64, exts );

    //--- UINT -----------------------------------------------------------------

    VolumeRefFormatWriter<uint8_t> *rfwu8 = new VolumeRefFormatWriter<uint8_t>;
    rfwu8->attach( rc_ptr<ImageWriter<uint8_t> >( new Nifti1ImageWriter<uint8_t> ) );
    FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "NIFTI-1", rfwu8, exts );

    VolumeRefFormatWriter<uint16_t> *rfwu16 = new VolumeRefFormatWriter<uint16_t>;
    rfwu16->attach( rc_ptr<ImageWriter<uint16_t> >( new Nifti1ImageWriter<uint16_t> ) );
    FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "NIFTI-1", rfwu16, exts );

    VolumeRefFormatWriter<uint32_t> *rfwu32 = new VolumeRefFormatWriter<uint32_t>;
    rfwu32->attach( rc_ptr<ImageWriter<uint32_t> >( new Nifti1ImageWriter<uint32_t> ) );
    FormatDictionary<VolumeRef<uint32_t> >::registerFormat( "NIFTI-1", rfwu32, exts );

    VolumeRefFormatWriter<uint64_t> *rfwu64 = new VolumeRefFormatWriter<uint64_t>;
    rfwu64->attach( rc_ptr<ImageWriter<uint64_t> >( new Nifti1ImageWriter<uint64_t> ) );
    FormatDictionary<VolumeRef<uint64_t> >::registerFormat( "NIFTI-1", rfwu64, exts );

    //--- FLOAT ----------------------------------------------------------------

    VolumeRefFormatWriter<float> *rfwf = new VolumeRefFormatWriter<float>;
    rfwf->attach( rc_ptr<ImageWriter<float> >( new Nifti1ImageWriter<float> ) );
    FormatDictionary<VolumeRef<float> >::registerFormat( "NIFTI-1", rfwf, exts );

    VolumeRefFormatWriter<double> *rfwd = new VolumeRefFormatWriter<double>;
    rfwd->attach( rc_ptr<ImageWriter<double> >( new Nifti1ImageWriter<double> ) );
    FormatDictionary<VolumeRef<double> >::registerFormat( "NIFTI-1", rfwd, exts );

    //--- RGB ----------------------------------------------------------------

    VolumeRefFormatWriter<VoxelRGB> *rfw_rgb = new VolumeRefFormatWriter<VoxelRGB>;
    rfw_rgb->attach( rc_ptr<ImageWriter<VoxelRGB> >( new Nifti1ImageWriter<VoxelRGB> ) );
    FormatDictionary<VolumeRef<VoxelRGB> >::registerFormat( "NIFTI-1", rfw_rgb, exts );

    VolumeRefFormatWriter<VoxelRGBA> *rfw_rgba = new VolumeRefFormatWriter<VoxelRGBA>;
    rfw_rgba->attach( rc_ptr<ImageWriter<VoxelRGBA> >( new Nifti1ImageWriter<VoxelRGBA> ) );
    FormatDictionary<VolumeRef<VoxelRGBA> >::registerFormat( "NIFTI-1", rfw_rgba, exts );

    VolumeRefFormatWriter<VoxelHSV> *rfw_hsv = new VolumeRefFormatWriter<VoxelHSV>;
    rfw_hsv->attach( rc_ptr<ImageWriter<VoxelHSV> >( new Nifti1ImageWriter<VoxelHSV> ) );
    FormatDictionary<VolumeRef<VoxelHSV> >::registerFormat( "NIFTI-1", rfw_hsv, exts );

    //--- COMPLEX -------------------------------------------------------------

    VolumeRefFormatWriter<std::complex<float> > *rfw_cf = new VolumeRefFormatWriter<std::complex<float> >;
    rfw_cf->attach( rc_ptr<ImageWriter<std::complex<float> > >( new Nifti1ImageWriter<std::complex<float> > ) );
    FormatDictionary<VolumeRef<std::complex<float> > >::registerFormat( "NIFTI-1", rfw_cf, exts );

    VolumeRefFormatWriter<std::complex<double> > *rfw_cd = new VolumeRefFormatWriter<std::complex<double> >;
    rfw_cd->attach( rc_ptr<ImageWriter<std::complex<double> > >( new Nifti1ImageWriter<std::complex<double> > ) );
    FormatDictionary<VolumeRef<std::complex<double> > >::registerFormat( "NIFTI-1", rfw_cd, exts );

}


NiftiPlugin::~NiftiPlugin()
{
}


string NiftiPlugin::name() const
{
  return string("NIFTI CARTO");
}


bool NiftiPlugin::noop()
{
  return true;
}

