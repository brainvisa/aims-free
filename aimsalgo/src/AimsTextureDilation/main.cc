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


#include <aims/distancemap/meshmorphomat.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/math/math_g.h>
#include <aims/getopt/getopt.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>

typedef float float3[3];

using namespace aims;
using namespace aims::meshdistance;
using namespace std;

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsTextureDilation -i[nput] <meshfilein>                                ",
  "                    -t[exture] <input_texture>                           ",
  "                    -o[utput <output_dist_texture>                       ",
  "		       -s[ize]    <erosion_size>                            ",
  "		       [-b[ackground]    <background label>]                  ",
  "		       [-f[orbidden]    <forbidden label>]                    ",
  "                    [--connexity]                                        ",
  "                    [--ascii]                                            ",
  "                    [-h[elp]]                                            ",
  "-------------------------------------------------------------------------",
  "  Dilation of an image(binar or grey level ) painted on a triangulation   ",
  "  Input texture of 'short int' are dilated with binar structurant element ",
  "  The 'background label' corresponds to the label of the background (default = 0)",
  "  The 'forbidden label' corresponds to the label out of the domain   (default = -1)",
  "  Input texture of 'float' are dilated with the max function as structurant element",
  "-------------------------------------------------------------------------",
  "     meshfilein          : input *.tri or *.mesh file                    ",
  "     output_dist_texture : output *.tex file (labelled opening)          ",
  "     input_texture       : object definition                             ",
  "     ascii         : write *.tex file in ASCII [default=binar]           ",
  "     connexity     : euclidean or mesh connexity distance [default=euclidean]     ",
  "-------------------------------------------------------------------------",
END_USAGE


//
// Usage
//
void Usage( void )
{
  AimsUsage( usage );
}


template<typename T> bool
texdilate( Process &, const string &, Finder & );


class TextureDilation : public Process
{
public:
  TextureDilation( AimsSurfaceTriangle &, float, int, const string &,short,short);
  virtual ~TextureDilation();

  AimsSurfaceTriangle	& surface;
  float			dilatesize;
  int			connexityFlag;
  string		outtexfile;
  short 		back,forbidden;
};


TextureDilation::TextureDilation( AimsSurfaceTriangle & m, float cs, 
				int cf, const string & of,short ba, short forb)
  : surface( m ), dilatesize( cs ), connexityFlag( cf ), outtexfile( of ), back(ba), forbidden(forb)
{
  registerProcessType( "Texture", "FLOAT", &texdilate<float> );
  registerProcessType( "Texture", "S16", &texdilate<short> );
}


TextureDilation::~TextureDilation()
{
}


int main( int argc, char** argv )
{
  char	*meshfile = 0, *intexfile = 0, *outtexfile = 0;
  int	connexityFlag=0;
  float dilatesize;
  int back = 0, forbidden = -1;
  //
  // Parser of options
  //
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage           ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"        ,AIMS_OPT_STRING,&meshfile       ,0                ,1},
  { 'o',"output"       ,AIMS_OPT_STRING,&outtexfile     ,0                ,1},
  { 't',"texture"      ,AIMS_OPT_STRING,&intexfile      ,0                ,1},
  { 'b',"background"   ,AIMS_OPT_INT,   &back           ,0                ,0},
  { 'f',"forbidden"    ,AIMS_OPT_INT,   &forbidden      ,0                ,0},
  { 's',"size"         ,AIMS_OPT_FLOAT, &dilatesize     ,0                ,1},
  { ' ',"connexity"    ,AIMS_OPT_FLAG  ,&connexityFlag  ,0                ,0},
  { 0  ,0              ,AIMS_OPT_END   ,0               ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );


  if (connexityFlag ) 
    cout << "Size of the dilation : " << dilatesize << " connections" << endl;
  else
     cout << "Size of the dilation : " << dilatesize << "mm" << endl;


  //
  // read triangulation
  //
  cout << "reading triangulation   : " << flush;
  AimsSurfaceTriangle surface;
  Reader<AimsSurfaceTriangle> surfaceR( meshfile );
  surfaceR >> surface;
  cout << "done" << endl;

  TextureDilation	proc( surface, dilatesize, connexityFlag, outtexfile,back,forbidden);
  if( !proc.execute( intexfile ) )
    cerr << "Failed\n";
}


template<typename T>
void dilatation( TextureDilation & tp, TimeTexture<T> & inpTex, 
		 TimeTexture<T> & outTex );


template<typename T> bool
texdilate( Process & p, const string & intexfile, Finder & )
{
  TextureDilation	& tp = (TextureDilation &) p;

  //
  // read input texture
  //
  cout << "reading texture   : " << flush;
  
  TimeTexture<T>	inpTex;
  Reader<TimeTexture<T> > texR( intexfile );
  texR >> inpTex;
 
  cout << "done" << endl;

  cout << "mesh vertices : " << tp.surface[0].vertex().size() << endl;
  cout << "mesh polygons : " << tp.surface[0].polygon().size() << endl;
  cout << "texture dim   : " << inpTex[0].nItem() << endl;
  
  TimeTexture<T>	outTex;

  dilatation( tp, inpTex, outTex );

  cout << "writing texture : " << flush;
  Writer<TimeTexture<T> >	texW( tp.outtexfile );
  texW << outTex;
  cout << "done" << endl;

  return true;
}


template<>
void dilatation( TextureDilation & tp, TimeTexture<float> & inpTex, 
		 TimeTexture<float> & outTex )
{
  cout << "Grey-level dilation\n";
  cout << "Dilation..." << flush;
  outTex[0] = MeshGreyDilation( tp.surface[0], inpTex[0], 
  			    (unsigned) tp.dilatesize );
  cout << "done\n";
}


template<>
void dilatation( TextureDilation & tp, TimeTexture<short> & inpTex, 
		 TimeTexture<short> & outTex )
{
  
  cout << "Binar dilation\n";
  if (tp.connexityFlag)
    cout << " metric : connexity";
  else
    cout << " metric : euclidean";
  outTex[0] = MeshDilation(tp.surface[0], inpTex[0], tp.back , tp.forbidden ,tp.dilatesize,tp.connexityFlag);

}
