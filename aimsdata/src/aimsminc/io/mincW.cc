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

#include <cstdlib>
#include <aims/io/mincW.h>
#include <aims/io/mincheader.h>
#include <aims/resampling/motion.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/fdinhibitor.h>

using namespace carto;
using namespace std;

namespace aims
{

template <class T>
bool MincWriter<T>::write( const AimsData<T>& thing )
{
  /*
  cout << "MincWriter<" << DataTypeCode<T>::name() << ">::write " << _name 
       << endl;
  */

  //Only works for 3D volume. Should add 4D support.
  int n_dimensions=4;
    
  if(thing.dimT()==1) {
    if(thing.dimZ()==1) {
      if(thing.dimY()==1) {
	n_dimensions=1;
      }
      else n_dimensions=2;
    }
    else n_dimensions=3;
  }
  ::Volume volume;
  STRING dim_names[4];
    
  dim_names[0] = create_string( const_cast<char*>( MIzspace) );
  dim_names[1] = create_string( const_cast<char*>( MIyspace ) );
  dim_names[2] = create_string( const_cast<char*>( MIxspace ) );
  dim_names[3] = create_string( const_cast<char*>( MItime ) );

  nc_type nc_data_type;
  BOOLEAN signed_flag;
  
  carto::DataTypeCode<T>	dtc;
  //std::cout << "Type: " << dtc.dataType() << "\n";
  if(dtc.dataType()=="U8") {
    nc_data_type=NC_BYTE;
    signed_flag=FALSE;
  }
  else if(dtc.dataType()=="S8") {
    nc_data_type=NC_BYTE;
    signed_flag=TRUE;
  }
  else if(dtc.dataType()=="U16") {
    nc_data_type=NC_SHORT;
    signed_flag=FALSE;
  }
  else if(dtc.dataType()=="S16") {
    nc_data_type=NC_SHORT;
    signed_flag=TRUE;
  }
  else if(dtc.dataType()=="U32") {
    nc_data_type=NC_LONG;
    signed_flag=FALSE;
  }
  else if(dtc.dataType()=="S32") {
    nc_data_type=NC_LONG;
    signed_flag=TRUE;
  }
  else if(dtc.dataType()=="FLOAT") {
    nc_data_type=NC_FLOAT;
    signed_flag=TRUE;
  }
  else if(dtc.dataType()=="DOUBLE") {
    nc_data_type=NC_DOUBLE;
    signed_flag=TRUE;
  }
  else
    throw datatype_format_error( string( "Unsupported data type " ) 
                                 + dtc.dataType(), _name );

  volume=create_volume(n_dimensions,
		       dim_names,
		       nc_data_type,
		       signed_flag,
		       thing.minimum(),
		       thing.maximum());
    
  set_volume_real_range(volume,thing.minimum(),thing.maximum());
    
  int       sizes[MAX_DIMENSIONS];
  sizes[3]=thing.dimZ();
  sizes[2]=thing.dimX();
  sizes[1]=thing.dimY();
  sizes[0]=thing.dimZ();

  set_volume_sizes(volume,sizes);
    
  alloc_volume_data(volume);

  MincHeader	hdr(  _name, dtc.dataType(), thing.dimX(), thing.dimY(), 
                      thing.dimZ(), thing.dimT(), 
		      thing.sizeX(), thing.sizeY(), thing.sizeZ(), 
		      thing.sizeT() );

  const PythonHeader *ph = dynamic_cast<const PythonHeader *>( thing.header() );

  float minc_sizeX=thing.sizeX();
  float minc_sizeY=thing.sizeY();
  float minc_sizeZ=thing.sizeZ();
  float minc_sizeT=thing.sizeT();
  
  if( ph ) {
    hdr.copy( *ph );
    
    
    if(hdr.hasProperty( "MINC_voxel_size" )) {
      vector<float> minc_vs;
      
      ASSERT(hdr.getProperty( "MINC_voxel_size", minc_vs)) ;
    
      minc_sizeX=minc_vs[0];
      minc_sizeY=minc_vs[1];
      minc_sizeZ=minc_vs[2];
      minc_sizeT=minc_vs[3];
    }
  }  

  //Handle positive/negative voxel size.
  //Variables dirX, dirY and dirZ are defined as 1 if voxel size is positive and as -1 if voxel size is negative.
  int dirX= (int)rint(minc_sizeX/(thing.sizeX()));
  int dirY= (int)rint(minc_sizeY/(thing.sizeY()));
  int dirZ= (int)rint(minc_sizeZ/(thing.sizeZ()));

  //Variables X_pos, Y_pos and Z_pos are defined as 1 if voxel size is positive and 0 if voxel size is negative.
  //This is further used during the actual reading.
  int X_pos,Y_pos,Z_pos;
  if(dirX>0) X_pos=1; 
  else X_pos=0;
  if(dirY>0) Y_pos=1; 
  else Y_pos=0;
  if(dirZ>0) Z_pos=1; 
  else Z_pos=0;

  Real separations[MAX_DIMENSIONS];
    
  separations[3]=minc_sizeT;
  separations[2]=minc_sizeX;
  separations[1]=minc_sizeY;
  separations[0]=minc_sizeZ;

  set_volume_separations(volume,separations);
  
  for( int t=0; t<thing.dimT(); ++t )  
    for( int z=0; z<thing.dimZ(); ++z )  
      for( int y=0; y<thing.dimY(); ++y )  
	for( int x=0; x<thing.dimX(); ++x ) {
	  set_volume_real_value( volume,  (Z_pos)*thing.dimZ()-dirZ*z-(Z_pos), (Y_pos)*thing.dimY()-dirY*y-(Y_pos), (X_pos)*thing.dimX()-dirX*x-(X_pos), t, 0, thing(x,y,z,t));
		    
	}


  //1) Voxel to world transform
  Object transs;
  try
  {
    // convert AIMS transformaton to storage coords
    transs = hdr.getProperty( "transformations" );
    if( !transs.isNull() && transs->size() != 0 )
    {
      Object t0 = transs->getArrayItem( 0 );
      if( !t0.isNull() )
      {
        Motion tr( t0 );
        Motion vsm;
        vsm.rotation()(0,0) = thing.sizeX();
        vsm.rotation()(1,1) = thing.sizeY();
        vsm.rotation()(2,2) = thing.sizeZ();
        tr *= vsm;
        vector<float> s2mvec;
        if( hdr.getProperty( "storage_to_memory", s2mvec ) )
        {
          Motion s2m( s2mvec );
          tr *= s2m;
        }
        // handle MINC transform
        General_transform *gt=get_voxel_to_world_transform(volume);
        gt->type=LINEAR;
        gt->inverse_flag=FALSE;
        gt->linear_transform=(Transform*)malloc(sizeof(Transform));
        for(int i=0;i<3;i++)
        {
          for(int j=0;j<3;j++)
            gt->linear_transform->m[j][i] = tr.rotation()(i,j);
          gt->linear_transform->m[3][i] = tr.translation()[i];
          gt->linear_transform->m[i][3] = 0;
        }
        gt->linear_transform->m[3][3] = 1;
        convert_transform_to_starts_and_steps( gt,
                                               get_volume_n_dimensions(volume),
                                               separations,
                                               volume->spatial_axes,
                                               volume->starts,
                                               volume->separations,
                                               volume->direction_cosines );
      }
    }
  }
  catch( ... )
  {
    cout << "no transform or problem in it\n";
  }

  //2) Space name
  if(hdr.hasProperty("MINC_space_type")) {
    std::string space_name;
    ASSERT( hdr.getProperty( "MINC_space_type", space_name) );
    set_volume_space_type(volume,(char*)space_name.c_str());
  }
  //3) Other attributes
  bool ok = true;
  fdinhibitor fdi( 2 );
  fdi.close(); // inhibit output on stderr
  if( output_volume((char*)(_name.c_str()),
                nc_data_type,
                signed_flag,
                thing.minimum(),
                thing.maximum(),
                volume,
                NULL,
                NULL) != OK )
    ok = false;
  int mincid = -1;
  if( ok )
  {
    mincid=ncopen((char*)(_name.c_str()),NC_WRITE);
    if( mincid < 0 )
      ok = false;
  }
  fdi.open(); // allow again output on stderr
  if( ok )
  {
    ncredef(mincid);

    //ncopts=NC_VERBOSE;
    ncopts=0;

    SyntaxSet	*s = PythonHeader::syntax();
    Syntax	&sx = (*s)[ "__generic__" /*"PythonHeader"*/ ];

    hdr.writeMincAttribute(sx,mincid,"patient", NC_LONG, "rootvariable", "varid", "MINC_patient:varid");
    hdr.writeMincAttribute(sx,mincid,"patient", NC_LONG, "rootvariable", "vartype", "MINC_patient:vartype");
    hdr.writeMincAttribute(sx,mincid,"patient", NC_LONG, "rootvariable", "version", "MINC_patient:version");
    hdr.writeMincAttribute(sx,mincid,"patient", NC_LONG, "rootvariable", "full_name", "patient_id");
    hdr.writeMincAttribute(sx,mincid,"patient", NC_LONG, "rootvariable", "identification", "MINC_patient:identification");
    hdr.writeMincAttribute(sx,mincid,"patient", NC_LONG, "rootvariable", "birthdate", "MINC_patient:birthdate");
    hdr.writeMincAttribute(sx,mincid,"patient", NC_LONG, "rootvariable", "sex", "MINC_patient:sex");
    hdr.writeMincAttribute(sx,mincid,"patient", NC_LONG, "rootvariable", "weight", "MINC_patient:weight");

    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "varid", "MINC_study:varid");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "vartype", "MINC_study:vartype");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "version", "MINC_study:version");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "start_time", "MINC_study:start_time");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "modality", "modality");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "institution", "MINC_study:institution");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "station_id", "MINC_study:station_id");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "procedure", "MINC_study:procedure");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "study_id", "study_id");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "acquisition_id", "MINC_study:acquisition_id");

    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "varid", "MINC_acquisition:varid");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "vartype", "MINC_acquisition:vartype");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "version", "MINC_acquisition:version");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "scanning_sequence", "MINC_acquisition:scanning_sequence");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "repetition_time", "tr");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "echo_time", "te");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "inversion_time", "MINC_acquisition:inversion_time");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "flip_angle", "flip_angle");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "num_averages", "MINC_acquisition:num_averages");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "imaging_frequency", "MINC_acquisition:imaging_frequency");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "imaged_nucleus", "MINC_acquisition:imaged_nucleus");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "comments", "MINC_acquisition:comments");

    hdr.writeMincAttribute(sx,mincid,"image-min", NC_DOUBLE, "image", "units", "MINC_image-min:units");
    hdr.writeMincAttribute(sx,mincid,"image-max", NC_DOUBLE, "image", "units", "MINC_image-max:units");

    //Siemens sonata
    hdr.writeMincAttribute(sx,mincid,"patient", NC_LONG, "rootvariable", "age", "MINC_patient:age");


    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "start_date", "MINC_study:start_date");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "manufacturer", "MINC_study:manufacturer");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "model", "MINC_study:model");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "field_value", "MINC_study:field_value");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "software_version", "MINC_study:software_version");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "serial_no", "MINC_study:serial_no");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "performing_physician", "MINC_study:performing_physician");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "operator", "MINC_study:operator");
    hdr.writeMincAttribute(sx,mincid,"study", NC_LONG, "rootvariable", "calibration_date", "MINC_study:calibration_date");

    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "acquisition_id", "MINC_acquisition:acquisition_id");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "start_time", "MINC_acquisition:start_time");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "protocol_name", "MINC_acquisition:protocol_name");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "receive_coil", "MINC_acquisition:receive_coil");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "transmit_coil", "MINC_acquisition:transmit_coil");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "echo_number", "MINC_acquisition:echo_number");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "slice_thickness", "MINC_acquisition:slice_thickness");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "num_slices", "MINC_acquisition:num_slices");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "num_dyn_scans", "MINC_acquisition:num_dyn_scans");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "window_center", "MINC_acquisition:window_center");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "window_width", "MINC_acquisition:window_width");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "num_phase_enc_steps", "MINC_acquisition:num_phase_enc_steps");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "percent_sampling", "MINC_acquisition:percent_sampling");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "percent_phase_fov", "MINC_acquisition:percent_phase_fov");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "pixel_bandwidth", "MINC_acquisition:pixel_bandwidth");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "phase_enc_dir", "MINC_acquisition:phase_enc_dir");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "SAR", "MINC_acquisition:SAR");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "mr_acq_type", "MINC_acquisition:mr_acq_type");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "image_type", "MINC_acquisition:image_type");
    hdr.writeMincAttribute(sx,mincid,"acquisition", NC_LONG, "rootvariable", "MrProt_dump", "MINC_acquisition:MrProt_dump");

    hdr.writeMincAttribute(sx,mincid,"processing", NC_LONG, "rootvariable", "transformation0-filename", "MINC_processing:transformation0-filename");
    hdr.writeMincAttribute(sx,mincid,"processing", NC_LONG, "rootvariable", "transformation0-filedata", "MINC_processing:transformation0-filedata");


    hdr.writeMincHistory(mincid);
    hdr.writeMinf( FileUtil::removeExtension( _name ) + ".mnc.minf" );
    miclose(mincid);
  }

  //std::cout << "Delete starts\n";
  delete_string(dim_names[0]);
  delete_string(dim_names[1]);
  delete_string(dim_names[2]);
  delete_string(dim_names[3]);
  //std::cout << "Delete ends\n";
  delete_volume(volume);
  //std::cout << "Delete ends2\n";

  if( !ok )
    io_error::launchErrnoExcept( _name );
  return( true );
}

}

using namespace aims ;

template class MincWriter<uint8_t> ;
template class MincWriter<int8_t> ;
template class MincWriter<uint16_t> ;
template class MincWriter<int16_t> ;
template class MincWriter<uint32_t> ;
template class MincWriter<int32_t> ;
template class MincWriter<float> ;
template class MincWriter<double> ;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class MincWriter<char> ;
#endif
template class MincWriter<long> ;
template class MincWriter<unsigned long> ;
