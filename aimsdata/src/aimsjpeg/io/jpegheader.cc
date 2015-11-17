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
#include <aims/io/jpegheader.h>

#include <cartobase/exception/file.h>
#include <cartobase/stream/directory.h>
#include <cartobase/stream/fileutil.h>
#include <unistd.h>
#include <sys/types.h>
#include <regex.h>
#include <vector>
#include <dirent.h>
#include <stdio.h>
#include <setjmp.h>
extern "C"
{
#include <jpeglib.h>
}

using namespace aims;
using namespace carto;
using namespace std;


set<string> JpegHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".jpg" );
  exts.insert( ".JPG" );
  exts.insert( ".jpeg" );
  exts.insert( ".JPEG" );
  return exts;
}


namespace
{

  struct private_jpeg_error_mgr
  {
    struct jpeg_error_mgr pub;	/* "public" fields */
    jmp_buf setjmp_buffer;	/* for return to caller */
    FILE *fp;
  };

  METHODDEF(void)
  private_jpeg_error_exit (j_common_ptr cinfo)
  {
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    private_jpeg_error_mgr *myerr = (private_jpeg_error_mgr *) cinfo->err;
    // close the file
    if( myerr->fp )
      fclose( myerr->fp );

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
  }

}


void JpegHeader::read()
{
  string fileName = _name;
  if( FileUtil::fileStat( fileName ).find( '+' ) == string::npos )
    fileName = removeExtension(_name) + ".jpg";

  struct jpeg_decompress_struct	cinfo;
  struct private_jpeg_error_mgr	jerr;
  FILE				*fp;

  cinfo.err = jpeg_std_error( &jerr.pub );
  jerr.pub.error_exit = private_jpeg_error_exit;
  jerr.fp = 0;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer))
  {
    /* If we get here, the JPEG code has signaled an error.
      * We need to clean up the JPEG object, close the input file, and return.
      */
    jpeg_destroy_decompress(&cinfo);
    throw carto::file_error( fileName );
  }
  jpeg_create_decompress( &cinfo );

  fp = fopen( fileName.c_str(), "rb" );
  jerr.fp = fp;
  if( !fp )
  {
    jpeg_destroy_decompress( &cinfo );
    throw carto::file_error( fileName );
  }

  jpeg_stdio_src( &cinfo, fp );
  if( jpeg_read_header( &cinfo, true ) != 1 )
  {
    jpeg_destroy_decompress( &cinfo );
    fclose( fp );
    throw carto::file_error( fileName );
  }
  fclose( fp );
  jerr.fp = 0;
  jpeg_destroy_decompress( &cinfo );

  vector<string>	pt;

  if( cinfo.num_components == 1 )
    switch( cinfo.data_precision )
    {
    case 16:
      _type = "U16";
      pt.push_back( "U16" );
      pt.push_back( "RGB" );
      break;
    default:
      _type = "U8";
      pt.push_back( "U8" );
      pt.push_back( "RGB" );
      break;
    }
  else
  {
    _type = "RGB";
    pt.push_back( "RGB" );
    pt.push_back( "U8" );
  }

  _dimX = cinfo.image_width;
  _dimY = cinfo.image_height;
  _dimZ = 1;
  _dimT = 1;
  if( cinfo.X_density == 0 || cinfo.density_unit == 0 )	// not filled
    _sizeX = 1;
  else
    _sizeX = ( cinfo.density_unit == 2 ? 1. : 25.4 ) / cinfo.X_density;
  if( cinfo.Y_density == 0 || cinfo.density_unit == 0 )	// not filled
    _sizeY = 1;
  else
    _sizeY = ( cinfo.density_unit == 2 ? 1. : 25.4 ) / cinfo.Y_density;
  _sizeZ = 1;
  _sizeT = 1;

  setProperty( "file_type", string( "JPEG" ) );

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
  readMinf( removeExtension( _name ) + extension() + ".minf" );

  inputFilenames();

  // if .minf has been modified
  if( getProperty( "volume_dimension", dims ) )
    if( dims.size() >= 1 )
    {
      _dimX = dims[0];
      if( dims.size() >= 2 )
      {
        _dimY = dims[1];
        if( dims.size() >= 3 )
        {
          _dimZ = dims[2];
          if( dims.size() >= 4 )
          {
            _dimT = dims[3];
          }
        }
      }
    }

  getProperty( "voxel_size", vs );
  if( vs.size() >= 3 )
    {
      _sizeX = vs[0];
      _sizeY = vs[1];
      _sizeZ = vs[2];
      if( vs.size() >= 4 )
        _sizeT = vs[3];
    }
}


vector<string> JpegHeader::possibleDataTypes() const
{
  vector<string>	pt;
  getProperty( "possible_data_types", pt );
  return( pt );
}


void JpegHeader::setType( const string & t )
{
  _type = t;
  setProperty( "data_type", t );
}


string JpegHeader::extension() const
{
  string ext = FileUtil::extension( _name );
  set<string> exts = extensions();
  if( !ext.empty() && exts.find( string( "." ) + ext ) != exts.end() )
    return string( "." ) + ext;
  return ".jpg";
}



