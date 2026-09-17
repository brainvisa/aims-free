
#include <cstdlib>
#include <aims/data/data.h>
#include <aims/data/pheader.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/io/reader.h>
#include <aims/io/finder.h>
#include <aims/io/process.h>
#include <soma-io/writer/pythonwriter.h>

using namespace aims;
using namespace carto;
using namespace std;

#if 0
template<typename T> 
static bool setminf( Process &, const string &, Finder & );

class SetMinf : public Process
{
public:
  SetMinf( vector<string>& target, vector<string>& value, string & fromi, 
           vector<string> & copya );
  virtual ~SetMinf();

  vector<string>	target;
  vector<string>        value;
  string		fromimage;
  vector<string>        copy;
};


SetMinf::SetMinf( string& target, string& value, string & fromi, 
                  vector<string> & copya ) 
  : target( target ), value( value ), fromimage( fromi ), copy( copya )
{
  registerProcessType( "Volume", "S8", &setminf<char> );
  registerProcessType( "Volume", "U8", &setminf<byte> );
  registerProcessType( "Volume", "S16", &setminf<short> );
  registerProcessType( "Volume", "U16", &setminf<ushort> );
  registerProcessType( "Volume", "S32", &setminf<int32_t> );
  registerProcessType( "Volume", "U32", &setminf<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &setminf<float> );
  registerProcessType( "Volume", "DOUBLE", &setminf<double> );
}


SetMinf::~SetMinf()
{
}


template<typename T>
bool setminf( Process & p, const string & filein, Finder & f )
{

  string v = ( (SetMinf &) p).value;
  string t = ( (SetMinf &) p).target;

  const Header	*h = f.header();
  if( !h )
    {
      cout << "format has no header (surprising...)\n";
      return( true );
    }
  const PythonHeader *ph = dynamic_cast<const PythonHeader *>( h );
  if( !ph )
    {
      cout << "header has no attributes\n";
      return( true );
    }

  Header *hdr = ph->cloneHeader();
  PythonHeader *phdr =  dynamic_cast< PythonHeader *>( hdr );
  unsigned	i, n = p.value.
  vector<string>::
  phdr->setProperty( t, v );
  
  phdr->writeMinf( ph->removeExtension( filein ) + ph->extension()  + ".minf");
  return( true );
}
#endif


int main( int argc, const char **argv )
{
  string		filein, filefrom;
  vector<string>        target, value, copy;

  AimsApplication	app( argc, argv, 
			     "Set arbitrary tag in the minf file ");
  app.addOption( filein, "-i", "input data" );
  app.alias( "--input", "-i" );
  app.addOptionSeries( target, "-t", "target name", 0 );
  app.alias( "--target", "-t");
  app.addOptionSeries( value, "-v", "value for target", 0 );
  app.alias( "--value", "-v" );
  app.addOption( filefrom, "-f", "from other image", true );
  app.alias( "--from", "-f" );
  app.addOptionSeries( copy, "-c", "copy given attribute(s) from other image " 
                       "(needs -f option)", 0 );
  app.alias( "--copy", "-c" );


  try
    {
      app.initialize();
      Finder	f;
      f.check( filein );
      const PythonHeader 
        *ph = dynamic_cast<const PythonHeader *>( f.header() );
      if( !ph )
        {
          cerr << "No header in image " << filein << endl;
          return EXIT_FAILURE;
        }

      unsigned	i, n = value.size(), m = target.size();
      if( n != m )
        {
          cerr << "Warning: attributes name/value list have differing sizes. " 
               << "Truncating" << endl;
          if( n < m )
            n = m;
        }

      // commandline attributes
      Header *hdr = ph->cloneHeader();
      PythonHeader *phdr =  dynamic_cast< PythonHeader *>( hdr );
      for( i=0; i<n; ++i )
        phdr->setProperty( target[i], value[i] );

      // attributes from image
      if( !copy.empty() )
      {
        if( filefrom.empty() )
          cerr << "Warning: no 'from' argument" << endl;
        else
          {
            Finder	f2;
            f2.check( filefrom );
            const PythonHeader 
              *ph2 = dynamic_cast<const PythonHeader *>( f2.header() );
            if( !ph2 )
              cerr << "could not read 'from' image header\n";
            else
              {
                n = copy.size();
                for( i=0; i<n; ++i )
                  try
                    {
                      Object	o = (*ph2).getProperty( copy[i] );
                      phdr->setProperty( copy[i], o );
                    }
                  catch( exception & e )
                    {
                      cerr << "warning: attribute " << copy[i] << " missing" 
                           << endl;
                      cerr << e.what() << endl;
                    }
              }
          }
      }

      phdr->writeMinf( ph->removeExtension( filein ) + ph->extension() 
                       + ".minf");

    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return( 1 );
    }

  return EXIT_SUCCESS;
}
