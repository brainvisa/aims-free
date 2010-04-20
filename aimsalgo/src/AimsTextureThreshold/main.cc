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
#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt.h>
#include <aims/utility/utility_g.h>
#include <assert.h>

using namespace aims;
using namespace std;

BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsTextureThreshold -i[nput] <filein>                          ",
  "              -o[utput] <fileout>                               ",
  "              -m[ode] <mm>                                      ",
  "              -t <threshold1>                                   ",
  "              [-u <threshold2>]                                 ",
  "              [-b[inary]]                                       ",
  "              [-h[elp]]                                         ",
  "----------------------------------------------------------------",
  "Threshold on texture  (each time serie is thresholded)          ",
  "----------------------------------------------------------------",
  "     filein    : origin file                                    ",
  "     fileout   : output file                                    ",
  "     mm        : mode that can be                               ",
  "                   lt --> lower than                            ",
  "                   le --> lower or equal to                     ",
  "                   gt --> greater than                          ",
  "                   ge --> greater or equal to                   ",
  "                   eq --> equal to                              ",
  "                   di --> differ                                ",
  "                   be --> between                               ",
  "                   ou --> outside                               ",
  "     threshold1 : first threshold                               ",
  "     threshold2 : second threshold                              ",
  "     binary     : returns a short binary texture                ",
  "                  (background : 0 ; object : 1)                 ",
  "----------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}



template<class T>
static bool doit( Process &, const string &, Finder & );


class Thresholder : public Process
{
public:
  Thresholder( threshold_t m, const string & fileout, float th1, float th2,  bool b );

private:
  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  threshold_t	mode;
  string	fout;
  float		t1;
  float		t2;
  bool		bin;

};


Thresholder::Thresholder( threshold_t m, const string & fileout, float th1, float th2, bool b ) 
  : Process(), mode( m ), fout( fileout ), t1( th1 ), t2( th2 ), bin( b )
  {
    registerProcessType( "Texture", "S16", &doit<short> );
    registerProcessType( "Texture", "FLOAT", &doit<float> );
    
  }


template<class T> bool 
doit( Process & p, const string & fname, Finder & f )
{
  Thresholder		&tc = (Thresholder &) p;
  TimeTexture<T>		tex;
  Reader<TimeTexture<T> >	r( fname );
  string	format = f.format();
  cout << "reading " << fname << "...\n";
  if( !r.read( tex ) )
    return( false );
  cout << "reading done\n";

  cout << "thresholding..." << flush;
  
  AimsTexThreshold<T,short> thresh( tc.mode, (T) tc.t1, (T) tc.t2 );
  if( tc.bin )
    {
      Writer<TimeTexture<short> > writer( tc.fout );
      return( writer.write( thresh.bin( tex ) ) );
    }
  else
    {
      Writer<TimeTexture<T> > writer( tc.fout );
      return( writer.write( thresh( tex ) ) );
    }
  cout << "done\n";

  return (true);
}


int main( int argc, char **argv )
{
  char *filein = NULL, *fileout = NULL, *smode = NULL;
  float t1 = 0, t2 = 0;
  int binary = 0;
  threshold_t  mode;

  AimsOption opt[] = {
  { 'h',"help"  ,AIMS_OPT_FLAG  ,( void* )Usage       ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input" ,AIMS_OPT_STRING,&filein     ,0                ,1},
  { 'o',"output",AIMS_OPT_STRING,&fileout    ,0                ,1},
  { 'm',"mode"  ,AIMS_OPT_STRING,&smode      ,0                ,1},
  { 't',"t"     ,AIMS_OPT_FLOAT ,&t1         ,0                ,1},
  { 'u',"u"     ,AIMS_OPT_FLOAT ,&t2         ,0                ,0},
  { 'b',"binary",AIMS_OPT_FLAG  ,&binary     ,0                ,0},
  { 0  ,0       ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  if      ( string( smode ) == "lt" ) mode = AIMS_LOWER_THAN;
  else if ( string( smode ) == "le" ) mode = AIMS_LOWER_OR_EQUAL_TO;
  else if ( string( smode ) == "gt" ) mode = AIMS_GREATER_THAN;
  else if ( string( smode ) == "ge" ) mode = AIMS_GREATER_OR_EQUAL_TO;
  else if ( string( smode ) == "eq" ) mode = AIMS_EQUAL_TO;
  else if ( string( smode ) == "di" ) mode = AIMS_DIFFER;
  else if ( string( smode ) == "be" ) mode = AIMS_BETWEEN;
  else if ( string( smode ) == "ou" ) mode = AIMS_OUTSIDE;
  else
    AimsError("AimsThreshold : bad mode" );

  Thresholder	proc( mode, fileout, t1, t2, binary );
  if( ! proc.execute( filein ) )
    throw logic_error( "Internal error: execute failed" );
  cout << "done\n";

  return( EXIT_SUCCESS );
}
