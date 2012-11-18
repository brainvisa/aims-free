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

#include <aims/io/ecatSinoheader.h>
#include <aims/def/general.h>
#include <ecat+/io/io.h>
#include <cartobase/exception/file.h>
#include <fstream>
#include <vector>

using namespace carto;
using namespace aims ;
using namespace std;

EcatSinoHeader::EcatSinoHeader(const string& name ) :
  PythonHeader(), 
  _name(name), _type( _typeINIT )
{
}


EcatSinoHeader::EcatSinoHeader(  int type, 
			const string & name ) :
  PythonHeader(), 
  _name( name ), _type( type )
{
}


EcatSinoHeader::~EcatSinoHeader()
{
}


set<string> EcatSinoHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".S" );
  return exts;
}

void EcatSinoHeader::read( )
{
//   cout << "EcatSinoHeader::read(), filename : " << _name << "...\n";

  string            fileName;
  UnifiedEcatSinoInfo  *uesi;


  fileName = _name;   // .S  ECAT sinos
  if( (uesi = EcatSinoOpen( (char*) fileName.c_str(),
    const_cast<char*>( "r" ) )) == ECATSHFJ_FAIL )
  {
    if ( (uesi = EcatSinoOpen( (char*) (fileName+".S").c_str(),
      const_cast<char*>( "r" ) ))
	      != ECATSHFJ_FAIL )
      _name = fileName + ".S";
    else
      throw file_error("Wrong format", fileName);
  }
  
  setProperty( "file_type", string( "ECAT" ) );   
  setProperty( "ecat_file_type", string(uesi->file_type) ); 
  setProperty( "ecat_system_type",  uesi->system_type);
  setProperty( "ecat_scan_start_time", uesi->scan_start_time);
  setProperty( "ecat_isotope_name", string(uesi->isotope_code) );
  setProperty( "ecat_isotope_halflife", uesi->isotope_halflife);   
  setProperty( "ecat_distance_scanned", uesi->distance_scanned) ;
  setProperty( "ecat_transaxial_FOV", uesi->transaxial_fov);
  setProperty( "ecat_angular_compression", uesi->angular_compression); 
  setProperty( "ecat_study_type", string(uesi->study_name));
  setProperty( "ecat_num_planes", uesi->num_planes); 
  
  setProperty( "ecat_num_gates", uesi->num_gates); 
  setProperty( "ecat_num_frames", uesi->num_frames); 
  setProperty( "ecat_num_bed_pos", uesi->num_bed_pos);
  setProperty( "ecat_bed_elevation", uesi->bed_elevation );
  setProperty( "ecat_coin_samp_mode", uesi->coin_samp_mode );
  setProperty( "ecat_axial_samp_mode", uesi->axial_samp_mode );
  setProperty( "ecat_calibration_factor", uesi->calibration_factor );
  vector<float> bedOffset ;
  for( int i=0;i< 15; ++i) bedOffset.push_back(uesi->bed_offset[i]);
  setProperty( "ecat_bed_offset", bedOffset );
  setProperty( "ecat_lwr_sctr_thres", uesi->lwr_sctr_thres );
  setProperty( "ecat_dosage", uesi->dosage );
  setProperty( "ecat_well_counter_factor", uesi->well_counter_factor );
  setProperty( "ecat_septa_state", uesi->septa_state );
  
  
  setProperty( "ecat_init_bed_position", uesi->init_bed_position);
  setProperty( "ecat_lwr_true_thres", uesi->lwr_true_thres);
  setProperty( "ecat_upr_true_thres", uesi->upr_true_thres); 
  setProperty( "ecat_branching_fraction", uesi->branching_fraction); 
  setProperty( "ecat_dose_start_time", uesi->dose_start_time); 
  setProperty( "ecat_patient_orientation", uesi->patient_orientation);
  setProperty( "ecat_acquisition_type", uesi->acquisition_type);
  setProperty( "ecat_acquisition_mode", uesi->acquisition_mode);
  setProperty( "ecat_original_file_name",string( uesi->original_file_name));
  setProperty( "ecat_radiopharmaceutical", string(uesi->radiopharmaceutical));
  setProperty( "ecat_corrections_applied", uesi->corrections_applied);
  setProperty( "ecat_bin_size", uesi->bin_size);
  setProperty( "ecat_plane_separation", uesi->plane_separation);
  setProperty( "ecat_x_resolution", uesi->x_resolution);
  setProperty( "ecat_v_resolution", uesi->v_resolution);
  setProperty( "ecat_z_resolution", uesi->z_resolution);
  setProperty( "ecat_w_resolution", uesi->w_resolution);

  // A study can only be multiple bed or multiple frame exclusively
  if ( (( uesi->num_frames > 1) && (uesi->num_bed_pos > 0) ) ||
       (( uesi->num_frames > 1) && (uesi->num_gates > 1)) ||
       (( uesi->num_bed_pos > 0) && (uesi->num_gates > 1)) )
     throw file_error( " A study can only be multiple bed or multiple frame exclusively", 
		       fileName);

  vector<int> p, d, m, t, st, dt, gt, tcr;
  vector<short> smin, smax ;
  vector<float> tc, tu, l ;
  vector<float> us;
  scan3DSub *it=uesi->subfirst;
  for( it=uesi->subfirst;it != NULL ; it=it->next )
    {
      p.push_back(it->prompts);
      d.push_back(it->delayed);
      m.push_back(it->multiples);
      t.push_back(it->net_trues);
      tc.push_back(it->tot_avg_cor);
      tu.push_back(it->tot_avg_uncor);
      tcr.push_back(it->total_coin_rate);
      l.push_back(it->loss_correction_fctr);
      st.push_back(it->frame_start_time);
      dt.push_back(it->frame_duration);
      gt.push_back(it->gate_duration) ;
      smin.push_back(it->scan_min) ;
      smax.push_back(it->scan_max) ;
      // Tr�s moche .. Qui fait plus joli.
      // ici les uncorr_singles sont a intepreter par paquet de 128
      // il faudrait un type vector<vector<float>> : Graph ne sait pas gerer.
      int i;
      for(i=0;i< 128; ++i) us.push_back(it->uncor_singles[i]);
    }
  setProperty( "ecat_prompts", p);
  setProperty( "ecat_delayed", d);
  setProperty( "ecat_multiples", m);
  setProperty( "ecat_net_trues", t);
  setProperty( "ecat_tot_avg_cor",tc);
  setProperty( "ecat_tot_avg_uncor", tu);
  setProperty( "ecat_total_coin_rate", tcr);
  setProperty( "ecat_loss_correction_fctr", l);
  setProperty( "ecat_frame_start_time", st);
  setProperty( "ecat_frame_duration", dt);
  setProperty( "ecat_gate_duration", gt);
  setProperty( "ecat_scan_min", smin);
  setProperty( "ecat_scan_max", smax);
  
  setProperty( "ecat_uncor_singles", us);
  
  
  
  EcatSinoClose( uesi );
  
  // add meta-info to header
  //   readMinf( removeExtension( _name ) + extension() + ".minf" );
}

PythonHeader
EcatSinoHeader::getSingleBedDataFrame( int bedDataOrFrame ) const
{
  PythonHeader subHdr ;
  string file_type, original_file_name, radiopharmaceutical, isotope_name, study_type ;
  short  system_type, acquisition_type, acquisition_mode, corrections_applied, angular_compression,
    num_planes, num_frames, num_gates, num_bed_pos, lwr_true_thres, upr_true_thres, 
    patient_orientation, coin_samp_mode, axial_samp_mode, lwr_sctr_thres, septa_state ;
  unsigned int scan_start_time, dose_start_time ;
  float bin_size, plane_separation, isotope_halflife, distance_scanned, transaxial_fov, 
    init_bed_position, branching_fraction, bed_elevation, calibration_factor, dosage, well_counter_factor,
    x_resolution, v_resolution, z_resolution, w_resolution ;
  std::vector<float> bed_offset ;
  
  subHdr.setProperty( "file_type", string( "ECAT" ) );
  
  getProperty( "ecat_file_type", file_type ); 
  subHdr.setProperty( "ecat_file_type", file_type );
  getProperty( "ecat_system_type",  system_type );
  subHdr.setProperty( "ecat_system_type",  system_type );
  getProperty( "ecat_scan_start_time", scan_start_time);
  subHdr.setProperty( "ecat_scan_start_time", scan_start_time);
  getProperty( "ecat_acquisition_type", acquisition_type);
  subHdr.setProperty( "ecat_acquisition_type", acquisition_type);
  getProperty( "ecat_acquisition_mode", acquisition_mode);
  subHdr.setProperty( "ecat_acquisition_mode", acquisition_mode);
  getProperty( "ecat_original_file_name", original_file_name);
  subHdr.setProperty( "ecat_original_file_name", original_file_name);
  getProperty( "ecat_radiopharmaceutical", radiopharmaceutical);
  subHdr.setProperty( "ecat_radiopharmaceutical", radiopharmaceutical);
  getProperty( "ecat_isotope_name", isotope_name );
  getProperty( "ecat_isotope_halflife", isotope_halflife);   
  getProperty( "ecat_distance_scanned", distance_scanned); 
  getProperty( "ecat_transaxial_FOV", transaxial_fov);
  getProperty( "ecat_angular_compression", angular_compression); 
  getProperty( "ecat_study_type", study_type);
  getProperty( "ecat_num_planes", num_planes); 
  getProperty( "ecat_num_gates", num_gates); 
  getProperty( "ecat_num_frames", num_frames); 
  getProperty( "ecat_num_bed_pos", num_bed_pos);
  getProperty( "ecat_init_bed_position", init_bed_position);
  getProperty( "ecat_lwr_true_thres", lwr_true_thres);
  getProperty( "ecat_upr_true_thres", upr_true_thres); 
  getProperty( "ecat_branching_fraction", branching_fraction); 
  getProperty( "ecat_dose_start_time", dose_start_time); 
  getProperty( "ecat_patient_orientation", patient_orientation);
  getProperty( "ecat_bed_elevation", bed_elevation );
  getProperty( "ecat_coin_samp_mode", coin_samp_mode );
  getProperty( "ecat_axial_samp_mode", axial_samp_mode );
  getProperty( "ecat_calibration_factor", calibration_factor );
  getProperty( "ecat_bed_offset", bed_offset );
  getProperty( "ecat_lwr_sctr_thres", lwr_sctr_thres );
  getProperty( "ecat_dosage", dosage );
  getProperty( "ecat_well_counter_factor", well_counter_factor );
  getProperty( "ecat_septa_state", septa_state );
  getProperty( "ecat_x_resolution", x_resolution );
  getProperty( "ecat_v_resolution", v_resolution );
  getProperty( "ecat_z_resolution", z_resolution );
  getProperty( "ecat_w_resolution", w_resolution );
  
  

  subHdr.setProperty( "ecat_isotope_name", isotope_name );
  subHdr.setProperty( "ecat_isotope_halflife", isotope_halflife);   
  subHdr.setProperty( "ecat_distance_scanned", distance_scanned); 
  subHdr.setProperty( "ecat_transaxial_FOV", transaxial_fov);
  subHdr.setProperty( "ecat_angular_compression", angular_compression); 
  subHdr.setProperty( "ecat_study_type", study_type);
  subHdr.setProperty( "ecat_num_planes", num_planes); 


  int multi_type, num_files ;
  if( num_frames > 1 ){
    multi_type = MULTIFRAME ;
    num_files = num_frames ;
  }
  else if( num_gates > 1 ){
    multi_type = MULTIGATE ;
    num_files = num_gates ;
  }
  else if( num_bed_pos > 0 ){
    multi_type = MULTIBED ;
    num_files = num_bed_pos + 1 ;
  }

  subHdr.setProperty( "multi_files_type", multi_type ) ;
//   subHdr.setProperty( "num_files", multi_type ) ; 
  subHdr.setProperty( "ecat_num_gates", num_gates); 
  subHdr.setProperty( "ecat_num_frames", num_frames); 
  subHdr.setProperty( "ecat_num_bed_pos", num_bed_pos);
  subHdr.setProperty( "ecat_init_bed_position", init_bed_position);
  subHdr.setProperty( "ecat_lwr_true_thres", lwr_true_thres);
  subHdr.setProperty( "ecat_upr_true_thres", upr_true_thres); 
  subHdr.setProperty( "ecat_branching_fraction", branching_fraction); 
  subHdr.setProperty( "ecat_dose_start_time", dose_start_time); 
  subHdr.setProperty( "ecat_patient_orientation", patient_orientation);
  subHdr.setProperty( "ecat_bed_elevation", bed_elevation ); 
  subHdr.setProperty( "ecat_coin_samp_mode", coin_samp_mode); 
  subHdr.setProperty( "ecat_axial_samp_mode", axial_samp_mode); 
  subHdr.setProperty( "ecat_calibration_factor", calibration_factor); 
  subHdr.setProperty( "ecat_bed_offset", bed_offset ); 
  subHdr.setProperty( "ecat_lwr_sctr_thres", lwr_sctr_thres); 
  subHdr.setProperty( "ecat_dosage", dosage ); 
  subHdr.setProperty( "ecat_well_counter_factor", well_counter_factor); 
  subHdr.setProperty( "ecat_septa_state", septa_state ); 
  subHdr.setProperty( "ecat_x_resolution", x_resolution ); 
  subHdr.setProperty( "ecat_v_resolution", v_resolution ); 
  subHdr.setProperty( "ecat_z_resolution", z_resolution ); 
  subHdr.setProperty( "ecat_w_resolution", w_resolution ); 

  getProperty( "ecat_corrections_applied", corrections_applied);
  subHdr.setProperty( "ecat_corrections_applied", corrections_applied);
  getProperty( "ecat_bin_size", bin_size);
  subHdr.setProperty( "ecat_bin_size", bin_size);
  getProperty( "ecat_plane_separation", plane_separation);
  subHdr.setProperty( "ecat_plane_separation", plane_separation);

  vector<int> p, d, m, t, st, dt, gt, tcr;
  vector<float> tc, tu, l ;
  vector<float> us ;
  vector<short> smin, smax;
 
  getProperty( "ecat_prompts", p);
  subHdr.setProperty( "ecat_prompts", p[bedDataOrFrame]);
  getProperty( "ecat_delayed", d);
  subHdr.setProperty( "ecat_delayed", d[bedDataOrFrame]);
  getProperty( "ecat_multiples", m);
  subHdr.setProperty( "ecat_multiples", m[bedDataOrFrame]);
  getProperty( "ecat_net_trues", t);
  subHdr.setProperty( "ecat_net_trues", t[bedDataOrFrame]);
  getProperty( "ecat_tot_avg_cor",tc);
  subHdr.setProperty( "ecat_tot_avg_cor",tc[bedDataOrFrame]);
  getProperty( "ecat_tot_avg_uncor", tu);
  subHdr.setProperty( "ecat_tot_avg_uncor", tu[bedDataOrFrame]);
  getProperty( "ecat_total_coin_rate", tcr);
  subHdr.setProperty( "ecat_total_coin_rate", tcr[bedDataOrFrame]);
  getProperty( "ecat_loss_correction_fctr", l);
  subHdr.setProperty( "ecat_loss_correction_fctr", l[bedDataOrFrame]);
  getProperty( "ecat_frame_start_time", st);
  subHdr.setProperty( "ecat_frame_start_time", st[bedDataOrFrame]);
  getProperty( "ecat_frame_duration", dt);
  subHdr.setProperty( "ecat_frame_duration", dt[bedDataOrFrame]);
  getProperty( "ecat_gate_duration", gt);
  subHdr.setProperty( "ecat_gate_duration", gt[bedDataOrFrame]);
  getProperty( "ecat_uncor_singles", us );
  vector<float> subUs(128) ;
  for( int i = 128*bedDataOrFrame, j = 0 ; j <128 ; ++i, ++j  )
    subUs[j] = us[i] ;
  subHdr.setProperty( "ecat_uncor_singles", subUs );
  getProperty( "ecat_scan_min", smin);
  subHdr.setProperty( "ecat_scan_min", smin[bedDataOrFrame]);
  getProperty( "ecat_scan_max", smax);
  subHdr.setProperty( "ecat_scan_max", smax[bedDataOrFrame]);
  
  return subHdr ;
}


void 
EcatSinoHeader::setHeader( const vector<PythonHeader>& subHeaders )
{
  int multi_type ;
  string file_type, original_file_name, radiopharmaceutical, isotope_name, study_type ;
  short  system_type, acquisition_type, acquisition_mode, corrections_applied, angular_compression,
    num_planes, lwr_true_thres, upr_true_thres, 
    patient_orientation, coin_samp_mode, axial_samp_mode, lwr_sctr_thres, septa_state ;
  unsigned int scan_start_time, dose_start_time ;
  float bin_size, plane_separation, isotope_halflife, distance_scanned, transaxial_fov, 
    init_bed_position, branching_fraction, bed_elevation, calibration_factor, dosage, well_counter_factor,
    x_resolution, v_resolution, z_resolution, w_resolution ;
  std::vector<float> bed_offset ;
  
  setProperty( "file_type", string( "ECAT" ) );
  subHeaders[0].getProperty( "ecat_file_type", file_type ); 
  setProperty( "ecat_file_type", file_type );
  subHeaders[0].getProperty( "ecat_system_type",  system_type );
  setProperty( "ecat_system_type",  system_type );
  subHeaders[0].getProperty( "ecat_scan_start_time", scan_start_time);
  setProperty( "ecat_scan_start_time", scan_start_time);
  subHeaders[0].getProperty( "ecat_acquisition_type", acquisition_type);
  setProperty( "ecat_acquisition_type", acquisition_type);
  subHeaders[0].getProperty( "ecat_acquisition_mode", acquisition_mode);
  setProperty( "ecat_acquisition_mode", acquisition_mode);
  subHeaders[0].getProperty( "ecat_original_file_name", original_file_name);
  setProperty( "ecat_original_file_name", original_file_name);
  subHeaders[0].getProperty( "ecat_radiopharmaceutical", radiopharmaceutical);
  setProperty( "ecat_radiopharmaceutical", radiopharmaceutical);
  subHeaders[0].getProperty( "ecat_corrections_applied", corrections_applied);
  setProperty( "ecat_corrections_applied", corrections_applied);
  subHeaders[0].getProperty( "ecat_bin_size", bin_size);
  setProperty( "ecat_bin_size", bin_size);
  subHeaders[0].getProperty( "ecat_plane_separation", plane_separation);
  setProperty( "ecat_plane_separation", plane_separation);

  subHeaders[0].getProperty( "ecat_isotope_halflife", isotope_halflife);   
  subHeaders[0].getProperty( "ecat_distance_scanned", distance_scanned); 
  subHeaders[0].getProperty( "ecat_transaxial_FOV", transaxial_fov);
  subHeaders[0].getProperty( "ecat_angular_compression", angular_compression); 
  subHeaders[0].getProperty( "ecat_study_type", study_type);
  subHeaders[0].getProperty( "ecat_num_planes", num_planes); 
//   subHeaders[0].getProperty( "ecat_num_gates", num_gates); 
//   subHeaders[0].getProperty( "ecat_num_frames", num_frames); 
//   subHeaders[0].getProperty( "ecat_num_bed_pos", num_bed_pos);
  subHeaders[0].getProperty( "multi_files_type", multi_type ) ;
  subHeaders[0].getProperty( "ecat_init_bed_position", init_bed_position);
  subHeaders[0].getProperty( "ecat_lwr_true_thres", lwr_true_thres);
  subHeaders[0].getProperty( "ecat_lwr_true_thres", upr_true_thres); 
  subHeaders[0].getProperty( "ecat_branching_fraction", branching_fraction); 
  subHeaders[0].getProperty( "ecat_dose_start_time", dose_start_time); 
  subHeaders[0].getProperty( "ecat_patient_orientation", patient_orientation);

  setProperty( "ecat_isotope_name", isotope_name );
  setProperty( "ecat_isotope_halflife", isotope_halflife);   
  setProperty( "ecat_distance_scanned", distance_scanned); 
  setProperty( "ecat_transaxial_FOV", transaxial_fov);
  setProperty( "ecat_angular_compression", angular_compression); 
  setProperty( "ecat_study_type", study_type);

  setProperty( "ecat_num_planes", num_planes); 
  setProperty( "ecat_num_gates", short(1) ); 
  setProperty( "ecat_num_frames", short(1) ) ; 
  setProperty( "ecat_num_bed_pos", short(0) );
  if( subHeaders.size() > 1 )
  {
    if( multi_type == MULTIFRAME )
      setProperty( "ecat_num_frames", short(subHeaders.size()) ) ; 
    else if( multi_type == MULTIGATE )
      setProperty( "ecat_num_gates", short(subHeaders.size()) ) ;
    else if( multi_type == MULTIBED) 
      setProperty( "ecat_num_bed_pos", short(subHeaders.size()-1) );
  }
  
  setProperty( "ecat_init_bed_position", init_bed_position);
  setProperty( "ecat_lwr_true_thres", lwr_true_thres);
  setProperty( "ecat_upr_true_thres", upr_true_thres); 
  setProperty( "ecat_branching_fraction", branching_fraction); 
  setProperty( "ecat_dose_start_time", dose_start_time); 
  setProperty( "ecat_patient_orientation", patient_orientation);
  
  subHeaders[0].getProperty( "ecat_bed_elevation", bed_elevation);
  setProperty( "ecat_bed_elevation", bed_elevation);
  subHeaders[0].getProperty( "ecat_coin_samp_mode", coin_samp_mode);
  setProperty( "ecat_coin_samp_mode", coin_samp_mode);
  subHeaders[0].getProperty( "ecat_axial_samp_mode", axial_samp_mode);
  setProperty( "ecat_axial_samp_mode", axial_samp_mode);
  subHeaders[0].getProperty( "ecat_calibration_factor",calibration_factor );
  setProperty( "ecat_calibration_factor", calibration_factor);
  subHeaders[0].getProperty( "ecat_bed_offset", bed_offset);
  setProperty( "ecat_bed_offset", bed_offset);
  subHeaders[0].getProperty( "ecat_lwr_sctr_thres",lwr_sctr_thres );
  setProperty( "ecat_lwr_sctr_thres", lwr_sctr_thres);
  subHeaders[0].getProperty( "ecat_dosage", dosage);
  setProperty( "ecat_dosage", dosage);
  subHeaders[0].getProperty( "ecat_well_counter_factor", well_counter_factor);
  setProperty( "ecat_well_counter_factor", well_counter_factor );
  subHeaders[0].getProperty( "ecat_septa_state", septa_state);
  setProperty( "ecat_septa_state", septa_state);
  subHeaders[0].getProperty( "ecat_x_resolution", x_resolution);
  setProperty( "ecat_x_resolution", x_resolution);
  subHeaders[0].getProperty( "ecat_v_resolution", v_resolution);
  setProperty( "ecat_v_resolution", v_resolution);
  subHeaders[0].getProperty( "ecat_z_resolution", z_resolution);
  setProperty( "ecat_z_resolution", z_resolution);
  subHeaders[0].getProperty( "ecat_w_resolution", w_resolution);
  setProperty( "ecat_w_resolution", w_resolution);
  

  int p, d, m, t, st, dt, gt, tcr;
  short smin, smax ;
  float tc, tu, l ;
  vector<float> us;
  vector<int> ps, ds, ms, ts, sts, dts, gts, tcrs;
  vector<float> tcs, tus, ls ;
  vector<float> uss ;
  vector<short> smins, smaxs ;
  
  for(unsigned int i = 0 ; i < subHeaders.size() ; ++i ){
    subHeaders[i].getProperty( "ecat_prompts", p ) ;
    ps.push_back(p);
    subHeaders[i].getProperty( "ecat_delayed", d ) ;
    ds.push_back(d);
    subHeaders[i].getProperty( "ecat_multiples", m ) ;
    ms.push_back(m);
    subHeaders[i].getProperty( "ecat_net_trues", t ) ;
    ts.push_back(t);
    subHeaders[i].getProperty( "ecat_tot_avg_cor", tc ) ;
    tcs.push_back(tc);
    subHeaders[i].getProperty( "ecat_tot_avg_uncor", tu ) ;
    tus.push_back(tu);
    subHeaders[i].getProperty( "ecat_total_coin_rate", tcr ) ;
    tcrs.push_back(tcr);
    subHeaders[i].getProperty( "ecat_loss_correction_fctr", l ) ;
    ls.push_back(l);
    subHeaders[i].getProperty( "ecat_frame_start_time", st ) ;
    sts.push_back(st);
    subHeaders[i].getProperty( "ecat_frame_duration", dt ) ;
    dts.push_back(dt);
    subHeaders[i].getProperty( "ecat_gate_duration", gt ) ;
    gts.push_back(gt);
    subHeaders[i].getProperty( "ecat_uncor_singles", us ) ;
    int j;
    for(j=0;j< 128; ++j) uss.push_back(us[j]);
    subHeaders[i].getProperty( "ecat_scan_min", smin ) ;
    smins.push_back(smin);
    subHeaders[i].getProperty( "ecat_scan_max", smax ) ;
    smaxs.push_back(smax);
  }
  setProperty( "ecat_prompts", ps );
  setProperty( "ecat_delayed", ds );
  setProperty( "ecat_multiples", ms );
  setProperty( "ecat_net_trues", ts );
  setProperty( "ecat_tot_avg_cor",tcs );
  setProperty( "ecat_tot_avg_uncor", tus );
  setProperty( "ecat_total_coin_rate", tcrs );
  setProperty( "ecat_loss_correction_fctr", ls );
  setProperty( "ecat_frame_start_time", sts );
  setProperty( "ecat_frame_duration", dts );
  setProperty( "ecat_gate_duration", gts );
  setProperty( "ecat_uncor_singles", uss );
  setProperty( "ecat_scan_min", smins );
  setProperty( "ecat_scan_max", smaxs );

}
  
void EcatSinoHeader::write() const
{
//   setProperty( "file_type", string( "ECAT" ) );
//   setProperty( "object_type", string( "Volume" ) );
//   if( imageIsProbablyLabel() )
//     setProperty( "data_type", string( "S16" ) );
//   else
//     setProperty( "data_type", string( "FLOAT" ) );
//   vector<int>	dims;
//   dims.push_back( dimX() );
//   dims.push_back( dimY() );
//   dims.push_back( dimZ() );
//   dims.push_back( dimT() );
//   setProperty( "volume_dimension", dims );
//   vector<float>	vs;
//   vs.push_back( sizeX() );
//   vs.push_back( sizeY() );
//   vs.push_back( sizeZ() );
//   vs.push_back( sizeT() );
//   setProperty( "voxel_size", vs );
  writeMinf( removeExtension( _name ) + extension() + ".minf" );
}
