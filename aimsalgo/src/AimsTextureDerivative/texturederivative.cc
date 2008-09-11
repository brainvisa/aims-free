/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#include <aims/scalespace/meshDiffuse.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <aims/mesh/texture.h>
#include <aims/data/pheader.h>
#include <assert.h>

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsTextureDerivative  -i[nput] <input_texture>                          ",
  "                  [-o[utput] <output_texture>]                           ",
  "                  -p[inv] <pinv_filename>                                ", 
  "                  -m[esh] <mesh_file>                                    ", 
  "                  --curvature                                            ",
  "                  --Lvv                                                  ",
  "                  --Lw                                                   ",
  "                  --laplacien                                            ",
  "                  --div                                                  ",
  "                  [-h[elp]]                                              ",
  "-------------------------------------------------------------------------",
  " Compute first and second derivative  of the input texture               ",
  "-------------------------------------------------------------------------",
  "     input_texture       : texture definition                            ",
  "     output_texture      : output *.tex file                             ",
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
  char		*intexfile = 0, *outtexfile = 0, *pinv = 0, *meshfile;
  Texture1d  	deriveTex ;
  bool  	curvatureFlag = false, LvvFlag = false , LwFlag = false, LaplacienFlag = false,DivFlag = false, AllFlag = false ; 
  int T=0;

  //
  // Parser of options
  //
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage  ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"        ,AIMS_OPT_STRING,&intexfile      ,0                ,1},
  { 'm',"mesh"         ,AIMS_OPT_STRING,&meshfile       ,0                ,1},
  { 'o',"output"       ,AIMS_OPT_STRING,&outtexfile     ,0                ,0},
  { 'p',"pinv"         ,AIMS_OPT_STRING,&pinv           ,0                ,1},
  { 'T',"Time"         ,AIMS_OPT_INT   ,&T              ,0                ,0},
  { ' ',"curvature"    ,AIMS_OPT_FLAG  ,&curvatureFlag  ,0                ,0},
  { ' ',"Lvv"          ,AIMS_OPT_FLAG  ,&LvvFlag        ,0                ,0},
  { ' ',"Lw"           ,AIMS_OPT_FLAG  ,&LwFlag         ,0                ,0},
  { ' ',"laplacien"    ,AIMS_OPT_FLAG  ,&LaplacienFlag  ,0                ,0},
  { ' ',"div"          ,AIMS_OPT_FLAG  ,&DivFlag        ,0                ,0},
  { ' ',"all"          ,AIMS_OPT_FLAG  ,&AllFlag        ,0                ,0},
  { 0  ,0              ,AIMS_OPT_END   ,0               ,0                ,0}};
  
  AimsParseOptions( &argc, argv, opt, usage );
 
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
  float 	Ix,Ixx,Ixy,Iy,Iyy;

  
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
      const char *outC = outtexc.c_str();
      Writer<Texture1d>    w( outC ) ;
      w << courbure;
    }

  if (LvvFlag)
    {
      string outtexl = outtex + "_Lvv";
      const char *outLvv = outtexl.c_str();
      Writer<Texture1d>    w1( outLvv ) ;
      w1 << Lvv;
    }
 
 if (LwFlag)
    {
      string outtexl = outtex + "_Lw";
      const char *outLw = outtexl.c_str();
      Writer<Texture1d>    w2( outLw ) ;
      w2 << Lw;
    }

 if (LaplacienFlag)
   {
     string outtexl = outtex + "_Lapl";
     const char *outLa = outtexl.c_str();
     Writer<Texture1d>    w3( outLa ) ;
     w3 << Laplacien;
   }

 if (DivFlag)
   {
     string outtexl = outtex + "_div";
     const char *outDiv = outtexl.c_str();
     Writer<Texture1d>    w4( outDiv ) ;
     w4 << div;
   }

 
  cout << "done\n";
  return( 0 );
}
