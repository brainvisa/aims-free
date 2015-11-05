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
#include <aims/getopt/getopt.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/curv.h>
#include <aims/data/pheader.h>

#include <aims/io/reader.h>
#include <aims/io/finder.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsTextureSmoothing  -i[nput] <input_texture>                           ",
  "                  [-S[ource] <input_source_texture> default = none]      ",
  "                  -o[utput <output_texture>]                             ",
  "                  [-t[imestep] <dt> default = automatic estimation]      ", 
  "                  -m[esh] <mesh_file>                                    ", 
  "                  [-W[max] <weight ratio threshold> default = 0.98       ", 
  "                  [-H[max] <output threshold> default = 10000]          ",
  "                  [-s[igma] <equivalent gaussian sigma>]                 ", 
  "                  [-T[ime]   <time serie> default = 0]                   ",
  "                  [-d[uration] <duration>]                               ", 
  "                  [-h[elp]]                                              ",
  "-------------------------------------------------------------------------",
  " Geodesic smoothing of the input texture (heat equation)                 ",
  "-------------------------------------------------------------------------",
  "     input_texture       : object definition                             ",
  "     input_source_texture: source definition                              ",
  "     output_texture      : output *.tex file (blobs)                     ",
  "-------------------------------------------------------------------------",
END_USAGE


//
// Usage
//
void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char** argv )
{
  char	*intexfile = 0, *outtexfile = 0, *meshfile, *sourcefile =0;
  float	sigma = 0, dt=0, dur = 0, FWHM, H=10000;
  float Wmax = 0.98;
  int T=0;

  //
  // Parser of options
  //
  AimsOption opt[] = {
    { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage  ,AIMS_OPT_CALLFUNC,0},
    { 'i',"input"        ,AIMS_OPT_STRING,&intexfile      ,0                ,1},
    { 'S',"Source"       ,AIMS_OPT_STRING,&sourcefile       ,0                ,0},
    { 'm',"mesh"         ,AIMS_OPT_STRING,&meshfile       ,0                ,1},
    { 'o',"output"       ,AIMS_OPT_STRING,&outtexfile     ,0                ,0},
    { 't',"timestep"     ,AIMS_OPT_FLOAT ,&dt             ,0                ,0},
    { 'W',"Wmax"         ,AIMS_OPT_FLOAT ,&Wmax           ,0                ,0},
    { 'T',"Time"         ,AIMS_OPT_INT   ,&T              ,0                ,0},
    { 's',"sigma"        ,AIMS_OPT_FLOAT ,&sigma          ,0                ,0},
    { 'H',"Hmax"         ,AIMS_OPT_FLOAT ,&H              ,0                ,0},
    { 'd',"duration"     ,AIMS_OPT_FLOAT ,&dur            ,0                ,0},
    { 0  ,0              ,AIMS_OPT_END   ,0               ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );
  
  //
  // read triangulation
  //
  cout << "reading triangulation   : " << flush;
  AimsSurfaceTriangle surface;
  Reader<AimsSurfaceTriangle> triR( meshfile );
  triR >> surface;
  cout << "done" << endl;

 
  //	read texture
  
  TimeTexture<float>	inpTex;
  
  cout << "reading texture " << intexfile << endl;
  Reader<TimeTexture<float> >	texR( intexfile );
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
  if (sourcefile == NULL)
    outTex[0] = TextureSmoothing::FiniteElementSmoothing(itex,dt,dur,H,weightLapl);
  else
    {
      Reader<TimeTexture<float> >	texRs( sourcefile );
      texRs >> stex ;
      outTex[0] = TextureSmoothing::FiniteElementSmoothingWithSource(itex,stex[0],dt,dur,weightLapl);
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
  Writer<Texture1d>	texW( outtexfile );
  texW.write( outTex );
  cout << "done " << endl;
  

  return( 0 );
}


