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
#include <aims/io/spmheader.h>
#include <aims/io/dbh.h>
#include <aims/io/byteswap.h>
#include <aims/def/general.h>
#include <aims/def/settings.h>
#include <aims/resampling/motion.h>
#include <aims/resampling/standardreferentials.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/fileutil.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;


namespace
{

  bool test_normalized( const vector<int> & dims, const vector<float> & vs )
  {
    float xfov = float( dims[0] ) * vs[0];
    float yfov = float( dims[1] ) * vs[1];
    float zfov = float( dims[2] ) * vs[2];

    if( (( xfov > 152.0 ) && ( xfov < 165.0 ) &&
	 ( yfov > 185.0 ) && ( yfov < 195.0 ) &&
	 ( zfov > 130.0 ) && ( zfov < 145.0 ))
	||
	(( xfov > 178.0 ) && ( xfov < 185.0 ) &&
	 ( yfov > 215.0 ) && ( yfov < 220.0 ) &&
	 ( zfov > 178.0 ) && ( zfov < 185.0 )) )
      return true;
    return false;
  }
  
  void safe_strcpy( char * destination, const char * source, size_t num )
  {
    strncpy( destination, source, num );
    destination[ num-1 ] = '\0';
  }

}


SpmHeader::SpmHeader( int dimx, int dimy, int dimz, int dimt, float sx, 
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
}


SpmHeader::SpmHeader( const string & name ) :
  PythonHeader(), 
  _name( name )
{
}


SpmHeader::~SpmHeader()
{
}


set<string> SpmHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".hdr" );
  exts.insert( ".img" );
  return exts;
}


const string& SpmHeader::name() const
{
  return _name;
}



int SpmHeader::dimX() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return vd[0];
  return 1;
}

int SpmHeader::dimY() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return vd[1];
  return 1;
}

int SpmHeader::dimZ() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) )
    return vd[2];
  return 1;
}

int SpmHeader::dimT() const
{
  vector< int > vd;
  if( getProperty("volume_dimension", vd) && vd.size() >= 4 )
    return vd[3];
  return 1;
}

float SpmHeader::sizeX() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs) )
    return vs[0];
  return 1;
}

float SpmHeader::sizeY() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs) )
    return vs[1];
  return 1;
}

float SpmHeader::sizeZ() const
{
  vector< float > vs;
  if( getProperty("voxel_size", vs) )
    return vs[2];
  return 1;
}

float SpmHeader::sizeT() const
{
  float tr = 0;
  vector< float > vs;
  if( getProperty( "tr", tr ) 
      || ( getProperty("voxel_size", vs) && vs.size() >= 4 
	   && (tr = vs[3] ) ) )
    return tr;
  return 1;
}

bool SpmHeader::read()
{
  string fileName = _name;
  // removeExtension
  string ext="",ext2="";

  if ( fileName.length() > 4 )
    ext = fileName.substr( int(fileName.length() - 4), 4 );
  if (ext == ".img"  || ext == ".hdr")
    fileName = fileName.substr( 0, fileName.length() - 4 );
  // ext removed

  string	matfile = fileName + ".mat";
  fileName += ".hdr";

  ifstream is( fileName.c_str(), ios::in | ios::binary );
  if( !is )
    io_error::launchErrnoExcept( fileName );
  is.unsetf( ios::skipws );
  struct dsr header;
#if defined( __APPLE__ ) && defined( __GNUC__ )
  is.read( (char *) &header, sizeof( struct dsr )-1 );
  if( !is )
  //{ cerr << "SpmHeader read FAILED: " << strerror( errno ) << "\n";
    io_error::launchErrnoExcept( fileName );
    //}
  /* workaround bug in gcc 3.1/darwin where reading the last byte leads to 
     eof + fail bits being set */
  is.read( ((char *) &header) + sizeof( struct dsr ), 1 );
  /*if( !is )
    cerr << "read2 FAILED: " << strerror( errno ) << "\n";
  cout << is.rdstate() << endl;
  cout << ios::goodbit << " " << ios::eofbit << " " << ios::failbit << " " << ios::badbit << endl;
  */
#else
  is.read( (char *) &header, sizeof( struct dsr ) );
  if( !is )
    io_error::launchErrnoExcept( fileName );
#endif
  is.close();
  bool	bswap = ( header.hk.sizeof_hdr > 1000 );
  /*if( bswap )
    cout << "SPM byte swapping ON\n";*/

  setProperty( "byte_swapping", (int) bswap );

  int		i;

  if( bswap )
    for( i=0; i<8; ++i )
      header.dime.dim[i] = byteswap16( header.dime.dim[i] );

  vector<int> dims(4);
  dims[0] = header.dime.dim[1];
  dims[1] = header.dime.dim[2];
  dims[2] = header.dime.dim[3];
  dims[3] = header.dime.dim[4];
  if( dims[3] == 0 )
    dims[3] = 1;
  if( dims[2] == 0 )
    dims[2] = 0;
  if( dims[1] == 0 )
    dims[1] = 1;
  if( dims[0] == 0 )
    dims[0] = 0;
  setProperty( "volume_dimension", dims );

#ifdef DEBUG
  cout << "dims : " << dims[0] << ", " << dims[1] << ", " << dims[2] << ", " 
       << dims[3] << endl;
#endif
  char vox[5] ;
  vox[0] = header.dime.vox_units[0] ;
  vox[1] = header.dime.vox_units[1] ;
  vox[2] = header.dime.vox_units[2] ;
  vox[3] = header.dime.vox_units[3] ;
  vox[4] = '\0' ;
  string voxUnits(vox) ;
 	
  setProperty( "vox_units", voxUnits );
  
  char cal[9];
  cal[0] = header.dime.cal_units[0] ;
  cal[1] = header.dime.cal_units[1] ;
  cal[2] = header.dime.cal_units[2] ;
  cal[3] = header.dime.cal_units[3] ;
  cal[4] = header.dime.cal_units[4] ;
  cal[5] = header.dime.cal_units[5] ;
  cal[6] = header.dime.cal_units[6] ;
  cal[7] = header.dime.cal_units[7] ;
  cal[8] = '\0' ;
  string calUnits ;
  
  setProperty( "cal_units", calUnits );


  if( bswap )
    header.dime.bitpix = byteswap16( header.dime.bitpix );
#ifdef DEBUG  
  cout << "bits/voxel : " << header.dime.bitpix << endl;
#endif
  string	type;
  short		typecode ;
  if( bswap )
    typecode = byteswap16( header.dime.datatype ) ;
  else
    typecode = header.dime.datatype ;
  
  short		sz = 0;

#ifdef DEBUG
  cout << "type code : " << typecode << endl;
#endif

  switch( typecode )
    {
    case 2:
      type = "U8";
      sz = 8;
      break;
    case 4:
      type = "S16";
      sz = 16;
      break;
    case 8:
      type = "S32";
      sz = 32;
      break;
    case 16:
      type = "FLOAT";
      sz = 32;
      break;
    case 64:
      type = "DOUBLE";
      sz = 64;
      break;
    case 130:
      type = "S8";
      sz = 8;
      break;
    case 132:
      type = "U16";
      sz = 16;
      break;
    case 136:
      type = "U32";
      sz = 32;
      break;
    default:
      cerr << "unrecognized type code - choosing type from bits/voxel\n";
    }

  if( type.empty() )
    {
      if ( header.dime.bitpix == 16 )
	type = "S16";
      else if ( header.dime.bitpix == 8 )
	type = "U8";
      else if ( header.dime.bitpix == 32 )
	{
	  type = "FLOAT";
	  cerr << "Warning: opening SPM file with 32 bits data: choosing "
	       << "FLOAT but could be INT32 format\n";
	}
      else if( header.dime.bitpix == 64 )
	type = "DOUBLE";
      else
	cerr << "Invalid bits/voxel - corrupted image ?\n";
    }
  if( type.empty() )
    throw invalid_format_error( fileName );
  if( sz > 0 && sz != header.dime.bitpix )
    {
      cerr << "Invalid bits/voxel " << header.dime.bitpix 
	   << " - fixing it to " << sz << endl;
      header.dime.bitpix = sz;
    }

  setProperty( "data_type", type );
  setProperty( "disk_data_type", type );
  setProperty( "bits_allocated", (int) header.dime.bitpix );

  if( bswap )
    {
      header.dime.glmin = byteswap32( header.dime.glmin );
      header.dime.glmax = byteswap32( header.dime.glmax );
      header.dime.roi_scale = byteswap32( header.dime.roi_scale );
    }

  setProperty( "minimum", (int) header.dime.glmin );
  setProperty( "maximum", (int) header.dime.glmax );
  
  setProperty( "scale_factor", float( header.dime.roi_scale ) );

  bool scaleFactorApplied = false ;
  setProperty( "scale_factor_applied", bool(scaleFactorApplied) ) ;

  setProperty( "db_name", string( header.hk.db_name ) );

  safe_strcpy( header.hist.descrip, "", 80 );

  setProperty( "aux_file", string( header.hist.aux_file) ) ;
  setProperty( "orient", int( header.hist.orient ) ) ;
  setProperty( "generated", string( header.hist.generated ) ) ;
  setProperty( "scannum", string( header.hist.scannum ) ) ;
  setProperty( "patient_id", string( header.hist.patient_id ) ) ;
  setProperty( "exp_date", string( header.hist.exp_date ) ) ;
  setProperty( "exp_time", string( header.hist.exp_time ) ) ;
  setProperty( "views", int( header.hist.views ) ) ;
  setProperty( "start_field", int( header.hist.start_field ) ) ;
  setProperty( "field_skip", int( header.hist.field_skip ) ) ;
  setProperty( "omax", int( header.hist.omax ) ) ;
  setProperty( "omin", int( header.hist.omin ) ) ;
  setProperty( "smax", int( header.hist.smax ) ) ;
  // check ror nifti identifiers
  char niftistring[] = "    ";
  niftistring[0] = *(char *) &header.hist.smin;
  niftistring[1] = *( (char *) &header.hist.smin + 1 );
  niftistring[2] = *( (char *) &header.hist.smin + 2 );
  niftistring[3] = *( (char *) &header.hist.smin + 3 );
  if( string( "ni1" ) == niftistring || string( "n+1" ) == niftistring )
  {
    // should be a nifti file, but we read it as Analyze: clear nifti ID
    setProperty( "smin", int( 0 ) ) ;
  }
  else
    setProperty( "smin", int( header.hist.smin ) ) ;
  setProperty( "SPM_data_type", string( header.hk.data_type ) ) ;

  vector<float> vs;

  if( bswap )
    for( i=1; i<5; ++i )
      header.dime.pixdim[i] = byteswap32( header.dime.pixdim[i] );

  vs.push_back( fabs( header.dime.pixdim[1] ) );
  vs.push_back( fabs( header.dime.pixdim[2] ) );
  vs.push_back( fabs( header.dime.pixdim[3] ) );
  vs.push_back( fabs( header.dime.pixdim[4] ) );

  if( vs[3] == 0 )
    vs[3] = 1;

  // try to detect normialized images
  bool  normalized = test_normalized( dims, vs );

  vector<string>	ptypes;

  if( imageIsProbablyLabel() )
    ptypes.push_back( type );
  else
    {
      if ( type == "DOUBLE" )
	{
	  ptypes.push_back( "DOUBLE" );
	  //ptypes.push_back( "FLOAT" );
	}
      else
	{
	  setProperty( "data_type", string( "FLOAT" ) );
	  ptypes.push_back( "FLOAT" );
	  //ptypes.push_back( "DOUBLE" );
	  if( type != "FLOAT" )
	    ptypes.push_back( type );
	}
    }
  setProperty( "possible_data_types", ptypes );

  readMinf( removeExtension( _name ) + extension() + ".minf" );

  /* tag attributes from global settings
     (they can be overriden in the .minf) */
  const Settings	&sett = Settings::settings();
  bool	radio = false;
  Motion stom;
  // unconditionally flip Y and Z axes
  stom.rotation()(1,1) = -1;
  stom.rotation()(2,2) = -1;
  stom.translation()[1] = dims[1] - 1;
  stom.translation()[2] = dims[2] - 1;

  try
    {
      radio = (bool) 
        sett.getProperty( "spm_input_radio_convention" )->getScalar();
    }
  catch( exception & )
    {
    }
  try
    {
      radio = (bool) getProperty( "spm_radio_convention" )->getScalar();
    }
  catch( exception & )
    {
    }
  try
    {
      normalized = (bool) getProperty( "spm_normalized" )->getScalar();
    }
  catch( exception & )
    {
    }

  if( normalized )
    {
      setProperty( "spm_normalized", true );
      bool	x = true;
      try
        {
          x = (bool) getProperty( "spm_spm2_normalization" )->getScalar();
          if( !x )
            {
              radio = false;
            }
        }
      catch( exception & )
        {
          try
            {
              x = (bool) sett.getProperty( "spm_input_spm2_normalization" )
                ->getScalar();
              if( x )
                setProperty( "spm_spm2_normalization", true );
            }
          catch( exception & )
            {
            }
          if( !x )
            {
              radio = false;
            }
        }
    }

  if( !radio )
  {
    // neuro storage: also flip X axis
    stom.rotation()(0,0) = -1;
    stom.translation()[0] = dims[0] - 1;
  }

  vector<float> stomvec;
  if( !getProperty( "storage_to_memory", stomvec ) || stomvec.size() != 16 )
  {
#ifdef DEBUG
    cout << "....put storage_to_memory in properties...(spmheader.cc)" << endl;
#endif
    setProperty( "storage_to_memory", stom.toVector() );
  }
  else
  {
#ifdef DEBUG
    cout << "...no storage_to_memory in properties...(spmheader.cc)" << endl;
#endif
    stom = Motion( stomvec );
  }
  Point3df df = stom.transform( Point3df( header.dime.dim[1],
                                header.dime.dim[2], header.dime.dim[3] ) )
      - stom.transform( Point3df( 0, 0, 0 ) );
  dims[0] = short( rint( fabs( df[0] ) ) );
  dims[1] = short( rint( fabs( df[1] ) ) );
  dims[2] = short( rint( fabs( df[2] ) ) );
  setProperty( "volume_dimension", dims );

  df = stom.transform( Point3df( header.dime.pixdim[1],
                       header.dime.pixdim[2], header.dime.pixdim[3] ) )
      - stom.transform( Point3df( 0, 0, 0 ) );

  vs[0] = fabs( df[0] );
  vs[1] = fabs( df[1] );
  vs[2] = fabs( df[2] );
  vs.push_back( fabs( header.dime.pixdim[4] ) );

  setProperty( "voxel_size", vs );
  setProperty( "tr", vs[3] );   // tr = voxel_size(T)

#ifdef DEBUG
  cout << "voxel size : " << vs[0] << ", " << vs[1] << ", " << vs[2] 
       << ", " << vs[3] << endl;
  cout << "type : " << type << endl;
#endif

  string	dt;
  if( getProperty( "data_type", dt ) && dt != "type" )
    {
      unsigned	i, n = ptypes.size();
      for( i=0; i<n; ++i )
        if( ptypes[i] == dt )
          break;
      if( i == n )
        {
          ptypes.insert( ptypes.begin(), dt );
          setProperty( "possible_data_types", ptypes );
        }
    }

  vector<float>	origin;
  if( getProperty( "origin", origin ) )
    {
      if( !radio )
        {
          origin[0] = dims[0] - origin[0];
          origin[1] = dims[1] - origin[1];
          origin[2] = dims[2] - origin[2];
        }
      else
        {
          ++origin[0];	// spm starts at 1...
          origin[1] = dims[1] - origin[1];
          origin[2] = dims[2] - origin[2];
        }

      vector<float>	origin2;
      if( getProperty( "spm_origin", origin2 ) 
          && ( origin2.size() < 3 || origin[0] != origin2[0] 
               || origin[1] != origin2[1] || origin[2] != origin2[2] ) )
        {
          cerr << "Warning in SPM header: contradictory information on SPM " 
               << "origin in .minf" << endl;
        }
      setProperty( "spm_origin", origin );
    }
  else
    {
      if( !getProperty( "spm_origin", origin ) 
          || origin.size() < 3 )
        {
          if( bswap )
            {
              // This code breaks the strict aliasing rule, but as the proper
              // way of doing the conversion is not obvious, just make the
              // warning non-fatal for now.
              // GCC 4.4 does not support the use of directive
              // #pragma GCC diagnostic
              // inside a function.
#if defined(__GNUC__) && ( __GNUC__ * 0x100 + __GNUC_MINOR__ >= 0x450 )
              #pragma GCC diagnostic push
              #pragma GCC diagnostic warning "-Wstrict-aliasing"
#endif
              origin.push_back( byteswap16( *( (short *)
                                               & header.hist.originator[0]
                                               ) ) );
              origin.push_back( byteswap16( *( (short *) 
                                               & header.hist.originator[2] 
                                               ) ) );
              origin.push_back( byteswap16( *( (short *) 
                                               & header.hist.originator[4] 
                                               ) ) );
#if defined(__GNUC__) && ( __GNUC__ * 0x100 + __GNUC_MINOR__ >= 0x450 )
              #pragma GCC diagnostic pop
#endif
            }
          else
            {
              short	s;
              *((unsigned char *) &s) = header.hist.originator[0];
              *(( (unsigned char *) &s ) +1 ) = header.hist.originator[1];
              origin.push_back( s );
              *((unsigned char *) &s) = header.hist.originator[2];
              *(( (unsigned char *) &s ) +1 ) = header.hist.originator[3];
              origin.push_back( s );
              *((unsigned char *) &s) = header.hist.originator[4];
              *(( (unsigned char *) &s ) +1 ) = header.hist.originator[5];
              origin.push_back( s );
            }
        }
#ifdef DEBUG
      cout << "SPM origin : " << origin[0] << ", " << origin[1] << ", " 
           << origin[2] << endl;
#endif

      setProperty( "spm_origin", origin );

      if( !radio )
        {
          origin[0] = dims[0] - origin[0] + 1;
          origin[1] = dims[1] - origin[1] + 1;
          origin[2] = dims[2] - origin[2] + 1;
        }
      else
        {
          --origin[0];	// spm starts at 1...
          origin[1] = dims[1] - origin[1] + 1;
          origin[2] = dims[2] - origin[2] + 1;
        }

      setProperty( "origin", origin );
    }

  setProperty( "file_type", string( "SPM" ) );

  if( FileUtil::fileStat( matfile ).find( '+' ) != string::npos )
    cerr << "Warning: the file " << matfile << " exists. The volume you are " 
      "reading is probably a SPM file, not a regular Analyze file. I cannot " 
      "read the Matlab .mat file but it may contain information that " 
      "override the Analyze header and make it obsolete - SPM2 doesn't write " 
      "updated information in the regular Analyze header." << endl 
         << "Use it at your own risk." << endl;

  return true;
}

bool SpmHeader::write( bool writeminf, bool allow4d )
{
  // convert transformation information to SPM/Analyze info when possible

  vector<string>  refs;
  Point3df  origin;
  bool      hasorg = false;
  bool      normalized = false;
  vector<float> vs;
  vector<int>   dims;

  if( ! getProperty( "volume_dimension", dims ) )
      throw logic_error( "Internal error: getProperty volume_dimension failed" );

  getProperty( "voxel_size", vs );
  if( vs.size() == 0 )
    vs.push_back( 1 );
  if( vs.size() <= 1 )
    vs.push_back( 1 );
  if( vs.size() <= 2 )
    vs.push_back( 1 );
  if( vs.size() <= 3 )
    vs.push_back( 1 );

  if( getProperty( "referentials", refs ) && !refs.empty() )
  {
    Object trans;
    try
    {
      trans = getProperty( "transformations" );
      if( !trans.isNull() )
      {
        unsigned  i, n = refs.size();
        if( trans->size() < n )
          n = trans->size();
        Motion m;
        m.rotation()(0,0) = -1.;
        m.rotation()(1,1) = -1.;
        m.rotation()(2,2) = -1.;
        hasorg = true;
        Object titer = trans->objectIterator(), reftrans;
        for( i=0; i<n; ++i, titer->next() )
        {
          if( refs[i] == StandardReferentials::mniTemplateReferential() )
          {
            cout << "convert MNI -> SPM\n";
            Motion  mm( titer->currentValue() );
            origin = mm.translation();
            // cout << "origin: " << origin << endl;
            mm.translation() = Point3df( 0, 0, 0 );
            if( mm == m )
            {
              reftrans = titer->currentValue();
              normalized = true;
              origin = Point3df( rint( origin[0] / vs[0] ),
                                rint( origin[1] / vs[1] ),
                                rint( origin[2] / vs[2] ) );
              setProperty( "spm_normalized", true );
              break;
            }
          }
          else if( i == 0 )
          {
            reftrans = titer->currentValue();
            Motion  mm( titer->currentValue() );
          }
        }
        if( !normalized )
        {
          if( hasProperty( "spm_normalized" ) )
            removeProperty( "spm_normalized" );
          reftrans = trans->objectIterator()->currentValue();
          Motion  mm( reftrans );
          origin = mm.inverse().transform( Point3df( 0, 0, 0 ) );
          // cout << "origin: " << origin << endl;
          origin = Point3df( rint( origin[0] / vs[0] ),
                              rint( origin[1] / vs[1] ),
                              rint( origin[2] / vs[2] ) );
          // cout << "origin2: " << origin << endl;
        }
      }
    }
    catch( ... )
    {
    }
  }
  if( !hasorg )
  {
    normalized = isNormalized();
    if( isNormalized() )
    {
      origin = Point3df( rint( dims[0] / vs[0] * 0.5 ),
                         rint( dims[1] / vs[1] * 0.5 - 18. ),
                         rint( dims[2] / vs[2] * 0.5 + 18. ) );
      hasorg = true;
    }
  }

  /* 31/10/2003 Denis: I used http://www.mayo.edu/bir/PDF/ANALYZE75.pdf to 
     complete missing fields */

  string fileName = _name;

  // removeExtension
  string ext="",ext2="";

  if ( fileName.length() > 4 )
    ext = fileName.substr( int(fileName.length() - 4), 4 );
  if (ext == ".img"  || ext == ".hdr")
    fileName = fileName.substr( 0, fileName.length() - 4 );
  // ext removed

  fileName += ".hdr";

  ofstream os( fileName.c_str(), ios::out | ios::binary );
  if( !os )
    io_error::launchErrnoExcept( fileName );
  os.unsetf( ios::skipws );

  struct dsr header;
  memset( &header, 0, sizeof(header) );

  header.dime.dim[0] = 4;	// 4 dimensions provided
  header.hk.regular = 'r';
  header.hk.extents = 16384;

  Point3d tdims( dims[0], dims[1], dims[2] );
  std::vector<float> vstom;
  Motion  stom;
  stom.setToIdentity();
  stom.rotation()(1,1) = -1;
  stom.rotation()(2,2) = -1;
  stom.translation()[1] = dims[1] - 1;
  stom.translation()[2] = dims[2] - 1;
  /* orient values:
  0: axial unflipped
  1: coronal unflipped
  2: sagittal unflipped
  3: axial flipped
  4: coronal flipped
  5: sagittal flipped
  */
  char orient = 0;
  if( getProperty( "storage_to_memory", vstom ) )
  {

    //cout << "...storage_to_memory in write spmheader.cc..." << endl;

    stom = Motion( vstom );
    // ensure image is axial
    if( fabs( stom.rotation()(0,0) ) < 0.9
        || stom.rotation()(1,1) < 0.9 || stom.rotation()(1,1) > 1.1
        || stom.rotation()(2,2) < 0.9 || stom.rotation()(2,2) > 1.1 )
    {
      // force axial storage
      stom.setToIdentity();
      stom.rotation()(1,1) = -1;
      stom.rotation()(2,2) = -1;
      stom.translation()[1] = dims[1] - 1;
      stom.translation()[2] = dims[2] - 1;

      //cout << "...storage modified..." << endl;

      setProperty( "storage_to_memory", stom.toVector() );
    }
  }
  else
  {

    //cout << "...no storage_to_memory in write spmheader.cc..." << endl;

    bool      radio = true;
    try
    {
      radio = getProperty( "spm_radio_convention" )->getScalar();
      if( !radio )
      {
        // flip X axis
        Motion m2;
        m2.rotation()(0,0) = -1.;
        stom.translation()[0] = dims[0] - 1;
        stom = m2 * stom;

        cout << "...flip x axis..." << endl;
      }
    }
    catch( exception & )
    {
    }
    setProperty( "storage_to_memory", stom.toVector() );
  }
  if( stom.rotation()(0,0) > 0 )
    orient = 3;
  else
    orient = 0;
  header.hist.orient = orient ;

  Motion  m2s = stom.inverse();
  Point3df df = m2s.transform( Point3df( dims[0], dims[1], dims[2] ) )
      - m2s.transform( Point3df( 0, 0, 0 ) );
  tdims = Point3d( short( rint( fabs( df[0] ) ) ),
                   short( rint( fabs( df[1] ) ) ),
                   short( rint( fabs( df[2] ) ) ) );
  header.dime.dim[1] = short( tdims[0] );
  header.dime.dim[2] = short( tdims[1] );
  header.dime.dim[3] = short( tdims[2] );
  if( allow4d )
    header.dime.dim[4] = short( dims[3] );
  else
    header.dime.dim[4] = short( 1 );
  /*SPM up to date : one frame per file */

  int		dlen = 0;

  if( ! getProperty( "bits_allocated", dlen ) )
    throw logic_error( "Internal error: getProperty bits_allocated failed" );
  header.dime.bitpix = dlen;
  header.dime.vox_offset = 0.0;

  df = m2s.transform( Point3df( vs[0], vs[1], vs[2] ) )
      - m2s.transform( Point3df( 0, 0, 0 ) );
  Point3df tvs = Point3df( fabs( df[0] ), fabs( df[1] ), fabs( df[2] ) );
  header.dime.pixdim[1] = tvs[0];
  header.dime.pixdim[2] = tvs[1];
  header.dime.pixdim[3] = tvs[2];
  header.dime.pixdim[4] = vs[3];

  int	min = 0, max = 0;
  float	cmin = 0, cmax = 0;

  getProperty( "minimum", min );
  getProperty( "maximum", max );
  getProperty( "cal_minimum", cmin );
  getProperty( "cal_maximum", cmax );
  
  bool scaleFactorApplied = false;
  float scaleFactor = 1.;
  getProperty("scale_factor", scaleFactor );
  if( getProperty("scale_factor_applied", scaleFactorApplied ) )
    if( scaleFactorApplied ) 
      scaleFactor = 1.;
  // Warning : the field roi_scale in spm99/dbh.h
  //    IS the float corresponding to the 
  //           displacement read in spm99/funused1 
  // See the discrepancy between
  //          roi_scale, funused1, funused2
  //    AND
  //          funused1, funsed2, funused3 (spmhread.h)
  header.dime.roi_scale = scaleFactor ;
  
  string voxUnits = "mm  ";
  getProperty( "vox_units", voxUnits );
  unsigned	i;
  for( i = 0 ; i < 4 && i < voxUnits.length(); ++i )
    header.dime.vox_units[i] = voxUnits[i] ;
  for( ; i < 4 ; ++i )
    header.dime.vox_units[i] = '\0';

  string calUnits ;
  getProperty( "cal_units", calUnits );
  for( i = 0 ; i < 8 && i < calUnits.length(); i++ )
    header.dime.cal_units[i] = calUnits[i] ;
  for( ; i < 8 ; ++i )
    header.dime.cal_units[i] = '\0';

  header.dime.cal_min = cmin;
  header.dime.cal_max = cmax;
  header.dime.glmin = min;
  header.dime.glmax = max;

  header.hk.sizeof_hdr = sizeof( struct dsr );
  
  string	type;
  if( !getProperty("disk_data_type", type ) )
    getProperty("data_type", type );

  if (type == "U8" )
    {
      header.dime.datatype = 2 ;
      header.dime.bitpix = 8;
    }
  else if (type == "S16" )
    {
      header.dime.datatype = 4 ;
      header.dime.bitpix = 16;
    }
  else if (type == "S32" )
    {
      header.dime.datatype = 8 ;
      header.dime.bitpix = 32;
    }
  else if (type == "FLOAT" )
    {
      header.dime.datatype = 16 ;
      header.dime.bitpix = 32;
    }
  else if (type == "DOUBLE" )
    {
      header.dime.datatype = 64 ;
      header.dime.bitpix = 64;
    }
  else if (type == "S8" )
    {
      header.dime.datatype = 130 ;
      header.dime.bitpix = 8;
    }
  else if (type == "U16" )
    {
      header.dime.datatype = 132 ;
      header.dime.bitpix = 16;
    }
  else if (type == "U32" )
    {
      header.dime.datatype = 136 ;
      header.dime.bitpix = 32;
    }
  else
    {
      if ( header.dime.bitpix == 16 )
        header.dime.datatype = 4 ;
      else if ( header.dime.bitpix == 8 )
        header.dime.datatype = 2 ;
      else if ( header.dime.bitpix == 32 )
        {
          header.dime.datatype = 16 ;
          cerr << "Warning: opening SPM file with 32 bits data: choosing "
               << "FLOAT but could be INT32 format\n";
        }
      else if( header.dime.bitpix == 64 )
        header.dime.datatype = 64 ;
      else
        {
          header.dime.datatype = 0 ;
          cerr << "Invalid bits/voxel - corrupted image ?\n";
        }
    }

  string dbName ;
  if( getProperty("db_name", dbName) )
    safe_strcpy( header.hk.db_name, dbName.c_str(), 18 );
  else
    header.hk.db_name[0] = '\0';

  safe_strcpy( header.hist.descrip, "Written with A.I.M.S.", 80 );
  string auxFile ;
  if( getProperty("aux_file", auxFile) )
    safe_strcpy( header.hist.aux_file, auxFile.c_str(), 24 ) ;
  else
    header.hist.aux_file[0] = '\0';

  short		orig;
  if( hasProperty( "spm_origin" ) )
    removeProperty( "spm_origin" );
  if( hasProperty( "origin" ) )
    removeProperty( "origin" );
  if( hasorg )
  {
    if( normalized )
    {
      origin[0] = dims[0] - origin[0];
      origin[1] = dims[1] - origin[1];
      origin[2] = dims[2] - origin[2];
    }
    else
    {
      origin[0] += 1;
      origin[1] = dims[1] - origin[1] + 1;
      origin[2] = dims[2] - origin[2] + 1;
    }
    orig = (short) origin[0];
  }
  else
    orig = dims[0]/2 + 1; // why +1 ? SPM guys say it must be like that!...
  header.hist.originator[0] = *(char *) &orig;
  header.hist.originator[1] = *( ( (char *) &orig ) + 1 );
  if( hasorg )
    orig = (short ) origin[1];
  else
    orig = dims[1]/2 + 1;
  header.hist.originator[2] = *(char *) &orig;
  header.hist.originator[3] = *( ( (char *) &orig ) + 1 );
  if( hasorg )
    orig = (short) origin[2];
  else
    orig = dims[2]/2 + 1;

  header.hist.originator[4] = *(char *) &orig;
  header.hist.originator[5] = *( ( (char *) &orig ) + 1 );

  string generated ;
  if( getProperty("generated", generated) )
    safe_strcpy( header.hist.generated, generated.c_str(), 10 ) ;
  else
    header.hist.generated[0] = '\0';

  string scannum ;
  if( getProperty("scannum", scannum) )
    safe_strcpy( header.hist.scannum, scannum.c_str(), 10 ) ;
  else
    header.hist.scannum[0] = '\0';

  string patient_id ;
  if( getProperty("patient_id", patient_id) ) {
    safe_strcpy( header.hist.patient_id, patient_id.c_str(), 10 ) ;
  } else {
    header.hist.patient_id[0] = '\0';
  }
  
  string exp_date ;
  if( getProperty("exp_date", exp_date) )
    safe_strcpy( header.hist.exp_date, exp_date.c_str(), 10 ) ;
  else
    header.hist.exp_date[0] = '\0';

  string exp_time ;
  if( getProperty("exp_time", exp_time) )
    safe_strcpy( header.hist.exp_time, exp_time.c_str(), 10 ) ;
  else
    header.hist.exp_time[0] = '\0';

  string SPM_data_type ;
  if( getProperty("SPM_data_type", SPM_data_type) )
    safe_strcpy( header.hk.data_type, SPM_data_type.c_str(), 10 ) ;
  else
    header.hk.data_type[0] = '\0';

  int views = 0;
  if( getProperty("views", views) )
    header.hist.views = views ;
  else
    header.hist.views = 0 ;
  
  int start_field = 0;
  if( getProperty("start_field", start_field) )
    header.hist.start_field = start_field ;
  else
    header.hist.start_field = 32768 ;

  int field_skip = 0;
  if( getProperty("field_skip", field_skip) )
    header.hist.field_skip = field_skip ;
  else
    header.hist.field_skip = 8192 ;

  int omax = 0;
  if( getProperty("omax", omax) )
    header.hist.omax = omax ;
  else
    header.hist.omax = 0 ;

  int omin = 0;
  if( getProperty("omin", omin) )
    header.hist.omin = omin ;
  else
    header.hist.omin = 0 ;

  int smax = 0;
  if( getProperty("smax", smax) )
    header.hist.smax = smax ;
  else
    header.hist.smax = 32 ;

  int smin = 0;
  if( getProperty("smin", smin) )
    header.hist.smin = smin ;
  else
    header.hist.smin = 0 ;

  os.write( (const char *) &header, sizeof( struct dsr ) );
  os.close();

  // write meta-info header
  setProperty( "file_type", string( "SPM" ) );
  setProperty( "object_type", string( "Volume" ) );
  if( hasProperty( "filenames" ) )
    removeProperty( "filenames" );
  if( writeminf )
    {
      bool	changedims = true;
      if( hasProperty( "series_filenames" ) )
        changedims = false;
      else
        removeProperty( "volume_dimension" );
      writeMinf( removeExtension( _name ) + extension() + ".minf" );
      if( changedims )
        setProperty( "volume_dimension", dims );
    }

  return true;
}

bool 
SpmHeader::imageIsProbablyLabel() const
{
  std::string	type;
  getProperty( "data_type", type );
  if( type != "U8" && type != "S8" && type != "U16" && type != "S16" 
      && type != "U32" && type != "S32" )
    return false ;
  bool scaleFactorApplied = false; float scaleFactor = false;
  if( getProperty( "scale_factor_applied", scaleFactorApplied ) 
      && getProperty( "scale_factor", scaleFactor ) 
      && !scaleFactorApplied && scaleFactor != 1. )
	return false ;
  return true ;
}


bool SpmHeader::isNormalized() const
{
  try
    {
      bool	n = getProperty( "spm_normalized" )->getScalar();
      return n;
    }
  catch( ... )
    {
    }
  vector<int>	vd;
  vector<float>	vs;
  if( !getProperty( "volume_dimension", vd ) 
      || !getProperty( "voxel_size", vs ) )
    return false;
  return test_normalized( vd, vs );
}


void SpmHeader::setupWriteAttributes( const string & code )
{
  bool	norm = isNormalized();
  if( norm )
    {
      setProperty( "spm_normalized", true );
    }
  else
    {
      if( hasProperty( "spm_normalized" ) )
        removeProperty( "spm_normalized" );
    }

  const Settings	sett = Settings::settings();
  bool			force = false, radio = false, norm2 = true;

  try
    {
      force = (bool) 
        sett.getProperty( "spm_force_output_convention" )->getScalar();
    }
  catch( exception & )
    {
    }

  if( norm )
    {
      try
        {
          norm2 = (bool) 
            sett.getProperty( "spm_output_spm2_normalization" )->getScalar();
        }
      catch( exception & )
        {
        }
    }

  if( norm && !norm2 )
    radio = false;
  else
    try
      {
        radio = (bool) 
          sett.getProperty( "spm_output_radio_convention" )->getScalar();
      }
    catch( exception & )
      {
      }

  if( !force )
    {
      if( norm )
        {
          try
            {
              norm2 = (bool) 
                getProperty( "spm_spm2_normalization" )->getScalar();
            }
          catch( exception & )
            {
            }
        }
      if( norm && !norm2 )
        radio = false;
      else
        try
          {
            radio = (bool) getProperty( "spm_radio_convention" )->getScalar();
          }
        catch( exception & )
          {
          }
    }

  if( norm )
    setProperty( "spm_spm2_normalization", norm2 );
  else if( hasProperty( "spm_spm2_normalization" ) )
    removeProperty( "spm_spm2_normalization" );
  setProperty( "spm_radio_convention", radio );

  setProperty( "data_type", code );
  setProperty( "disk_data_type", code );
  setProperty( "scale_factor", float(1.) ) ;
  setProperty( "scale_factor_applied", true ) ;
  setProperty( "file_type", std::string( "SPM" ) );
}



