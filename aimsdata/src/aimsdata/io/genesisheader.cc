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

#include <aims/io/genesisheader.h>
#include <aims/io/ge_imageFileOffsets.h>
#include <aims/io/ge_imghdr.h>
#include <aims/io/byteswap.h>
#include <cartobase/exception/ioexcept.h>
#include <fstream>
#include <iostream>
#include <math.h>

using namespace aims;
using namespace carto;
using namespace std;

GenesisHeader::GenesisHeader( const string & name, int dimx, int dimy, 
			      int dimz, int dimt, float sx, float sy, 
			      float sz, float st, const string & dtype )
  : PythonHeader(), _name( name ), _type( dtype ), _dimX( dimx ), 
    _dimY( dimy ), _dimZ( dimz ), _dimT( dimt ), _sizeX( sx ), _sizeY( sy ), 
    _sizeZ( sz ), _sizeT( st ), _compress( 0 ), _data_offset( 0 ), 
    _bswap( false )
{
}


GenesisHeader::~GenesisHeader()
{
}


set<string> GenesisHeader::extensions() const
{
  set<string>	exts;
  return( exts );
}


void GenesisHeader::read()
{
  static const int	IMG_MAGIC = 0x494d4746;
  string fileName = _name;

  ifstream is( fileName.c_str(), ios::in | ios::binary );
  if( !is )
    io_error::launchErrnoExcept( fileName );
  is.unsetf( ios::skipws );

  cerr << "GENESIS header " << fileName << endl;
  ImgHdr	gehdr;

#define macroRead( field ) \
  is.seekg( _IH_##field, ios::beg ); \
  is.read( reinterpret_cast<char *>( &gehdr.field ), sizeof( gehdr.field ) ); \
  if ( bswap ) gehdr.field = byteswap(  gehdr.field )
  
  bool	bswap = false;
  gehdr.img_magic = 0;
  macroRead( img_magic );
  if( gehdr.img_magic != IMG_MAGIC ) {
    gehdr.img_magic = byteswap( gehdr.img_magic );
    if( gehdr.img_magic != IMG_MAGIC )
      throw wrong_format_error( fileName );
    bswap = true;
    //cerr << "byteswapped Genesis file" << endl;
  }
  //else {
  //  cerr << "not byteswapped Genesis file" << endl;
  //}
  
  macroRead( img_hdr_length );
  macroRead( img_width );
  macroRead( img_height );
  macroRead( img_depth );
  macroRead( img_compress );
  macroRead( img_p_id );
  macroRead( img_l_id );
  macroRead( img_p_exam );
  macroRead( img_l_exam );
  macroRead( img_p_series );
  macroRead( img_l_series );
  macroRead( img_p_image );
  macroRead( img_l_image );
#undef macroRead

  _dimX = gehdr.img_width;
  _dimY = gehdr.img_height;
  _data_offset = gehdr.img_hdr_length;
  _compress = gehdr.img_compress;
  _bswap = bswap;
  //cerr << "bits/pixel: " << gehdr.img_depth << endl;
  //cerr << "compress: " << gehdr.img_compress << endl;
  
  string	dtype;
  if( gehdr.img_depth == 8 )
    dtype = "U8";
  else if( gehdr.img_depth == 16 )
    dtype = "S16";
  else
    {
      cerr << "Unrecognized data type, size: " << gehdr.img_depth << endl;
    }

  vector<char>	exam( gehdr.img_l_exam );
  vector<char>	series( gehdr.img_l_series );
  vector<char>	image( gehdr.img_l_image );

  is.seekg( gehdr.img_p_exam, ios::beg );
  if( !is )
    io_error::launchErrnoExcept( fileName );
  is.seekg( gehdr.img_p_series, ios::beg );
  is.read(  reinterpret_cast<char *>( &image[0] ), gehdr.img_l_series );
  if( !is )
    io_error::launchErrnoExcept( fileName );
  is.seekg( gehdr.img_p_image, ios::beg );
  is.read(  reinterpret_cast<char *>( &image[0] ), gehdr.img_l_image );
  if( !is )
    io_error::launchErrnoExcept( fileName );

  short		dimx, dimy, dimz, patage, patian, patsex;
  float		sx, sy, sz, loc, sloc, eloc, spc; //, lmhor;
  int		tr, te, ti;
  string	patid, patname;

#define macroReadSeries( var, field ) \
  if( !is ) io_error::launchErrnoExcept( fileName ); \
  is.seekg( gehdr.img_p_series +  _SE_##field, ios::beg ); \
  is.read( reinterpret_cast<char *>( &var ), sizeof( var ) ); \
  if ( bswap ) var = byteswap( var )

  macroReadSeries( sloc, start_loc );
  macroReadSeries( eloc, end_loc );

#undef macroReadSeries

#define macroReadExam( var, field ) \
  if( !is ) io_error::launchErrnoExcept( fileName ); \
  is.seekg( gehdr.img_p_exam + _EX_##field, ios::beg ); \
  is.read( reinterpret_cast<char *>( &var ), sizeof( var ) ); \
  if ( bswap ) var = byteswap( var )
#define macroReadExamString( var, field ) \
  if( !is ) io_error::launchErrnoExcept( fileName ); \
  is.seekg( gehdr.img_p_exam + _EX_##field, ios::beg ); \
  var.erase(); \
  for( char c = is.get(); c!= '\0'; c= is.get() ) var += c

  macroReadExamString( patid, patid );
  macroReadExamString( patname, patname );
  macroReadExam( patage, patage );
  macroReadExam( patian, patian );
  macroReadExam( patsex, patsex );
  string ex_typ;
  macroReadExamString( ex_typ, ex_typ );
#undef macroReadExam
#undef macroReadExamString

  setProperty( "patient_id", patid );
  setProperty( "patient_name", patname );
  setProperty( "patient_age", patage );
  setProperty( "patient_age_notation", patian );
  if( patsex == 2 )
    setProperty( "patient_sex", string( "female" ) );
  else
    setProperty( "patient_sex", string( "male" ) );

#define macroReadMR( var, field ) \
  if( !is ) io_error::launchErrnoExcept( fileName ); \
  is.seekg( gehdr.img_p_image +  _MR_##field, ios::beg ); \
  is.read( reinterpret_cast<char *>( &var ), sizeof( var ) ); \
  if ( bswap ) var = byteswap( var )

#define macroReadMRString( var, field ) \
  if( !is ) io_error::launchErrnoExcept( fileName ); \
  is.seekg( gehdr.img_p_image + _MR_##field, ios::beg ); \
  var.erase(); \
  for( char c = is.get(); c!= '\0'; c= is.get() ) var += c

  if( string( "MR" ) == ex_typ ) {
    macroReadMR( dimx, imatrix_X );
    macroReadMR( dimy, imatrix_Y );
    macroReadMR( dimz, slquant );
    macroReadMR( sx, pixsize_X );
    macroReadMR( sy, pixsize_Y );
    macroReadMR( sz, slthick );
    macroReadMR( spc, scanspacing );
    macroReadMR( tr, tr );
    macroReadMR( te, te );
    macroReadMR( ti, ti );
    macroReadMR( loc, loc );
    sz += spc;

  // Temporary add Genesis attributes in order to analyse their contents
  float tmpFloat;
  int tmpInt;
  short tmpShort;
  string tmpString;
  ObjectVector tmpList;
  macroReadMR( tmpFloat, ctr_R );
  setProperty( "genesis_ctr_R", tmpFloat );
  macroReadMR( tmpFloat, ctr_A );
  setProperty( "genesis_ctr_A", tmpFloat );
  macroReadMR( tmpFloat, ctr_S );
  setProperty( "genesis_ctr_S", tmpFloat );
  macroReadMR( tmpFloat, norm_R );
  setProperty( "genesis_norm_R", tmpFloat );
  macroReadMR( tmpFloat, norm_A );
  setProperty( "genesis_norm_A", tmpFloat );
  macroReadMR( tmpFloat, norm_S );
  setProperty( "genesis_norm_S", tmpFloat );
  macroReadMR( tmpFloat, tlhc_R );
  setProperty( "genesis_tlhc_R", tmpFloat );
  macroReadMR( tmpFloat, tlhc_A );
  setProperty( "genesis_tlhc_A", tmpFloat );
  macroReadMR( tmpFloat, tlhc_S );
  setProperty( "genesis_tlhc_S", tmpFloat );
  macroReadMR( tmpFloat, trhc_R );
  setProperty( "genesis_trhc_R", tmpFloat );
  macroReadMR( tmpFloat, trhc_A );
  setProperty( "genesis_trhc_A", tmpFloat );
  macroReadMR( tmpFloat, trhc_S );
  setProperty( "genesis_trhc_S", tmpFloat );
  macroReadMR( tmpFloat, brhc_R );
  setProperty( "genesis_brhc_R", tmpFloat );
  macroReadMR( tmpFloat, brhc_A );
  setProperty( "genesis_brhc_A", tmpFloat );
  macroReadMR( tmpFloat, brhc_S );
  setProperty( "genesis_brhc_S", tmpFloat );
  macroReadMR( tmpFloat, nex );
  setProperty( "genesis_nex", tmpFloat );
  
  // Requested by Paola
  macroReadMR( tmpInt, autoxmtfreq );
  setProperty( "genesis_autoxmtfreq", tmpInt );
  macroReadMR( tmpShort, autoxmtgain );
  setProperty( "genesis_autoxmtgain", tmpShort );
  macroReadMR( tmpShort, prescan_r1 );
  setProperty( "genesis_prescan_r1", tmpShort );
  macroReadMR( tmpShort, prescan_r2 );
  setProperty( "genesis_prescan_r2", tmpShort );

  // Can be interesting
  macroReadMRString( tmpString, psdname );
  setProperty( "genesis_psdname", tmpString );
  tmpList.clear();
  macroReadMR( tmpFloat, user0 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user1 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user2 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user3 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user4 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user5 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user6 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user7 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user8 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user9 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user10 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user11 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user12 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user13 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user14 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user15 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user16 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user17 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user18 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user19 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user20 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user21 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user22 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user23 );
  tmpList.push_back( Object::value( tmpFloat ) );
  macroReadMR( tmpFloat, user24 );
  tmpList.push_back( Object::value( tmpFloat ) );
  setProperty( "genesis_user", tmpList );
  
#undef macroReadMR
#undef macroReadMRString
  } else {
    cerr << "CT image - *** NOT FINISHED ***\n";
  }

  //cerr << "loc: " << loc << endl;
  int	slice = int( rint( loc / sz ) );
  //cerr << "slice: " << slice << endl;
  //cerr << "start loc: " << sloc << ", slice: " << sloc / sz << endl;
  //cerr << "end   loc: " << eloc << ", slice: " << eloc / sz << endl;
  setProperty( "slice", slice );
  setProperty( "start_slice", int( rint( sloc / sz ) ) );
  setProperty( "end_slice", int( rint( eloc / sz ) ) );

  if( dimx != _dimX || dimy != _dimY )
    {
      cerr << "Genesis dimensions inconsistancy: " << _dimX << "x" << _dimY 
	   << " in ImgHdr, " << dimx << "x" << dimy << " in MR header\n";
      _dimX = dimx;
      _dimY = dimy;
    }
  _dimZ = dimz;
  _dimT = 1;
  _sizeX = sx;
  _sizeY = sy;
  _sizeZ = sz;
  _sizeT = 1;
  vector<int>	dims( 4 );
  dims[0] = dimx;
  dims[1] = dimy;
  dims[2] = dimz;
  dims[3] = 1;
  setProperty( "volume_dimension", dims );
  vector<float>	vs( 4 );
  vs[0] = sx;
  vs[1] = sy;
  vs[2] = sz;
  vs[3] = ((float) tr) / 1e6;
  setProperty( "voxel_size", vs );
  if( spc != 0 )
    setProperty( "slice_spacing", spc );
  setProperty( "tr", vs[3] );
  setProperty( "te", float( te ) / 1e3f );
  setProperty( "ti", float( ti ) / 1e3f );
  setProperty( "file_type", string( "GENESIS" ) );
  setProperty( "data_type", dtype );
  setProperty( "object_type", string( "Volume" ) );
}
