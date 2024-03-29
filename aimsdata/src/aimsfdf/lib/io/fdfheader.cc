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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  lecture de fichiers FDF
 */

#include <cartobase/exception/ioexcept.h>
#include <cartobase/type/byte_order.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/directory.h>
#include <cartobase/type/string_conversion.h>
#include <aims/io/fdfutil.h>
#include <aims/io/fdfheader.h>
#include <aims/io/fdfprocpar_g.h>
#include <cartobase/type/limits.h>

#include <iostream>
#include <iomanip>
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

set<string> FdfHeader::getFiles(string) const
{
    set<string> result;
    return result;
}

void FdfHeader::read()
{
  // Extension check
  set<string>   exts = extensions();

  if (exts.find("." + stringLower(FileUtil::extension(_name))) == exts.end())
  {
    throw invalid_format_error( "FDF file has not a consistent extension", _name );
  }

  // Header elements
  // Motion transfo;
  vector<float> resolutions, location, orientation, span, roi, storagetomemory(16);
  vector<int> matrix, dims;
  vector<string> pt;
  bool chksumfound = false;
  long chksum = 0;
  int typesize = 32, bits_allocated = 32, byte_swapping = 0;
  int rank = 2, dimz = 1;

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

  // Initialize the transformation
  for (uint x=0; x<4; x++)
    for (uint y=0; y<4; y++) {
        storagetomemory[4*x + y] = ((x == y) ? 1 : 0);
    }

  bool headerstarted = false;

  int linenum = 0;
  while (inFile && getline(inFile, line, '\n'))
  {
    if ( stringStrip(line) == "\0" ) {
      if ( not headerstarted ) {
        continue;
      }
      else {
        break;
      }
    }
    if( !inFile ) {
      io_error::launchErrnoExcept( _name );
    }

    // Formats the lines in the FDF header such as removing whitespace between {}
    try
    {
      line = parseLine(line);
    }
    catch( parse_error & )
    {
      throw parse_error( "invalid character encountered", "", _name, linenum );
    }
    tokenize(line, tokens, " ;");
    if(tokens.size() == 4) {

      type = tokens[0];
      if ( ( type != "/*" ) && ( type != "//" ) ) {
          headerstarted=true;
          name = tokens[1];
          value = tokens[3];

          if (name == "rank") {
              stringTo(value, rank);
          }

          else if (name == "slices") {
              stringTo(value, dimz);
          }
            
          else if (name == "matrix") {
              stringToVector(value, matrix);

              for(uint i=0; i < matrix.size(); i++) {
                  // Set the size of each dimension
                  dims.push_back( matrix[i] );
              }
          }

          else if (name == "span") {
              stringToVector(value, span);

              if (!span.empty()) {
                setProperty( "span", span);
              }
          }

          else if (name == "roi") {
              stringToVector(value, roi);

              if (!roi.empty()) {
                setProperty( "roi", roi);
              }
          }

          else if (name == "location") {
              stringToVector(value, location);
              if (!location.empty()) {
                for (uint i=0; i < 3; i++) {
                    storagetomemory[ 4 * i + 3 ] = location[i];
                }
              }
          }

          else if (name == "orientation"){
              stringToVector(value, orientation);
              if (!orientation.empty()) {
                for (uint x=0; x < 3; x++)
                    for (uint y=0; y < 3; y++) {
                        storagetomemory[ 4 * x + y ] = orientation[ 3 * x + y];
                    }
              }
          }

          // Get the binary data type
          else if (name == "storage") {

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
          else if (name == "bits") {
              stringTo(value, bits_allocated);
          }

          // Get the bits order
          else if (name == "bigendian") {
              stringTo(value, byte_swapping);
          }

          // Get the checksum
          else if (name == "checksum") {
              stringTo(value, chksum);
              chksumfound = true;
              break;
          }
      }
    }
    tokens.clear();
    ++linenum;
  }

  if( ( !chksumfound ) || ( matrix.size() == 0 ) || ( roi.size() == 0 ) ) {
    throw wrong_format_error( _name );
  }

  if( matrix.size() > roi.size() ) {
    throw invalid_format_error( "matrix and roi attributes in FDF file are not consistent", _name );
  }
  
  // Process image resolution
  for(unsigned int i=0; i<matrix.size(); i++) {
    resolutions.push_back( (roi[i] * 10 ) / matrix[i] );
  }

  if ((matrix.size() < 3) && (rank == 2) && (dimz > 1)) {
    // We add a third dimension to the image
    dims.push_back(dimz);
    resolutions.push_back(roi[2] * 10);
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

  if( dims.size() == 0 ) {
    throw invalid_format_error( "no dimensions in FDF file", _name );
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

  // Set properties
  //setProperty( "storage_to_memory", storagetomemory);
  setProperty( "rank", rank );
  setProperty( "file_type", string( "FDF" ) );
  setProperty( "bits_allocated", bits_allocated );
  setProperty( "byte_swapping", byte_swapping );
  setProperty( "volume_dimension", dims );
  setProperty( "voxel_size", resolutions );
  setProperty( "data_type", _type );

  if( !pt.empty() )
    setProperty( "possible_data_types", pt );

  // add meta-info to header
  readProcPar( FileUtil::dirname( _name ) + FileUtil::separator() + "procpar" );
  readMinf( removeExtension( _name ) + extension() + ".minf" );

  // check if rank have changed
  rank = (int)getProperty( "rank" )->getScalar();

  string pattern;
  if (rank < 3) {
    // We use 2D slices to try to get a complete volume
    inputFilenames();
  }

  // check if dimensions have changed
  getProperty( "volume_dimension", dims );
  if( dims.size() >= 3 )
  {
      _dimX = dims[0];
      _dimY = dims[1];
      _dimZ = dims[2];
      if( dims.size() >= 4 )
          _dimT = dims[3];
  }
}

void FdfHeader::readProcPar( string name ) {
    string stat = FileUtil::fileStat( name );

    if ( stat.find('r') != string::npos ) {
        // File exists and is readable
        FdfProcPar procpar( name );
    
        // Set header properties
        setProperty( "manufacturer",  "varian" );
    
        string patientid = procpar.value<string>("ident");
        if (!patientid.empty()) {
            setProperty( "patient_id", patientid );
        }
    
        string studyid = procpar.value<string>("studyid_");
        if (!studyid.empty()) {
            setProperty( "study_id", studyid );
        }
    
        vector< float > patientposition;
        float position1 = procpar.value<float>("position1");
        float position2 = procpar.value<float>("position2");
    
        if (position1 != 0.) {
            patientposition.push_back( position1 );
        }
        if (position2 != 0.) {
            patientposition.push_back( position2 );
        }
        if (! patientposition.empty() ) {
            setProperty( "patient_position", patientposition );
        }
        
        string acquisitionmode = procpar.value<string>("acquisition_mode");
        if (!acquisitionmode.empty()) {
            setProperty( "acquisition_mode", acquisitionmode );
        }
        
        string tn = procpar.value<string>("tn");
        if (!tn.empty()) {
            setProperty( "tn", tn );
        }
    
        string dn = procpar.value<string>("dn");
        if (!dn.empty()) {
            setProperty( "dn", dn );
        }
    
        vector<float> nuclearfrequency;
        float sfrq = procpar.value<float>("sfrq");
        float dfrq = procpar.value<float>("dfrq");
    
        if (sfrq != 0.) {
            nuclearfrequency.push_back( sfrq );
        }
        if (dfrq != 0.) {
            nuclearfrequency.push_back( dfrq );
        }
        if (!nuclearfrequency.empty()) {
            setProperty( "nuclear_frequency", nuclearfrequency );
        }
    
        float lro = procpar.value<float>("lro");
        if (lro != 0.) {
            setProperty( "lro", lro );
        }
    
        float lpe = procpar.value<float>("lpe");
        if (lpe != 0.) {
            setProperty( "lpe", lpe );
        }
    
        float lpe2 = procpar.value<float>("lpe2");
        if (lpe2 != 0.) {
            setProperty( "lpe2", lpe2 );
        }
    
        float pro = procpar.value<float>("pro");
        if (pro != 0.) {
            setProperty( "pro", pro );
        }
    
        float ppe = procpar.value<float>("ppe");
        if (ppe != 0.) {
            setProperty( "ppe", ppe );
        }
    
        float ppe2 = procpar.value<float>("ppe2");
        if (ppe2 != 0.) {
            setProperty( "ppe2", ppe2 );
        }
    
        float thk = procpar.value<float>("thk");
        if (thk != 0.) {
            setProperty( "thk", thk );
        }
    
        float gap = procpar.value<float>("gap");
        if (gap != 0.) {
            setProperty( "gap", gap );
        }
    
        float psi = procpar.value<float>("psi");
        if (psi != 0.) {
            setProperty( "psi", psi );
        }
    
        float phi = procpar.value<float>("phi");
        if (phi != 0.) {
            setProperty( "phi", phi );
        }
    
        float theta = procpar.value<float>("theta");
        if (theta != 0.) {
            setProperty( "theta", theta );
        }
    
        float dro = procpar.value<float>("dro");
        if (dro != 0.) {
            setProperty( "dro", dro );
        }
        
        float dpe = procpar.value<float>("dpe");
        if (dpe != 0.) {
            setProperty( "dpe", dpe );
        }
    
        float dsl = procpar.value<float>("dsl");
        if (dsl != 0.) {
            setProperty( "dsl", dsl );
        }
    
        float bvalue = procpar.value<float>("bvalue");
        if (bvalue != 0.) {
            setProperty( "b_value", bvalue );
        }
    
        float te = procpar.value<float>("te");
        if (te != 0.) {
            setProperty( "te", te );
        }
    
        float tr = procpar.value<float>("tr");
        if (tr != 0.) {
            setProperty( "tr", tr );
        }
    
        float ti = procpar.value<float>("ti");
        if (ti != 0.) {
            setProperty( "ti", ti );
        }
    
        int ns = procpar.value<int>("ns");
        if (ns != 0) {
            setProperty( "ns", ns );
        }
    
        int ne = procpar.value<int>("ne");
        if (ne != 0) {
            setProperty( "ne", ne );
        }
    }
}

vector<string> FdfHeader::inputFilenames() {

  bool          loop = false;
  string        regex = "";
  string        prefix = "", suffix;
  string        pattern = "";
  string	name = _name;
  string	dirname = FileUtil::dirname( _name );
  regex_t	reg;
  regmatch_t	rmatch[2], pmatch[2];

  name = FileUtil::basename( name );

  // Scan directory for matching files
  Directory	        dir( dirname );
  set<string>	        files = dir.files();
  set<string>::iterator	is, es = files.end();
  vector<string>        foundfiles;

  loop = true;

  while ( loop ) {
    regex = "^" + prefix + "[^0-9]*([0-9]+).*" + extension() + "$";  
    regcomp( &reg, regex.c_str(), REG_EXTENDED | REG_ICASE );

    if( !regexec( &reg, name.c_str(), 2, rmatch, 0 ) ) {

      // We found a number in the file name
      prefix = name.substr( 0, rmatch[1].rm_so );
      suffix = name.substr( rmatch[1].rm_eo );

      pattern = "^" + prefix + "([0-9]+)" + suffix + "$";
      //cout << "Pattern : '" << pattern << "'" << endl << flush;
      regcomp( &reg, pattern.c_str(), REG_EXTENDED | REG_ICASE );

      vector<string> matchingfiles;
      int min = std::numeric_limits<int>::max();
      int max = std::numeric_limits<int>::min();

      for( is=files.begin(); is!=es; ++is ) {
          // We check for files that match the pattern
          if( !regexec( &reg, is->c_str(), 2, pmatch, 0 ) ) {
            int slicenumber;
            istringstream sstream(is->substr( pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so ));
            sstream >> slicenumber;

            if ( slicenumber < min ) {
              min = slicenumber;
            }
            if ( slicenumber > max ) {
              max = slicenumber;
            }
            matchingfiles.push_back( *is );
            //cout << "File : '" << (*is) << "' matches fdf." << endl << flush;
          }
      }
      // cout << "matchingfiles : " << matchingfiles.size() << ", min : " << min << ", max : " << max << endl << flush;

      if ( ( ((int)matchingfiles.size()) == _dimZ ) && ( ( max - min + 1 ) == _dimZ ) ) {
        loop = false;
        foundfiles = matchingfiles;

        vector<Object>	names;
        names.reserve(foundfiles.size());
        for( uint i=0; i<foundfiles.size(); ++i )
          names.push_back( Object::value( foundfiles[i] ) );
        setProperty( "filenames", names );
        //cout << "Found pattern : " << pattern << endl;
      }
      else {
        prefix += name.substr( rmatch[1].rm_so, rmatch[1].rm_eo - rmatch[1].rm_so );
      }
    }
    else {
      loop = false;
    }
  }

  regfree( &reg );
  return foundfiles;
}

