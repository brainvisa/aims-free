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

#include <aims/io/tiffheader.h>

#include <cartobase/exception/ioexcept.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include <dirent.h>
extern "C"
{
#include <tiffio.h>
}

using namespace aims;
using namespace carto;
using namespace std;


set<string> TiffHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".tif" );
  exts.insert( ".tiff" );
  exts.insert( ".Tif" );
  exts.insert( ".Tiff" );
  return exts;
}


void TiffHeader::read()
{
  string fileName = _name;
  ushort  typesize, mbps, spp, unit, sampleformat, photometric;
  ostringstream oss;

  _dimZ = 0;
  TIFFSetErrorHandler( 0 );
  TIFFSetWarningHandler( 0 );


  TIFF* tif = TIFFOpen( _name.c_str(), "r" );
  if( !tif )
    {
      string	base = removeExtension( _name );
      set<string>	exts = extensions();
      set<string>::iterator	ie, ee = exts.end();
      for( ie=exts.begin(); ie!=ee && !tif; ++ie )
        {
          fileName = base + *ie;
          tif = TIFFOpen( fileName.c_str(), "r" );
        }
    }
  if( !tif )
    throw file_not_found_error( _name );

    //printf("Fichier ouvert \n");
  _name = fileName;

  vector<string> pt;
  _dimZ = TIFFNumberOfDirectories(tif);
  
  TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &mbps);
  TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
  TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &unit);
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &_dimX);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &_dimY);
  TIFFGetField(tif, TIFFTAG_XRESOLUTION, &_sizeX);
  TIFFGetField(tif, TIFFTAG_YRESOLUTION, &_sizeY);
  if( TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleformat) == 0 )
    sampleformat = SAMPLEFORMAT_UINT;
  if( TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric) == 0 )
    photometric = PHOTOMETRIC_MINISBLACK;
            
  TIFFClose(tif);

  /* ici code du default val en cm que nous convertissons en mm */
  _sizeX = 10 / _sizeX;
  _sizeY = 10 / _sizeY;

  /* ici machinerie pour determiner l'unite */
  if (unit == RESUNIT_INCH) {
    _sizeX *= 2.54;
    _sizeY *= 2.54;
  }
  
  _type = "";
  switch( spp ) {
    case 1:
    case 2:
        switch( sampleformat )
        {
            case SAMPLEFORMAT_VOID:
                switch ( photometric ) {
                    case PHOTOMETRIC_PALETTE :
                        // Indexed colormap
                        _type = "RGBA";
                        pt.push_back( "RGBA" );
                        pt.push_back( "RGB" );
                        break;
                    default:
                        _type = "unknown";
                        break;
                }
                break;
            case SAMPLEFORMAT_UINT:
                typesize = ( spp * mbps );
                if ( ( typesize % 8 ) > 0 ) {
                    // Get the next multiple of 8
                    typesize = ( ( typesize / 8 ) + 1 ) * 8;
                }
                oss << ( typesize );
                _type = "U" + oss.str();
                pt.push_back( _type );
                pt.push_back( "RGB" );
                pt.push_back( "RGBA" );
                break;
            case SAMPLEFORMAT_INT:
                typesize = ( spp * mbps );
                if ( ( typesize % 8 ) > 0 ) {
                // Get the next multiple of 8
                typesize = ( ( typesize / 8 ) + 1 ) * 8;
                }
                oss << ( typesize );
                _type = "S" + oss.str();
                pt.push_back( _type );
                pt.push_back( "RGB" );
                pt.push_back( "RGBA" );
                break;
            case SAMPLEFORMAT_IEEEFP:
                if( mbps == 32 )
                _type = "FLOAT";
                else
                _type = "DOUBLE";
                pt.push_back( _type );
                break;
            case SAMPLEFORMAT_COMPLEXINT:
            case SAMPLEFORMAT_COMPLEXIEEEFP:
                if( mbps == 32 ) // not sure it is 32 or 64...
                _type = "CFLOAT";
                else
                _type = "CDOUBLE";
                pt.push_back( _type );
                break;
            default: // undefined
                _type = "unknown";
                break;
        }
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

  _dimT =  1;
  
  setProperty( "file_type", string( "TIFF" ) );
  
  
  vector<int>	dims;
  dims.push_back( dimX() );
  dims.push_back( dimY() );
  dims.push_back( dimZ() );
  dims.push_back( dimT() );
  setProperty( "volume_dimension", dims );

  vector<float>	vs;
  vs.push_back( sizeX() );
  vs.push_back( sizeY() );
  vs.push_back( sizeZ() );
  vs.push_back( sizeT() );
  setProperty( "voxel_size", vs );
  setProperty( "data_type", _type );
  if( !pt.empty() )
    setProperty( "possible_data_types", pt );

  // add meta-info to header
  readMinf( fileName + ".minf" );

  inputFilenames();
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


string TiffHeader::extension() const
{
  set<string> pext = extensions();
  set<string>::const_iterator i, e = pext.end();
  for( i=pext.begin(); i!=e; ++i )
    if( _name.substr( _name.length() - i->length(), i->length() ) == *i )
      return *i;
  return ".tiff";
}


vector<string> TiffHeader::possibleDataTypes() const
{
  vector<string>	pt;
  getProperty( "possible_data_types", pt );
  return( pt );
}


void TiffHeader::setType( const string & t )
{
  _type = t;
  setProperty( "data_type", t );
}
