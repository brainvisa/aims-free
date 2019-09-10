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
#include <aims/def/general.h>
#include <aims/resampling/motion.h>
#include <aims/io/mincheader.h>
#include <aims/resampling/standardreferentials.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/thread/mutex.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

using namespace aims;
using namespace carto;
using namespace std;

/*
namespace
{
  const char *type_names[] = {
    NULL, "byte", "char", "short", "long", "float", "double"
  };

  const std::string str_type_names[] = {
    "null", "byte", "char", "short", "long", "float", "double"
  };
}
*/


//This function is used to make MINC functions behave silently
void MincHeader::my_empty_print_error(char *message)
{
  /* do nothing. This is just to avoid the warning...*/
  message=NULL;
}


set<string> MincHeader::extensions() const
{
  set<string> exts;
  exts.insert( ".mnc" );
  exts.insert( ".mnc.gz" );
  return exts;
}


Mutex & MincHeader::mincMutex()
{
  // Must be initialized (generally in main thread) before using concurrently
  static Mutex mutex( Mutex::Recursive );
  return mutex;
}


// Reads a MINC attribute and writes it as an AIMS attribute.
// A MINC attribute is always associated to a MINC variable.
//
// Parameters:
// sx : The AIMS syntax defining the types of the AIMS attributes.
// mincid : MINC file id.
// minc_var : Name of the MINC variable.
// minc_att : Name of the MINC attribute.
// aism_att_name : Name of the AIMS attribute.
//
// Return value:
//  0 : success
// -1 : no such variable
// -2 : no such attribute
// -3 : wrong attribute type
int MincHeader::readMincAttribute(Syntax &sx, int mincid, std::string minc_var, std::string minc_att, std::string aims_att_name)  {

  //Get variable id from name
  int varid=ncvarid(mincid, minc_var.c_str()); 
  
  // If this variable does not exist
  if(varid==-1) {
    return(-1); 
  }

  nc_type att_datatype;
  int att_length;
  // Get attribute id, type and length from name
  int attid=ncattinq(mincid, varid, minc_att.c_str(), &att_datatype, &att_length);
  
  //If this attribute does not exist
  if(attid==-1) {
    return(-2);
  }
  
  // If this MINC attribute is a string
  if (att_datatype == NC_CHAR) {

    // If the corresponding AIMS attribute is not a string according to the syntax sx
    if(sx[aims_att_name].type!="string") {
      return(-3);
    }
    
    char *cdata;
    cdata = (char*)malloc((att_length+1)*sizeof(char));
    miattgetstr(mincid, varid, (char*)minc_att.c_str(), att_length+1, cdata);
    
    std::string str_cdata=cdata;
    setProperty( aims_att_name, str_cdata );
    
    free(cdata);
  }
  // The attribute is then of numerical type.
  else {
    double *ddata;
    ddata = (double*)malloc(att_length * sizeof(double));
    miattget(mincid, varid, (char*)minc_att.c_str(), NC_DOUBLE, att_length,
             ddata, NULL);

    // If the attribute length is 1, then it is stored as a single float
    if(att_length==1) {
      
      // If the corresponding AIMS attribute is not a float according to the syntax sx
      if(sx[aims_att_name].type!="float") {
        free(ddata);
        return(-3);
      }

      float float_ddata=ddata[0];
      setProperty( aims_att_name, float_ddata );
    }
    // The attribute is then a vector of floats.
    else {
      
      // If the corresponding AIMS attribute is not a vector of floats according to the syntax sx      
      if(sx[aims_att_name].type!="float_vector") {
        free(ddata);
        return(-3);
      }
      vector<float> vec_ddata;
      
      for (int iiatt=0; iiatt<att_length; iiatt++) {
        vec_ddata.push_back(ddata[iiatt]);
      }
      
      setProperty( aims_att_name, vec_ddata );


    }
    free(ddata);
  }  
  return(0);
}

// Read the MINC history and store it in the AIMS attribute "MINC_history".
// In the future, an history attribute might be added to AIMS and then it would be good to use it also to store the MINC history.
//
// Parameters:
// mincid : MINC file id.
//
// Return value:
//  0 : success
// -1 : no history attribute
// -3 : wrong attribute type

int MincHeader::readMincHistory(int mincid) {
  int varid=NC_GLOBAL;

  nc_type att_datatype;
  int att_length;
  int attid=ncattinq(mincid, varid, "history", &att_datatype, &att_length);
  
  // If there is no "history" attribute
  if(attid==-1) {
    return(-1);
  }
  std::string history;
  // If the "history" attribute is not of type "string"

  if (att_datatype == NC_CHAR) {
    char *cdata;
    cdata = (char*)malloc((att_length+1)*sizeof(char));
    miattgetstr(mincid, varid, const_cast<char*>( "history" ), att_length+1, cdata);
    
    history=cdata;
    free(cdata);
  }
  else {
    return(-3);
  }  
  
  // Add a new line to history specifying the date and AIMS conversion
  time_t tod;
  tod=time(NULL);
  char *char_time=ctime(&tod);
  
  char_time[strlen(char_time)-1]='\0';
  
  history=history+char_time+">>> AIMS read MINC file ("+_name+")\n";
  
  setProperty("MINC_history",history);
  return(0);
 
}


// Reads an AIMS attribute and writes it as a MINC attribute.
// A MINC attribute is always associated to a MINC variable.
// If the MINC variable does not exist, it is created. A MINC variable is always associated to a parent variable which must exist and be given as a parameter.
//
// Parameters:
// sx : The AIMS syntax defining the types of the AIMS attributes.
// mincid : MINC file id.
// minc_var : Name of the MINC variable.
// minc_var_type : Type of the MINC variable.
// minc_var_parent : Name of the parent MINC variable.
// minc_att : Name of the MINC attribute.
// aism_att_name : Name of the AIMS attribute.
//
// Return value:
//  0 : success
// -1 : this AIMS attribute does not exist
// -2 : MINC parent variable does not exist
// -3 : wrong attribute type
int MincHeader::writeMincAttribute(Syntax &sx, int mincid, std::string minc_var, nc_type minc_var_type, std::string minc_var_parent, std::string minc_att, std::string aims_att_name) {
 
  //std::cout << sx[aims_att_name].type << "\n";

  if(!hasProperty( aims_att_name )) {
    return(-1);
  }  
    
  int varid=ncvarid(mincid, minc_var.c_str()); 

  //If the variable does not exist, create it
  if(varid==-1) {
    varid=ncvardef(mincid,minc_var.c_str(), minc_var_type, 0, NULL);
    ncattput(mincid, varid, MIparent, NC_CHAR, minc_var_parent.length()+1, minc_var_parent.c_str());

    int parent_varid=ncvarid(mincid, minc_var_parent.c_str()); 

    if(parent_varid==-1) {
      return(-2);
    }
    
    nc_type child_att_datatype;
    int child_att_length;
    int child_attid=ncattinq(mincid, parent_varid, MIchildren, &child_att_datatype, &child_att_length);
  
    string children="";
    //std::cout << "Child att : " << child_attid <<"\n";
    //If the children attribute already exist, we need to read its current value to preserve it.
    if(child_attid!=-1) {
      
      char *cdata;
      cdata = (char*)malloc((child_att_length+1)*sizeof(char));
      miattgetstr(mincid, parent_varid, const_cast<char*>( MIchildren ),
        child_att_length+1, cdata);
      children=cdata;
      children=children+"\n";
      free(cdata);
    }
    children=children+minc_var;
    //std::cout << "Children : " <<children <<"\n";
    ncattput(mincid, parent_varid, MIchildren, NC_CHAR, children.length()+1, children.c_str());
  }

  if(sx[aims_att_name].type=="string") {

    std::string aims_value;
  
    if(getProperty( aims_att_name, aims_value)==false ) {
      return(-1);
    }  
    ncattput(mincid, varid, minc_att.c_str(), NC_CHAR, aims_value.length()+1, aims_value.c_str());
  }

  if(sx[aims_att_name].type=="float") {
    float aims_value;
  
    if(getProperty( aims_att_name, aims_value)==false ) {
      return(-1);
    }  
    miattputdbl(mincid, varid, (char*)minc_att.c_str(), (double)aims_value);

  }


  return(0);
  
}

// Read the  AIMS attribute "MINC_history" and store it in the MINC history.
// In the future, an history attribute might be added to AIMS and then it would be good to use it also to store the MINC history.
//
// Parameters:
// mincid : MINC file id.
//
// Return value:
//  0 : success
int MincHeader::writeMincHistory(int mincid) {
  int varid=NC_GLOBAL;

  std::string history="";
  
  if(hasProperty("MINC_history")) {
    getProperty( "MINC_history", history);
  }
  
  time_t tod;
  tod=time(NULL);
  char *char_time=ctime(&tod);
  
  char_time[strlen(char_time)-1]='\0';
  
  history=history+char_time+">>> AIMS write MINC file ("+_name+")\n";
  miattputstr(mincid, varid, const_cast<char*>( "history" ), (char*) history.c_str());
  return(0);
 
}

namespace
{
  inline float checkbounds( double x )
  {
    if( std::isnan(x) || std::isnan(float(x))
        || x == numeric_limits<double>::infinity()
        || x < -numeric_limits<float>::max() 
        || x > numeric_limits<float>::max() )
      return 1.F;
    return float(x);
  }
}


void MincHeader::read()
{
  // cout << "MincHeader::read(), file : " << _name << endl;

  if( FileUtil::fileStat( name() ).empty() )
  {
    if( name().length() < 4 || name().substr( name().length()-4, 4 ) != ".mnc" )
      if( !FileUtil::fileStat( name() + ".mnc" ).empty() )
        _name = name() + ".mnc";
      else
        throw file_not_found_error( name() );
    else
      throw file_not_found_error( name() );
  }

  if( ( _name.length() >= 4 && _name.substr( _name.length()-4, 4 ) == ".nii" )
      || ( _name.length() >= 7
           && _name.substr( _name.length()-7, 7 ) == ".nii.gz" ) )
    // refuse reading nifti: we handle it in our reader.
    throw wrong_format_error( name() );

  // Work aroud BUG in netCDF which incorrectly uses assert and
  // aborts on empty files
  struct stat   st;
  stat( name().c_str(), &st ); // succeeds because it has already been done
  if( st.st_size == 0 )
    throw eof_error( name() );

  // MINC does not support windows filenames, so we replace it using linux
  // separators
  string fname = FileUtil::linuxFilename(_name);

  VIO_Volume volume;
  vector<VIO_STR> dim_names( VIO_MAX_DIMENSIONS );
  VIO_STR fileName = create_string ( const_cast<char *>( fname.c_str() ) );

  dim_names[0] = create_string( const_cast<char *>( MIxspace ) );
  dim_names[1] = create_string( const_cast<char *>( MIyspace ) );
  dim_names[2] = create_string( const_cast<char *>( MIzspace ) );
  dim_names[3] = create_string( const_cast<char *>( MItime ) );
  for( unsigned i=4; i<VIO_MAX_DIMENSIONS; ++i )
    dim_names[i] = 0;

  volume_input_struct input_info;
  //Read the header of the MINC file. This volume is not allocated and its content is not read from disk.
  set_print_error_function(my_empty_print_error);

  mincMutex().lock();
  // avoid printing anything from Minc/NetCDF lib  
  milog_init(CARTOBASE_STREAM_NULLDEVICE);
  milog_set_verbosity(0);
  int   status = 0;
  try
  {
  status = start_volume_input( fileName, 0, &dim_names[0],
                               MI_ORIGINAL_TYPE, TRUE,
                               0.0, 0.0, TRUE, &volume,
                               (minc_input_options *) NULL, &input_info );
  milog_init("stderr");
  
  if(status != VIO_OK)
    throw wrong_format_error( fileName );

#ifdef MINC_NIFTI_SUPPORT
  if( input_info.file_format == NII_FORMAT )
    // refuse reading nifti: we handle it in our reader.
    throw wrong_format_error( fileName );
#endif

  if(volume->nc_data_type==NC_BYTE && volume->signed_flag==FALSE) 
    {
      _type = "U8";
      //std::cout << _type << "\n";
    }
  else if(volume->nc_data_type==NC_BYTE && volume->signed_flag==TRUE)
    {
      _type = "S8";
      //std::cout << _type << "\n";
    }
  else if(volume->nc_data_type==NC_SHORT && volume->signed_flag==FALSE)
    {
      _type = "U16";
      //std::cout << _type << "\n";
    }
  else if(volume->nc_data_type==NC_SHORT && volume->signed_flag==TRUE)
    {
      _type = "S16";
      //std::cout << _type << "\n";
    }
  else if(volume->nc_data_type==NC_LONG && volume->signed_flag==FALSE)
    {
      _type = "U32";
      //std::cout << _type << "\n";
    }
  else if(volume->nc_data_type==NC_LONG && volume->signed_flag==TRUE)
    {
      _type = "S32";
      //std::cout << _type << "\n";
    }
  else if(volume->nc_data_type==NC_FLOAT)
    {
      _type = "FLOAT";
      //std::cout << _type << "\n";
    }
  else if(volume->nc_data_type==NC_DOUBLE)
    {
      _type = "DOUBLE";
      //std::cout << _type << "\n";
    }
  else
    throw wrong_format_error( fileName );

  //Read the volume size
  int n_dimensions = get_volume_n_dimensions(volume);
  int       sizes[VIO_MAX_DIMENSIONS];
  get_volume_sizes( volume, sizes );
  if( n_dimensions >= 4 )
    _dimT = sizes[3];
  else
    _dimT = 1;
  if( n_dimensions >= 3 )
    _dimZ = sizes[2];
  else
    _dimZ = 1;
  if( n_dimensions >= 2 )
    _dimY = sizes[1];
  else
    _dimY = 1;
  if( n_dimensions >= 1 )
    _dimX = sizes[0];
  else
    _dimX = 1;

  // cout << "read " << n_dimensions << " dims: " << Point4d(_dimX, _dimY, _dimZ, _dimT) << endl;

  //Read voxel size
  //In MINC, voxel size can be positive or negative. Here we take the absolute value of the voxel size and the case of negative increment steps (negative voxel sizes) is treated when the volume is read (in MincReader).
  _sizeT = fabs(volume->separations[3]);
  //std::cout << "dx="<< volume->separations[2] << ", dy="<< volume->separations[1] << ", dz="<< volume->separations[0]<<"\n";
  if( _name.substr( _name.length() - 4, 4 ) == ".mgz"
      || _name.substr( _name.length() - 4, 4 ) == ".mgh" )
  {
    // freesurfer mgz / mgh files seem to be in a different order
    // (is this a bug in minc io or in our interpretation ?)
    _sizeX = fabs(volume->separations[2]);
    _sizeY = fabs(volume->separations[1]);
    _sizeZ = fabs(volume->separations[0]);
  }
  else
  {
    _sizeX = fabs(volume->separations[0]);
    _sizeY = fabs(volume->separations[1]);
    _sizeZ = fabs(volume->separations[2]);
  }

  if(n_dimensions==5) {
    throw wrong_format_error( fileName );
  }
  if(n_dimensions<=3) {
    _dimT=1;
    _sizeT=1.0;
    if(n_dimensions<=2) {
      _dimZ=1;
      _sizeZ=1.0;
      if(n_dimensions==1) {
        _dimY=1;
        _sizeY=1.0;
      }
    }
  }
  
  //Read voxel range and real range. Only for debugging purpose: the values are not stored in the class.
  VIO_Real vmin,vmax,rmin,rmax;
  get_volume_voxel_range(volume,&vmin,&vmax);
  //std::cout << "Voxel range : " << vmin << ";" <<vmax<<"\n";
  get_volume_real_range(volume,&rmin,&rmax);
  // std::cout << "Real range : " << rmin << ";" <<rmax<<"\n";

  _pdt = vector<string>();
  _pdt.reserve( 8 );
  set<string> types;
  types.insert( "U8" );
  types.insert( "S8" );
  types.insert( "U16" );
  types.insert( "S16" );
  types.insert( "U32" );
  types.insert( "S32" );
  types.insert( "FLOAT" );
  types.insert( "DOUBLE" );

  if( ( rmin != vmin || rmax != vmax ) && _type != "DOUBLE" )
    {
      // voxel and pixel ranges differ: read as float preferably
      _pdt.push_back( "FLOAT" );
      types.erase( "FLOAT" );
      _pdt.push_back( _type );
      types.erase( _type );
      _type = "FLOAT";
    }
  else
    {
      _pdt.push_back( _type );
      types.erase( _type );
    }
  set<string>::iterator is, es = types.end();
  for( is=types.begin(); is!=es; ++is )
    _pdt.push_back( *is );


  setProperty( "file_type", string( "MINC" ) );

  std::vector<int> dims;
  dims.push_back( dimX() );
  dims.push_back( dimY() );
  dims.push_back( dimZ() );
  dims.push_back( dimT() );
  setProperty( "volume_dimension", dims );


  vector<float> vs;
  vs.push_back( sizeX() );
  vs.push_back( sizeY() );
  vs.push_back( sizeZ() );
  vs.push_back( sizeT() );
  setProperty( "voxel_size", vs );

  vector<float> minc_vs;
  minc_vs.push_back( checkbounds( volume->separations[2] ) );
  minc_vs.push_back( checkbounds( volume->separations[1] ) );
  minc_vs.push_back( checkbounds( volume->separations[0] ) );
  minc_vs.push_back( checkbounds( volume->separations[3] ) );
  setProperty( "MINC_voxel_size", minc_vs );



  setProperty( "data_type", _type );


  //OC : Meta-info: should we add something to read the rest of the minc header ?
  
  //1) Voxel to world tranform
  //2) Space name
  //3) All other attributes


  //1) Voxel to world tranform
  //Handle positive/negative voxel size.
  Motion s2m;
  s2m.rotation()( 0, 0 )
      = -(int)(volume->separations[2]/fabs(volume->separations[2]));
  s2m.rotation()( 1, 1 )
      = -(int)(volume->separations[1]/fabs(volume->separations[1]));
  s2m.rotation()( 2, 2 )
      = -(int)(volume->separations[0]/fabs(volume->separations[0]));

  if( s2m.rotation()( 0, 0 ) < 0 )
    s2m.translation()[0] = _dimX-1;
  if( s2m.rotation()( 1, 1 ) < 0 )
    s2m.translation()[1] = _dimY-1;
  if( s2m.rotation()( 2, 2 ) < 0 )
    s2m.translation()[2] = _dimZ-1;
  setProperty( "storage_to_memory", s2m.toVector() );

  VIO_General_transform *gt=get_voxel_to_world_transform(volume);

  if(gt->type!=LINEAR) {
    string transfo_type="NOT SUPPORTED !!";
    setProperty ( "MINC_general_transform:type", transfo_type );
    //std::cout << "MINC Non-linear voxel to world transform are not yet supported !!\n";
  }
  else
  {
    string transfo_type="linear";
    setProperty ( "MINC_general_transform:type", transfo_type );
    //std::cout << gt->inverse_flag << "\n";

    //std::cout   << "Writing MINC_general_transform:inverse_flag\n";
    int inv_flag=gt->inverse_flag;
    setProperty ( "MINC_general_transform:inverse_flag",  inv_flag );

    vector<float> transfo( 16 );
    if(gt->inverse_flag==FALSE) {
      for(int i=0;i<4;i++) {
        for(int j=0;j<4;j++) {
          // transfo1.push_back( Transform_elem(*(gt->linear_transform),j,i) );
          transfo[i*4+j] = Transform_elem(*(gt->linear_transform),i,j);
        }
      }
    } else {
      for(int i=0;i<4;i++) {
        for(int j=0;j<4;j++) {
          transfo[i*4+j]
              = Transform_elem(*(gt->inverse_linear_transform),i,j);
        }
      }
    }
    Motion tr( transfo );
    tr *= s2m.inverse();
    Motion vsmi;
    vsmi.rotation()(0,0) = 1./_sizeX;
    vsmi.rotation()(1,1) = 1./_sizeY;
    vsmi.rotation()(2,2) = 1./_sizeZ;
    tr *= vsmi;
    vector< vector<float> > transs;
    transs.push_back( tr.toVector() );
    setProperty( "transformations", transs );
    vector<string> refs;
    // check to recognize MNI template referential
    if( _dimX * _sizeX == 182 && _dimY * _sizeY == 218
        && _dimZ * _sizeZ == 182
        && tr.transform( Point3df( 0, 0, 0 ) ) == Point3df( 90, 90, 108 ) )
      refs.push_back( StandardReferentials::mniTemplateReferential() );
    else
      refs.push_back( "Scanner-based anatomical coordinates" );
    setProperty( "referentials", refs );
  }

  //2) Space name
  std::string space_name=get_volume_space_type(volume);
  setProperty( "MINC_space_type", space_name);
  
  //3) Other attributes

  // std::cout << "MincHeader::read() 4\n";
  if( input_info.file_format == MNC_FORMAT
#ifdef MINC_MNC2_SUPPORT
      || input_info.file_format == MNC2_FORMAT
#endif
    )
  {
    // only allowed for "real" minc
    Minc_file minc_file = get_volume_input_minc_file(&input_info);
    if( !minc_file )
      throw wrong_format_error( fileName );

    int mincid = get_minc_file_id(minc_file );

    //ncopts=NC_VERBOSE;
    ncopts=0;

    SyntaxSet *s = PythonHeader::syntax();
    Syntax &sx = (*s)[ "__generic__" /*"PythonHeader"*/ ];

    readMincAttribute(sx, mincid, "patient", "varid", "MINC_patient:varid");
    readMincAttribute(sx, mincid, "patient", "vartype", "MINC_patient:vartype");
    readMincAttribute(sx, mincid, "patient", "version", "MINC_patient:version");
    readMincAttribute(sx, mincid, "patient", "full_name", "patient_id");
    readMincAttribute(sx, mincid, "patient", "identification", "MINC_patient:identification");
    readMincAttribute(sx, mincid, "patient", "birthdate", "MINC_patient:birthdate");
    readMincAttribute(sx, mincid, "patient", "sex", "MINC_patient:sex");
    readMincAttribute(sx, mincid, "patient", "weight", "MINC_patient:weight");

    readMincAttribute(sx, mincid, "study", "varid", "MINC_study:varid");
    readMincAttribute(sx, mincid, "study", "vartype", "MINC_study:vartype");
    readMincAttribute(sx, mincid, "study", "version", "MINC_study:version");
    readMincAttribute(sx, mincid, "study", "start_time", "MINC_study:start_time");
    readMincAttribute(sx, mincid, "study", "modality", "modality");
    readMincAttribute(sx, mincid, "study", "institution", "MINC_study:institution");
    readMincAttribute(sx, mincid, "study", "station_id", "MINC_study:station_id");
    readMincAttribute(sx, mincid, "study", "procedure", "MINC_study:procedure");
    readMincAttribute(sx, mincid, "study", "study_id", "study_id");
    readMincAttribute(sx, mincid, "study", "acquisition_id", "MINC_study:acquisition_id");

    readMincAttribute(sx, mincid, "acquisition", "varid", "MINC_acquisition:varid");
    readMincAttribute(sx, mincid, "acquisition", "vartype", "MINC_acquisition:vartype");
    readMincAttribute(sx, mincid, "acquisition", "version", "MINC_acquisition:version");
    readMincAttribute(sx, mincid, "acquisition", "scanning_sequence", "MINC_acquisition:scanning_sequence");
    readMincAttribute(sx, mincid, "acquisition", "repetition_time", "tr");
    readMincAttribute(sx, mincid, "acquisition", "echo_time", "te");
    readMincAttribute(sx, mincid, "acquisition", "inversion_time", "MINC_acquisition:inversion_time");
    readMincAttribute(sx, mincid, "acquisition", "flip_angle", "flip_angle");
    readMincAttribute(sx, mincid, "acquisition", "num_averages", "MINC_acquisition:num_averages");
    readMincAttribute(sx, mincid, "acquisition", "imaging_frequency", "MINC_acquisition:imaging_frequency");
    readMincAttribute(sx, mincid, "acquisition", "imaged_nucleus", "MINC_acquisition:imaged_nucleus");
    readMincAttribute(sx, mincid, "acquisition", "comments", "MINC_acquisition:comments");

    readMincAttribute(sx, mincid, "image-min", "units", "MINC_image-min:units");
    readMincAttribute(sx, mincid, "image-max", "units", "MINC_image-max:units");


    readMincAttribute(sx, mincid, "patient", "age", "MINC_patient:age");

    readMincAttribute(sx, mincid, "study", "start_date", "MINC_study:start_date");
    readMincAttribute(sx, mincid, "study", "manufacturer", "MINC_study:manufacturer");
    readMincAttribute(sx, mincid, "study", "model", "MINC_study:model");
    readMincAttribute(sx, mincid, "study", "field_value", "MINC_study:field_value");
    readMincAttribute(sx, mincid, "study", "software_version", "MINC_study:software_version");
    readMincAttribute(sx, mincid, "study", "serial_no", "MINC_study:serial_no");
    readMincAttribute(sx, mincid, "study", "performing_physician", "MINC_study:performing_physician");
    readMincAttribute(sx, mincid, "study", "operator", "MINC_study:operator");
    readMincAttribute(sx, mincid, "study", "calibration_date", "MINC_study:calibration_date");

    readMincAttribute(sx, mincid, "acquisition", "acquisition_id", "MINC_acquisition:acquisition_id");
    readMincAttribute(sx, mincid, "acquisition", "start_time", "MINC_acquisition:start_time");
    readMincAttribute(sx, mincid, "acquisition", "protocol_name", "MINC_acquisition:protocol_name");
    readMincAttribute(sx, mincid, "acquisition", "receive_coil", "MINC_acquisition:receive_coil");
    readMincAttribute(sx, mincid, "acquisition", "transmit_coil", "MINC_acquisition:transmit_coil");
    readMincAttribute(sx, mincid, "acquisition", "echo_number", "MINC_acquisition:echo_number");
    readMincAttribute(sx, mincid, "acquisition", "slice_thickness", "MINC_acquisition:slice_thickness");
    readMincAttribute(sx, mincid, "acquisition", "num_slices", "MINC_acquisition:num_slices");
    readMincAttribute(sx, mincid, "acquisition", "num_dyn_scans", "MINC_acquisition:num_dyn_scans");
    readMincAttribute(sx, mincid, "acquisition", "window_center", "MINC_acquisition:window_center");
    readMincAttribute(sx, mincid, "acquisition", "window_width", "MINC_acquisition:window_width");
    readMincAttribute(sx, mincid, "acquisition", "num_phase_enc_steps", "MINC_acquisition:num_phase_enc_steps");
    readMincAttribute(sx, mincid, "acquisition", "percent_sampling", "MINC_acquisition:percent_sampling");
    readMincAttribute(sx, mincid, "acquisition", "percent_phase_fov", "MINC_acquisition:percent_phase_fov");
    readMincAttribute(sx, mincid, "acquisition", "pixel_bandwidth", "MINC_acquisition:pixel_bandwidth");
    readMincAttribute(sx, mincid, "acquisition", "phase_enc_dir", "MINC_acquisition:phase_enc_dir");
    readMincAttribute(sx, mincid, "acquisition", "SAR", "MINC_acquisition:SAR");
    readMincAttribute(sx, mincid, "acquisition", "mr_acq_type", "MINC_acquisition:mr_acq_type");
    readMincAttribute(sx, mincid, "acquisition", "image_type", "MINC_acquisition:image_type");
    readMincAttribute(sx, mincid, "acquisition", "MrProt_dump", "MINC_acquisition:MrProt_dump");

    readMincAttribute(sx, mincid, "processing", "transformation0-filename", "MINC_processing:transformation0-filename");
    readMincAttribute(sx, mincid, "processing", "transformation0-filedata", "MINC_processing:transformation0-filedata");

    readMincHistory(mincid);
  }

  readMinf( removeExtension( fname ) + extension() + ".minf" );

  //Free memory
  delete_volume(volume);
  delete_string(fileName);
  delete_string(dim_names[0]);
  delete_string(dim_names[1]);
  delete_string(dim_names[2]);
  delete_string(dim_names[3]);
  delete_general_transform(gt);
  delete_volume_input(&input_info);
  }
  catch( ... )
  {
    milog_init("stderr");
    mincMutex().unlock();
    throw;
  }
  // std::cout << "MincHeader::read() done\n";

  mincMutex().unlock();
}

void MincHeader::write()
{
}





  /* CODE TO READ ALL ATTRIBUTES IN MINC FILE
     CURRENTLY UNUSED


  if(ncvarid(mincid,"patient")) {
    std::coutn<< "No patient variable\n";
  }

  char var_name[MAX_NC_NAME];
  char dim_name[MAX_NC_NAME];
  char att_name[MAX_NC_NAME];
  
  nc_type var_datatype;
  nc_type att_datatype;

  long var_length,row_length, length;
  int att_length;

  int nvars;
  int ndims, my_dims[MAX_VAR_DIMS];
  int natts;

  long start[MAX_VAR_DIMS], count[MAX_VAR_DIMS];
  //Read number of variables
  ncinquire(mincid, NULL, &nvars, NULL, NULL);
  
  (void) printf("Number of variables: %d\n", nvars);
  //Read all variables
  for (int varid=0; varid<nvars; varid++) {
    //Read variable name
    ncvarinq(mincid, varid, var_name, NULL, NULL, NULL, NULL);
    string str_var_name=var_name;
    (void) printf("%s ", var_name);

    //Read variable type
    ncvarinq(mincid, varid, NULL, &var_datatype, NULL, NULL, NULL);
    (void) printf(" %s", type_names[var_datatype]);
    setProperty(sx,  "MINC_"+str_var_name+":type", str_type_names[var_datatype]);

    //Read number of dimensions
    ncvarinq(mincid, varid, NULL, NULL, &ndims, my_dims, NULL);
    (void) printf(" %d\n", ndims);
    setProperty(sx,  "MINC_"+str_var_name+":ndims", ndims);

    //Read dimensions names
    vector<std::string> dim_names;
    for (int idim=0; idim<ndims; idim++) {
      ncdiminq(mincid, my_dims[idim], dim_name, NULL);
      dim_names.push_back(dim_name);
      (void) printf("\tDim %d: %s\n", idim, dim_name);
    }
    setProperty(sx,  "MINC_"+str_var_name+":dim_names", dim_names);
    

    //Read number of attributes
    ncvarinq(mincid, varid, NULL, NULL, NULL, NULL, &natts);

    //Read all attributes
    for (int iatt=0; iatt<natts; iatt++) {
      //Read attribute name
      ncattname(mincid, varid, iatt, att_name);

      std::string str_att_name=att_name;

      //Read attribute type
      ncattinq(mincid, varid, att_name, &att_datatype, NULL);
      (void) printf("\t%s:%s(%s) = ", var_name, att_name, type_names[att_datatype]);

       
      //Read attribute value
      
      ncattinq(mincid, varid, att_name, &att_datatype, &att_length);
      if (att_datatype == NC_CHAR) {
    char *cdata;
    cdata = (char*)malloc((att_length+1)*sizeof(char));
    miattgetstr(mincid, varid, att_name, att_length+1, cdata);
    (void) printf("%s\n", cdata);
    
    std::string str_cdata=cdata;
    setProperty(sx,  "MINC_"+str_var_name+":"+str_att_name, str_cdata );

    free(cdata);
      }
      else {
    double *ddata;
    ddata = (double*)malloc(att_length * sizeof(double));
    miattget(mincid, varid, att_name, NC_DOUBLE, att_length,
         ddata, NULL);
    
    vector<float> vec_ddata;

    for (int iiatt=0; iiatt<att_length; iiatt++) {
      (void) printf("%.20g ", ddata[iiatt]);
      vec_ddata.push_back(ddata[iiatt]);
    }

    setProperty(sx,  "MINC_"+str_var_name+":"+str_att_name, vec_ddata );
    
    (void) printf("\n");
    free(ddata);
      }
    }
    
    //Read variable values
    var_length = 1;
    if(strcmp(var_name,"image")!=0) {
      for (int idim=0; idim<ndims; idim++) {
    ncdiminq(mincid, my_dims[idim], NULL, &length);
    start[idim] = 0;
    count[idim] = length;
    var_length *= length;
    if (idim==ndims-1)
      row_length = length;
      }
      if (var_datatype==NC_CHAR) {
    char *cdata;
    
    cdata = (char*)malloc((var_length+1)*sizeof(char));
    ncvarget(mincid, varid, start, count, cdata);
    cdata[var_length]=0;
    (void) printf("\tValue ");
    for (int ival=0; ival<var_length; ival++) {
      (void) putchar((int) cdata[ival]);
      if (((ival+1) % row_length) == 0)
        (void) putchar((int)'\n');
    }
    std::string str_cdata=cdata;
    setProperty(sx,  "MINC_"+str_var_name+":value", str_cdata );
    free(cdata);
      }
      else {
    double *ddata;
    ddata = (double*)malloc(var_length*sizeof(double));
    mivarget(mincid, varid, start, count, 
         NC_DOUBLE, NULL, ddata);
    (void) printf("\tValue ");
    vector<float> vec_ddata;

    for (int ival=0; ival<var_length; ival++) {
      (void) printf("%.20g\n", ddata[ival]);
      vec_ddata.push_back(ddata[ival]);
    }
    setProperty(sx,  "MINC_"+str_var_name+":value", vec_ddata );
    free(ddata);
      }
      (void) printf("\n");
    }
  }
 
  //Read global attributes
  int varid=NC_GLOBAL;
  
  //Read number of attributes
  ncinquire(mincid, NULL, NULL, &natts, NULL);
  //Read all attributes
  for (int iatt=0; iatt<natts; iatt++) {
    ncattname(mincid, varid, iatt, att_name);
      std::string str_att_name=att_name;
    
    //Read attribute type
    ncattinq(mincid, varid, att_name, &att_datatype, NULL);
    (void) printf("global:%s(%s) = ",  att_name, type_names[att_datatype]);
    
    
    //Read attribute value
    ncattinq(mincid, varid, att_name, &att_datatype, &att_length);
    if (att_datatype == NC_CHAR) {
      char *cdata;
      cdata = (char*)malloc((att_length+1)*sizeof(char));
      miattgetstr(mincid, varid, att_name, att_length+1, cdata);
      (void) printf("%s\n", cdata);
      std::string str_cdata=cdata;
      setProperty(sx,  "MINC_global:"+str_att_name, str_cdata );

      free(cdata);
    }
    else {
      double *ddata;
      ddata = (double*)malloc(att_length * sizeof(double));
      miattget(mincid, varid, att_name, NC_DOUBLE, att_length,
           ddata, NULL);
    
    vector<float> vec_ddata;

    for (int iiatt=0; iiatt<att_length; iiatt++) {
      (void) printf("%.20g ", ddata[iiatt]);
      vec_ddata.push_back(ddata[iiatt]);
    }

    setProperty(sx,  "MINC_global:"+str_att_name, vec_ddata );
      free(ddata);
    }
  }
  */
