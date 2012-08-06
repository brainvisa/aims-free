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
#include <aims/io/niftiheader.h>
#include <aims/io/niftilib/nifti1_io.h>
#include <aims/io/byteswap.h>
#include <aims/def/general.h>
#include <aims/def/settings.h>
#include <aims/resampling/standardreferentials.h>
#include <aims/resampling/motion.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/fileutil.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;

NiftiHeader::NiftiHeader( int dimx, int dimy, int dimz, int dimt, float sx, 
		      float sy, float sz, float st, const string & name ) :
  PythonHeader(), 
  _name( name )
{
  vector<int>	dims( 4 );
  dims[0] = dimx;
  dims[1] = dimy;
  dims[2] = dimz;
  dims[3] = dimt;
  setProperty( "volume_dimension", dims );
  
  vector<float>	vs( 4 );
  vs[0] = sx;
  vs[1] = sy;
  vs[2] = sz;
  vs[3] = st;
  setProperty( "voxel_size", vs );
  
  _nim = NULL;
  /*
  _nim = nifti_simple_init_nim( );
  _nim->nifti_type = NIFTI_FTYPE_NIFTI1_1;
  nifti_set_filenames( _nim, _name.c_str(), 0, 1 );
  _nim->ndim = _nim->dim[0] = (dimt == 1) ? 3 : 4;
  _nim->nx   = _nim->dim[1] = dimx;
  _nim->ny   = _nim->dim[2] = dimy;
  _nim->nz   = _nim->dim[3] = dimz;
  _nim->nt   = _nim->dim[4] = dimt;
  _nim->nu   = _nim->dim[5] = 1;
  _nim->nv   = _nim->dim[6] = 1;
  _nim->nw   = _nim->dim[7] = 1;
  _nim->pixdim[0] = 1;
  _nim->dx   = _nim->pixdim[1] = sx;
  _nim->dy   = _nim->pixdim[2] = sy;
  _nim->dz   = _nim->pixdim[3] = sz;
  _nim->dt   = _nim->pixdim[4] = st;
  _nim->du   = _nim->pixdim[5] = 1.0;
  _nim->dv   = _nim->pixdim[6] = 1.0;
  _nim->dw   = _nim->pixdim[7] = 1.0;
  */
}


NiftiHeader::NiftiHeader( const string & name ) :
  PythonHeader(), 
  _name( name )
{
  _nim = NULL;
  /*
  _nim = nifti_simple_init_nim( );
  _nim->nifti_type = NIFTI_FTYPE_NIFTI1_1;
  nifti_set_filenames( _nim, _name.c_str(), 0, 1 );
  */
}


NiftiHeader::~NiftiHeader()
{
  freeNiftiStruct();
}

void NiftiHeader::freeNiftiStruct()
{
  nifti_image_free( _nim );
  _nim = NULL;
}

string NiftiHeader::extension() const
{
  char *ext = nifti_find_file_extension( _name.c_str() );
  string s(ext);
  return s;
}

string NiftiHeader::basename() const
{
  char *bn = nifti_makebasename( _name.c_str() );
  string s( bn );
  free( bn );
  return s;
}

set<string> NiftiHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".hdr" );
  exts.insert( ".hdr.gz" );
  exts.insert( ".img" );
  exts.insert( ".img.gz" );
  exts.insert( ".nii" );
  exts.insert( ".nii.gz" );
  return exts;
}


const string& NiftiHeader::name() const
{
  return _name;
}



int NiftiHeader::dimX() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return vd[0];
  return 1;
}

int NiftiHeader::dimY() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return vd[1];
  return 1;
}

int NiftiHeader::dimZ() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return vd[2];
  return 1;
}

int NiftiHeader::dimT() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) && vd.size() >= 4 )
    return vd[3];
  return 1;
}

float NiftiHeader::sizeX() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs) && vs.size() >= 1 )
    return vs[0];
  return 1;
}

float NiftiHeader::sizeY() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs) && vs.size() >= 2 )
    return vs[1];
  return 1;
}

float NiftiHeader::sizeZ() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs) && vs.size() >= 3 )
    return vs[2];
  return 1;
}

float NiftiHeader::sizeT() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs ) && vs.size() >= 4 )
    return vs[3];
  return 1;
}

namespace {
  string NiftiReferential(int xform_code)
  {
	switch ( xform_code ){
      case NIFTI_XFORM_UNKNOWN:
    	// No transformation is provided, this should never be returned.
    	return "Arbitrary coordinates";
    	break;
	  case NIFTI_XFORM_SCANNER_ANAT: 
    	return "Scanner-based anatomical coordinates";
    	break;
	  case NIFTI_XFORM_ALIGNED_ANAT: 
    	return "Coordinates aligned to another file or to anatomical truth";
    	break;
	  case NIFTI_XFORM_TALAIRACH: 
    	return StandardReferentials::talairachReferential();
    	break;
	  case NIFTI_XFORM_MNI_152: 
    	return StandardReferentials::mniTemplateReferential();
    	break;
	  default: 
    	// A transformation is provided but its target referential is unknown.
    	return "Unknown transformation";
    	break;
	}
  }
  
  int NiftiReferential(string ref)
  {
    if (ref == "Scanner-based anatomical coordinates")
	  return NIFTI_XFORM_SCANNER_ANAT;
	else if (ref == "Coordinates aligned to another file or to anatomical truth")
	  return NIFTI_XFORM_ALIGNED_ANAT;
	else if (ref == StandardReferentials::acPcReferential())
      return NIFTI_XFORM_TALAIRACH;
	else if (ref == StandardReferentials::mniTemplateReferential())
	  return NIFTI_XFORM_MNI_152;
	else
	  return NIFTI_XFORM_UNKNOWN;
  }
}

bool NiftiHeader::read()
{
  nifti_image *nim = NULL;
  string      fileName( _name );

  // niftilib's global debug level for status reporting
  nifti_set_debug_level( 0 ); 
  
  // check that given file is NIFTI and not ANALYZE
  if( is_nifti_file(fileName.c_str()) <= 0 )
    throw wrong_format_error( fileName );
  
  // read header using niftilib without loading data
  nim = nifti_image_read( fileName.c_str() , 0 );
  if( !nim )
  {
    io_error::launchErrnoExcept( fileName );
    throw wrong_format_error( fileName );
  }

  /***********************/
  /* DATA DIMENSIONALITY */
  /***********************/
  
  /* Dimensions of grid array */
  vector< int > dims;
  for (int i=1;i<=nim->ndim;++i)
    dims.push_back(nim->dim[i]);
  while( dims.size() < 3 ) // complete to at least 3D
    dims.push_back( 1 );
  // space: x,y,z, time: t and 3 extra
  // dim[8]: dim[0]=ndim, dim[1]=nx, etc.
  
  /* Grid spacings */
  vector< float > vs;
  vs.reserve( nim->ndim >= 3 ? nim->ndim : 3 );
  for (int i=1;i<=nim->ndim;++i)
    vs.push_back(nim->pixdim[i]);
  for( int i=vs.size(); i<3; ++i )
    vs.push_back( 1. );
  // space: x,y,z, time: t and 3 extra; index 0 is used for qfac
  // pixdim[8]: pixdim[1]=dx, etc.
  
  /****************/
  /* TYPE OF DATA */
  /****************/
  
  /* Type of data in voxels: DT_* code */
  string type;
  short	 sz = 0;
  switch( nim->datatype )
  {
  case DT_INT8:
    type = "S8";
    sz = 8;
    break;
  case DT_INT16:
    type = "S16";
    sz = 16;
    break;
  case DT_INT32:
    type = "S32";
    sz = 32;
    break;
  case DT_FLOAT32:
    type = "FLOAT";
    sz = 32;
    break;
  case DT_DOUBLE:
    type = "DOUBLE";
    sz = 64;
    break;
  case DT_UINT8:
    type = "U8";
    sz = 8;
    break;
  case DT_UINT16:
    type = "U16";
    sz = 16;
    break;
  case DT_UINT32:
    type = "U32";
    sz = 32;
    break;
  case DT_RGB:
    type = "RGB";
    sz = sizeof( AimsRGB ) * 8;
    break;
  case DT_RGBA32:
    type = "RGBA";
    sz = 32;
    break;
  default:
    cerr << "Unrecognized datatype code in NIFTI file \"" << fileName
        << "\"" << endl;
  }
  setProperty( "disk_data_type", type );
  setProperty( "bits_allocated", (int) sz );
  
  if ( sz != nim->nbyper * 8 )
    cerr << "Number of bytes per voxel doesn't match datatype in NIFTI file \"" << fileName << "\"" << endl;

  /******************************/
  /* DATA SCALING and DATA_TYPE */
  /******************************/
  
  /* Scaling parameter: slope, intercept */
  string dt = type;
  if( (nim->scl_slope != 0.0)
       && ((nim->scl_slope != 1.0) || (nim->scl_inter != 0.0) ) )
  {
    setProperty( "scale_factor", (float) nim->scl_slope);
    setProperty( "scale_offset", (float) nim->scl_inter);
    if (type != "DOUBLE")
      dt = "FLOAT";
  }
  setProperty( "data_type", dt );
  setProperty( "scale_factor_applied", false );
  vector<string> vt;
  vt.push_back( dt );
  if( dt != type )
    vt.push_back( type );
  if( dt != "FLOAT" )
    vt.push_back( "FLOAT" );
  if( dt != "DOUBLE" )
    vt.push_back( "DOUBLE" );
  setProperty( "possible_data_types", vt );

  /* Calibration parameter: min, max */
  setProperty( "cal_min", (float) nim->cal_min);
  setProperty( "cal_max", (float) nim->cal_max);

  /*************************************************/
  /* MRI-SPECIFIC SPATIAL AND TEMPORAL INFORMATION */
  /*************************************************/

  /* Indexes (1,2,3, or 0) for MRI */
  setProperty( "freq_dim", (int) nim->freq_dim);
  /* Directions in dim[]/pixdim[] */
  setProperty( "phase_dim", (int) nim->phase_dim);
  /* Directions in dim[]/pixdim[] */
  setProperty( "slice_dim", (int) nim->slice_dim);

  /* Code for slice timing pattern */
  setProperty( "slice_code", (int) nim->slice_code);
  /* Index for start of slices */
  setProperty( "slice_start", (int) nim->slice_start);
  /* Index for end of slices */
  setProperty( "slice_end", (int) nim->slice_end);
  /* Time between individual slices */
  setProperty( "slice_duration", (float) nim->slice_duration);

  /**********************************************/
  /* 3D IMAGE ORIENTATION AND LOCATION IN SPACE */
  /**********************************************/
  vector< string > referentials;
  vector< vector < float > > transformations;
  
  /* Transformation from NIFTI (i,j,k) indexes to AIMS (x,y,z) indexes */
  /* NIFTI transformations are +x=Right, +y=Anterior, +z=Superior while */
  /* AIMS coordinates are +x=Left, +y=Posterior, +z=Inferior */
  
  int idir, jdir, kdir;
  vector< float > storage_to_memory( 16 );
  if ( nim->qform_code > NIFTI_XFORM_UNKNOWN ) {
    nifti_mat44_to_orientation( nim->qto_xyz , &idir,&jdir,&kdir ) ;
  }
  else if ( nim->sform_code > NIFTI_XFORM_UNKNOWN ) {
    nifti_mat44_to_orientation( nim->sto_xyz , &idir,&jdir,&kdir ) ;
  }
  else {
    // default is left-to-right, anterior-to-posterior and inferior-to-superior
    idir = NIFTI_L2R;
    jdir = NIFTI_P2A;
    kdir = NIFTI_I2S;
  }
  storage_to_memory[15] = 1;
  if (idir == NIFTI_R2L || idir == NIFTI_L2R)
  {
    storage_to_memory[0] = (idir == NIFTI_R2L) ? 1 : -1;
	storage_to_memory[3] = (idir == NIFTI_R2L) ? 0 : nim->dim[1]-1;
	if (jdir == NIFTI_A2P || jdir == NIFTI_P2A)
	{
	  // simplest case: no axe swapping
	  // i -> x, j -> y, k -> z
	  storage_to_memory[5]  = (jdir == NIFTI_A2P) ? 1 : -1;
	  storage_to_memory[7]  = (jdir == NIFTI_A2P) ? 0 : nim->dim[2]-1;
	  storage_to_memory[10] = (kdir == NIFTI_S2I) ? 1 : -1;
	  storage_to_memory[11] = (kdir == NIFTI_S2I) ? 0 : nim->dim[3]-1;
	}
	else
	{
	  // i -> x, j -> z, k -> y
	  storage_to_memory[6]  = (kdir == NIFTI_A2P) ? 1 : -1;
	  storage_to_memory[7]  = (kdir == NIFTI_A2P) ? 0 : nim->dim[3]-1;
	  storage_to_memory[9]  = (jdir == NIFTI_S2I) ? 1 : -1;
	  storage_to_memory[11] = (jdir == NIFTI_S2I) ? 0 : nim->dim[2]-1;
	}
  }
  else
  {
    if (jdir == NIFTI_A2P || jdir == NIFTI_P2A)
	{
	  // i -> z, j -> y, k -> x
	  storage_to_memory[2]  = (kdir == NIFTI_R2L) ? 1 : -1;
	  storage_to_memory[3]  = (kdir == NIFTI_R2L) ? 0 : nim->dim[3]-1;
	  storage_to_memory[5]  = (jdir == NIFTI_A2P) ? 1 : -1;
	  storage_to_memory[7]  = (jdir == NIFTI_A2P) ? 0 : nim->dim[2]-1;
	  storage_to_memory[8]  = (idir == NIFTI_S2I) ? 1 : -1;
	  storage_to_memory[11] = (idir == NIFTI_S2I) ? 0 : nim->dim[1]-1;
	}
	else
	{
	  if (kdir == NIFTI_S2I || kdir == NIFTI_I2S)
	  {
	    // i -> y, j -> x, k -> z
		storage_to_memory[1]  = (jdir == NIFTI_R2L) ? 1 : -1;
	    storage_to_memory[3]  = (jdir == NIFTI_R2L) ? 0 : nim->dim[2]-1;
	    storage_to_memory[4]  = (idir == NIFTI_A2P) ? 1 : -1;
	    storage_to_memory[7]  = (idir == NIFTI_A2P) ? 0 : nim->dim[1]-1;
		storage_to_memory[10] = (kdir == NIFTI_S2I) ? 1 : -1;
	    storage_to_memory[11] = (kdir == NIFTI_S2I) ? 0 : nim->dim[3]-1;
      }
	  else
	  {
	    if (idir == NIFTI_A2P || idir == NIFTI_P2A)
		{
		  // i -> y, j -> z, k-> x
		  storage_to_memory[2]  = (kdir == NIFTI_R2L) ? 1 : -1;
	      storage_to_memory[3]  = (kdir == NIFTI_R2L) ? 0 : nim->dim[3]-1;
		  storage_to_memory[4]  = (idir == NIFTI_A2P) ? 1 : -1;
	      storage_to_memory[7]  = (idir == NIFTI_A2P) ? 0 : nim->dim[1]-1;
		  storage_to_memory[9]  = (jdir == NIFTI_S2I) ? 1 : -1;
	      storage_to_memory[11] = (jdir == NIFTI_S2I) ? 0 : nim->dim[2]-1;
		}
		else
		{
		  // i -> z, j -> x, k -> y
		  storage_to_memory[1]  = (jdir == NIFTI_R2L) ? 1 : -1;
	      storage_to_memory[3]  = (jdir == NIFTI_R2L) ? 0 : nim->dim[2]-1;
		  storage_to_memory[6]  = (kdir == NIFTI_A2P) ? 1 : -1;
	      storage_to_memory[7]  = (kdir == NIFTI_A2P) ? 0 : nim->dim[3]-1;
		  storage_to_memory[8]  = (idir == NIFTI_S2I) ? 1 : -1;
	      storage_to_memory[11] = (idir == NIFTI_S2I) ? 0 : nim->dim[1]-1;
		}
	  }
	}
  }
  Motion s2m( storage_to_memory );
  setProperty( "storage_to_memory", s2m.toVector() );

  Point3df pdim = s2m.transform( Point3df( dims[0], dims[1], dims[2] ) )
      - s2m.transform( Point3df( 0, 0, 0 ) );
  dims[0] = (int) rint( fabs( pdim[0] ) );
  dims[1] = (int) rint( fabs( pdim[1] ) );
  dims[2] = (int) rint( fabs( pdim[2] ) );
  setProperty( "volume_dimension", dims );

  Point3df pvs = s2m.transform( Point3df( vs[0], vs[1], vs[2] ) )
      - s2m.transform( Point3df( 0, 0, 0 ) );
  vs[0] = fabs( pvs[0] );
  vs[1] = fabs( pvs[1] );
  vs[2] = fabs( pvs[2] );
  setProperty( "voxel_size", vs );

  Motion vsM;
  vsM.rotation()(0,0) = vs[0]; // voxel size in memory orientation
  vsM.rotation()(1,1) = vs[1];
  vsM.rotation()(2,2) = vs[2];
  /* Motion vsD; // voxel size in disk orientation
  vsD.rotation()(0,0) = nim->pixdim[1];
  vsD.rotation()(1,1) = nim->pixdim[2];
  vsD.rotation()(2,2) = nim->pixdim[3]; */


  /* !!! As I understand, both qform and sform are transforming voxels to mm
     (Denis 2008/08/21) */

  /* Codes for (x,y,z) space meaning and associated transform */
  if( nim->qform_code > NIFTI_XFORM_UNKNOWN )
  {
    referentials.push_back( NiftiReferential( nim->qform_code ) );
    Motion qto_xyz;
    qto_xyz.translation()[0] = nim->qto_xyz.m[0][3];
    qto_xyz.translation()[1] = nim->qto_xyz.m[1][3];
    qto_xyz.translation()[2] = nim->qto_xyz.m[2][3];
    for (int i=0;i<3;++i)
      for (int j=0;j<3;++j)
        qto_xyz.rotation()(i,j) = nim->qto_xyz.m[i][j];
    qto_xyz = qto_xyz * ( vsM * s2m ).inverse();
    transformations.push_back( qto_xyz.toVector() );
  }
  if( nim->sform_code > NIFTI_XFORM_UNKNOWN )
  {
    referentials.push_back( NiftiReferential( nim->sform_code ) );
    Motion sto_xyz;
    sto_xyz.translation()[0] = nim->sto_xyz.m[0][3];
    sto_xyz.translation()[1] = nim->sto_xyz.m[1][3];
    sto_xyz.translation()[2] = nim->sto_xyz.m[2][3];
    for (int i=0;i<3;++i)
      for (int j=0;j<3;++j)
        sto_xyz.rotation()(i,j) = nim->sto_xyz.m[i][j];
    sto_xyz = sto_xyz  * ( vsM * s2m ).inverse();
    transformations.push_back( sto_xyz.toVector() );
  }
  
  setProperty( "referentials", referentials );
  setProperty( "transformations", transformations );

  /********************************************/
  /* UNITS OF SPATIAL AND TEMPORAL DIMENSIONS */
  /********************************************/
  
  /* Time coordinate offset */
  setProperty( "toffset", (float) nim->toffset );
  
  /* dx,dy,dz units: NIFTI_UNITS_* code */
  setProperty( "xyz_units", (int) nim->xyz_units );
  /* dt       units: NIFTI_UNITS_* code */
  setProperty( "time_units", (int) nim->time_units );
  
  /********************************/
  /* INTERPRETATION OF VOXEL DATA */
  /********************************/
  
  if( nim->intent_code > NIFTI_INTENT_NONE ) {
    /* Statistic type (or something) */
    setProperty( "intent_code", (int) nim->intent_code );
    /* Intent parameters */
    setProperty( "intent_p1", (float) nim->intent_p1 );
    setProperty( "intent_p2", (float) nim->intent_p2 );
    setProperty( "intent_p3", (float) nim->intent_p3 );
    /* Optional description of intent data */
    setProperty( "intent_name", string( nim->intent_name ) );
  }

  /**********************************/
  /* DESCRIPTION AND AUXILIARY FILE */
  /**********************************/
  
  /* Optional text to describe dataset */
  setProperty( "descrip", string( nim->descrip ) );
  /* Auxiliary filename */
  setProperty( "aux_file", string( nim->aux_file ) );
  
  /*************************************/
  /* INTERNAL HEADERS (TO BE REMOVED)? */
  /*************************************/
  
  /* Header filename (.hdr or .nii) */
  /* setProperty( "fname", string( nim->fname ) ); */
  /* Image filename  (.img or .nii) */
  /* setProperty( "iname", string( nim->iname ) ); */
  /* Offset into iname where data starts */
  //setProperty( "iname_offset", (int) nim->iname_offset );
  /* Swap unit in image data (might be 0) */
  //setProperty( "swapsize", (int) nim->swapsize );
  /* Byte order on disk (MSB_ or LSB_FIRST) */
  //setProperty( "byteorder", (int) nim->byteorder );
  /* ANALYZE (0), NIFTI-1 1 file (1), NIFTI-1 2 files (2), NIFTI-ASCII (3) */
  setProperty( "nifti_type", (int) nim->nifti_type );
  
  /*********************/
  /* HEADER EXTENSIONS */
  /*********************/

  if( nim->num_ext ) {
    vector < int > extcode( nim->num_ext );
	vector < vector < char > > extdata( nim->num_ext );
    cerr << "NIFTI-1 extensions are present in header but will be ignored: \"" << fileName << "\"" << endl;
    for (int i=0; i<nim->num_ext;++i) {
	  extcode.push_back(nim->ext_list[i].ecode);
	  vector < char > edata(nim->ext_list[i].esize);
	  memcpy(&edata[0],nim->ext_list[i].edata,nim->ext_list[i].esize);
	  extdata.push_back(edata);
	}
	setProperty( "extcode", extcode );
	setProperty( "extdata", extdata );
  }
  
  /* free the nifti structure and assign private one to current*/
  freeNiftiStruct();
  _nim = nim;

  /* Read the associated .minf */
  readMinf( string(nim->iname) + ".minf" );

  // check consistency of the overriden storage_to_memory in .minf
  Motion s2mbis( getProperty( "storage_to_memory" ) );
  if( ! (s2mbis == s2m ) )
  {
    pdim = s2mbis.transform( Point3df( dims[0], dims[1], dims[2] ) )
        - s2m.transform( Point3df( 0, 0, 0 ) );
    Point3d dimbis( (int) rint( fabs( pdim[0] ) ),
                    (int) rint( fabs( pdim[1] ) ),
                    (int) rint( fabs( pdim[2] ) ) );
    getProperty( "volume_dimension", dims );
    if( dimbis[0] != dims[0] || dimbis[1] != dims[1] || dimbis[2] != dims[2] )
    {
      setProperty( "volume_dimension", dims );
      setProperty( "storage_to_memory", s2m.toVector() );
      setProperty( "voxel_size", vs );
      cerr << "Warning: .minf storage_to_memory / volume_dimension mismatch. "
          << "setting back initial NIFTI1 header values:\"" << fileName << "\"" << endl;
    }
  }
  setProperty( "file_type", string( "NIFTI1" ) );

  return true;
}


bool NiftiHeader::fillNim( bool allow4d )
{
  /* initialization of a nifti image struct */
  nifti_image *nim = nifti_simple_init_nim( );
  
  // cf nifti_convert_nim2nhdr() to know which fields need to be filled 
  
  /***********************/
  /* DATA DIMENSIONALITY */
  /***********************/

  /* Dimensions of grid array */
  vector< int > dims;
  getProperty( "volume_dimension", dims );
  unsigned i;
  for( i=dims.size(); i<4; ++i )
    dims.push_back( 1 );

  vector <float > storage_to_memory;
  Motion s2m;
  if(getProperty( "storage_to_memory", storage_to_memory ))
  {
    s2m = storage_to_memory;
  }
  else
  {
    // cout << "Saving in neurological orientation: ";
    /* TODO */ // Should use 'spm_force_output_convention' and 'spm_output_radio_convention'
    /* TODO */ // and also see if 'spm_radio_convention' is present
    s2m.translation()[0] = dims[0] - 1;
    s2m.translation()[1] = dims[1] - 1;
    s2m.translation()[2] = dims[2] - 1;
    s2m.rotation()(0,0) = -1.0;
    s2m.rotation()(1,1) = -1.0;
    s2m.rotation()(2,2) = -1.0;
  }

  Motion  m2s = s2m.inverse();
  Point3df df = m2s.transform( Point3df( dims[0], dims[1], dims[2] ) )
      - m2s.transform( Point3df( 0, 0, 0 ) );
  Point4d tdims = Point4d( short( rint( fabs( df[0] ) ) ),
                 short( rint( fabs( df[1] ) ) ),
                 short( rint( fabs( df[2] ) ) ), dims[3] );
  nim->ndim = nim->dim[0] = dims.size();
  nim->dim[1] = tdims[0];
  nim->dim[2] = tdims[1];
  nim->dim[3] = tdims[2];
  nim->dim[4] = tdims[3];
  for (i=dims.size()+1;i<8;++i)
    nim->dim[i] = 1;
  nim->nx = nim->dim[1];
  nim->ny = nim->dim[2];
  nim->nz = nim->dim[3];
  nim->nt = nim->dim[4];
  nim->nu = nim->dim[5];
  nim->nv = nim->dim[6];
  nim->nw = nim->dim[7];
  if(nim->dim[dims.size()] == 1)
    nim->ndim = nim->dim[0] -= 1;
  
  if (!allow4d)
  {
    nim->nt = nim->nu = nim->nv = nim->nw = 1;
    nim->dim[4] = nim->dim[5] = nim->dim[6] = nim->dim[7] = 1;
	nim->ndim = nim->dim[0] = (dims.size() >= 4)? 3 : dims.size();
  }
  
  nim->nvox =  nim->nx * nim->ny * nim->nz
             * nim->nt * nim->nu * nim->nv * nim->nw ;
  
  /* Grid spacings */
  vector< float > vs;
  getProperty( "voxel_size", vs );
  for( i=vs.size(); i<4; ++i )
    vs.push_back( 1. );
  df = m2s.transform( Point3df( vs[0], vs[1], vs[2] ) )
      - m2s.transform( Point3df( 0, 0, 0 ) );
  Point4df tvs = Point4df( fabs( df[0] ), fabs( df[1] ), fabs( df[2] ), 
        vs[3] );

  nim->pixdim[0] = 0;
  nim->pixdim[1] = tvs[0];
  nim->pixdim[2] = tvs[1];
  nim->pixdim[3] = tvs[2];
  nim->pixdim[4] = tvs[3];
  for (unsigned int i=vs.size()+1;i<8;++i)
    nim->pixdim[i] = 1.0;
  nim->dx = nim->pixdim[1];
  nim->dy = nim->pixdim[2];
  nim->dz = nim->pixdim[3];
  nim->dt = nim->pixdim[4];
  nim->du = nim->pixdim[5];
  nim->dv = nim->pixdim[6];
  nim->dw = nim->pixdim[7];
  
  if (!allow4d)
  {
    nim->dt = nim->du = nim->dv = nim->dw = 1.0;
	nim->pixdim[4] = nim->pixdim[5] = nim->pixdim[6] = nim->pixdim[7] = 1.0;
  }

  /****************/
  /* TYPE OF DATA */
  /****************/

  /* Type of data in voxels: DT_* code */
  string type;
  if( !getProperty("disk_data_type", type ) )
    getProperty("data_type", type );

  if (type == "U8" )
    nim->datatype = DT_UINT8;
  else if (type == "S16" )
    nim->datatype = DT_INT16;
  else if (type == "S32" )
    nim->datatype = DT_INT32;
  else if (type == "FLOAT" )
    nim->datatype = DT_FLOAT32;
  else if (type == "DOUBLE" )
    nim->datatype = DT_DOUBLE;
  else if (type == "S8" )
    nim->datatype = DT_INT8;
  else if (type == "U16" )
    nim->datatype = DT_UINT16;
  else if (type == "U32" )
    nim->datatype = DT_UINT32;
  else if( type == "RGB" )
    nim->datatype = DT_RGB;
  else if( type == "RGBA" )
    nim->datatype = DT_RGBA32;
  else
  {
    cerr << "Unknown NIFTI1 datatype, using FLOAT: " << type << endl;
    nim->datatype = DT_FLOAT32;
  }

  /* set nbyper,swapsize from datatype */
  nifti_datatype_sizes( nim->datatype , &nim->nbyper, &nim->swapsize );

  /****************/
  /* DATA SCALING */
  /****************/

  /* Scaling parameter: slope, intercept */
  float scl_slope = 0.0;
  if( getProperty( "scale_factor", scl_slope ) )
    nim->scl_slope = scl_slope;
  else
    nim->scl_slope = 0.0;

  float scl_inter = 0.0;
  if( getProperty( "scale_offset", scl_inter ) )
    nim->scl_inter = scl_inter;
  else
    nim->scl_inter = 0.0;

  /* Calibration parameter: min, max */
  float cal_min = 0.0;
  if( getProperty( "cal_min", cal_min ) )
    nim->cal_min = cal_min;
  else
    nim->cal_min = 0.0;
  
  float cal_max = 0.0;
  if( getProperty( "cal_max", cal_max ) )
    nim->cal_max = cal_max;
  else
    nim->cal_max = 0.0;
  
  /*************************************************/
  /* MRI-SPECIFIC SPATIAL AND TEMPORAL INFORMATION */
  /*************************************************/
  
  int freq_dim = 0;
  if( getProperty( "freq_dim", freq_dim ) )
    nim->freq_dim = freq_dim;
  else
    nim->freq_dim = 0;
  
  int phase_dim = 0;
  if( getProperty( "phase_dim", phase_dim ) )
    nim->phase_dim = phase_dim;
  else
    nim->phase_dim = 0;
	
  int slice_dim = 0;
  if( getProperty( "slice_dim", slice_dim ) )
    nim->slice_dim = slice_dim;
  else
    nim->slice_dim = 0; /* TODO */ // should be nim->nz ?
  
  int slice_code = 0;
  if( getProperty( "slice_code", slice_code ) )
    nim->slice_code = slice_code;
  else
    nim->slice_code = 0;
  
  int slice_start = 0;
  if( getProperty( "slice_start", slice_start ) )
    nim->slice_start = slice_start;
  else
    nim->slice_start = 0; 
	
  int slice_end = 0;
  if( getProperty( "slice_end", slice_end ) )
    nim->slice_end = slice_end;
  else
    nim->slice_end = 0;
  
  float slice_duration = 0.0;
  if( getProperty( "slice_duration", slice_duration ) )
    nim->slice_duration = slice_duration;
  else
    nim->slice_duration = 0.0; /* TODO */ // should be tr?
  
   /**********************************************/
  /* 3D IMAGE ORIENTATION AND LOCATION IN SPACE */
  /**********************************************/

  Object referentials;
  Object transformations;

  try
  {
    referentials = getProperty( "referentials" );
    transformations = getProperty( "transformations" );
  }
  catch( ... )
  {
  }

  if( transformations.isNull() )
  {
    // cout << "no transformations\n";
    transformations = Object::value( vector<vector<float> >() );
    referentials = Object::value( vector<string>() );
  }

  Motion voxsz; // voxel size transformation in memory orientation
  voxsz.rotation()(0,0) = vs[0];
  voxsz.rotation()(1,1) = vs[1];
  voxsz.rotation()(2,2) = vs[2];

  nim->qform_code = NIFTI_XFORM_UNKNOWN;
  nim->sform_code = NIFTI_XFORM_UNKNOWN;
  // cout << "transformations: " << transformations->size() << endl;
  unsigned nt = transformations->size();
  vector<Motion> mv( nt );
  if( nt > 0 )
    mv[0] = Motion(transformations->getArrayItem(0));
  unsigned j;
  bool skip;
  Object transformations2 = Object::value( vector<vector<float> >() );
  vector<vector<float> > & t2
      = transformations2->value<vector<vector<float> > >();
  Object referentials2 = Object::value( vector<string>() );
  vector<string> & r2 = referentials2->value<vector<string> >();

  // s2m orientation codes
  vector < float > mvec = s2m.toVector();
  mat44 S2M_m44;
  for (int i=0;i<4;++i)
    for (int j=0;j<4;++j)
      S2M_m44.m[i][j] = mvec[j+4*i];
  int icod, jcod, kcod;
  nifti_mat44_to_orientation( S2M_m44, &icod, &jcod, &kcod );

  bool s2moriented = false, s2morientedbis;

  for( i=0; i<nt; ++i )
  {
    mv[i] = Motion(transformations->getArrayItem(i));
    skip = false;
    for( j=0; j<i; ++j )
      if( mv[j] == mv[i] )
      {
        /* cout << "duplicate transformation " << i << " identical to " << j
            << endl; */
        skip = true;
        break;
      }
    if( !skip )
    {
      Motion mot( mv[i] );
      string ref = referentials->getArrayItem(i)->getString();
      // cout << "store transformation " << i << ":\n";
      /* cout << mot << endl;
      cout << mot * voxsz * s2m << endl; */
      r2.push_back( ref );
      t2.push_back( mot.toVector() );

      vector < float > m = (mot * voxsz * s2m).toVector();
      mat44 R;

      s2morientedbis = false;
      if( !s2moriented )
      {
        // check if it can also store s2m
        Motion mi;
        mi.rotation()( 0, 0 ) = -1;
        mi.rotation()( 1, 1 ) = -1;
        mi.rotation()( 2, 2 ) = -1;
        vector<float> m = (mot * voxsz * s2m * mi).toVector();
        for (int x=0; x<4; ++x)
          for (int j=0; j<4; ++j)
            R.m[x][j] = m[j+4*x];
        int icod2, jcod2, kcod2;
        nifti_mat44_to_orientation( R, &icod2, &jcod2, &kcod2 );
        s2morientedbis = ( icod == icod2 && jcod == jcod2 && kcod == kcod2 );
        // cout << "s2moriented: " << s2morientedbis << endl;
      }

      for (int x=0; x<4; ++x)
        for (int j=0; j<4; ++j)
          R.m[x][j] = m[j+4*x];

      bool ok = false;
      int xform_code = NiftiReferential(ref);
      if (((xform_code == NIFTI_XFORM_SCANNER_ANAT)
            /* || (xform_code == NIFTI_XFORM_ALIGNED_ANAT) */ )
            && (nim->qform_code == NIFTI_XFORM_UNKNOWN) && s2morientedbis )
      {
        // cout << "save transformation " << i << " as qform\n";
        nim->qform_code = xform_code;
        nifti_mat44_to_quatern( R, &nim->quatern_b, &nim->quatern_c,
                                &nim->quatern_d, &nim->qoffset_x,
                                &nim->qoffset_y, &nim->qoffset_z,
                                NULL, NULL, NULL, &nim->qfac );
        // check if the matrix can actually be stored as a quaternion
        mat44 P = nifti_quatern_to_mat44( nim->quatern_b, nim->quatern_c,
                                          nim->quatern_d, nim->qoffset_x,
                                          nim->qoffset_y, nim->qoffset_z,
                                          tvs[0], tvs[1], tvs[2], nim->qfac );
        ok = true;
        s2moriented = true;
        for( int x=0; x<3 && ok; ++x )
          for( int j=0; j<3; ++j )
            if( fabs( P.m[x][j] - R.m[x][j] ) > 1e-5 )
            {
              // cancel
              // cout << "transformation does not fit as a quaternion" << endl;
              ok = false;
              nim->qform_code = NIFTI_XFORM_UNKNOWN;
              break;
            }
      }
      if( !ok && nim->sform_code == NIFTI_XFORM_UNKNOWN )
      {
        // cout << "save transformation " << i << " as sform\n";
        ok = true;
        nim->sform_code = xform_code;
        for (int x=0; x<4; ++x)
          for (int j=0;j<4;++j)
            nim->sto_xyz.m[x][j] = m[j+x*4];
      }
      if( !ok )
      {
        cout << "Could not save transformation." << endl;
      }
    }
  }

  if( !s2moriented && nim->qform_code == NIFTI_XFORM_UNKNOWN )
  {
    /* cout << "add s2m\n";
    cout << s2m << endl; */
    Motion NIs2m;
    NIs2m.rotation()( 0, 0 ) = -1; // invert all axes
    NIs2m.rotation()( 1, 1 ) = -1;
    NIs2m.rotation()( 2, 2 ) = -1;
    NIs2m = s2m * NIs2m;
    /* if( NIs2m.rotation()(0,0) == 1 && NIs2m.rotation()(1,1) == 1
      && NIs2m.rotation()(2,2) == 1)
    {
      cout << "default orientation\n";
    } */
    mvec = NIs2m.toVector();
    mat44 P;
    for (int x=0; x<4; ++x)
      for (int j=0; j<4; ++j)
        P.m[x][j] = mvec[j+4*x];
    // cout << "matrix: " << NIs2m << endl;
    nim->qform_code = NIFTI_XFORM_SCANNER_ANAT;
    nifti_mat44_to_quatern( P, &nim->quatern_b, &nim->quatern_c,
                            &nim->quatern_d, &nim->qoffset_x,
                            &nim->qoffset_y, &nim->qoffset_z,
                            NULL, NULL, NULL, &nim->qfac );
    /* TODO */ // use 'origin' field
    nim->qoffset_x = -(nim->nx / 2.0 - 1.0) * nim->dx;
    nim->qoffset_y = -(nim->ny / 2.0) * nim->dy;
    nim->qoffset_z = -(nim->nz / 2.0) * nim->dz;
  }

  if( !t2.empty() )
  {
    setProperty( "referentials", referentials2 );
    setProperty( "transformations", transformations2 );
  }
  setProperty( "storage_to_memory", s2m.toVector() );

  /********************************************/
  /* UNITS OF SPATIAL AND TEMPORAL DIMENSIONS */
  /********************************************/
  
  float toffset = 0.0;
  if( getProperty( "toffset", toffset ) )
    nim->toffset = toffset;
  else
    nim->toffset = 0.0;
  
  int xyz_units = 0;
  if( getProperty( "xyz_units", xyz_units ) )
    nim->xyz_units = xyz_units;
  else
    nim->xyz_units = 0;
  
  int time_units = 0;
  if( getProperty( "time_units", time_units ) )
    nim->time_units = time_units;
  else
    nim->time_units = 0;
  
  /********************************/
  /* INTERPRETATION OF VOXEL DATA */
  /********************************/
  
  int intent_code = NIFTI_INTENT_NONE;
  if( getProperty( "intent_code", intent_code ) )
    nim->intent_code = intent_code;
  else
    nim->intent_code = NIFTI_INTENT_NONE;
  
  float intent_p1 = 0.0;
  if( getProperty( "intent_p1", intent_p1 ) )
    nim->intent_p1 = intent_p1;
  else
    nim->intent_p1 = 0.0;
  
  float intent_p2 = 0.0;
  if( getProperty( "intent_p2", intent_p2 ) )
    nim->intent_p2 = intent_p2;
  else
    nim->intent_p2 = 0.0;
	
  float intent_p3 = 0.0;
  if( getProperty( "intent_p3", intent_p3 ) )
    nim->intent_p3 = intent_p3;
  else
    nim->intent_p3 = 0.0;
  
  string intent_name;
  if( getProperty( "intent_name", intent_name ) )
  {
    nim->intent_name[15] = '\0';
    strncpy(nim->intent_name, intent_name.c_str(), 15);
  }
  else 
    nim->intent_name[0] = '\0';
  
  /**********************************/
  /* DESCRIPTION AND AUXILIARY FILE */
  /**********************************/
  
  strcpy(nim->descrip, "Written with A.I.M.S.\0"); /* yeah baby! */
  string descrip;
  if( getProperty( "descrip", descrip ) )
  {
    nim->descrip[79] = '\0';
    strncpy(nim->descrip, descrip.c_str(), 79);
  }
  else 
    nim->descrip[0] = '\0';
  
  string aux_file;
  if( getProperty( "aux_file", aux_file ) )
  {
    nim->aux_file[23] = '\0';
    strncpy(nim->aux_file, aux_file.c_str(), 23);
  }
  else 
    nim->aux_file[0] = '\0'; 
  
  /*********************/
  /* HEADER EXTENSIONS */
  /*********************/
  
  vector < int > extcode;
  if( getProperty( "extcode", extcode ) )
  {
    int next = extcode.size();
    vector < vector < char > > extdata( next );
    getProperty( "extdata", extdata );
    if( extdata.size() < next )
      next = extdata.size();
    for (int i=0;i<next;++i)
    {
      nifti_add_extension( nim, &extdata[i][0], extdata[i].size(),
                           extcode[i] );
    }
  }

  /* Set Nifti type from given extension (default is Nifti 1 file) */
  string Extension = extension( );
  if((Extension == ".hdr") || (Extension == ".hdr.gz") || 
     (Extension == ".img") || (Extension == ".img.gz"))
    nim->nifti_type = NIFTI_FTYPE_NIFTI1_2;
  else
    nim->nifti_type = NIFTI_FTYPE_NIFTI1_1;
  /* set fname, iname, byteorder from filenames or nifti_type */
  nifti_set_filenames( nim, _name.c_str(), 0, 1 ); 
  /* set nifti_type from  */
  //nifti_set_type_from_names( nim );
  /* set iname_offset from nifti_type */
  //nifti_set_iname_offset( nim );

  freeNiftiStruct();
  _nim = nim;

  // write meta-info header
  setProperty( "file_type", string( "NIFTI1" ) );
  setProperty( "object_type", string( "Volume" ) );

  return true;
}

bool NiftiHeader::write( bool writeminf)
{
  fillNim();

  /* call niftilib */
  nifti_image_write_hdr_img( _nim, 0, "wb" );

  if( writeminf )
  {
    bool changedims = true;
    if( hasProperty( "series_filenames" ) )
      changedims = false;
    else
      removeProperty( "volume_dimension" );
    if( hasProperty( "filenames" ) )
      removeProperty( "filenames" );
    writeMinf( string(_nim->iname) + ".minf" );
    //if( changedims )
    //  setProperty( "volume_dimension", dims );
    /* TODO */ // handle series_filenames properly
  }
  return true;
}
