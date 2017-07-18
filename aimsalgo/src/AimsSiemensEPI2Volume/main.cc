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
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/directory.h>
#include <map>
#include <string>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;

BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsSiemensEPI2Volume -i[nput] <filein>                         ",
  "                      -o[utput] <fileout>                       ",
  "                      -f[irst] <num>                            ",
  "                      -s[lices] <nbslice>                       ",
  "                      -r[epetitions] <nbrep>                    ",
  "                      [-w[rite] <writer>]                       ",
  "                      [-h[elp]]                                 ",
  "----------------------------------------------------------------",
  "Concatenates all Siemens NEMA EPI images to a 4D data file      ",
  "----------------------------------------------------------------",
  "     filein  : origin file                                      ",
  "     fileout : output file                                      ",
  "     num     : first image number in series                     ",
  "     nbslice : number of slices                                 ",
  "     nbrep   : number of repetitions                            ",
  "----------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char **argv )
{
  char *filein, *fileout;
  int first = -1, nbslice = -1, nbrep = -1;
 
  AimsOption opt[] = {
    { 'h',"help"        ,AIMS_OPT_FLAG  ,( void* )Usage       ,AIMS_OPT_CALLFUNC,0,},
    { 'i',"input"       ,AIMS_OPT_STRING,&filein     ,0                ,1},
    { 'o',"output"      ,AIMS_OPT_STRING,&fileout    ,0                ,1},
    { 'f',"first"       ,AIMS_OPT_INT   ,&first      ,0                ,1},
    { 's',"slices"      ,AIMS_OPT_INT   ,&nbslice    ,0                ,1},
    { 'r',"repetitions" ,AIMS_OPT_INT   ,&nbrep      ,0                ,1},
    { 0  ,0             ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  if ( first < 0 )  AimsUsage( usage );
  if ( nbslice <= 0 )  AimsUsage( usage );
  if ( nbrep <= 0 )  AimsUsage( usage );
  std::string filebasename( FileUtil::basename( filein ) );
  std::string directoryname( FileUtil::dirname( filein ) );

  Directory	dir( directoryname );
  set<string>	files = dir.files();
  set<string>::iterator	ifl, efl = files.end();
  map< int, string > listname;
  int nfin = first + nbslice * nbrep;

  for( ifl=files.begin(); ifl!=efl; ++ifl )
    {
      const string	& s = *ifl;
      string sno = s.substr( s.rfind( "_" ) + 1, 
                             s.rfind( ".nema" ) - s.rfind( "_" ) );
      int no = atoi( sno.c_str() );
      if ( no >= first && no < nfin && ( ( no - first ) % nbslice ) == 0 )
        listname[ no ] = directoryname + FileUtil::separator() + s;
    }

  map< int, string >::const_iterator it = listname.begin();

  cout << "Repetition 1 : " << it->second << endl;
  AimsData< short > in;
  Reader< AimsData<short> > readfirst( it->second );
  readfirst >> in;
  ++it;

  int i, j, k, theRep = 0;
  int dx = in.dimX();
  int dy = in.dimY();
  AimsData< short > res( dx, dy, nbslice, nbrep );
  res.setSizeXYZT( in.sizeX(), in.sizeY(), in.sizeZ() );

  for ( k=0; k<nbslice; k++ )
    for ( j=0; j<dy; j++ )
      for ( i=0; i<dx; i++ )
	res( i, j, k, theRep ) = in( i, j, k );
	
  theRep++;
  
  do 
    {
      cout << "Repetition " << theRep + 1 << " : " << it->second << endl;
      Reader< AimsData<short> > reader( it->second );
      reader >> in;
      ++it;

      for ( k=0; k<nbslice; k++ )
        for ( j=0; j<dy; j++ )
	  for ( i=0; i<dx; i++ )
	    res( i, j, k, theRep ) = in( i, j, k );

      theRep++;
    }
  while ( it != listname.end() );

  Writer<AimsData<short> > writer( fileout );
  writer << res;
 
  return EXIT_SUCCESS;
}

