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
#include <aims/scalespace/meshDiffuse.h>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/curv.h>
#include <aims/data/pheader.h>

#include <aims/io/reader.h>
#include <aims/io/finder.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  Reader<TimeTexture<float> > texR;
  Reader<AimsSurfaceTriangle> triR;
  Reader<TimeTexture<float> > texRs;
  Writer<Texture1d>   texW;
  float	sigma = 0, dt=0, dur = 0, FWHM, H=10000;
  float Wmax = 0.98;
  int T=0;

  //
  // Parser of options
  //
  AimsApplication app( argc, argv,
    "Geodesic smoothing of the input texture (Laplacian: heat equation)" );
  app.addOption( texR, "-i", "input object definition texture" );
  app.alias( "--input", "-i" );
  app.addOption( texW, "-o", "output texture file" );
  app.alias( "--output", "-o" );
  app.addOption( triR, "-m", "mesh file" );
  app.alias( "--mesh", "-m" );
  app.addOption( texRs, "-S", "input source definition texture", true );
  app.alias( "--Source", "-S" );
  app.alias( "--source", "-S" );
  app.addOption( sigma, "-s",
    "equivalent gaussian sigma. Specify either sigma or duration.", true );
  app.alias( "--sigma", "-s" );
  app.addOption( dur, "-d",
    "Diffusion process duration. Either sigma or duration must be specified.",
    true );
  app.alias( "--duration", "-d" );
  app.addOption( dt, "-t", "timestep. Default = automatic estimation", true );
  app.alias( "--timestep", "-t" );
  app.addOption( T, "-T",
    "Timepoint in the input texture to be smoothed. Default: 0", true );
  app.alias( "--Time", "-T" );
  app.alias( "--time", "-T" );
  app.addOption( Wmax, "-W", "weight ratio threshold. Default: 0.98", true );
  app.alias( "--Wmax", "-W" );
  app.addOption( H, "-H", "output threshold (max of the output texture). "
    "Default: 10000", true );
  app.alias( "--Hmaw", "-H" );

  try
  {
    app.initialize();

    //
    // read triangulation
    //
    cout << "reading triangulation   : " << flush;
    AimsSurfaceTriangle surface;
    triR >> surface;
    cout << "done" << endl;


    //	read texture

    TimeTexture<float>	inpTex;

    cout << "reading texture " << texR.fileName() << endl;
    texR >> inpTex ;
    unsigned n=inpTex[(unsigned)T].nItem();


    Texture<float> itex(n),otex(n);
    TimeTexture<float>  outTex(1,n);

    map<unsigned, set< pair<unsigned,float> > > weightLapl;
    weightLapl = AimsMeshWeightFiniteElementLaplacian(surface[0],Wmax);

    /*
    map<unsigned, set< pair<unsigned,float> > >::iterator iw,ew;
    set< pair<unsigned,float> >::iterator ip,ep;

    for ( iw = weightLapl.begin(), ew = weightLapl.end(); iw != ew; ++iw )
      {
        for ( ip = iw->second.begin(), ep = iw->second.end(); ip != ep; ++ip  )
        cout << ip->second << " ";
        //cout << endl;
      }
    */

    set<float>   hval;
    for (unsigned i=0; i<n; ++i)
      {
        itex.item(i)= inpTex[(unsigned)T].item(i);
        hval.insert(fabs(inpTex[(unsigned)T].item(i)));
      }

    //The  smoothed texture values are threshold.
    //The threshold is the max of the input texture.
    if (H == 0)
      H = *hval.rbegin() ;

    if (dt == 0)
      {
        cout << "Estime the laplacian\n";
        otex =  AimsMeshLaplacian(itex,weightLapl);
        float dte;
        cout << "Estime dt\n";
        dte = AimsMeshFiniteElementDt(itex,otex,H);
        cout << "dt estimated : " << dte << endl;
        dt =  dte;
      }



    if( dur == 0 )
      {
        if( sigma == 0 )
          {
            cerr << "Must provide either sigma or duration\n";
            exit( 1 );
          }
        dur = rint( (sigma * sigma) / (8 * ::log(2) ) );
      }


    FWHM = 4 * sqrt ( ::log(2) * dur  );
    cout << "dur=" << dur*dt << " and equivalent FWHM =" << FWHM << " mm" << endl;
    cout << "Weight ratio thresold: " << Wmax << endl;
    cout << "output threshold: "<< H << endl;

    TimeTexture<float> stex;
    if ( texRs.fileName().empty() )
      outTex[0] = TextureSmoothing::FiniteElementSmoothing(
        itex, dt, dur, H, weightLapl );
    else
    {
      texRs >> stex;
      outTex[0] = TextureSmoothing::FiniteElementSmoothingWithSource(
        itex, stex[0], dt, dur, weightLapl );
    }

    float m=0;
    sigma=0;

    for (unsigned i=0; i<outTex[0].nItem(); ++i)
      m+=outTex[0].item(i);
    m /= outTex[0].nItem();
    for (unsigned i=0; i<outTex[0].nItem(); ++i)
      sigma += (outTex[0].item(i) - m)*(outTex[0].item(i) - m);
    sigma = sqrt(sigma);
    sigma /= outTex[0].nItem();
    cout << endl << "Mean: " << m << " sigma: " << sigma << endl;

    set<float> values;
    for (unsigned i=0; i<outTex[0].nItem(); ++i)
          values.insert(outTex[0].item(i));
    cout << "min: " << *values.begin() << " max: " << *values.rbegin() << endl;

    cout << "writing texture : " << flush;
    texW.write( outTex );
    cout << "done " << endl;
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}


