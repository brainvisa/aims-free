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
  set<string>  exts;
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
    struct jpeg_error_mgr pub;  /* "public" fields */
    jmp_buf setjmp_buffer;  /* for return to caller */
    FILE *fp;
  };

  METHODDEF(void)
  private_jpeg_error_exit (j_common_ptr cinfo)
  {
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    private_jpeg_error_mgr *myerr = (private_jpeg_error_mgr *) cinfo->err;
    // close the file
    if( myerr->fp ) {
      fclose( myerr->fp );
      myerr->fp = 0;
    }
    
    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
  }

}


void JpegHeader::read()
{
  string fileName = _name;
  if( FileUtil::fileStat( fileName ).find( '+' ) == string::npos )
    fileName = removeExtension(fileName) + ".jpg";

  struct jpeg_decompress_struct cinfo;
  struct private_jpeg_error_mgr jerr;
  FILE        *fp;

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
  if( jpeg_read_header( &cinfo, TRUE ) != 1 )
  {
    if (fp) {
        fclose( fp );
        fp = 0;
    }
    jpeg_destroy_decompress( &cinfo );
    throw carto::file_error( fileName );
  }
  fclose( fp );
  jerr.fp = 0;
  jpeg_destroy_decompress( &cinfo );

  vector<string>  pt;

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
  if( cinfo.X_density == 0 || cinfo.density_unit == 0 )  // not filled
    _sizeX = 1;
  else
    _sizeX = ( cinfo.density_unit == 2 ? 1. : 25.4 ) / cinfo.X_density;
  if( cinfo.Y_density == 0 || cinfo.density_unit == 0 )  // not filled
    _sizeY = 1;
  else
    _sizeY = ( cinfo.density_unit == 2 ? 1. : 25.4 ) / cinfo.Y_density;
  _sizeZ = 1;
  _sizeT = 1;

  setProperty( "file_type", string( "JPEG" ) );

  vector<int>  dims;
  dims.push_back( dimX() );
  dims.push_back( dimY() );
  dims.push_back( dimZ() );
  dims.push_back( dimT() );
  setProperty( "volume_dimension", dims );

  vector<float>  vs;
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
  vector<string>  pt;
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



