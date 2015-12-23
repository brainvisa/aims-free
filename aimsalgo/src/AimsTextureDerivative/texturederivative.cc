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
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <aims/data/pheader.h>
#include <assert.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  string intexfile, outtexfile, pinv, meshfile;
  Texture1d  	deriveTex ;
  bool  	curvatureFlag = false, LvvFlag = false , LwFlag = false,
    LaplacienFlag = false,DivFlag = false, AllFlag = false;
  int T = 0;

  //
  // Parser of options
  //
  AimsApplication app( argc, argv,
                       "Compute first and second derivative  of the input "
                       "texture" );
  app.addOption( intexfile, "-i", "texture definition" );
  app.alias( "--input", "-i" );
  app.addOption( meshfile, "-m", "mesh file" );
  app.alias( "--mesh", "-m" );
  app.addOption( outtexfile, "-o", "output texture" );
  app.alias( "--output", "-o" );
  app.addOption( pinv, "-p", "pinv_filename" );
  app.alias( "--pinv", "-p" );
  app.addOption( T, "-T", "time point [default: 0]", true );
  app.alias( "--Time", "-T" );
  app.addOption( curvatureFlag, "--curvature", "curvature", true );
  app.addOption( LvvFlag, "--Lvv", "means certainly something", true );
  app.addOption( LwFlag, "--Lw", "means certainly something", true );
  app.addOption( LaplacienFlag, "--laplacian", "means certainly something",
                 true );
  app.alias( "--laplacien", "--laplacian" );
  app.addOption( DivFlag, "--div", "means certainly something", true );
  app.addOption( AllFlag, "--all", "implies all of --curvature, --Lvv, --Lw, "
    "--laplacian flags", true );

  try
  {
    app.initialize();

    if (AllFlag)
        curvatureFlag = LvvFlag = LwFlag = LaplacienFlag = true ;


    if ( !curvatureFlag && !LvvFlag && !LwFlag && !LaplacienFlag && !DivFlag  )
    {
      cerr << "At least curvature or Lvv or Lw or Laplacien must be choosen \n";
      assert(0);
    }

    cout << "Reading...";

    string	tex = intexfile, inv = pinv, outtex = intexfile;
    string::size_type	pos = tex.rfind( '/' ), pos2 = outtex.rfind( '.' );
    string	path;
    if( pos != string::npos )
    {
      path = tex.substr( 0, pos );
      tex.erase( 0, pos );
    }

    if( pos2 != string::npos )
    {
      outtex = outtex.substr( 0, pos2 );
    }


    pos = inv.rfind( '/' );
    string	pathinv;
    if( pos != string::npos )
      pathinv = inv.substr( 0, pos );

    ifstream	maxnfile( ( pathinv + "MaxNeighs" ).c_str() );
    if( !maxnfile )
    {
      cerr << "Couldn't open file " <<  pathinv << "MaxNeighs\n";
      exit( 1 );
    }

    string::size_type	max_neigh;
    maxnfile >> max_neigh;
    maxnfile.close();

    Finder	f;
    if( !f.check( meshfile ) )
    {
      cerr << "Couldn't open mesh file " << meshfile << endl;
      exit( 1 );
    }
    int	nnodes;
    const PythonHeader	*hdr
      = dynamic_cast<const PythonHeader *>( f.header() );
    if( !hdr )
    {
      cerr << "File " << meshfile << " hasn't a valid header\n";
      exit( 1 );
    }
    if( !hdr->getProperty( "vertex_number", nnodes ) )
    {
      cerr << "Couldn't determine mesh nodes number (strange !)\n";
      exit( 1 );
    }

    TimeTexture<float> texture,courbure, Lvv, Lw, Laplacien,div;
    Texture<float> text;

    Reader<TimeTexture<float> >	r( intexfile );
    r >> texture;
    text = texture[(unsigned)T];

    cout << "done \n";
    cout << "Computing derivatives..." << flush;

    int		i;
    float 	Ix,Ixx,Iy,Iyy, Ixy = 0;


    deriveTex = TextureSmoothing::derivative( inv, text,  max_neigh );
    for (i=0;i<nnodes;i++)
    {
      Ix=deriveTex[0].item(i);
      Iy=deriveTex[1].item(i);
      Ixx=deriveTex[2].item(i);
      Iyy=deriveTex[3].item(i);
      // Ixy=deriveTex[4].item(i); case NVAR=4; Ixy is supposed to be null
      if (curvatureFlag)
        courbure[0].push_back( (2 * Ix * Iy * Ixy - Ix * Ix * Iyy - Iy * Iy * Ixx) /
                              sqrt( ( Ix * Ix + Iy * Iy ) * ( Ix * Ix + Iy * Iy ) * ( Ix * Ix + Iy * Iy ) ) );
      if (LvvFlag)
        Lvv[0].push_back( ( Iy * Iy * Ixx - 2 * Iy * Ix * Ixy + Ix * Ix * Iyy ) /
                          ( Iy * Iy + Ix * Ix ) );
      if (LwFlag)
        Lw[0].push_back( sqrt( Iy * Iy + Ix * Ix  ) ) ;

      if (LaplacienFlag)
        Laplacien[0].push_back( Ixx + Iyy );

      if (DivFlag)
        div[0].push_back(Ix + Iy);
    }

    if (curvatureFlag)
    {
      string outtexc = outtex + "_curv";
      Writer<Texture1d>    w( outtexc );
      w << courbure;
    }

    if (LvvFlag)
    {
      string outtexl = outtex + "_Lvv";
      Writer<Texture1d>    w1( outtexl );
      w1 << Lvv;
    }

    if (LwFlag)
    {
      string outtexl = outtex + "_Lw";
      Writer<Texture1d>    w2( outtexl );
      w2 << Lw;
    }

    if (LaplacienFlag)
    {
      string outtexl = outtex + "_Lapl";
      Writer<Texture1d>    w3( outtexl );
      w3 << Laplacien;
    }

    if (DivFlag)
    {
      string outtexl = outtex + "_div";
      Writer<Texture1d>    w4( outtexl ) ;
      w4 << div;
    }

    cout << "done\n";

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
