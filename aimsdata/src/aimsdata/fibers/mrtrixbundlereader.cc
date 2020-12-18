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

#include <aims/fibers/mrtrixbundlereader.h>
#include <cartobase/type/byte_order.h>
// #include <aims/transformation/affinetransformation3d.h>
#include <cartobase/stream/stringutil.h>

using namespace aims;
using namespace carto;
using namespace std;


//-----------------------------------------------------------------------------
MRTrixBundleReader::MRTrixBundleReader( const string &fileName )
  : BundleFormatReader()
{
  _fileName = fileName;
}


//-----------------------------------------------------------------------------
MRTrixBundleReader::~MRTrixBundleReader()
{
}


//-----------------------------------------------------------------------------
Object MRTrixBundleReader::readHeader()
{
  ifstream file( _fileName.c_str(), ios::binary | ios::in );
  if( !file.good() )
    io_error::launchErrnoExcept( _fileName );

  return readHeaderStream( file );
}


//-----------------------------------------------------------------------------
Object MRTrixBundleReader::readHeaderStream( istream & file )
{
  // Read header
  Object header;
  header = Object::value( Dictionary() );

  char c;
  char buffer[256];
  unsigned int i;
  bool ok = false;

  map<string, string> types;
  types[ "backtrack" ] = "U64";
  types[ "count" ] = "U64";
  types[ "crop_at_gmwmi" ] = "BOOL";
  types[ "downsample_factor" ] = "U32";
  types[ "init_threshold" ] = "DOUBLE";
  types[ "fod_power" ] = "DOUBLE";
  types[ "lmax" ] = "U64";
  types[ "max_angle" ] = "DOUBLE";
  types[ "max_dist" ] = "DOUBLE";
  types[ "max_num_attempts" ] = "U64";
  types[ "max_num_tracks" ] = "U64";
  types[ "max_seed_attempts" ] = "U64";
  types[ "max_trials" ] = "U64";
  types[ "min_dist" ] = "DOUBLE";
  types[ "output_step_size" ] = "DOUBLE";
  types[ "rk4" ] = "BOOL";
  types[ "samples_per_step" ] = "U64";
  types[ "sh_precomputed" ] = "BOOL";
  types[ "step_size" ] = "DOUBLE";
  types[ "stop_on_all_include" ] = "BOOL";
  types[ "threshold" ] = "DOUBLE";
  types[ "unidirectional" ] = "BOOL";
  types[ "total_count" ] = "U64";

  file.getline( buffer, 256 );
  if( !file.good() )
  {
    throw wrong_format_error( _fileName );
  }
  if( StringUtil::strip( buffer ) != "mrtrix tracks" )
    throw wrong_format_error( _fileName );

  while( !ok )
  {
    file.getline( buffer, 256 );
    if( !file.good() )
      throw wrong_format_error( _fileName );
    string line( StringUtil::strip( buffer ) );
    if( line == "END" )
    {
      ok = true;
      break;
    }

    vector<string> ls = StringUtil::split( line, ":", 1 );
    if( ls.size() != 2 )
        throw wrong_format_error( _fileName );

    string prop = StringUtil::strip( ls[0] );
    string svalue = StringUtil::strip( ls[1] );

    if( prop == "datatype" )
    {
      bool bswap = false;
      if( svalue.length() >= 9 && svalue.substr( 0, 7 ) == "Float32" )
        header->setProperty( "tck_data_type", "FLOAT" );
      else if( svalue.length() >= 9 && svalue.substr( 0, 7 ) == "Float64" )
        header->setProperty( "tck_data_type", "DOUBLE" );

      string endian = svalue.substr( svalue.length() - 2, 2 );
      if( endian == "LE" )
      {
        if( string( byteOrderString() ) == "ABCD" )
          bswap = true;
      }
      else if( string( byteOrderString() ) == "DCBA" )
        bswap = true;

      header->setProperty( "byte_swapping", bswap );

      continue;
    }

    map<string, string>::const_iterator it = types.find( prop );
    if( it != types.end() )
    {
      string t = it->second;
      if( t == "U64")
      {
        uint64_t v;
        stringTo( svalue, v );
        header->setProperty( prop, v );
      }
      else if( t == "S64")
      {
        int64_t v;
        stringTo( svalue, v );
        header->setProperty( prop, v );
      }
      else if( t == "U32")
      {
        uint32_t v;
        stringTo( svalue, v );
        header->setProperty( prop, v );
      }
      else if( t == "S32")
      {
        int32_t v;
        stringTo( svalue, v );
        header->setProperty( prop, v );
      }
      else if( t == "FLOAT")
      {
        float v;
        stringTo( svalue, v );
        header->setProperty( prop, v );
      }
      else if( t == "DOUBLE")
      {
        double v;
        stringTo( svalue, v );
        header->setProperty( prop, v );
      }
      else if( t == "BOOL")
      {
        bool v;
        stringTo( svalue, v );
        header->setProperty( prop, v );
      }
      else
        header->setProperty( prop, svalue );
    }
    else
      header->setProperty( prop, svalue );

    if( prop == "count" )
    {
      header->setProperty( "curves_count", header->getProperty( prop ) );
    }
  }

  // get data size
  streampos pos = file.tellg();
  file.seekg( 0, ios::end );
  header->setProperty( "data_size", size_t( file.tellg() - pos ) );
  file.seekg( pos, ios::beg );

  return header;
}


void MRTrixBundleReader::read()
{
  ifstream file( _fileName.c_str(), ios::binary | ios::in );
  if( !file.good() )
    io_error::launchErrnoExcept( _fileName );

  Object header = readHeaderStream( file );

  string dtype = "FLOAT";
  header->getProperty( "tck_data_type", dtype );

//   int64_t n_count;
  bool byte_swapping;

//   vector<float> s2m_v;
//   unsigned char invert_x = 0, invert_y = 0, invert_z = 0;

//   header->getProperty( "count", n_count );
  header->getProperty( "byte_swapping", byte_swapping );
//   header->getProperty( "storage_to_memory", s2m_v );
  ByteSwapper byteSwapper;
  byteSwapper.setSwapped( byte_swapping );
  string file_offset;
  size_t offset;
  header->getProperty( "file", file_offset );
  vector<string> off_v = StringUtil::split( file_offset, " " );
  if( off_v.size() != 0 )
  {
    stringTo( StringUtil::strip( off_v[ off_v.size() - 1 ] ), offset );
    file.seekg( offset, ios::beg );
  }

//   AffineTransformation3d s2m( s2m_v );

  FiberPoint pos;
  int i = 0;
  BundleInfo currentBundle( i, "mrtrix_bundle" );
  startBundle( currentBundle );

  while( file.good() && !file.eof() )
  {
    FiberInfo fiberInfo( i );
    startFiber( currentBundle, fiberInfo );
    ++i;

    while( file.good() && !file.eof() )
    {
      // TODO handle dtype
      byteSwapper.read( file, pos[0] );
      byteSwapper.read( file, pos[1] );
      byteSwapper.read( file, pos[2] );
      if( ( isnan( pos[0] ) && isnan( pos[1] ) && isnan( pos[2] ) )
          || ( isinf( pos[0] ) && isinf( pos[1] ) && isinf( pos[2] ) ) )
        break;
      // this transform is to get to AIMS coordinates,
      // like connectomist tracts
//       Point3df pt = s2m.transform( Point3df( pos[0], pos[1], pos[2] ) );
      Point3df pt = Point3df( pos[0], pos[1], pos[2] );
      pos[0] = pt[0];
      pos[1] = pt[1];
      pos[2] = pt[2];
      if( !file.good() )
        io_error::launchErrnoExcept( _fileName );
      addFiberPoint( currentBundle, fiberInfo, pos );
    }
    terminateFiber( currentBundle, fiberInfo );
    if( isinf( pos[0] ) && isinf( pos[1] ) && isinf( pos[2] ) )
      break;
  }
  terminateBundle( currentBundle );
  noMoreBundle();

}

