
#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/morphology/morphology_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>

using namespace aims;
using namespace carto;
using namespace std;


static AimsData<short> MedianSubSampling( AimsData<unsigned short> in, int factor)
{
  AimsData<short> out(in.dimX()/factor, in.dimY()/factor);
  out.setSizeXYZT(in.sizeX()*factor, in.sizeY()*factor, 1, 1);

  AimsData<short> tab( factor*factor );
  int medianIndex = factor*factor/2;
  int x, y, nx, ny, n, factorX, factorY;
  int dimX = out.dimX(), dimY = out.dimY();

  for ( y = 0; y < dimY; y++ )
    for ( x = 0; x < dimX; x++ )
      {
	n=0; factorX = factor*x; factorY=factor*y;
	for ( ny = 0; ny < factor; ny++ )
	  for ( nx = 0; nx < factor; nx++ )
	    {
	      tab( n++ ) = in( factorX+nx, factorY+ny ) >> 1; // Divide by 2
	    }
	incSorting( tab );
	out( x, y ) = tab( medianIndex );
      }


  return( out );
}

int main( int argc, const char **argv )
{
  Reader<AimsData<unsigned short> > reader;
  Writer<AimsData<short> > writer;
  int	xmask = 15,ymask = 15,zmask = 3;
  float	radius=0,morphoFactor = 50;

  AimsApplication	app( argc, argv, "Split one array image in print-tip images" );

  app.addOption( reader, "-i", "array image" );
  app.addOption( writer, "-o", "name of output pseudo volume" );
//   app.addOption( radius, "-r", "radius of the structuring element" );
//   app.addOption( xmask, "-x", "X size of the distance mask [default=3]", 
//                  true );
//   app.addOption( ymask, "-y", "Y size of the distance mask [default=3]", 
//                  true );
//   app.addOption( zmask, "-z", "Z size of the distance mask [default=3]", 
//                  true );
//   app.addOption( factor, "-f", "chamfer multiplication factor [default=50]", 
//                  true );

  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
//   app.alias( "--radius", "-r" );
//   app.alias( "--xmask", "-x" );
//   app.alias( "--ymask", "-y" );
//   app.alias( "--zmask", "-z" );
//   app.alias( "--factor", "-f" );

  try
    {
      //option parsing
      app.initialize();


      // Read inout data : assume U16 data from tiff (AimsRTiff)
      // with one pixel border !!!
      AimsData<unsigned short> arrayIn;
      reader.read(arrayIn,1);


      // Perform Median subsampling
      AimsData<short> arrayTmp;
      int subSamplingFactor = 4;
      arrayTmp = MedianSubSampling(arrayIn, subSamplingFactor);


      //Perform 2-level thresholding to keep between levels
 

      //Perform closing with vertical line like structuring element
      AimsData<short> clo;
      if ( !radius) radius = 30 * arrayIn.sizeX();
      clo = AimsMorphoChamferClosing<int16_t>( arrayTmp, radius, xmask, ymask,
                                               zmask, morphoFactor );


      //Output various profiles : vertical projection 
      int y, x, dimY = clo.dimY(), dimX = clo.dimX();
      AimsData< float > vertProj( dimX );
      for(y=0; y < dimY; y++ )
	for(x=0; x < dimX; x++)
	  vertProj( x ) += ( clo( x, y ) ? 1.0 : 0.0);
      
      //Output resulting image.
      int notFirstCall = 0;
      ofstream coutVertProj("VertProfile.txt",(notFirstCall ? ios::app : ios::out) );
      for(x=0;x<dimX;x++)
	{
	  coutVertProj << vertProj( x ) << endl;
	}
      coutVertProj.close();
      writer.write( clo );

      return EXIT_SUCCESS;
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }
  return EXIT_FAILURE;
}
