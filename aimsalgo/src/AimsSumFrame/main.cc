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
#include <aims/getopt/getopt.h>
#include <aims/data/data_g.h>
#include <aims/data/data.h>
#include <aims/io/io_g.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/roi/hie.h>
#include <aims/io/hieR.h>
#include <aims/io/roiselectorR.h>
#include <aims/def/path.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <cartobase/stream/sstream.h>
#include <cartobase/config/verbose.h>
#include <cartobase/config/cartobase_config.h>
#include <iostream>

using namespace aims; 
using namespace std; 
using namespace carto; 


template<class T>
static bool doit( Process &, const string &, Finder & );


class FrameAverager : public Process
{
public:
  FrameAverager();

  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  string inputseries;
  string sum_volume;
  int		start_f;
  int		end_f;
  bool  byframe;
};


FrameAverager::FrameAverager() 
  : Process()
  {
    registerProcessType( "Volume", "S8", &doit<int8_t> );
    registerProcessType( "Volume", "U8", &doit<uint8_t> );
    registerProcessType( "Volume", "S16", &doit<int16_t> );
    registerProcessType( "Volume", "U16", &doit<uint16_t> );
    registerProcessType( "Volume", "S32", &doit<int32_t> );
    registerProcessType( "Volume", "U32", &doit<uint32_t> );
    registerProcessType( "Volume", "FLOAT", &doit<float> );
    registerProcessType( "Volume", "DOUBLE", &doit<double> );
  }

template<class T> bool 
doit( Process & p, const string & fname, Finder & f )
{
  FrameAverager		&tc = (FrameAverager &) p;
  AimsData<T>		series;
  Reader<AimsData<T> >	qrd( fname );
  string	format = f.format();
  int parsed_start=0, parsed_end=0;
  if ( verbose ) cout << "reading " << fname << "...\n";
  if( !qrd.read( series ) )
    return( false );
  if ( verbose ) cout << "reading done\n";

  if ( !tc.byframe )
    {
      qrd >> series;
      if  ((tc.start_f > 0) && (tc.start_f <= series.dimT()))
	      parsed_start = tc.start_f-1;
      else 
	      parsed_start = 0;

      if ( (tc.end_f > 0) && (tc.end_f <= series.dimT()) && 
           (tc.end_f >= tc.start_f) )
	      parsed_end = tc.end_f;
      else
	      parsed_end =   series.dimT();
      cerr << "AimsSumFrame : summing frames #" << parsed_start+1
	         << " to #"  << parsed_end << endl;

      AimsData< float > *sum;
      sum = new AimsData<float>(series.dimX(), series.dimY(), series.dimZ(),1);

      sum->setSizeXYZT(series.sizeX(), series.sizeY(), series.sizeZ(),1);
      sum->setHeader( series.header()->cloneHeader() ) ;
      *sum = 0;
      // For Eacat images only
      vector<int> st ;
      vector<int> dt ;
      const PythonHeader * poh = dynamic_cast< const PythonHeader*>( series.header() ) ;
      PythonHeader * sumpoh = dynamic_cast<PythonHeader *>(poh->cloneHeader() ) ;
      //PythonHeader * sumpoh = poh->cloneHeader() ;
      if(sumpoh)
      {
        if( sumpoh->hasProperty("start_time") 
            && sumpoh->hasProperty("duration_time") )
          {
	      throw runtime_error( "Image is a PET image and shold not be !" ) ;
          } else {
	     int i, j, k, l;
	     for (l=parsed_start; l< parsed_end; l++)
	        ForEach3d( (*sum),i,j,k) 
	         (*sum)(i,j,k) += series(i,j,k,l) ;
    	  }
      }
      Writer<AimsData<float> > writer( tc.sum_volume );
      writer << *sum;   
    }
  else
    {
      cout << "not implemented yet" << endl;
    }
  return( true );
}


int main( int argc, const char **argv )
{
  try
    {
      AimsApplication	app( argc, argv, "Compute The sum of the dynamic series :"
        "for a PET file, use AimsSumPETImage");
      string inputseries, sum_volume;
      int start_f=0, end_f=0;
      bool byframe=false;
      FrameAverager		proc;
      ProcessInput		seriesr( proc );

      app.addOption(seriesr, "-i", "input series <dynamic series>" );
      app.addOption(start_f , "-s", "start frame < between 1 and maxFrame >", true  );
      app.addOption(end_f , "-e", "end frame < between 1 and maxFrame >", true  );
      app.addOption(byframe , "-b", "(to enforce frame by frame computation)", true );
      app.addOption(sum_volume, "-o", "output average volume" );

      app.alias( "--input", "-i" );
      app.alias( "--output", "-o" );
      app.alias( "--startframe", "-s" );
      app.alias( "--endframe", "-e" );
      app.alias( "--byframe", "-b" );

      app.initialize();

      proc.inputseries = inputseries;
      proc.sum_volume = sum_volume;
      proc.start_f = start_f;
      proc.end_f = end_f;
      proc.byframe = byframe;
      proc.execute( seriesr.filename );
      if ( verbose ) cout << "done\n";

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

/*
BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsSumFrame -i[nput series]   < dynamic series >               ",
  "             -o[utput]   < output >                             ",
  "             -s[start frame]   < between 1 and maxFrame >       ",
  "             -e[end frame]     < between 1 and maxFrame >       ",
  "             --byframe  (to enforce frame by frame compoutation)",
  "            [-h[elp]]                                           ",
  "----------------------------------------------------------------",
  " Compute The sum of the dynamic series : if it is a PET file,   ",
  " average is computed ('sum image') in PET terminology           ",
  "----------------------------------------------------------------",
END_USAGE

void Usage( void )
{
  AimsUsage( usage );
}

  char *fileseries=NULL; // Mandatory file name input

  char   *fileoutput=NULL;
  char   *outtype = "gis";
  int    byframe = 0;
  int    start=-1, end=-1;
  int    parsed_start, parsed_end;


  AimsOption opt[] = {
  { 'h',"help"     ,AIMS_OPT_FLAG  ,( void* )Usage       ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input series",AIMS_OPT_STRING,&fileseries ,0             ,1},
  { 'o',"output"   ,AIMS_OPT_STRING,&fileoutput ,0                ,1},
  { 's',"start_frame"   ,AIMS_OPT_INT,&start ,0                ,0},
  { 'e',"end_frame"   ,AIMS_OPT_INT,&end ,0                ,0},
  { ' ',"outtype",  AIMS_OPT_STRING,&outtype    ,0                ,0},
  { ' ',"byframe",  AIMS_OPT_FLAG,  &byframe    ,0                ,0},
  { 0  ,0          ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions(&argc,argv,opt,usage);

  //Init Read series info ====================================================
  Reader<AimsData<float> > qrd( fileseries );
  AimsData< float > series;
  AimsData< float > *sum;




  if ( !byframe )
    {
      qrd >> series;
      
      if  ((start > 0) && (start <= series.dimT()))
	parsed_start = start-1;
      else 
	parsed_start = 0;
      if ( (end > 0) && (end <= series.dimT()) && (end >= start) )
	parsed_end = end;
      else
	parsed_end =   series.dimT();
      
      
      cerr << "AimsSumFrame : summing frames #" << parsed_start+1
	   << " to #"  << parsed_end << endl;
      
      
      sum = new AimsData<float>(series.dimX(), series.dimY(), series.dimZ(),1);
      sum->setSizeXYZT(series.sizeX(), series.sizeY(), series.sizeZ(),1);
      sum->setHeader( series.header()->cloneHeader() ) ;
      *sum = 0;

      // For Eacat images only

      vector<int> st ;
      vector<int> dt ;
      const PythonHeader * poh = dynamic_cast< const PythonHeader*>( series.header() ) ;
      PythonHeader * sumpoh = dynamic_cast<PythonHeader *>(poh->cloneHeader() ) ;
      //PythonHeader * sumpoh = poh->cloneHeader() ;
      if(sumpoh)
        if( sumpoh->hasProperty("start_time") 
            && sumpoh->hasProperty("duration_time") ){
           sumpoh->getProperty("start_time", st) ;
           sumpoh->getProperty("duration_time", dt) ;
	   if( int(st.size()) == series.dimT() && int(dt.size()) == series.dimT()){
  	     vector<int> sumSt(1) ;
             sumSt[0] = st[parsed_start] ;
	     sumpoh->setProperty("start_time", sumSt) ;
       	     vector<int> sumDt(1, 0) ;
	     for( int l = parsed_start; l < parsed_end; ++l )
	       sumDt[0] += dt[l] ;
	     // sumDt[0] = st[series.dimT()-1] - st[parsed_start] + dt[series.dimT()-1]
	     sumpoh->setProperty("duration_time", sumDt) ;
	     sum->setHeader(sumpoh) ;
	     
	     int i, j, k, l;
	     for (l=parsed_start; l< parsed_end; l++)
	       ForEach3d( (*sum),i,j,k) 
		 (*sum)(i,j,k) += series(i,j,k,l) * dt[l] / sumDt[0] ;
	   }
	 } 
	else
	  {
	    int i, j, k, l;
	    for (l=parsed_start; l< parsed_end; l++)
	      ForEach3d( (*sum),i,j,k) 
	      (*sum)(i,j,k) += series(i,j,k,l) ;
	    
	  }
    }
  else
    {
//       int dimT = GetDimTFromHeader(fileseries);
//       sum = new AimsData<float>(dimX(), dimY(), dimZ(),1);
//       sum->setSizeXYZT(series.sizeX(), series.sizeY(), series.sizeZ(),1);
//       if ( dimT )
// 	{
// 	  cout << "Gather stats through frame by frame reading" << endl;
// 	  for (int t = 0; t < dimT; t++)
// 	    {
// 	      qrd.frameRead( series, t, 0L );
// 	      int i, j, k;
// 	      ForEach3d(series,i,j,k) sum(i,j,k) += series(i,j,k);
// 	    }
// 	}
      cout << "not implemented yet" << endl;
    }

  Writer<AimsData<float> > writer( fileoutput );
  writer << *sum;   
}
*/
