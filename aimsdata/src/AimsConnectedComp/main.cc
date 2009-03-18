/*
 *  Copyright (C) 1998-2004 CEA
 *
 *  This software and supporting documentation were developed by
 *  	CEA/DSV/SHFJ
 *  	4 place du General Leclerc
 *  	91401 Orsay cedex
 *  	France
 *
 *  Connected components
 */

#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/connectivity/component.h>
#include <aims/data/data.h>
#include <aims/io/finder.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;


class CC : public Process
{
public:
  CC();
  virtual ~CC() {}

  template <typename T> 
  static bool doit( Process &, const string & filename, Finder & finder );

  Connectivity::Type	connectivity;
  size_t		minSize;
  size_t		numMax;
  bool			bin;
  string		out;
};


CC::CC()
  : Process(), connectivity( Connectivity::CONNECTIVITY_26_XYZ ), 
    minSize( 0 ), numMax( 0 ), bin( false )
{
  registerProcessType( "Volume", "S16", &doit<AimsData<int16_t> > );
  registerProcessType( "Bucket", "VOID", &doit<BucketMap<Void> > );
  registerProcessType( "Bucket", "S16", &doit<BucketMap<int16_t> > );
}


namespace
{

  template <typename T> 
  class datatype
  {
  public:
    typedef int data_type;
  };


  template <typename T> 
  class datatype<AimsData<T> >
  {
  public:
    typedef T data_type;
  };


  template <typename T> 
  class datatype<BucketMap<T> >
  {
  public:
    typedef T data_type;
  };

}


template <typename T>
bool CC::doit( Process & p, const string & filename, Finder & finder )
{
  CC	& cc = (CC &) p;

  T		data;
  string	format = finder.format();
  Reader<T>	dataR( filename );

  dataR.read( data, 1, format.empty() ? 0 : &format );

  cout << "minsize: " << cc.minSize << ", nummax: " << cc.numMax << endl;
  AimsConnectedComponent( data, cc.connectivity, 
                          typename datatype<T>::data_type(0), 
                          cc.bin, cc.minSize, cc.numMax );

  Writer<T> dataW( cc.out );
  dataW.write( data );
  return true;
}


int main( int argc, const char **argv )
{
  CC	proc;
  ProcessInput	pi( proc );
  string	connect = "26";

  AimsApplication	app( argc, argv, "Extract connected components of a " 
                             "binary data volume" );
  app.addOption( pi, "-i", "input SHORT binary volume" );
  app.addOption( proc.out, "-o", 
                 "output labelled connected components volume" );
  app.addOption( connect, "-c", "connectivity  4xy / 4xz / 4yz / 6 / 8xy / " 
                 "8xz / 8yz / 26 [default=26]", true );
  app.addOption( proc.minSize, "-s", "minimum size of the seeds [default=0]", 
                 true );
  app.addOption( proc.numMax, "-n", 
                 "number of components kept  [default=all]", true );
  app.addOption( proc.bin, "-b", "force binary mode [default=false]", true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--connectivity", "-c" );
  app.alias( "--size", "-s" );

  try
    {
      app.initialize();

      if ( connect == "4xy" ) 
        proc.connectivity = Connectivity::CONNECTIVITY_4_XY;          
      else if ( connect == "4xz" ) 
        proc.connectivity = Connectivity::CONNECTIVITY_4_XZ;       
      else if ( connect == "4yz" ) 
        proc.connectivity = Connectivity::CONNECTIVITY_4_YZ;       
      else if ( connect == "6" ) 
        proc.connectivity = Connectivity::CONNECTIVITY_6_XYZ;       
      else if ( connect == "8xy" ) 
        proc.connectivity = Connectivity::CONNECTIVITY_8_XY;       
      else if ( connect == "8xz" ) 
        proc.connectivity = Connectivity::CONNECTIVITY_8_XZ;       
      else if ( connect == "8yz" ) 
        proc.connectivity = Connectivity::CONNECTIVITY_8_YZ;       
      else if ( connect == "26" ) 
        proc.connectivity = Connectivity::CONNECTIVITY_26_XYZ;       
      else
        throw invalid_number( "Invalid connectivity value" );

      if( proc.execute( pi.filename ) )

      return( EXIT_SUCCESS );
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
