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
#include <cartodata/plugin/mincplugin.h>
#include <soma-io/image/mincimagereader.h>
#include <soma-io/image/mincimagewriter.h>
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

namespace
{

  bool initminc()
  {
    new MincPlugin;
    return true;
  }

  bool mincinit __attribute__((unused)) = initminc();

}

MincPlugin::MincPlugin() : Plugin()
{
    vector<string>  exts;
    exts.push_back( "mnc" );
    exts.push_back( "mnc.gz" );
#ifdef MINC_MGH_SUPPORT
    vector<string>  exts2;
    exts2.push_back( "mgh" ); // Freesurfer format
    exts2.push_back( "mgz" ); // compressed Freesurfer format
#endif

    ////////////////////////////////////////////////////////////////////////////
    ////                           R E A D E R                              ////
    ////////////////////////////////////////////////////////////////////////////

    //==========================================================================
    //   V O L U M E
    //==========================================================================

    //--- INT ------------------------------------------------------------------

    VolumeFormatReader<int8_t> *vfr8 = new VolumeFormatReader<int8_t>;
    vfr8->attach( rc_ptr<ImageReader<int8_t> >( new MincImageReader<int8_t> ) );
    FormatDictionary<Volume<int8_t> >::registerFormat( "MINC", vfr8, exts );

    VolumeFormatReader<int16_t> *vfr16 = new VolumeFormatReader<int16_t>;
    vfr16->attach( rc_ptr<ImageReader<int16_t> >( new MincImageReader<int16_t> ) );
    FormatDictionary<Volume<int16_t> >::registerFormat( "MINC", vfr16, exts );

    VolumeFormatReader<int32_t> *vfr32 = new VolumeFormatReader<int32_t>;
    vfr32->attach( rc_ptr<ImageReader<int32_t> >( new MincImageReader<int32_t> ) );
    FormatDictionary<Volume<int32_t> >::registerFormat( "MINC", vfr32, exts );

    VolumeFormatReader<int64_t> *vfr64 = new VolumeFormatReader<int64_t>;
    vfr64->attach( rc_ptr<ImageReader<int64_t> >(
      new MincImageReader<int64_t> ) );
    FormatDictionary<Volume<int64_t> >::registerFormat( "MINC", vfr64, exts );

#ifdef MINC_MGH_SUPPORT
    vfr8 = new VolumeFormatReader<int8_t>;
    vfr8->attach( rc_ptr<ImageReader<int8_t> >(
      new FreeSurferMincImageReader<int8_t> ) );
    FormatDictionary<Volume<int8_t> >::registerFormat( "FREESURFER-MINC",
                                                       vfr8, exts2 );

    vfr16 = new VolumeFormatReader<int16_t>;
    vfr16->attach( rc_ptr<ImageReader<int16_t> >(
      new FreeSurferMincImageReader<int16_t> ) );
    FormatDictionary<Volume<int16_t> >::registerFormat( "FREESURFER-MINC",
                                                        vfr16, exts2 );

    vfr32 = new VolumeFormatReader<int32_t>;
    vfr32->attach( rc_ptr<ImageReader<int32_t> >(
      new FreeSurferMincImageReader<int32_t> ) );
    FormatDictionary<Volume<int32_t> >::registerFormat( "FREESURFER-MINC",
                                                        vfr32, exts2 );

    vfr64 = new VolumeFormatReader<int64_t>;
    vfr64->attach( rc_ptr<ImageReader<int64_t> >(
      new FreeSurferMincImageReader<int64_t> ) );
    FormatDictionary<Volume<int64_t> >::registerFormat( "FREESURFER-MINC",
                                                        vfr64, exts2 );
#endif

    //--- UINT -----------------------------------------------------------------

    VolumeFormatReader<uint8_t> *vfru8 = new VolumeFormatReader<uint8_t>;
    vfru8->attach( rc_ptr<ImageReader<uint8_t> >( new MincImageReader<uint8_t> ) );
    FormatDictionary<Volume<uint8_t> >::registerFormat( "MINC", vfru8, exts );

    VolumeFormatReader<uint16_t> *vfru16 = new VolumeFormatReader<uint16_t>;
    vfru16->attach( rc_ptr<ImageReader<uint16_t> >( new MincImageReader<uint16_t> ) );
    FormatDictionary<Volume<uint16_t> >::registerFormat( "MINC", vfru16, exts );

    VolumeFormatReader<uint32_t> *vfru32 = new VolumeFormatReader<uint32_t>;
    vfru32->attach( rc_ptr<ImageReader<uint32_t> >( new MincImageReader<uint32_t> ) );
    FormatDictionary<Volume<uint32_t> >::registerFormat( "MINC", vfru32, exts );

    VolumeFormatReader<uint64_t> *vfru64 = new VolumeFormatReader<uint64_t>;
    vfru64->attach( rc_ptr<ImageReader<uint64_t> >( new MincImageReader<uint64_t> ) );
    FormatDictionary<Volume<uint64_t> >::registerFormat( "MINC", vfru64, exts );

#ifdef MINC_MGH_SUPPORT
    vfru8 = new VolumeFormatReader<uint8_t>;
    vfru8->attach( rc_ptr<ImageReader<uint8_t> >(
      new FreeSurferMincImageReader<uint8_t> ) );
    FormatDictionary<Volume<uint8_t> >::registerFormat( "FREESURFER-MINC",
                                                        vfru8, exts2 );

    vfru16 = new VolumeFormatReader<uint16_t>;
    vfru16->attach( rc_ptr<ImageReader<uint16_t> >(
      new FreeSurferMincImageReader<uint16_t> ) );
    FormatDictionary<Volume<uint16_t> >::registerFormat( "FREESURFER-MINC",
                                                         vfru16, exts2 );

    vfru32 = new VolumeFormatReader<uint32_t>;
    vfru32->attach( rc_ptr<ImageReader<uint32_t> >(
      new FreeSurferMincImageReader<uint32_t> ) );
    FormatDictionary<Volume<uint32_t> >::registerFormat( "FREESURFER-MINC",
                                                         vfru32, exts2 );

    vfru64 = new VolumeFormatReader<uint64_t>;
    vfru64->attach( rc_ptr<ImageReader<uint64_t> >(
      new FreeSurferMincImageReader<uint64_t> ) );
    FormatDictionary<Volume<uint64_t> >::registerFormat( "FREESURFER-MINC",
                                                         vfru64, exts2 );
#endif

    //--- FLOAT ----------------------------------------------------------------

    VolumeFormatReader<float> *vfrf = new VolumeFormatReader<float>;
    vfrf->attach( rc_ptr<ImageReader<float> >( new MincImageReader<float> ) );
    FormatDictionary<Volume<float> >::registerFormat( "MINC", vfrf, exts );

    VolumeFormatReader<double> *vfrd = new VolumeFormatReader<double>;
    vfrd->attach( rc_ptr<ImageReader<double> >( new MincImageReader<double> ) );
    FormatDictionary<Volume<double> >::registerFormat( "MINC", vfrd, exts );

#ifdef MINC_MGH_SUPPORT
    vfrf = new VolumeFormatReader<float>;
    vfrf->attach( rc_ptr<ImageReader<float> >(
      new FreeSurferMincImageReader<float> ) );
    FormatDictionary<Volume<float> >::registerFormat( "FREESURFER-MINC",
                                                      vfrf, exts2 );

    vfrd = new VolumeFormatReader<double>;
    vfrd->attach( rc_ptr<ImageReader<double> >(
      new FreeSurferMincImageReader<double> ) );
    FormatDictionary<Volume<double> >::registerFormat( "FREESURFER-MINC",
                                                       vfrd, exts2 );
#endif

    //==========================================================================
    //   V O L U M E   R E F
    //==========================================================================

    //--- INT ------------------------------------------------------------------

    VolumeRefFormatReader<int8_t> *rfr8 = new VolumeRefFormatReader<int8_t>;
    rfr8->attach( rc_ptr<ImageReader<int8_t> >( new MincImageReader<int8_t> ) );
    FormatDictionary<VolumeRef<int8_t> >::registerFormat( "MINC", rfr8, exts );

    VolumeRefFormatReader<int16_t> *rfr16 = new VolumeRefFormatReader<int16_t>;
    rfr16->attach( rc_ptr<ImageReader<int16_t> >( new MincImageReader<int16_t> ) );
    FormatDictionary<VolumeRef<int16_t> >::registerFormat( "MINC", rfr16, exts );

    VolumeRefFormatReader<int32_t> *rfr32 = new VolumeRefFormatReader<int32_t>;
    rfr32->attach( rc_ptr<ImageReader<int32_t> >( new MincImageReader<int32_t> ) );
    FormatDictionary<VolumeRef<int32_t> >::registerFormat( "MINC", rfr32, exts );

    VolumeRefFormatReader<int64_t> *rfr64 = new VolumeRefFormatReader<int64_t>;
    rfr64->attach( rc_ptr<ImageReader<int64_t> >( new MincImageReader<int64_t> ) );
    FormatDictionary<VolumeRef<int64_t> >::registerFormat( "MINC", rfr64, exts );

#ifdef MINC_MGH_SUPPORT
    rfr8 = new VolumeRefFormatReader<int8_t>;
    rfr8->attach( rc_ptr<ImageReader<int8_t> >(
      new FreeSurferMincImageReader<int8_t> ) );
    FormatDictionary<VolumeRef<int8_t> >::registerFormat( "FREESURFER-MINC",
                                                          rfr8, exts2 );

    rfr16 = new VolumeRefFormatReader<int16_t>;
    rfr16->attach( rc_ptr<ImageReader<int16_t> >(
      new FreeSurferMincImageReader<int16_t> ) );
    FormatDictionary<VolumeRef<int16_t> >::registerFormat( "FREESURFER-MINC",
                                                           rfr16, exts2 );

    rfr32 = new VolumeRefFormatReader<int32_t>;
    rfr32->attach( rc_ptr<ImageReader<int32_t> >(
      new FreeSurferMincImageReader<int32_t> ) );
    FormatDictionary<VolumeRef<int32_t> >::registerFormat( "FREESURFER-MINC",
                                                           rfr32, exts2 );

    rfr64 = new VolumeRefFormatReader<int64_t>;
    rfr64->attach( rc_ptr<ImageReader<int64_t> >(
      new FreeSurferMincImageReader<int64_t> ) );
    FormatDictionary<VolumeRef<int64_t> >::registerFormat( "FREESURFER-MINC",
                                                           rfr64, exts2 );
#endif

    //--- UINT -----------------------------------------------------------------

    VolumeRefFormatReader<uint8_t> *rfru8 = new VolumeRefFormatReader<uint8_t>;
    rfru8->attach( rc_ptr<ImageReader<uint8_t> >( new MincImageReader<uint8_t> ) );
    FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "MINC", rfru8, exts );

    VolumeRefFormatReader<uint16_t> *rfru16 = new VolumeRefFormatReader<uint16_t>;
    rfru16->attach( rc_ptr<ImageReader<uint16_t> >( new MincImageReader<uint16_t> ) );
    FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "MINC", rfru16, exts );

    VolumeRefFormatReader<uint32_t> *rfru32 = new VolumeRefFormatReader<uint32_t>;
    rfru32->attach( rc_ptr<ImageReader<uint32_t> >( new MincImageReader<uint32_t> ) );
    FormatDictionary<VolumeRef<uint32_t> >::registerFormat( "MINC", rfru32, exts );

    VolumeRefFormatReader<uint64_t> *rfru64 = new VolumeRefFormatReader<uint64_t>;
    rfru64->attach( rc_ptr<ImageReader<uint64_t> >( new MincImageReader<uint64_t> ) );
    FormatDictionary<VolumeRef<uint64_t> >::registerFormat( "MINC", rfru64, exts );

#ifdef MINC_MGH_SUPPORT
    rfru8 = new VolumeRefFormatReader<uint8_t>;
    rfru8->attach( rc_ptr<ImageReader<uint8_t> >(
      new FreeSurferMincImageReader<uint8_t> ) );
    FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "FREESURFER-MINC",
                                                           rfru8, exts2 );

    rfru16 = new VolumeRefFormatReader<uint16_t>;
    rfru16->attach( rc_ptr<ImageReader<uint16_t> >(
      new FreeSurferMincImageReader<uint16_t> ) );
    FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "FREESURFER-MINC",
                                                            rfru16, exts2 );

    rfru32 = new VolumeRefFormatReader<uint32_t>;
    rfru32->attach( rc_ptr<ImageReader<uint32_t> >(
      new FreeSurferMincImageReader<uint32_t> ) );
    FormatDictionary<VolumeRef<uint32_t> >::registerFormat( "FREESURFER-MINC",
                                                            rfru32, exts2 );

    rfru64 = new VolumeRefFormatReader<uint64_t>;
    rfru64->attach( rc_ptr<ImageReader<uint64_t> >(
      new FreeSurferMincImageReader<uint64_t> ) );
    FormatDictionary<VolumeRef<uint64_t> >::registerFormat( "FREESURFER-MINC",
                                                            rfru64, exts2 );
#endif

    //--- FLOAT ----------------------------------------------------------------

    VolumeRefFormatReader<float> *rfrf = new VolumeRefFormatReader<float>;
    rfrf->attach( rc_ptr<ImageReader<float> >( new MincImageReader<float> ) );
    FormatDictionary<VolumeRef<float> >::registerFormat( "MINC", rfrf, exts );

    VolumeRefFormatReader<double> *rfrd = new VolumeRefFormatReader<double>;
    rfrd->attach( rc_ptr<ImageReader<double> >( new MincImageReader<double> ) );
    FormatDictionary<VolumeRef<double> >::registerFormat( "MINC", rfrd, exts );

#ifdef MINC_MGH_SUPPORT
    rfrf = new VolumeRefFormatReader<float>;
    rfrf->attach( rc_ptr<ImageReader<float> >(
      new FreeSurferMincImageReader<float> ) );
    FormatDictionary<VolumeRef<float> >::registerFormat( "FREESURFER-MINC",
                                                         rfrf, exts2 );

    rfrd = new VolumeRefFormatReader<double>;
    rfrd->attach( rc_ptr<ImageReader<double> >(
      new FreeSurferMincImageReader<double> ) );
    FormatDictionary<VolumeRef<double> >::registerFormat( "FREESURFER-MINC",
                                                          rfrd, exts2 );
#endif

    ////////////////////////////////////////////////////////////////////////////
    ////                           W R I T E R                              ////
    ////////////////////////////////////////////////////////////////////////////

//     exts.resize(2);
//     exts[0] = "mnc";
//     exts[1] = "mnc.gz";
// #ifdef MINC_MGH_SUPPORT
//     exts2.clear();
//     exts2.push_back( "mgh" ); // Freesurfer format
//     exts2.push_back( "mgz" ); // compressed Freesurfer format
// #endif

    //==========================================================================
    //   V O L U M E
    //==========================================================================

    //--- INT ------------------------------------------------------------------

    VolumeFormatWriter<int8_t> *vfw8 = new VolumeFormatWriter<int8_t>;
    vfw8->attach( rc_ptr<ImageWriter<int8_t> >( new MincImageWriter<int8_t> ) );
    FormatDictionary<Volume<int8_t> >::registerFormat( "MINC", vfw8, exts );

    VolumeFormatWriter<int16_t> *vfw16 = new VolumeFormatWriter<int16_t>;
    vfw16->attach( rc_ptr<ImageWriter<int16_t> >( new MincImageWriter<int16_t> ) );
    FormatDictionary<Volume<int16_t> >::registerFormat( "MINC", vfw16, exts );

    VolumeFormatWriter<int32_t> *vfw32 = new VolumeFormatWriter<int32_t>;
    vfw32->attach( rc_ptr<ImageWriter<int32_t> >( new MincImageWriter<int32_t> ) );
    FormatDictionary<Volume<int32_t> >::registerFormat( "MINC", vfw32, exts );

    VolumeFormatWriter<int64_t> *vfw64 = new VolumeFormatWriter<int64_t>;
    vfw64->attach( rc_ptr<ImageWriter<int64_t> >( new MincImageWriter<int64_t> ) );
    FormatDictionary<Volume<int64_t> >::registerFormat( "MINC", vfw64, exts );

    //--- UINT -----------------------------------------------------------------

    VolumeFormatWriter<uint8_t> *vfwu8 = new VolumeFormatWriter<uint8_t>;
    vfwu8->attach( rc_ptr<ImageWriter<uint8_t> >( new MincImageWriter<uint8_t> ) );
    FormatDictionary<Volume<uint8_t> >::registerFormat( "MINC", vfwu8, exts );

    VolumeFormatWriter<uint16_t> *vfwu16 = new VolumeFormatWriter<uint16_t>;
    vfwu16->attach( rc_ptr<ImageWriter<uint16_t> >( new MincImageWriter<uint16_t> ) );
    FormatDictionary<Volume<uint16_t> >::registerFormat( "MINC", vfwu16, exts );

    VolumeFormatWriter<uint32_t> *vfwu32 = new VolumeFormatWriter<uint32_t>;
    vfwu32->attach( rc_ptr<ImageWriter<uint32_t> >( new MincImageWriter<uint32_t> ) );
    FormatDictionary<Volume<uint32_t> >::registerFormat( "MINC", vfwu32, exts );

    VolumeFormatWriter<uint64_t> *vfwu64 = new VolumeFormatWriter<uint64_t>;
    vfwu64->attach( rc_ptr<ImageWriter<uint64_t> >( new MincImageWriter<uint64_t> ) );
    FormatDictionary<Volume<uint64_t> >::registerFormat( "MINC", vfwu64, exts );

    //--- FLOAT ----------------------------------------------------------------

    VolumeFormatWriter<float> *vfwf = new VolumeFormatWriter<float>;
    vfwf->attach( rc_ptr<ImageWriter<float> >( new MincImageWriter<float> ) );
    FormatDictionary<Volume<float> >::registerFormat( "MINC", vfwf, exts );

    VolumeFormatWriter<double> *vfwd = new VolumeFormatWriter<double>;
    vfwd->attach( rc_ptr<ImageWriter<double> >( new MincImageWriter<double> ) );
    FormatDictionary<Volume<double> >::registerFormat( "MINC", vfwd, exts );

    //==========================================================================
    //   V O L U M E   R E F
    //==========================================================================

    //--- INT ------------------------------------------------------------------

    VolumeRefFormatWriter<int8_t> *rfw8 = new VolumeRefFormatWriter<int8_t>;
    rfw8->attach( rc_ptr<ImageWriter<int8_t> >( new MincImageWriter<int8_t> ) );
    FormatDictionary<VolumeRef<int8_t> >::registerFormat( "MINC", rfw8, exts );

    VolumeRefFormatWriter<int16_t> *rfw16 = new VolumeRefFormatWriter<int16_t>;
    rfw16->attach( rc_ptr<ImageWriter<int16_t> >( new MincImageWriter<int16_t> ) );
    FormatDictionary<VolumeRef<int16_t> >::registerFormat( "MINC", rfw16, exts );

    VolumeRefFormatWriter<int32_t> *rfw32 = new VolumeRefFormatWriter<int32_t>;
    rfw32->attach( rc_ptr<ImageWriter<int32_t> >( new MincImageWriter<int32_t> ) );
    FormatDictionary<VolumeRef<int32_t> >::registerFormat( "MINC", rfw32, exts );

    VolumeRefFormatWriter<int64_t> *rfw64 = new VolumeRefFormatWriter<int64_t>;
    rfw64->attach( rc_ptr<ImageWriter<int64_t> >( new MincImageWriter<int64_t> ) );
    FormatDictionary<VolumeRef<int64_t> >::registerFormat( "MINC", rfw64, exts );

    //--- UINT -----------------------------------------------------------------

    VolumeRefFormatWriter<uint8_t> *rfwu8 = new VolumeRefFormatWriter<uint8_t>;
    rfwu8->attach( rc_ptr<ImageWriter<uint8_t> >( new MincImageWriter<uint8_t> ) );
    FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "MINC", rfwu8, exts );

    VolumeRefFormatWriter<uint16_t> *rfwu16 = new VolumeRefFormatWriter<uint16_t>;
    rfwu16->attach( rc_ptr<ImageWriter<uint16_t> >( new MincImageWriter<uint16_t> ) );
    FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "MINC", rfwu16, exts );

    VolumeRefFormatWriter<uint32_t> *rfwu32 = new VolumeRefFormatWriter<uint32_t>;
    rfwu32->attach( rc_ptr<ImageWriter<uint32_t> >( new MincImageWriter<uint32_t> ) );
    FormatDictionary<VolumeRef<uint32_t> >::registerFormat( "MINC", rfwu32, exts );

    VolumeRefFormatWriter<uint64_t> *rfwu64 = new VolumeRefFormatWriter<uint64_t>;
    rfwu64->attach( rc_ptr<ImageWriter<uint64_t> >( new MincImageWriter<uint64_t> ) );
    FormatDictionary<VolumeRef<uint64_t> >::registerFormat( "MINC", rfwu64, exts );

    //--- FLOAT ----------------------------------------------------------------

    VolumeRefFormatWriter<float> *rfwf = new VolumeRefFormatWriter<float>;
    rfwf->attach( rc_ptr<ImageWriter<float> >( new MincImageWriter<float> ) );
    FormatDictionary<VolumeRef<float> >::registerFormat( "MINC", rfwf, exts );

    VolumeRefFormatWriter<double> *rfwd = new VolumeRefFormatWriter<double>;
    rfwd->attach( rc_ptr<ImageWriter<double> >( new MincImageWriter<double> ) );
    FormatDictionary<VolumeRef<double> >::registerFormat( "MINC", rfwd, exts );

}


MincPlugin::~MincPlugin()
{
}


string MincPlugin::name() const
{
  return string("MINC CARTO");
}


bool MincPlugin::noop()
{
  return true;
}

