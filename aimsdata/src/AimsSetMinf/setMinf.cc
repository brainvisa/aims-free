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
#include <aims/data/data.h>
#include <aims/data/pheader.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/io/reader.h>
#include <aims/io/finder.h>
#include <aims/io/process.h>
#include <cartobase/object/pythonwriter.h>

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
