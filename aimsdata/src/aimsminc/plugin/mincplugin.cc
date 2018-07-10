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

#include <aims/plugin/mincplugin.h>
#include <aims/io/mincformat_d.h>
#include <aims/io/mincfinderformat.h>
#include <aims/io/finder.h>
#include <aims/rgb/rgb.h>
#include <aims/data/pheader.h>
#include <aims/io/baseFormats_cartovolume.h>
#ifdef __APPLE__
#include <aims/io/macosxbugs.h>
#endif

using namespace aims;
using namespace std;
using namespace carto;

namespace
{

  bool initminc()
  {
    new MincPlugin;
    return true;
  }

  bool mincinit = initminc();

}

MincPlugin::MincPlugin() : Plugin()
{
  vector<string>	ext;
  ext.push_back( "mnc" );
  ext.push_back( "mnc.gz" );
  ext.push_back( "mgh" ); // Freesurfer format
  ext.push_back( "mgz" ); // compressed Freesurfer format
  MincFormat<int8_t>	*df1 = new MincFormat<int8_t>;
#ifdef AIMS_APPLE_GCC33BUG
  macosxbugs::fileFormatDictionary_dataint8_registerFormat( "MINC", df1, ext );
#else
  FileFormatDictionary<AimsData<int8_t> >::registerFormat( "MINC", 
                                                           df1, ext );
#endif
  VolumeFormat<int8_t>	*vf1 = new VolumeFormat<int8_t>( "MINC" );
  FileFormatDictionary<carto::Volume<int8_t> >::registerFormat( "MINC", vf1, 
                                                                ext );

  MincFormat<uint8_t>	*df2 = new MincFormat<uint8_t>;
  FileFormatDictionary<AimsData<uint8_t> >::registerFormat( "MINC", df2, 
                                                            ext );
  VolumeFormat<uint8_t>	*vf2 = new VolumeFormat<uint8_t>( "MINC" );
  FileFormatDictionary<carto::Volume<uint8_t> >::registerFormat( "MINC", vf2, 
                                                                 ext );

  MincFormat<int16_t>	*df3 = new MincFormat<int16_t>;
  FileFormatDictionary<AimsData<int16_t> >::registerFormat( "MINC", df3, ext );
  VolumeFormat<int16_t>	*vf3 = new VolumeFormat<int16_t>( "MINC" );
  FileFormatDictionary<carto::Volume<int16_t> >::registerFormat( "MINC", vf3, 
                                                                 ext );

  MincFormat<uint16_t>	*df4 = new MincFormat<uint16_t>;
  FileFormatDictionary<AimsData<uint16_t> >::registerFormat( "MINC", 
                                                             df4, ext );
  VolumeFormat<uint16_t>	*vf4 = new VolumeFormat<uint16_t>( "MINC" );
  FileFormatDictionary<carto::Volume<uint16_t> >::registerFormat( "MINC", vf4, 
                                                                  ext );

  MincFormat<int32_t>	*df5 = new MincFormat<int32_t>;
  FileFormatDictionary<AimsData<int32_t> >::registerFormat( "MINC", df5, ext );
  VolumeFormat<int32_t>	*vf5 = new VolumeFormat<int32_t>( "MINC" );
  FileFormatDictionary<carto::Volume<int32_t> >::registerFormat( "MINC", vf5, 
                                                                 ext );

  MincFormat<uint32_t>	*df6 = new MincFormat<uint32_t>;
  FileFormatDictionary<AimsData<uint32_t> >::registerFormat( "MINC", 
                                                             df6, ext );
  VolumeFormat<uint32_t>	*vf6 = new VolumeFormat<uint32_t>( "MINC" );
  FileFormatDictionary<carto::Volume<uint32_t> >::registerFormat( "MINC", vf6, 
                                                                  ext );

  MincFormat<float>	*df7 = new MincFormat<float>;
  FileFormatDictionary<AimsData<float> >::registerFormat( "MINC", df7, ext );
  VolumeFormat<float>	*vf7 = new VolumeFormat<float>( "MINC" );
  FileFormatDictionary<carto::Volume<float> >::registerFormat( "MINC", vf7, 
                                                               ext );

  MincFormat<double>	*df8 = new MincFormat<double>;
  FileFormatDictionary<AimsData<double> >::registerFormat( "MINC", df8, ext );
  VolumeFormat<double>	*vf8 = new VolumeFormat<double>( "MINC" );
  FileFormatDictionary<carto::Volume<double> >::registerFormat( "MINC", vf8, 
                                                                ext );

  // ### remove after everything has been moved to intN_t/uintN_t
  MincFormat<char>	*df10 = new MincFormat<char>;
  FileFormatDictionary<AimsData<char> >::registerFormat( "MINC", 
                                                         df10, ext );
  VolumeFormat<char>	*vf10 = new VolumeFormat<char>( "MINC" );
  FileFormatDictionary<carto::Volume<char> >::registerFormat( "MINC", vf10, 
                                                              ext );

  MincFormat<int64_t>	*df11 = new MincFormat<int64_t>;
  FileFormatDictionary<AimsData<int64_t> >::registerFormat( "MINC", 
                                                         df11, ext );
  VolumeFormat<int64_t>	*vf11 = new VolumeFormat<int64_t>( "MINC" );
  FileFormatDictionary<carto::Volume<int64_t> >::registerFormat( "MINC", vf11, 
                                                              ext );

  MincFormat<uint64_t>	*df12 = new MincFormat<uint64_t>;
  FileFormatDictionary<AimsData<uint64_t> >::registerFormat( "MINC", 
                                                                  df12, ext );
  VolumeFormat<uint64_t>	*vf12
    = new VolumeFormat<uint64_t>( "MINC" );
  FileFormatDictionary<carto::Volume<uint64_t> >::registerFormat( "MINC", 
                                                                  vf12, ext );

  Finder::registerFormat( "MINC", new FinderMincFormat, ext );

  SyntaxSet	*s = PythonHeader::syntax();
  Syntax	&sx = (*s)[ "__generic__" /*"PythonHeader"*/ ];

  (sx)[ "MINC_voxel_size" ] = Semantic( "float_vector" );


  (sx)[ "MINC_general_transform:inverse_flag" ] = Semantic( "int" );
  (sx)[ "MINC_general_transform:type" ] = Semantic( "string" );
  (sx)[ "MINC_general_transform:transform0" ] = Semantic( "float_vector" );
  (sx)[ "MINC_general_transform:transform1" ] = Semantic( "float_vector" );
  (sx)[ "MINC_general_transform:transform2" ] = Semantic( "float_vector" );
  (sx)[ "MINC_general_transform:transform3" ] = Semantic( "float_vector" );

  (sx)[ "MINC_space_type" ] = Semantic( "string" );

  (sx)[ "MINC_history" ] = Semantic( "string" );


  // AIMS "patient_id" is used to store MINC "patient:full_name" and not "patient:identification
  (sx)[ "MINC_patient:varid" ] = Semantic( "string" );
  (sx)[ "MINC_patient:vartype" ] = Semantic( "string" );
  (sx)[ "MINC_patient:version" ] = Semantic( "string" );
  //"patient:fullname" --> "patient_id"
  (sx)[ "MINC_patient:identification" ] = Semantic( "string" );
  (sx)[ "MINC_patient:birthdate" ] = Semantic( "string" );
  (sx)[ "MINC_patient:sex" ] = Semantic( "string" );
  (sx)[ "MINC_patient:weight" ] = Semantic( "float" );


  (sx)[ "MINC_study:varid" ] = Semantic( "string" );
  (sx)[ "MINC_study:vartype" ] = Semantic( "string" );
  (sx)[ "MINC_study:version" ] = Semantic( "string" );
  // Would like to use AIMS "start_time" but don't know how to make correspond this string and the int_vector
  (sx)[ "MINC_study:start_time" ] = Semantic( "string" );
  //"study:modality" --> "modality"
  (sx)[ "MINC_study:institution" ] = Semantic( "string" );
  (sx)[ "MINC_study:station_id" ] = Semantic( "string" );
  (sx)[ "MINC_study:procedure" ] = Semantic( "string" );
  //"study:study_id" --> "study_id"
  (sx)[ "MINC_study:acquisition_id" ] = Semantic( "string" );


  (sx)[ "MINC_acquisition:varid" ] = Semantic( "string" );
  (sx)[ "MINC_acquisition:vartype" ] = Semantic( "string" );
  (sx)[ "MINC_acquisition:version" ] = Semantic( "string" );
  (sx)[ "MINC_acquisition:scanning_sequence" ] = Semantic( "string" );
  //"acquisition:repetition_time" --> "tr"
  //"acquisition:echo_time" --> "te"
  (sx)[ "MINC_acquisition:inversion_time" ] = Semantic( "float" );
  //"acquisition:flip_angle" --> "flip_angle"
  (sx)[ "MINC_acquisition:num_averages" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:imaging_frequency" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:imaged_nucleus" ] = Semantic( "string" );
  (sx)[ "MINC_acquisition:comments" ] = Semantic( "string" );


  (sx)[ "MINC_image-min:units" ] = Semantic( "string" );
  (sx)[ "MINC_image-max:units" ] = Semantic( "string" );

  //From Siemens Sonata
  (sx)[ "MINC_patient:age" ] = Semantic( "string" );

  (sx)[ "MINC_study:start_date" ] = Semantic( "string" );
  (sx)[ "MINC_study:manufacturer" ] = Semantic( "string" );
  (sx)[ "MINC_study:model" ] = Semantic( "string" );
  (sx)[ "MINC_study:field_value" ] = Semantic( "float" );
  (sx)[ "MINC_study:software_version" ] = Semantic( "string" );
  (sx)[ "MINC_study:serial_no" ] = Semantic( "string" );
  (sx)[ "MINC_study:performing_physician" ] = Semantic( "string" );
  (sx)[ "MINC_study:operator" ] = Semantic( "string" );
  (sx)[ "MINC_study:calibration_date" ] = Semantic( "string" );

  (sx)[ "MINC_acquisition:acquisition_id" ] = Semantic( "string" );
  (sx)[ "MINC_acquisition:start_time" ] = Semantic( "string" );
  (sx)[ "MINC_acquisition:protocol_name" ] = Semantic( "string" );
  (sx)[ "MINC_acquisition:receive_coil" ] = Semantic( "string" );
  (sx)[ "MINC_acquisition:transmit_coil" ] = Semantic( "string" );
  (sx)[ "MINC_acquisition:echo_number" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:slice_thickness" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:num_slices" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:num_dyn_scans" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:window_center" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:window_width" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:num_phase_enc_steps" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:percent_sampling" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:percent_phase_fov" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:pixel_bandwidth" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:phase_enc_dir" ] = Semantic( "string" );
  (sx)[ "MINC_acquisition:SAR" ] = Semantic( "float" );
  (sx)[ "MINC_acquisition:mr_acq_type" ] = Semantic( "string" );
  (sx)[ "MINC_acquisition:image_type" ] = Semantic( "string" );
  (sx)[ "MINC_acquisition:MrProt_dump" ] = Semantic( "string" );

  (sx)[ "MINC_processing:transformation0-filename" ] = Semantic( "string" );
  (sx)[ "MINC_processing:transformation0-filedata" ] = Semantic( "string" );

  MincHeader::mincMutex(); // force creating it now.
}


MincPlugin::~MincPlugin()
{
}


string MincPlugin::name() const
{
  return "MINC IO";
}


bool MincPlugin::noop()
{
  return true;
}
