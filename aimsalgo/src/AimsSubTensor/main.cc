
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cstdlib>
#include <aims/bucket/bucket.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
#include <cartodata/volume/volume.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  Reader<AimsBucket<DtiTensor> > bucketR;
  Writer<AimsBucket<DtiTensor> > bucketW;
    Reader<AimsBucket<Void> > bucketRM;
  short xmin = -1, xmax = -1, ymin = -1, ymax = -1, zmin = -1, zmax = -1;
  int ascii = 0;

  AimsApplication app( argc, argv,
                       "Extracting a subpart from a DtiTensor bucket" );
  app.addOption( bucketR, "-i", "source tensor bucket" );
  app.addOption( bucketW, "-o", "output bucket of sub-dtitensor" );
  app.addOption( xmin, "-x", "inferior point of the box sub-part", true );
  app.addOption( ymin, "-y", "inferior point of the box sub-part", true );
  app.addOption( zmin, "-z", "inferior point of the box sub-part", true );
  app.addOption( xmax, "-X", "superior point of the box sub-part", true );
  app.addOption( ymax, "-Y", "superior point of the box sub-part", true );
  app.addOption( zmax, "-Z", "superior point of the box sub-part", true );
  app.addOption( bucketRM, "-m", "Void bucket file mask", true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--mask", "-m" );

  try
  {
    app.initialize();

    cout << "reading " << bucketR.fileName() << " bucket : " << flush;
    AimsBucket<DtiTensor> bucket;
    bucketR.read( bucket );
    cout << "done" << endl;


    VolumeRef<::byte> mask;
    int dimX = 0, dimY = 0, dimZ = 0;

    if( !bucketRM.fileName().empty() )
    {
      cout << "reading " << bucketRM.fileName() << " void bucket : " << flush;
      AimsBucket<Void> bucketM;
      bucketRM.read( bucketM );
      list< AimsBucketItem<DtiTensor> >::iterator it;
      for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
      {
        if ( it->location().item(0) > dimX )
          dimX = it->location().item(0);
        if ( it->location().item(1) > dimY )
          dimY = it->location().item(1);
        if ( it->location().item(2) > dimZ )
          dimZ = it->location().item(2);
      }
      mask.reset( new Volume<::byte>( ++dimX, ++dimY, ++dimZ ) );
      mask = false;
      list< AimsBucketItem<Void> >::iterator vi;
      for ( vi = bucketM[0].begin(); vi != bucketM[0].end(); vi++ )
        mask( vi->location() ) = true;

      cout << "done" << endl;
    }
    cout << "dimX=" << dimX << endl
        << "dimY=" << dimY << endl
        << "dimZ=" << dimZ << endl;

    AimsBucket<DtiTensor> subpart;

    subpart.setSizeX( bucket.sizeX() );
    subpart.setSizeY( bucket.sizeY() );
    subpart.setSizeZ( bucket.sizeZ() );
    subpart.setSizeT( bucket.sizeT() );


    cout << "filling subpart : " << flush;
    if ( bucketRM.fileName().empty() )
    {
      list< AimsBucketItem<DtiTensor> >::iterator it;
      for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
        if ( it->location().item(0) >= xmin &&
            it->location().item(0) <= xmax &&
            it->location().item(1) >= ymin &&
            it->location().item(1) <= ymax &&
            it->location().item(2) >= zmin &&
            it->location().item(2) <= zmax   )
          subpart.push_back( *it );
    }
    else
    {
      list< AimsBucketItem<DtiTensor> >::iterator it;
      for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
        if ( mask( it->location().item(0),
                      it->location().item(1),
                      it->location().item(2) ) == true )
          subpart.push_back( *it );
    }
    cout << "done" << endl;


    cout << "writing " << bucketW.fileName() << " bucket : " << flush;
    bucketW.write( subpart, ascii );
    cout << "done" << endl;


    return EXIT_SUCCESS;
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
  }
}
