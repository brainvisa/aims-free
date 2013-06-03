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

//--- aims ---------------------------------------------------------------------
#include <aims/rgb/rgb.h>
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/io/reader.h>
#include <soma-io/io/writer.h>
#include <soma-io/io/formatdictionary.h>
#include <soma-io/io/readeralgorithm.h>
#include <soma-io/datasource/filedatasource.h>                    // catch error
#include <soma-io/datasourceinfo/datasourceinfo.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <soma-io/writer/pythonwriter.h>
//--- cartodata ----------------------------------------------------------------
#include <cartodata/volume/volumeview.h>
//--- cartobase ----------------------------------------------------------------
#include <cartobase/object/object.h>
#include <cartobase/getopt/getopt.h>
#include <cartobase/config/verbose.h>
//--- system -------------------------------------------------------------------
#include <iostream>
#include <cstdlib>
#include <limits>
//------------------------------------------------------------------------------

using namespace soma;
using namespace carto;
using namespace std;

template <typename T> 
bool readPartial( ReaderAlgorithm& a, Object hdr, rc_ptr<DataSource> src );

class PartialIOAlgo : public ReaderAlgorithm
{
  public:
    PartialIOAlgo() : ReaderAlgorithm( "PartialIO" )
    {
      registerAlgorithmType( "Volume of S8",     &readPartial<int8_t> );
      registerAlgorithmType( "Volume of U8",     &readPartial<uint8_t> );
      registerAlgorithmType( "Volume of S16",    &readPartial<int16_t> );
      registerAlgorithmType( "Volume of U16",    &readPartial<uint16_t> );
      registerAlgorithmType( "Volume of S32",    &readPartial<int32_t> );
      registerAlgorithmType( "Volume of U32",    &readPartial<uint32_t> );
      registerAlgorithmType( "Volume of FLOAT",  &readPartial<float> );
      registerAlgorithmType( "Volume of DOUBLE", &readPartial<double> );
      registerAlgorithmType( "Volume of RGB",    &readPartial<AimsRGB> );
      registerAlgorithmType( "Volume of RGBA",   &readPartial<AimsRGBA> );
    }
    void setOrigin( const vector<int> & in ) { origin = in; }
    void setFrame( const vector<int> & in )  { frame = in; }
    void setFileOut( const string & in )  { ofname = in; }
    void setPartialWriting( bool in )     { partial_writing = in; }
    void setByteSwapping( bool in )       { byte_swapping = in; }
    void setNoCreate( bool in )           { no_create = in; }

    vector<int>  origin;
    vector<int>  frame;
    string       ofname;
    bool         partial_writing;
    bool         byte_swapping;
    bool         no_create;
};

void printheader( Object hdr )
{
  PythonWriter  pw;
  pw.attach( cout );
  pw.write( hdr );
}


template <typename T> 
bool readPartial( ReaderAlgorithm& a, Object hdr, rc_ptr<DataSource> src )
{
  PartialIOAlgo & ma = (PartialIOAlgo &) a;
  
  //=== DECLARE VARIABLES ======================================================
  Object options;
  typename VolumeView<T>::Position4Di origin( ma.origin[0], ma.origin[1], ma.origin[2], ma.origin[3] );
  typename VolumeView<T>::Position4Di frame( ma.frame[0], ma.frame[1], ma.frame[2], ma.frame[3] );

  //=== SET UNALLOCATED VOLUME =================================================
  cout << "=== READING UNALLOCATED VOLUME ======================================" << endl;
  Reader<Volume<T> > rVol( src->url() );
  cout << "filename: " << rVol.dataSource()->url() << endl;
  options = Object::value( PropertySet() );
  options->setProperty( "unallocated", true );
  options->setProperty( "resolution_level", 0 );
  rVol.setOptions( options );
  cout << "reading unallocated volume..." << endl;
  VolumeRef<T>  vol( rVol.read() );
  cout << "done" << endl;
  
  //=== SET FRAME IF NOT INITIALIZED ===========================================
  cout << "reading size from header..." << endl;
  typename VolumeView<T>::Position4Di size;
  rVol.dataSourceInfo()->header()->getProperty( "sizeX", size[ 0 ] );
  rVol.dataSourceInfo()->header()->getProperty( "sizeY", size[ 1 ] );
  rVol.dataSourceInfo()->header()->getProperty( "sizeZ", size[ 2 ] );
  rVol.dataSourceInfo()->header()->getProperty( "sizeT", size[ 3 ] );
  if( frame[ 0 ] == 0 || frame[ 1 ] == 0 || 
      frame[ 2 ] == 0 || frame[ 3 ] == 0 )
    frame = size;
  cout << "done" << endl;
  cout << "=====================================================================" << endl;
  
  //=== SET ALLOCATED VOLUME VIEW ==============================================
  cout << "=== READING VIEW ====================================================" << endl;
  Reader<Volume<T> > rView( rVol.dataSourceInfo() );
  options = Object::value( PropertySet() );
  options->setProperty( "partial_reading", true );
  options->setProperty( "resolution_level", 0 );
  rView.setOptions( options );
  VolumeRef<T> view( new VolumeView<T>( vol, origin, frame ) );
  
  cout << "reading partial volume..." << endl;
  rView.read( *view );
  cout << "done" << endl;
  cout << "=====================================================================" << endl;
  
  //=== SHOW RESULTS ===========================================================
  cout << "=== HEADER ==========================================================" << endl;
  cout << " Volume:" << endl;
  printheader( Object::value( vol->header() ) );
  cout << "View:" << endl;
  printheader( Object::value( view->header() ) );
  cout << "=====================================================================" << endl;
  cout << "=== WRITE VOLUME ====================================================" << endl;
  if( !ma.ofname.empty() ) {
    Writer<VolumeRef<T> > vfw( ma.ofname );
    options = Object::value( PropertySet() );
    if( ma.byte_swapping )
      options->setProperty( "byte_swapping", true );
    if( ma.partial_writing ) {
      if( !ma.no_create )
        vfw.write( vol, options );
      options = Object::value( PropertySet() );
      options->setProperty( "partial_writing", true );
      vfw.attach( ma.ofname );
    }
    vfw.write( view, options );
  }
  cout << "=====================================================================" << endl;
  
  return true;
}

int main( int argc, const char** argv )
{
  string  fname;
  try
  {
    //=== APPLICATION ==========================================================
    vector<int>  origin( 4, 0 );
    vector<int>  frame( 4, 0 );
    string       ofname;
    bool partial_writing=false, byte_swapping=false, no_create=false;

    CartoApplication  app( argc, argv, "Test for soma partial reading" );
    app.addOption( fname, "-i", "input filename to be read\n" );
    app.addOption( ofname, "-o", "output filename to be written\n", true );
    app.addOption( origin[ 0 ], "-ox", "frame origin (x comp)\n", true );
    app.addOption( origin[ 1 ], "-oy", "frame origin (y comp)\n", true );
    app.addOption( origin[ 2 ], "-oz", "frame origin (z comp)\n", true );
    app.addOption( origin[ 3 ], "-ot", "frame origin (t comp)\n", true );
    app.addOption( frame[ 0 ], "-sx", "frame size (x comp)\n", true );
    app.addOption( frame[ 1 ], "-sy", "frame size (y comp)\n", true );
    app.addOption( frame[ 2 ], "-sz", "frame size (z comp)\n", true );
    app.addOption( frame[ 3 ], "-st", "frame size (t comp)\n", true );
    app.addOption( byte_swapping, "-bs", "force byte swapping for writing\n", true );
    app.addOption( partial_writing, "-pw", "partial writing of volume\n", true );
    app.addOption( no_create, "-nc", "if partial writing, does not create" 
    " an empty volume (warning: it must then already exist)\n", true );
    app.alias( "-v", "--verbose" );

    app.initialize();
    
    //=== SHOW PARAMETERS ======================================================
    cout << "=== PARAMETERS ======================================================" << endl;
    cout << "Input filename: " << fname << endl;
    cout << "Output filename: " << ofname << endl;
    cout << "Origin: " << origin[0] << ", " << origin[1] << ", "
                       << origin[2] << ", " << origin[3] << endl;
    cout << "Frame: " << frame[0] << ", " << frame[1] << ", "
                      << frame[2] << ", " << frame[3] << endl;
    cout << "Force byte swapping at writing: " << byte_swapping << endl;
    cout << "Enable partial writing: " << partial_writing << endl;
    cout << "Create global volume for partial writing: " << !no_create << endl;
    cout << "=====================================================================" << endl;
    
    //=== INITIALIZE ALGORITHM =================================================
    PartialIOAlgo  palgo;
    palgo.setOrigin( origin );
    palgo.setFrame( frame );
    palgo.setFileOut( ofname );
    palgo.setPartialWriting( partial_writing );
    palgo.setByteSwapping( byte_swapping );
    palgo.setNoCreate( no_create );
    
    //=== RUN ALGORITHM ========================================================
    if( !palgo.execute( fname ) ){
      cerr << "couldn't process file " << fname << endl;
      return EXIT_FAILURE;
    }
    cout << "OK\n";
    
  }
  catch( user_interruption & )
  {
  }
  catch( datatype_format_error & e )
  {
    DataSourceInfoLoader  f;
    DataSourceInfo dsi( rc_ptr<DataSource>( new FileDataSource( fname ) ) );
    dsi = f.check( dsi );
    if( !dsi.header().isNone() )
      printheader( dsi.header() );
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

