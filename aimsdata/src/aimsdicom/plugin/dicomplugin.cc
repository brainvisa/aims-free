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

#include <aims/plugin/dicomplugin.h>
#include <aims/io/dicomformat_d.h>
#include <aims/io/dicomfinderformat.h>
#include <aims/io/finder.h>
#include <aims/io/baseFormats_cartovolume.h>
#ifdef __APPLE__
#include <aims/io/macosxbugs.h>
#endif

using namespace aims;
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
  vector<string>	ext;
  ext.push_back( "dcm" );
  ext.push_back( "" );

  DicomFormat<int8_t>	*df1 = new DicomFormat<int8_t>;
#ifdef AIMS_APPLE_GCC33BUG
  macosxbugs::fileFormatDictionary_dataint8_registerFormat( "DICOM", df1, ext );
#else
  FileFormatDictionary<AimsData<int8_t> >::registerFormat( "DICOM", df1, ext );
#endif
  VolumeFormat<int8_t>	*vf1 = new VolumeFormat<int8_t>( "DICOM" );
  FileFormatDictionary<Volume<int8_t> >::registerFormat( "DICOM", vf1, ext );

  DicomFormat<uint8_t>	*df2 = new DicomFormat<uint8_t>;
  FileFormatDictionary<AimsData<uint8_t> >::registerFormat( "DICOM", df2, 
                                                            ext );
  VolumeFormat<uint8_t>	*vf2 = new VolumeFormat<uint8_t>( "DICOM" );
  FileFormatDictionary<Volume<uint8_t> >::registerFormat( "DICOM", vf2, ext );

  DicomFormat<int16_t>	*df3 = new DicomFormat<int16_t>;
  FileFormatDictionary<AimsData<int16_t> >::registerFormat( "DICOM", df3, 
                                                            ext );
  VolumeFormat<int16_t>	*vf3 = new VolumeFormat<int16_t>( "DICOM" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "DICOM", vf3, ext );

  DicomFormat<uint16_t>	*df4 = new DicomFormat<uint16_t>;
  FileFormatDictionary<AimsData<uint16_t> >::registerFormat( "DICOM", df4, 
                                                             ext );
  VolumeFormat<uint16_t>	*vf4 = new VolumeFormat<uint16_t>( "DICOM" );
  FileFormatDictionary<Volume<uint16_t> >::registerFormat( "DICOM", vf4, ext );

  DicomFormat<int32_t>	*df5 = new DicomFormat<int32_t>;
  FileFormatDictionary<AimsData<int32_t> >::registerFormat( "DICOM", df5, 
                                                            ext );
  VolumeFormat<int32_t>	*vf5 = new VolumeFormat<int32_t>( "DICOM" );
  FileFormatDictionary<Volume<int32_t> >::registerFormat( "DICOM", vf5, ext );

  DicomFormat<uint32_t>	*df6 = new DicomFormat<uint32_t>;
  FileFormatDictionary<AimsData<uint32_t> >::registerFormat( "DICOM", df6, 
                                                             ext );
  VolumeFormat<uint32_t>	*vf6 = new VolumeFormat<uint32_t>( "DICOM" );
  FileFormatDictionary<Volume<uint32_t> >::registerFormat( "DICOM", vf6, ext );

  DicomFormat<float>	*df7 = new DicomFormat<float>;
  FileFormatDictionary<AimsData<float> >::registerFormat( "DICOM", df7, ext );
  VolumeFormat<float>	*vf7 = new VolumeFormat<float>( "DICOM" );
  FileFormatDictionary<Volume<float> >::registerFormat( "DICOM", vf7, ext );

  DicomFormat<double>	*df8 = new DicomFormat<double>;
  FileFormatDictionary<AimsData<double> >::registerFormat( "DICOM", df8, ext );
  VolumeFormat<double>	*vf8 = new VolumeFormat<double>( "DICOM" );
  FileFormatDictionary<Volume<double> >::registerFormat( "DICOM", vf8, ext );

  // ### remove after everything has been moved to intN_t/uintN_t
  DicomFormat<char>	*df10 = new DicomFormat<char>;
  FileFormatDictionary<AimsData<char> >::registerFormat( "DICOM", df10, ext );
  VolumeFormat<char>	*vf10 = new VolumeFormat<char>( "DICOM" );
  FileFormatDictionary<Volume<char> >::registerFormat( "DICOM", vf10, ext );

  DicomFormat<int64_t>	*df11 = new DicomFormat<int64_t>;
  FileFormatDictionary<AimsData<int64_t> >::registerFormat( "DICOM", df11, ext );
  VolumeFormat<int64_t>	*vf11 = new VolumeFormat<int64_t>( "DICOM" );
  FileFormatDictionary<Volume<int64_t> >::registerFormat( "DICOM", vf11, ext );

  DicomFormat<uint64_t>	*df12 = new DicomFormat<uint64_t>;
  FileFormatDictionary<AimsData<uint64_t> >::registerFormat( "DICOM", 
                                                                  df12, ext );
  VolumeFormat<uint64_t>	*vf12
    = new VolumeFormat<uint64_t>( "DICOM" );
  FileFormatDictionary<Volume<uint64_t> >::registerFormat( "DICOM", vf12, 
                                                                ext );

  Finder::registerFormat( "DICOM", new FinderDicomFormat, ext );
}


DicomPlugin::~DicomPlugin()
{
}


string DicomPlugin::name() const
{
  return string("DICOM IO");
}


bool DicomPlugin::noop()
{
  return true;
}

