/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#include <aims/fibers/trackvisbundlereader.h>
#include <cartobase/type/byte_order.h>
#include <aims/transformation/affinetransformation3d.h>

using namespace aims;
using namespace carto;
using namespace std;


//-----------------------------------------------------------------------------
TrackvisBundleReader::TrackvisBundleReader( const string &fileName )
  : BundleFormatReader()
{
  _fileName = fileName;
}


//-----------------------------------------------------------------------------
TrackvisBundleReader::~TrackvisBundleReader()
{
}


//-----------------------------------------------------------------------------
Object TrackvisBundleReader::readHeader()
{
  ifstream file( _fileName.c_str(), ios::binary | ios::in );
  if( !file.good() )
    io_error::launchErrnoExcept( _fileName );

  return readHeaderStream( file );
}


//-----------------------------------------------------------------------------
Object TrackvisBundleReader::readHeaderStream( istream & file )
{
  // Read header
  Object header;
  header = Object::value( Dictionary() );

  char id_string[6];
  unsigned int i;

  file.read( id_string, 6 );
  if( !file.good() )
    throw wrong_format_error( _fileName );
  id_string[5] = '\0';
  if( string( id_string ) != "TRACK" )
    throw wrong_format_error( _fileName );

  short dim[3];
  vector<float> voxel_size(3);
  vector<float> origin(3);
  short n_scalars;
  vector<string> scalar_name;
  short n_properties;
  vector<string> property_name;
  vector<float> vox_to_ras(16);
  char voxel_order[4];
  char pad2[4];
  vector<float> image_orientation_patient(6);
  char pad1[2];
  unsigned char invert_x;
  unsigned char invert_y;
  unsigned char invert_z;
  unsigned char swap_xy;
  unsigned char swap_yz;
  unsigned char swap_zx;
  int n_count;
  int version;
  int hdr_size;

  file.seekg( 996, istream::beg );
  if( !file.good() )
    throw wrong_format_error( _fileName );

  file.read( reinterpret_cast<char *>( &hdr_size ), 4 );
  if( !file.good() )
    throw wrong_format_error( _fileName );

  header->setProperty( "byte_swapping", int( hdr_size != 1000 ) );
  ByteSwapper byteSwapper( hdr_size == 1000 ? byteOrder() : byteOrder() + 1 );
  file.seekg( 6, istream::beg );
  if( !file.good() )
    throw wrong_format_error( _fileName );

  byteSwapper.read( file, dim[0] );
  byteSwapper.read( file, dim[1] );
  byteSwapper.read( file, dim[2] );
  if( !file.good() )
    throw wrong_format_error( _fileName );
  vector<int> int_dims(3);
  int_dims[0] = dim[0];
  int_dims[1] = dim[1];
  int_dims[2] = dim[2];
  header->setProperty( "dim", int_dims );

  byteSwapper.read( file, voxel_size[0] );
  byteSwapper.read( file, voxel_size[1] );
  byteSwapper.read( file, voxel_size[2] );
  if( !file.good() )
    throw wrong_format_error( _fileName );
  header->setProperty( "voxel_size", voxel_size );

  byteSwapper.read( file, origin[0] );
  byteSwapper.read( file, origin[1] );
  byteSwapper.read( file, origin[2] );
  if( !file.good() )
    throw wrong_format_error( _fileName );
  header->setProperty( "origin", origin );

  byteSwapper.read( file, n_scalars );
  if( !file.good() )
    throw wrong_format_error( _fileName );
  header->setProperty( "n_scalars", int(n_scalars) );

  char tmp_str[21];
  tmp_str[20] = '\0';
  scalar_name.reserve( 10 );
  for( i=0; i<10; ++i )
  {
    file.read( tmp_str, 20 );
    if( !file.good() )
      throw wrong_format_error( _fileName );
    string tmp_str2 = tmp_str;
    if( !tmp_str2.empty() )
    {
      while( scalar_name.size() < i )
        scalar_name.push_back( "" );
      scalar_name.push_back( tmp_str2 );
    }
  }
  if( !scalar_name.empty() )
    header->setProperty( "scalar_name", scalar_name );

  byteSwapper.read( file, n_properties );
  if( !file.good() )
    throw wrong_format_error( _fileName );
  header->setProperty( "n_properties", int(n_properties) );

  property_name.reserve( 10 );
  for( i=0; i<10; ++i )
  {
    file.read( tmp_str, 20 );
    if( !file.good() )
      throw wrong_format_error( _fileName );
    string tmp_str2 = tmp_str;
    if( !tmp_str2.empty() )
    {
      while( property_name.size() < i )
        property_name.push_back( "" );
      property_name.push_back( tmp_str2 );
    }
  }
  if( !property_name.empty() )
    header->setProperty( "property_name", property_name );

  for( i=0; i<16; ++i )
  {
    byteSwapper.read( file, vox_to_ras[i] );
    if( !file.good() )
      throw wrong_format_error( _fileName );
  }
  if( vox_to_ras[15] != 0 )
    header->setProperty( "vox_to_ras", vox_to_ras );

  file.seekg( 444, istream::cur );
  if( !file.good() )
    throw wrong_format_error( _fileName );

  file.read( voxel_order, 4 );
  if( !file.good() )
    throw wrong_format_error( _fileName );

  file.read( pad2, 4 );
  if( !file.good() )
    throw wrong_format_error( _fileName );

  for( i=0; i<6; ++i )
  {
    byteSwapper.read( file, image_orientation_patient[i] );
    if( !file.good() )
      throw wrong_format_error( _fileName );
  }
  header->setProperty( "image_orientation_patient",
                       image_orientation_patient );

  file.read( pad1, 2 );
  if( !file.good() )
    throw wrong_format_error( _fileName );

  file.read( reinterpret_cast<char *>( &invert_x ), 1 );
  file.read( reinterpret_cast<char *>( &invert_y ), 1 );
  file.read( reinterpret_cast<char *>( &invert_z ), 1 );
  if( !file.good() )
    throw wrong_format_error( _fileName );
  header->setProperty( "invert_x", invert_x );
  header->setProperty( "invert_y", invert_y );
  header->setProperty( "invert_z", invert_z );

  file.read( reinterpret_cast<char *>( &swap_xy ), 1 );
  file.read( reinterpret_cast<char *>( &swap_yz ), 1 );
  file.read( reinterpret_cast<char *>( &swap_zx ), 1 );
  if( !file.good() )
    throw wrong_format_error( _fileName );
  header->setProperty( "swap_xy", swap_xy );
  header->setProperty( "swap_yz", swap_yz );
  header->setProperty( "swap_zx", swap_zx );

  byteSwapper.read( file, n_count );
  header->setProperty( "n_count", n_count );

  if( !file.good() )
    throw wrong_format_error( _fileName );

  byteSwapper.read( file, version );
  if( !file.good() )
    throw wrong_format_error( _fileName );
  header->setProperty( "version", version );

  byteSwapper.read( file, hdr_size );
  if( !file.good() )
    throw wrong_format_error( _fileName );
  if( hdr_size != 1000 )
    throw wrong_format_error( _fileName );

  AffineTransformation3d aims_s2m, aims_tr;
  if( !invert_x )
  {
    aims_s2m.rotation()( 0, 0 ) = -1;
    aims_s2m.matrix()(0, 3) = dim[0] - 1;
  }
  if( !invert_y )
  {
    aims_s2m.rotation()( 1, 1 ) = -1;
    aims_s2m.matrix()(1, 3) = dim[1] - 1;
  }
  if( !invert_z )
  {
    aims_s2m.rotation()( 2, 2 ) = -1;
    aims_s2m.matrix()(2, 3) = dim[2] - 1;
  }
  // TODO: handle flips in s2m
  header->setProperty( "storage_to_memory", aims_s2m.toVector() );

  if( header->getProperty( "vox_to_ras", vox_to_ras )
    && vox_to_ras.size() == 16 )
  {
    AffineTransformation3d v2r( vox_to_ras );
    AffineTransformation3d tvs;
    tvs.rotation()( 0, 0 ) = 1. / voxel_size[0];
    tvs.rotation()( 1, 1 ) = 1. / voxel_size[1];
    tvs.rotation()( 2, 2 ) = 1. / voxel_size[2];
    aims_tr = v2r * aims_s2m.inverse() * tvs;
    vector<vector<float> > trans;
    trans.push_back( aims_tr.toVector() );
    header->setProperty( "transformations", trans );
    vector<string> refs;
    refs.push_back( "Scanner-based anatomical coordinates" );
    header->setProperty( "referentials", refs );
  }

  // get data size
  streampos pos = file.tellg();
  file.seekg( 0, ios::end );
  header->setProperty( "data_size", size_t( file.tellg() - pos ) );
  file.seekg( pos, ios::beg );

  return header;
}


void TrackvisBundleReader::read()
{
  ifstream file( _fileName.c_str(), ios::binary | ios::in );
  if( !file.good() )
    io_error::launchErrnoExcept( _fileName );

  Object header = readHeaderStream( file );

  int n_count;
  int n_scalars;
  int n_properties;
  int byte_swapping;
  vector<int> dim;
  vector<float> vs;
  vector<float> s2m_v;

  header->getProperty( "n_count", n_count );
  header->getProperty( "n_scalars", n_scalars );
  header->getProperty( "n_properties", n_properties );
  header->getProperty( "byte_swapping", byte_swapping );
  header->getProperty( "dim", dim );
  header->getProperty( "voxel_size", vs );
  header->getProperty( "storage_to_memory", s2m_v );
  ByteSwapper byteSwapper;
  byteSwapper.setSwapped( byte_swapping );

  AffineTransformation3d s2m( s2m_v );
  AffineTransformation3d tvs;
  tvs.rotation()( 0, 0 ) = vs[0];
  tvs.rotation()( 1, 1 ) = vs[1];
  tvs.rotation()( 2, 2 ) = vs[2];
  s2m = tvs * s2m * tvs.inverse(); // s2m in mm -> mm

  int npts, p;
  FiberPoint pos;
  int i = 0;
  BundleInfo currentBundle( i, "trackvis_bundle" );
  startBundle( currentBundle );

  for( i=0; i<n_count || ( n_count == 0 && file.good() && !file.eof() ); ++i )
  {
    byteSwapper.read( file, npts );
    if( !file.good() )
    {
      if( n_count == 0 )
        break; // here we just detect the end of file
      io_error::launchErrnoExcept( _fileName );
    }
    FiberInfo fiberInfo( i );
    startFiber( currentBundle, fiberInfo );

    for( p=0; p<npts; ++p )
    {
      byteSwapper.read( file, pos[0] );
      byteSwapper.read( file, pos[1] );
      byteSwapper.read( file, pos[2] );
      // this transform is to get to AIMS coordinates,
      // like connectomist tracts
      Point3df pt = s2m.transform( Point3df( pos[0], pos[1], pos[2] ) );
      pos[0] = pt[0];
      pos[1] = pt[1];
      pos[2] = pt[2];
      if( !file.good() )
        io_error::launchErrnoExcept( _fileName );
      addFiberPoint( currentBundle, fiberInfo, pos );
      file.seekg( n_scalars * 4, istream::cur );
      if( !file.good() )
        io_error::launchErrnoExcept( _fileName );
    }
    terminateFiber( currentBundle, fiberInfo );
    // track properties
    file.seekg( n_properties * 4, istream::cur );
  }
  terminateBundle( currentBundle );
  noMoreBundle();

}

