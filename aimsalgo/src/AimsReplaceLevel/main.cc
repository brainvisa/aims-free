
#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/utility/linearcomb.h>
#include <functional>
#include <iostream>
#include <string>
#include <map>

using namespace aims;
using namespace carto;
using namespace std;

template<class T>
static bool doit( Process &, const string &, Finder & );


class Replacer : public Process
{
public:
  Replacer( vector<string> l1, vector<string> l2, const string & fout );

private:
  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  vector<string>		level1;
  vector<string>		level2;
  string		fileout;
};


::Replacer::Replacer( vector<string> l1, vector<string> l2,
                      const string & fout )
    : Process(), level1( l1 ), level2( l2 ), fileout( fout )
{
  registerProcessType( "Volume", "S8", &doit<int8_t> );
  registerProcessType( "Volume", "U8", &doit<uint8_t> );
  registerProcessType( "Volume", "S16", &doit<int16_t> );
  registerProcessType( "Volume", "U16", &doit<uint16_t> );
  registerProcessType( "Volume", "S32", &doit<int32_t> );
  registerProcessType( "Volume", "U32", &doit<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &doit<float> );
  registerProcessType( "Volume", "DOUBLE", &doit<double> );
  registerProcessType( "Volume", "RGB", &doit<AimsRGB> );
  registerProcessType( "Volume", "RGBA", &doit<AimsRGBA> );
}


template<class T> bool
doit( Process & p, const string & fname, Finder & f )
{
  ::Replacer		& rp = (::Replacer &) p;
  string		format = f.format();
  Reader<Volume<T> >	r( fname );
  VolumeRef<T>		in;

  in.reset( r.read( 0, &format ) );

  typename aims::Replacer<T>::MapType	levels;
  
  for(unsigned int i = 0; i < rp.level1.size(); i++)
  {
     T l1, l2;
     carto::stringTo(rp.level1[i], l1);
     carto::stringTo(rp.level2[i], l2);
     //std::cout << carto::toString(l1) << " => " << carto::toString(l2) << std::endl << std::flush;
     levels[l1] = l2;
  }

  aims::Replacer<T>::replace( *in, *in, levels );

  Writer<Volume<T> > writer( rp.fileout );
  return writer.write( *in );
}


int main( int argc, const char **argv )
{
  
string			filein, fileout;
vector<string>		graylevel, newlevel;

  AimsApplication	app( argc, argv, "Replace gray levels by others");
  
  app.addOption(filein,"-i", "input");
  app.addOption(fileout,"-o", "output");
  app.addOptionSeries(graylevel,"-g", 
                      "gray levels or RGB/RGBA values to replace", true);
  app.addOptionSeries(newlevel,"-n", "new values for gray levels or RGB/RGBA", 
                      true);
  
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--graylevel", "-g" );
  app.alias( "--new", "-n" );

  if(graylevel.size() != newlevel.size())
  {
     cout << "The number of gray levels and new gray levels does not match";
     return EXIT_FAILURE;
  }

 try
    {
      app.initialize();
 
      ::Replacer proc( graylevel, newlevel, fileout );
      
      if( !proc.execute( filein ) )
      {
      	cerr << "Failed " << filein << "\n";
        return EXIT_FAILURE;        
      }
	
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
