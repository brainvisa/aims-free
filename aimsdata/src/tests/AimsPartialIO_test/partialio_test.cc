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
//--- cartodata ----------------------------------------------------------------
#include <cartodata/volume/volumeview.h>
//--- soma-io ------------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/io/reader.h>
#include <soma-io/io/writer.h>
#include <soma-io/io/readeralgorithm.h>
#include <soma-io/writer/pythonwriter.h>                   // if catch exception
#include <soma-io/datasourceinfo/datasourceinfo.h>         // if catch exception
#include <soma-io/datasourceinfo/datasourceinfoloader.h>   // if catch exception
//--- cartobase ----------------------------------------------------------------
#include <cartobase/object/object.h>                                  // options
#include <cartobase/getopt/getopt.h>                       // create application
#include <cartobase/config/verbose.h>
#include <cartobase/stream/fileutil.h>  // read uri to check for partial writing
//--- system -------------------------------------------------------------------
#include <iostream>
//------------------------------------------------------------------------------

using namespace soma;
using namespace carto;
using namespace std;

template <typename T> 
bool readPartial( ReaderAlgorithm& a, Object hdr, rc_ptr<DataSource> src );

class PartialIOAlgo : public ReaderAlgorithm
{
  public:
    PartialIOAlgo() : ReaderAlgorithm( "PartialIO" ), create( true )
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

    string ofname;
    bool   create;
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
  Object options, urioptions;
  bool   partial_writing = false;
  string fullname;
  
  //=== READ VOLUME ============================================================
  Reader<Volume<T> > rVol( src );
  
  //=== FIND FULL VOLUME =======================================================
  VolumeRef<T> view( rVol.read() );
  Volume<T> *isvolume = view.get();
  VolumeView<T> *isview = 0;
  while( isview = dynamic_cast<carto::VolumeView<T> *>( isvolume ) ) {
    isvolume = isview->refVolume().get();
    isview = 0;
  }
  VolumeRef<T> vol( isvolume );
  
  //=== WRITE VOLUME ===========================================================
  if( !ma.ofname.empty() ) {
    cout << endl;
    cout << "=== WRITE VOLUME ====================================================" << endl;
    urioptions = FileUtil::uriOptions( ma.ofname );
    fullname  = FileUtil::uriFilename( ma.ofname );
    try {
      partial_writing = (bool) urioptions->getProperty( "partial_writing" )->getScalar();
    } catch( ... ) {}
    Writer<VolumeRef<T> > vfw( fullname );
    
    if( partial_writing && ma.create) {
      options = Object::value( PropertySet() );
      cout << "writing empty full volume..." << endl;
      vfw.write( vol, options );
    }
    options = Object::value( PropertySet() );
    vfw.attach( ma.ofname );
    cout << "writing view..." << endl;
    vfw.write( view, options );
    cout << "=====================================================================" << endl;
  }
  
  return true;
}

int main( int argc, const char** argv )
{
  string  fname;
  try
  {
    //=== APPLICATION ==========================================================
    string  ofname;
    bool    create = true;

    CartoApplication  app( argc, argv, "Test for soma partial reading/writing" );
    app.addOption( fname, "-i", "input filename to be read\n" );
    app.addOption( ofname, "-o", "output filename to be written\n", true );
    app.addOption( create, "-c", "if partial writing enabled, create" 
    " an empty volume (warning: if false, volume must already exist)\n", true );
    app.alias( "-v", "--verbose" );

    app.initialize();
    
    //=== INITIALIZE ALGORITHM =================================================
    PartialIOAlgo  palgo;
    palgo.create =  create;
    palgo.ofname =  ofname;
    
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
    DataSourceInfo dsi( fname );
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

