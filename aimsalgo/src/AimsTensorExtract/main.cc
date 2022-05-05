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
#include <aims/data/data.h>
#include <aims/bucket/bucket.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
#include <aims/rgb/rgb.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char **argv )
{
  Reader<AimsBucket<DtiTensor> >	bucketR;
  string fileout;
  int dimX = 0, dimY = 0, dimZ = 0;

  AimsApplication app( argc, argv,
                       "Extract anisotropy,trace,directions from dtitensor "
                       "bucket" );
  app.addOption( bucketR, "-i", "source dtitensor bucket" );
  app.addOption( fileout, "-o",
                 "destination base file name, will be suffixed:"
                 "fileout_traceTC.ima, "
                 "fileout_traceEV.ima, "
                 "fileout_VR.ima, "
                 "fileout_FA.ima, "
                 "fileout_dir.ima, "
                 "fileout_dirRGB.ima. default=input file name", true );
  app.addOption( dimX, "-x", "dimensions of output images" );
  app.addOption( dimY, "-y", "dimensions of output images" );
  app.addOption( dimZ, "-z", "dimensions of output images" );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--xdim", "-x" );
  app.alias( "--ydim", "-y" );
  app.alias( "--zdim", "-z" );

  try
  {
    app.initialize();

    if ( fileout.empty() )
      fileout = FileUtil::removeExtension( bucketR.fileName() );

    AimsBucket<DtiTensor> bucket;
    list< AimsBucketItem<DtiTensor> >::iterator it;

    cout << "reading tensor bucket : " << flush;
    bucketR.read( bucket );
    cout << "done" << endl;

    cout << "saving trace  : " << flush;
    Writer<AimsData<float> > dataW0( string( fileout ) + "_trace" );
    AimsData<float>* trace = new AimsData<float>( dimX, dimY, dimZ );
    trace->setSizeX( bucket.sizeX() );
    trace->setSizeY( bucket.sizeY() );
    trace->setSizeZ( bucket.sizeZ() );
    *trace = 0;
    for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
      if ( it->value().category() <= DtiTensor::NO_PROBLEM )
        (*trace)( it->location().item(0),
                  it->location().item(1),
                  it->location().item(2) ) = it->value().base().trace();
    dataW0.write( *trace );
    delete trace;
    cout << "done" << endl;

    cout << "saving volume ratio : " << flush;
    Writer<AimsData<float> > dataW2( string( fileout ) + "_VR" );
    AimsData<float>* vr = new AimsData<float>( dimX, dimY, dimZ );
    vr->setSizeX( bucket.sizeX() );
    vr->setSizeY( bucket.sizeY() );
    vr->setSizeZ( bucket.sizeZ() );
    *vr = 0;
    for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
      if ( it->value().category() <= DtiTensor::NO_PROBLEM )
        (*vr)( it->location().item(0),
              it->location().item(1),
              it->location().item(2) ) = it->value().anisotropyVR();
    dataW2.write( *vr );
    delete vr;
    cout << "done" << endl;

    cout << "saving fractional anisotropy : " << flush;
    Writer<AimsData<float> > dataW3( string( fileout ) + "_FA" );
    AimsData<float>* fa = new AimsData<float>( dimX, dimY, dimZ );
    fa->setSizeX( bucket.sizeX() );
    fa->setSizeY( bucket.sizeY() );
    fa->setSizeZ( bucket.sizeZ() );
    *fa = 0;
    for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
      if ( it->value().category() <= DtiTensor::NO_PROBLEM )
        (*fa)( it->location().item(0),
              it->location().item(1),
              it->location().item(2) ) = it->value().anisotropyFA();
    dataW3.write( *fa );
    delete fa;
    cout << "done" << endl;

    cout << "saving direction : " << flush;
    Writer<AimsData<Point3df> > dataW4( string( fileout ) + "_dir" );
    AimsData<Point3df>* dir = new AimsData<Point3df>( dimX, dimY, dimZ );
    dir->setSizeX( bucket.sizeX() );
    dir->setSizeY( bucket.sizeY() );
    dir->setSizeZ( bucket.sizeZ() );
    *dir = Point3df(0,0,0);
    for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
      if ( it->value().category() <= DtiTensor::NO_PROBLEM )
        (*dir)( it->location().item(0),
                it->location().item(1),
                it->location().item(2) ) = it->value().dir();
    dataW4.write( *dir );
    delete dir;
    cout << "done" << endl;

    cout << "saving direction in RGB : " << flush;
    Writer<AimsData<AimsRGB> > dataW5( string( fileout ) + "_dirRGB" );
    AimsData<AimsRGB>* dirRGB = new AimsData<AimsRGB>( dimX, dimY, dimZ );
    dirRGB->setSizeX( bucket.sizeX() );
    dirRGB->setSizeY( bucket.sizeY() );
    dirRGB->setSizeZ( bucket.sizeZ() );
    *dirRGB = AimsRGB(0,0,0);
    for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
      if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      {
        (*dirRGB)( it->location().item(0),
                  it->location().item(1),
                  it->location().item(2) ).red() =
                ::byte( fabs( it->value().dir().item(0) ) * 255.0 );
        (*dirRGB)( it->location().item(0),
                  it->location().item(1),
                  it->location().item(2) ).green() =
                ::byte( fabs( it->value().dir().item(1) ) * 255.0 );
        (*dirRGB)( it->location().item(0),
                  it->location().item(1),
                  it->location().item(2) ).blue() =
                ::byte( fabs( it->value().dir().item(2) ) * 255.0 );
      }
    dataW5.write( *dirRGB );
    delete dirRGB;
    cout << "done" << endl;

    return EXIT_SUCCESS;
  }
  catch( user_interruption &)
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
  }
  return EXIT_FAILURE;
}
