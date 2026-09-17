
#include <cstdlib>
#include <aims/data/data_g.h> 
#include <aims/io/io_g.h>
#include <aims/io/writer.h> 
#include <aims/io/reader.h>
#include <aims/getopt/getopt2.h>
#include <aims/math/math_g.h>
#include <aims/utility/utility_g.h>
#include <aims/morphology/morphology_g.h>
#include <vector>
#include <aims/roi/roiIterator.h>

using namespace aims;   
using namespace std;
using namespace carto;

int main( int argc, const char **argv )

{
  int result = EXIT_SUCCESS;
  try {
    
    //
    // Collecte des arguments.
    //
    
    string filein, fileout, filesum = "" ;
    float radius = 8. ;
    
    AimsApplication application( argc, argv, "Lighten anatomical probabilistic atlas" );
    application.addOption( filein, "-i", "Probabilistic atlas image with one region per frame" );
    application.addOption( fileout, "-o", "Lightened atlas image" );    
    application.addOption( filesum, "-s", "Sum of the lightened atlas image", 1 );    
    application.addOption( radius, "-r", "Radius around a point in the structure within which a point with a 1 probability\n"
			   "to belong to the structure is supposed to exist (default : 8 mm)", 1 );    
    application.initialize();
    
    
    AimsData<float> atlasImage ;
    Reader< AimsData<float> > reader(filein) ;
    reader.read( atlasImage ) ;
    
    
    AimsData<float> newAtlasImage(  atlasImage.dimX(), atlasImage.dimY(), atlasImage.dimZ(), atlasImage.dimT() ) ; 
    newAtlasImage.setSizeXYZT(  atlasImage.sizeX(), atlasImage.sizeY(), atlasImage.sizeZ(), 1.0 ) ;
    
    AimsData<float> maxAtlasImage( atlasImage.dimX(), atlasImage.dimY(), atlasImage.dimZ(), atlasImage.dimT() ) ; 
    maxAtlasImage.setSizeXYZT(  atlasImage.sizeX(), atlasImage.sizeY(), atlasImage.sizeZ(), 1.0 ) ;
    
    AimsData<float> maxAtlasImage2( atlasImage.dimX(), atlasImage.dimY(), atlasImage.dimZ() ) ; 
    maxAtlasImage2.setSizeXYZT(  atlasImage.sizeX(), atlasImage.sizeY(), atlasImage.sizeZ(), 1.0 ) ;

    int x, y, z;
    for( int t = 0 ; t < newAtlasImage.dimT() ; ++t ){
      cout << "t = " << t << endl ;
      ForEach3d( atlasImage, x, y, z ){
	if( atlasImage(x, y, z, t) > 0.01 ){
	  maxAtlasImage(x, y, z, t) = 0. ;
	  for( int dz = -int(radius/atlasImage.sizeZ() - 1) ; dz <= int(radius/atlasImage.sizeZ() + 1) ; ++dz ){
	    if( z + dz >= 0 && z + dz < atlasImage.dimZ() )
	      if( atlasImage(x, y, z+dz, t) > maxAtlasImage( x, y, z, t ) )
		maxAtlasImage( x, y, z, t ) = atlasImage(x, y, z+dz, t) ;
	  }
	}
      }
      ForEach3d( atlasImage, x, y, z ){
	if( atlasImage(x, y, z, t) > 0.01 ){
	  maxAtlasImage2(x, y, z) = 0. ;
	  for( int dy = -int(radius/atlasImage.sizeY() - 1) ; dy <= int(radius/atlasImage.sizeY() + 1) ; ++dy ){
	    if( y + dy >= 0 && y + dy < atlasImage.dimY() )
	      if( maxAtlasImage(x, y+dy, z, t) > maxAtlasImage2( x, y, z) )
		maxAtlasImage2( x, y, z ) = maxAtlasImage(x, y+dy, z, t) ;
	  }
	}
      }
      ForEach3d( atlasImage, x, y, z ){
	if( atlasImage(x, y, z, t) > 0.01 ){
	  maxAtlasImage(x, y, z, t) = 0. ;
	  for( int dx = -int(radius/atlasImage.sizeX() - 1) ; dx <= int(radius/atlasImage.sizeX() + 1) ; ++dx ){
	    if( x + dx >= 0 && x + dx < atlasImage.dimX() )
	      if( maxAtlasImage2(x+dx, y, z) > maxAtlasImage( x, y, z, t ) )
		maxAtlasImage( x, y, z, t ) = maxAtlasImage2(x+dx, y, z) ;
	  }
	}
      }
      ForEach3d( atlasImage, x, y, z )
	if( atlasImage(x, y, z, t) > 0.01 )
	  newAtlasImage( x, y, z, t ) = atlasImage( x, y, z, t ) / maxAtlasImage( x, y, z, t ) ;
	else
	  newAtlasImage( x, y, z, t ) = 0. ;
    }
    
    AimsData<float> sumAtlasImage( atlasImage.dimX(), atlasImage.dimY(), atlasImage.dimZ() ) ; 
    sumAtlasImage.setSizeXYZT(  atlasImage.sizeX(), atlasImage.sizeY(), atlasImage.sizeZ(), 1.0 ) ;
    atlasImage = AimsData<float>(1) ;

     ForEach3d( newAtlasImage, x, y, z ){
      for( int t = 0 ; t < newAtlasImage.dimT() ; ++t )
	sumAtlasImage(x, y, z) += newAtlasImage( x, y, z, t ) ;
      if( sumAtlasImage(x, y, z) > 1. ){
        sumAtlasImage(x, y, z) = 1. ;
	for( int t = 0 ; t < newAtlasImage.dimT() ; ++t )
	  newAtlasImage( x, y, z, t ) /= sumAtlasImage(x, y, z) ;
      }
    }
    
    Writer< AimsData<float> > writer( fileout ) ;
    writer.write( newAtlasImage ) ;
    if( filesum != "" ){
      Writer< AimsData<float> > writerSum( filesum ) ;
      writerSum.write( sumAtlasImage ) ;
    }
      
    
  } catch( user_interruption &e ) {
  } catch( std::exception &e ) {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }
  return result;  
}

