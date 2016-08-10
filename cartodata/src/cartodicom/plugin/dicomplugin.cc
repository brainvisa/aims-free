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
#include <cartodata/plugin/dicomplugin.h>
#include <soma-io/image/dicomimagereader.h>
#include <soma-io/image/dicomimagewriter.h>
//--- cartodata ----------------------------------------------------------------
#include <cartodata/io/volumeformatreader.h>
#include <cartodata/io/volumeformatwriter.h>
//--- soma-io ------------------------------------------------------------------
#include <soma-io/io/formatdictionary.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/type/types.h>
#include <cartobase/type/voxelrgb.h>
//--- system -------------------------------------------------------------------
#include <vector>
#include <string>
//------------------------------------------------------------------------------

using namespace cartodata;
using namespace soma;
using namespace carto;
using namespace std;

namespace
{

  bool initdicom()
  {
    new DicomPlugin;
    return true;
  }

  bool dicominit = initdicom();

}

DicomPlugin::DicomPlugin() : Plugin()
{
    vector<string>  exts(3);
    exts[0] = "dcm";
    exts[1] = "dic";
    exts[2] = "";
    
    ////////////////////////////////////////////////////////////////////////////
    ////                           R E A D E R                              ////
    ////////////////////////////////////////////////////////////////////////////
    
    //==========================================================================
    //   V O L U M E
    //==========================================================================
    
    //--- RGB ------------------------------------------------------------------
    
    VolumeFormatReader<VoxelRGB> *vfr_rgb = new VolumeFormatReader<VoxelRGB>;
    vfr_rgb->attach( rc_ptr<ImageReader<VoxelRGB> >( new DicomImageReader<VoxelRGB> ) );
    FormatDictionary<Volume<VoxelRGB> >::registerFormat( "DICOM", vfr_rgb, exts );
    
    //--- INT ------------------------------------------------------------------
    
    VolumeFormatReader<int8_t> *vfr8 = new VolumeFormatReader<int8_t>;
    vfr8->attach( rc_ptr<ImageReader<int8_t> >( new DicomImageReader<int8_t> ) );
    FormatDictionary<Volume<int8_t> >::registerFormat( "DICOM", vfr8, exts );
    
    VolumeFormatReader<int16_t> *vfr16 = new VolumeFormatReader<int16_t>;
    vfr16->attach( rc_ptr<ImageReader<int16_t> >( new DicomImageReader<int16_t> ) );
    FormatDictionary<Volume<int16_t> >::registerFormat( "DICOM", vfr16, exts );
    
    //--- UINT -----------------------------------------------------------------
    
    VolumeFormatReader<uint8_t> *vfru8 = new VolumeFormatReader<uint8_t>;
    vfru8->attach( rc_ptr<ImageReader<uint8_t> >( new DicomImageReader<uint8_t> ) );
    FormatDictionary<Volume<uint8_t> >::registerFormat( "DICOM", vfru8, exts );
    
    VolumeFormatReader<uint16_t> *vfru16 = new VolumeFormatReader<uint16_t>;
    vfru16->attach( rc_ptr<ImageReader<uint16_t> >( new DicomImageReader<uint16_t> ) );
    FormatDictionary<Volume<uint16_t> >::registerFormat( "DICOM", vfru16, exts );
    
    //--- FLOAT ----------------------------------------------------------------
    
    VolumeFormatReader<float> *vfrf = new VolumeFormatReader<float>;
    vfrf->attach( rc_ptr<ImageReader<float> >( new DicomImageReader<float> ) );
    FormatDictionary<Volume<float> >::registerFormat( "DICOM", vfrf, exts );
    
    //==========================================================================
    //   V O L U M E   R E F
    //==========================================================================

    //--- RGB ------------------------------------------------------------------
    
    VolumeRefFormatReader<VoxelRGB> *rfr_rgb = new VolumeRefFormatReader<VoxelRGB>;
    rfr_rgb->attach( rc_ptr<ImageReader<VoxelRGB> >( new DicomImageReader<VoxelRGB> ) );
    FormatDictionary<VolumeRef<VoxelRGB> >::registerFormat( "DICOM", rfr_rgb, exts );
    
    //--- INT ------------------------------------------------------------------
    
    VolumeRefFormatReader<int8_t> *rfr8 = new VolumeRefFormatReader<int8_t>;
    rfr8->attach( rc_ptr<ImageReader<int8_t> >( new DicomImageReader<int8_t> ) );
    FormatDictionary<VolumeRef<int8_t> >::registerFormat( "DICOM", rfr8, exts );
    
    VolumeRefFormatReader<int16_t> *rfr16 = new VolumeRefFormatReader<int16_t>;
    rfr16->attach( rc_ptr<ImageReader<int16_t> >( new DicomImageReader<int16_t> ) );
    FormatDictionary<VolumeRef<int16_t> >::registerFormat( "DICOM", rfr16, exts );
    
    //--- UINT -----------------------------------------------------------------
    
    VolumeRefFormatReader<uint8_t> *rfru8 = new VolumeRefFormatReader<uint8_t>;
    rfru8->attach( rc_ptr<ImageReader<uint8_t> >( new DicomImageReader<uint8_t> ) );
    FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "DICOM", rfru8, exts );
    
    VolumeRefFormatReader<uint16_t> *rfru16 = new VolumeRefFormatReader<uint16_t>;
    rfru16->attach( rc_ptr<ImageReader<uint16_t> >( new DicomImageReader<uint16_t> ) );
    FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "DICOM", rfru16, exts );
    
    //--- FLOAT ----------------------------------------------------------------
    
    VolumeRefFormatReader<float> *rfrf = new VolumeRefFormatReader<float>;
    rfrf->attach( rc_ptr<ImageReader<float> >( new DicomImageReader<float> ) );
    FormatDictionary<VolumeRef<float> >::registerFormat( "DICOM", rfrf, exts );


    ////////////////////////////////////////////////////////////////////////////
    ////                           W R I T E R                              ////
    ////////////////////////////////////////////////////////////////////////////

    exts.resize(1);
    exts[0] = "dcm";
    
    //==========================================================================
    //   V O L U M E
    //==========================================================================
    
    //--- RGB ------------------------------------------------------------------
    
    VolumeFormatWriter<VoxelRGB> *vfw_rgb = new VolumeFormatWriter<VoxelRGB>;
    vfw_rgb->attach( rc_ptr<ImageWriter<VoxelRGB> >( new DicomImageWriter<VoxelRGB> ) );
    FormatDictionary<Volume<VoxelRGB> >::registerFormat( "DICOM", vfw_rgb, exts );
    
    //--- INT ------------------------------------------------------------------
    
    VolumeFormatWriter<int8_t> *vfw8 = new VolumeFormatWriter<int8_t>;
    vfw8->attach( rc_ptr<ImageWriter<int8_t> >( new DicomImageWriter<int8_t> ) );
    FormatDictionary<Volume<int8_t> >::registerFormat( "DICOM", vfw8, exts );
    
    VolumeFormatWriter<int16_t> *vfw16 = new VolumeFormatWriter<int16_t>;
    vfw16->attach( rc_ptr<ImageWriter<int16_t> >( new DicomImageWriter<int16_t> ) );
    FormatDictionary<Volume<int16_t> >::registerFormat( "DICOM", vfw16, exts );
    
    //--- UINT -----------------------------------------------------------------
    
    VolumeFormatWriter<uint8_t> *vfwu8 = new VolumeFormatWriter<uint8_t>;
    vfwu8->attach( rc_ptr<ImageWriter<uint8_t> >( new DicomImageWriter<uint8_t> ) );
    FormatDictionary<Volume<uint8_t> >::registerFormat( "DICOM", vfwu8, exts );
    
    VolumeFormatWriter<uint16_t> *vfwu16 = new VolumeFormatWriter<uint16_t>;
    vfwu16->attach( rc_ptr<ImageWriter<uint16_t> >( new DicomImageWriter<uint16_t> ) );
    FormatDictionary<Volume<uint16_t> >::registerFormat( "DICOM", vfwu16, exts );
    
    //==========================================================================
    //   V O L U M E   R E F
    //==========================================================================

    //--- RGB ------------------------------------------------------------------
    
    VolumeRefFormatWriter<VoxelRGB> *rfw_rgb = new VolumeRefFormatWriter<VoxelRGB>;
    rfw_rgb->attach( rc_ptr<ImageWriter<VoxelRGB> >( new DicomImageWriter<VoxelRGB> ) );
    FormatDictionary<VolumeRef<VoxelRGB> >::registerFormat( "DICOM", rfw_rgb, exts );
    
    //--- INT ------------------------------------------------------------------
    
    VolumeRefFormatWriter<int8_t> *rfw8 = new VolumeRefFormatWriter<int8_t>;
    rfw8->attach( rc_ptr<ImageWriter<int8_t> >( new DicomImageWriter<int8_t> ) );
    FormatDictionary<VolumeRef<int8_t> >::registerFormat( "DICOM", rfw8, exts );
    
    VolumeRefFormatWriter<int16_t> *rfw16 = new VolumeRefFormatWriter<int16_t>;
    rfw16->attach( rc_ptr<ImageWriter<int16_t> >( new DicomImageWriter<int16_t> ) );
    FormatDictionary<VolumeRef<int16_t> >::registerFormat( "DICOM", rfw16, exts );
    
    //--- UINT -----------------------------------------------------------------
    
    VolumeRefFormatWriter<uint8_t> *rfwu8 = new VolumeRefFormatWriter<uint8_t>;
    rfwu8->attach( rc_ptr<ImageWriter<uint8_t> >( new DicomImageWriter<uint8_t> ) );
    FormatDictionary<VolumeRef<uint8_t> >::registerFormat( "DICOM", rfwu8, exts );
    
    VolumeRefFormatWriter<uint16_t> *rfwu16 = new VolumeRefFormatWriter<uint16_t>;
    rfwu16->attach( rc_ptr<ImageWriter<uint16_t> >( new DicomImageWriter<uint16_t> ) );
    FormatDictionary<VolumeRef<uint16_t> >::registerFormat( "DICOM", rfwu16, exts );

}


DicomPlugin::~DicomPlugin()
{
}


string DicomPlugin::name() const
{
  return string("DICOM CARTO");
}


bool DicomPlugin::noop()
{
  return true;
}

