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
