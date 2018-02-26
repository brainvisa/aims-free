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

class CCProcess : public Process
{
  public :
    CCProcess();
  
    inline
    CCProcess & operator = (const CCProcess &val) { 
      this->connectivity = val.connectivity;
      this->minSize = val.minSize;
      this->maxSize = val.maxSize;
      this->numMax = val.numMax;
      this->bin = val.bin;
      this->in = val.in;
      this->out = val.out;
      this->dtype = val.dtype;
      return (*this);
    }
  
    Connectivity::Type  connectivity;
    size_t    minSize;
    size_t    maxSize;
    size_t    numMax;
    bool      bin;
    string    in;
    string    out;
    string    dtype;
};

class CC : public CCProcess
{
  public:
    CC();
    virtual ~CC() {}

    template <typename T> 
    static bool convert( Process &, const string & filename, Finder & finder );
    
    using CCProcess::operator=;
};

template <typename T>
class CCConverter : public CCProcess
{
  public:
    CCConverter();
    virtual ~CCConverter() {}
    
    using CCProcess::operator=;
};

template <typename T, typename O>
class DataConverter
{
  public:
    DataConverter();
    
    static bool doit( Process &, const string & filename, Finder & finder );
};

// Partial specialization
template <typename T>
class DataConverter<T, T>
{
  public:
    DataConverter();
    
    static bool doit( Process &, const string & filename, Finder & finder );
};


CCProcess::CCProcess()
  : connectivity( Connectivity::CONNECTIVITY_26_XYZ ), 
                minSize( 0 ), maxSize( 0 ), numMax( 0 ), bin( false )
{}

                
CC::CC()
  : CCProcess()
{
  registerProcessType( "Volume", "U8", &convert<uint8_t> );
  registerProcessType( "Volume", "S8", &convert<int8_t> );
  registerProcessType( "Volume", "U16", &convert<uint16_t> );
  registerProcessType( "Volume", "S16", &convert<int16_t> );
  registerProcessType( "Volume", "U32", &convert<uint32_t> );
  registerProcessType( "Volume", "S32", &convert<int32_t> );
  registerProcessType( "Volume", "FLOAT", &convert<float> );
  registerProcessType( "Volume", "DOUBLE", &convert<double> );
  
  registerProcessType( "Bucket", "VOID", &DataConverter<BucketMap<Void>, BucketMap<Void> >::doit );
  registerProcessType( "Bucket", "S16", &DataConverter<BucketMap<int16_t>, BucketMap<int16_t> >::doit );
}

template <typename T>
bool CC::convert( Process & p, const string & filename, Finder & finder )
{
  CCConverter<T> c;
  c = (CCProcess &) p;
  
  // Set output data type
  Finder    f;
  f.setObjectType( finder.objectType() );
  
  if( c.dtype.empty() )
    f.setDataType( finder.dataType() );
  else
    f.setDataType( c.dtype );
  
  cout << "input file: " << c.in << " is processed as " << finder.objectType() << " / " << finder.dataType() << endl << flush;

  if(! c.execute( f, c.out ))
    return false;
  
  return true;
}

template <typename T>
CCConverter<T>::CCConverter()
  : CCProcess()
{
  
  registerProcessType( "Volume", "U8", &DataConverter<AimsData<T>, AimsData<uint8_t> >::doit );
  registerProcessType( "Volume", "S8", &DataConverter<AimsData<T>, AimsData<int8_t> >::doit );
  registerProcessType( "Volume", "U16", &DataConverter<AimsData<T>, AimsData<uint16_t> >::doit );
  registerProcessType( "Volume", "S16", &DataConverter<AimsData<T>, AimsData<int16_t> >::doit );
  registerProcessType( "Volume", "U32", &DataConverter<AimsData<T>, AimsData<uint32_t> >::doit );
  registerProcessType( "Volume", "S32", &DataConverter<AimsData<T>, AimsData<int32_t> >::doit );
  registerProcessType( "Volume", "FLOAT", &DataConverter<AimsData<T>, AimsData<float> >::doit );
  registerProcessType( "Volume", "DOUBLE", &DataConverter<AimsData<T>, AimsData<double> >::doit );
}

template <typename T, typename O>
bool DataConverter<T, O>::doit( Process & p, const string & filename, Finder & finder )
{
//   cout << "data type needs conversion to " << finder.objectType() << " / " << finder.dataType() << endl << flush;
  CCProcess & c = (CCProcess &) p;

  T   data;
  string  format = finder.format();
  Reader<T> dataR( c.in );
  dataR.read( data, 1, format.empty() ? 0 : &format );
  
  // Allocate output volume using output data_type
  cout << "output file: " << c.out << " is processed as " << finder.objectType() << " / " << finder.dataType() << " (data type changed)" << endl << flush;
  O out(data.dimX(), data.dimY(), data.dimZ(), data.dimT() );
  out.volume()->header() = data.volume()->header();

  cout << "minsize: " << c.minSize << ", maxsize: " << c.maxSize << ", nummax: " << c.numMax << endl;
  ConnectedComponentEngine<T, O>::connected( data, out, c.connectivity, 
                                             typename datatype<T>::data_type(0), 
                                             c.bin, c.minSize, c.maxSize, c.numMax );

  Writer<O> dataW( c.out );
  dataW.write( out );
  return true;
}

template <typename T>
bool DataConverter<T, T>::doit( Process & p, const string & filename, Finder & finder )
{
  CCProcess & c = (CCProcess &) p;
  cout << "output file: " << c.out << " is processed as " << finder.objectType() << " / " << finder.dataType() << " (data type kept)" << endl << flush;
  
  T   data;
  string  format = finder.format();
  Reader<T> dataR( c.in );

  dataR.read( data, 1, format.empty() ? 0 : &format );

  cout << "minsize: " << c.minSize << ", maxsize: " << c.maxSize << ", nummax: " << c.numMax << endl;
  AimsConnectedComponent( data, c.connectivity, 
                          typename datatype<T>::data_type(0),
                          c.bin, c.minSize, c.maxSize, c.numMax );

  Writer<T> dataW( c.out );
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
  app.addOption( proc.maxSize, "-S", "maximum size of the seeds [default=0]", 
                 true );
  app.addOption( proc.numMax, "-n", 
                 "number of components kept  [default=all]", true );
  app.addOption( proc.bin, "-b", "force binary mode [default=false]", true );
  app.addOption( proc.dtype, "-t", "output data type (only applicable to " 
                                   "volumes, default: same as input)",
                                   true );
  app.alias( "--type", "-t" );
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
        throw invalid_number( "Invalid connectivity value " + connect );

      proc.in = pi.filename;
      
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
