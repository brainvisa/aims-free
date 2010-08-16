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
                 "8xz / 8yz / 18 / 26 [default=26]", true );
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
      else if ( connect == "18" )
        proc.connectivity = Connectivity::CONNECTIVITY_18_XYZ;
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
