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

/*
 *  lecture de fichiers FDF
 */

#include <cartobase/stream/fileutil.h>
#include <aims/io/fdfutil.h>
#include <aims/io/fdfheader.h>

#include <cartobase/exception/ioexcept.h>
#include <cartobase/type/byte_order.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include <dirent.h>

#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>

using namespace aims;
using namespace carto;
using namespace std;

set<string> FdfHeader::extensions() const
{
  set<string>   exts;
  exts.insert( ".fdf" );
  exts.insert( ".Fdf" );
  return exts;
}

vector<string> FdfHeader::possibleDataTypes() const
{
  vector<string>    pt;
  getProperty( "possible_data_types", pt );
  return( pt );
}


void FdfHeader::setType( const string & t )
{
  _type = t;
  setProperty( "data_type", t );
}

void FdfHeader::read()
{
  // Header elements
  vector<float> resolutions, location, span, roi;
  string spatial_rank, checksum, storage, bits, bigendian;
  vector<int> matrix, dims;
  vector<string> pt;
  int typesize = 32, bits_allocated = 32, matrix_size = 1, byte_swapping;
  uint byte_order = 1;

  // Parsing elements
  string line;
  vector<std::string> tokens;
  string type, name, value;

  // Open file to read
  std::ifstream inFile( _name.c_str(), std::ios::in | std::ios::binary );

  // Check if there was an error opening the file
  if (!inFile)
  {
    io_error::launchErrnoExcept( _name );
  }

  while (getline(inFile, line, '\n')) {

    if ( line == "\0" ) {
      break;
    }

    // Formats the lines in the FDF header such as removing whitespace between {}
    line = ParseLine(line);
    Tokenize(line, tokens, " ;");
    
    if(tokens.size() == 4) {
                                                                                                                            
          type = tokens[0];
          name = tokens[1];
          value = tokens[3];

          if (name == "spatial_rank") {
              spatial_rank = value;
          }

          if (name == "matrix") {
              StringToVector(value, matrix);

              for(unsigned int i=0; i<matrix.size(); i++) {
                  matrix_size *= matrix[i];
                  // Set the size of each dimension
                  dims.push_back( matrix[i] );
              }
          }

          if (name == "span") {
              StringToVector(value, span);
          }

          if (name == "roi") {
              StringToVector(value, roi);
          }

          if (name == "location") {
              StringToVector(value, location);
          }

          // Get the binary data type
          if (name == "storage") {
              storage = value;

              if (value == "double" ) {
                  _type = "DOUBLE";
                  typesize = sizeof(double);
              }
              else if (value == "float" ) {
                  _type = "FLOAT";
                  typesize = sizeof(float);
              }
              else if (value == "long" ) {
                  _type = "S64";
                  typesize = sizeof(long);
              }
              else if (value == "unsigned long" ) {
                  _type = "U64";
                  typesize = sizeof(unsigned long);
              }
              else if (value == "int" ) {
                  _type = "S32";
                  typesize = sizeof(int);
              }
              else if (value == "unsigned int" ) {
                  _type = "U32";
                  typesize = sizeof(unsigned int);
              }
              else if (value == "short" ) {
                  _type = "S16";
                  typesize = sizeof(short);
              }
              else if (value == "unsigned short" ) {
                  _type = "U16";
                  typesize = sizeof(unsigned short);
              }
              else if (value == "char" ) {
                  _type = "S8";
                  typesize = sizeof(char);
              }
              else if (value == "unsigned char" ) {
                  _type = "U8";
                  typesize = sizeof(unsigned char);
              }
          }

          // Get the bits
          if (name == "bits") {
              ConvertFromString (value, bits);
              istringstream is(bits);
              is >> bits_allocated;
          }

          // Get the bits order
          if (name == "bigendian") {
              ConvertFromString (value, bigendian);
              istringstream is(bigendian);
              is >> byte_order;
          }

          // Get the checksum
          if (name == "checksum") {
              ConvertFromString (value, checksum);
          }

      }

      tokens.clear();
  }

  for(unsigned int i=0; i<matrix.size(); i++) {
    resolutions.push_back( (roi[i] * 10 ) / matrix[i] );
  }

  // Get image type
  switch( bits_allocated / ( typesize * 8 ) ) {
    case 1:
    case 2:
      pt.push_back( _type );
      pt.push_back( "RGB" );
      pt.push_back( "RGBA" );
      break;
    case 3:
      _type = "RGB";
      pt.push_back( "RGB" );
      pt.push_back( "RGBA" );
      break;
    case 4:
      _type = "RGBA";
      pt.push_back( "RGBA" );
      pt.push_back( "RGB" );
      break;
    default:
      break;
  }

  if ( dims.size() > 0 ) {
    _dimX = dims[0];
  }
  if ( dims.size() > 1 ) {
    _dimY = dims[1];
  }
  if ( dims.size() > 2 ) {
    _dimZ = dims[2];
  }
  if ( dims.size() > 3 ) {
    _dimT =  dims[3];
  }

  if ( resolutions.size() > 0 ) {
    _sizeX = resolutions[0];
  }
  if ( resolutions.size() > 0 ) {
    _sizeY = resolutions[1];
  }
  if ( resolutions.size() > 0 ) {
    _sizeZ = resolutions[2];
  }
  if ( resolutions.size() > 0 ) {
    _sizeT = resolutions[3];
  }

  setProperty( "file_type", string( "FDF" ) );
  setProperty( "bits_allocated", bits_allocated );

  if (byte_order) {
    byte_swapping = stringToByteOrder( "DCBA" );
  }
  else {
    byte_swapping = stringToByteOrder( "ABCD" );
  }
  setProperty( "byte_swapping", byte_swapping );

  setProperty( "volume_dimension", dims );
  setProperty( "voxel_size", resolutions );
  setProperty( "data_type", _type );

  if( !pt.empty() )
    setProperty( "possible_data_types", pt );

  // add meta-info to header
  readMinf( removeExtension( _name ) + extension() + ".minf" );

}
