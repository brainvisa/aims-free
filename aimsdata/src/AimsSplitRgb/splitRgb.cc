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

#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/rgb/rgb.h>
#include <cartobase/stream/fileutil.h>


using namespace aims;
using namespace carto;
using namespace std;

template <typename T>
void doit( AimsData<AimsRGB> & dataRgb, const string & fileOut )
{
  int x, y, z, t;
  Writer< AimsData< T > > writer;
  string base = FileUtil::removeExtension( fileOut ), 
    ext = FileUtil::extension( fileOut );
  if( ext.empty() )
    ext = "ima";

  AimsData< T > oneChannel( dataRgb.dimX(),
			    dataRgb.dimY(),
			    dataRgb.dimZ(),
			    dataRgb.dimT() );
  oneChannel.setSizeX( dataRgb.sizeX() );
  oneChannel.setSizeY( dataRgb.sizeY() );
  oneChannel.setSizeZ( dataRgb.sizeZ() );
  oneChannel.setSizeT( dataRgb.sizeT() );
  cout << endl;

  cout << "getting R channel";
  for ( t = 0; t < dataRgb.dimT(); t++ )
    for ( z = 0; z < dataRgb.dimZ(); z++ )
      for ( y = 0; y < dataRgb.dimY(); y++ )
	for ( x = 0; x < dataRgb.dimX(); x++ )
	  oneChannel( x, y, z, t ) = dataRgb( x, y, z, t ).red();
  cout << endl;

  cout << "saving R channel";
  writer.setFileName( base + "_r." + ext );
  writer << oneChannel;
  cout << endl;

  cout << "getting G channel";
  for ( t = 0; t < dataRgb.dimT(); t++ )
    for ( z = 0; z < dataRgb.dimZ(); z++ )
      for ( y = 0; y < dataRgb.dimY(); y++ )
	for ( x = 0; x < dataRgb.dimX(); x++ )
	  oneChannel( x, y, z, t ) = dataRgb( x, y, z, t ).green();
  cout << endl;

  cout << "saving G channel";
  writer.setFileName( base + "_g." + ext );
  writer << oneChannel;
  cout << endl;

  cout << "getting B channel";
  for ( t = 0; t < dataRgb.dimT(); t++ )
    for ( z = 0; z < dataRgb.dimZ(); z++ )
      for ( y = 0; y < dataRgb.dimY(); y++ )
	for ( x = 0; x < dataRgb.dimX(); x++ )
	  oneChannel( x, y, z, t ) = dataRgb( x, y, z, t ).blue();
  cout << endl;

  cout << "saving B channel";
  writer.setFileName( base + "_b." + ext );
  writer << oneChannel;
  cout << endl;
}


int main( int argc, const char **argv )
{
  string fileIn, fileOut, datatype = "U8";

  AimsApplication app( argc, argv,
                  "Split an AimsData<AimsRGB> into 3 AimsData<byte> files" );
  app.addOption( fileIn, "-i", "input RGB data" );
  app.addOption( fileOut, "-o", "output data", 1 );
  app.addOption( datatype, "-t", "output data type [default: U8]", true );

  try
  {

    app.initialize();

    if ( fileOut.length() == 0 )
    {

      string res = fileIn;
      string ext="";
      if ( res.length() > 4 )
        ext = res.substr( int(res.length() - 4), 4 );
      if (ext == ".dim" || ext == ".ima")
        res = res.substr( 0, res.length() - 4 );

      fileOut = res;

    }
    else
    {

      string res = fileOut;
      string ext="";
      if ( res.length() > 4 )
        ext = res.substr( int(res.length() - 4), 4 );
      if (ext == ".dim" || ext == ".ima")
        res = res.substr( 0, res.length() - 4 );

      fileOut = res;

    }

    Reader< AimsData< AimsRGB > > reader( fileIn );
    AimsData< AimsRGB > dataRgb;

    cout << "reading RGB input";
    reader >> dataRgb;

    if( datatype == "U8" )
      doit<byte>( dataRgb, fileOut );
    else if( datatype == "S16" )
      doit<int16_t>( dataRgb, fileOut );
    else if( datatype == "U16" )
      doit<uint16_t>( dataRgb, fileOut );
    else if( datatype == "S32" )
      doit<int32_t>( dataRgb, fileOut );
    else if( datatype == "U32" )
      doit<uint32_t>( dataRgb, fileOut );
    else if( datatype == "FLOAT" )
      doit<float>( dataRgb, fileOut );
    else if( datatype == "DOUBLE" )
      doit<double>( dataRgb, fileOut );
    else
      {
	cerr << "unknown/unsupported output data type " << datatype << endl;
	return EXIT_FAILURE;
      }
  }
  catch( user_interruption &e )
  {

  }
  catch( std::exception &e )
  {

    cerr << argv[ 0 ] << ": " << e.what() << endl;
    return EXIT_FAILURE;

  }

  return EXIT_SUCCESS;

}


