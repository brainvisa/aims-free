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

/*
 *  Extract info of from standard data
 */
/*	AimsFileInfo will give you some information about data contained in a 
	file. Some file types (volumes of scalar elements) may give you more 
	specific information than others
*/

#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <cartobase/config/verbose.h>
#include <aims/io/reader.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/mesh/surface.h>
#include <aims/bucket/bucket.h>
#include <aims/mesh/texture.h>
#include <aims/resampling/motion.h>
#include <soma-io/writer/pythonwriter.h>
#include <cartobase/object/attributed.h>
#include <cartobase/stream/sstream.h>
#include <time.h>

using namespace aims;
using namespace carto;
using namespace std;


// helper for printing values

template<typename T> void outHelper( T value, ostream& os )
{
    os << value;
}

template<> void outHelper<int8_t>( int8_t value, ostream& os )
{
    os << int(value);
}

template<> void outHelper<uint8_t>( uint8_t value, ostream& os )
{
    os << int(value);
}

#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template<> void outHelper<char>( char value, ostream& os )
{
    os << (char)value;
}
#endif

//	helper for printing attributes
template<typename T> void outHelper( const AttributedObject& object,
				     const string& semantic, ostream& os )
{
  T	i;
  object.getProperty(semantic, i);
  os << i;
}


template<> void outHelper<int8_t>( const AttributedObject& object,
                                   const string& semantic, ostream& os )
{
  char	i;
  object.getProperty(semantic, i);
  os << (int) i;
}


template<> void outHelper<uint8_t>( const AttributedObject& object,
                                    const string& semantic, ostream& os )
{
  char	i;
  object.getProperty(semantic, i);
  os << (int) i;
}


// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template<> void outHelper<char>( const AttributedObject& object,
				 const string& semantic, ostream& os )
{
  char	i;
  object.getProperty(semantic, i);
  os << (int) i;
}
#endif


template<typename T> void outVecHelper( const AttributedObject& object,
					const string& semantic, ostream& os )
{
  vector<T>	x;
  object.getProperty(semantic, x);
  unsigned	i, n = x.size();
  for( i=0; i<n; ++i )
    {
      os << x[i];
      if( i < n-1 )
	os << ", ";
    }
}


/*	print the generic part of the info: header, no template needed here
	(this function doesn't access the data) */
bool printHeader( const Finder & f )
{
  string	type = f.dataType();
  string	format = f.format();

  const Header		*h = f.header();
  const PythonHeader	*ph = 0;
  if( !h )
    cerr << "format has no header (surprising...)\n";
  else
    {
      ph = dynamic_cast<const PythonHeader *>( h );
      if( !ph )
        cerr << "header has no attributes\n";
    }
  Object ph2 = Object::value<PropertySet>();
  if( ph )
  {
    Object tmpObject = Object::reference( *ph );
    ph2->copyProperties( tmpObject );
  }
  if( !ph || !ph->hasProperty( "object_type" ) )
    ph2->setProperty( "object_type", f.objectType() );
  if( !ph || !!ph->hasProperty( "data_type" ) )
    ph2->setProperty( "data_type", type );
  if( !ph || !!ph->hasProperty( "file_type" ) )
    ph2->setProperty( "file_type", format );
  vector<string>	types = f.possibleDataTypes();
  if( types.size() > 1 
      && ( !ph || !ph->hasProperty( "possible_data_types" ) ) )
    ph2->setProperty( "possible_data_types", types );

  PythonWriter	pw( *ph->syntax() );
  pw.attach( cout );
  pw.write( ph2, true );
  return true;
}


/*	Print data extrema, only works on volumes of data types whith an 
	ordering relation */
template<class T>
bool printExtrema ( VolumeRef<T> & vol, const string & filename, 
		    const Finder & finder, bool verbose )
{
  printHeader( finder );
  if( !verbose )
    return( true );

  Reader<VolumeRef<T> >	r( filename );
  r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly, 
                                           DataSource::none(), 
                                           false, 0.01 ) );
  string		format = finder.format();
  /*
  cout << "extrema - read vol " << &vol << " w/ header " 
       << dynamic_cast<PythonHeader *>( vol.header() ) 
       << endl;
  */

  try
  {
    VolumeRef<T> *pv = r.read( 0, &format );
    vol = *pv;
    delete pv;
    if( !vol.get() )
      return false;
  }
 catch( exception & e )
   {
     cerr << "read failed\n";
     cerr << e.what() << endl;
     throw;
   }

  /*
  cout << "extrema - vol read OK, header now: " 
       << dynamic_cast<PythonHeader *>( &vol->header() ) 
       << vol.get() 
       << endl;
  */

  int		x, y, z, t, xmin, ymin, zmin, tmin, xmax, ymax, zmax, tmax;
  int		xm, ym, zm, tm;
  T		valmin = vol( 0, 0, 0 ), valmax = vol( 0, 0, 0 ), val;
  double	sum = 0;

  xmin = ymin = zmin = tmin = xmax = ymax = zmax = tmax = 0;
  xm = vol.getSizeX();
  ym = vol.getSizeY();
  zm = vol.getSizeZ();
  tm = vol.getSizeT();

  for( t=0; t<tm; ++t )
    for( z=0; z<zm; ++z )
      for( y=0; y<ym; ++y )
        for( x=0; x<xm; ++x )
          {
            val = vol( x, y, z, t );
            if( val < valmin )
              {
                valmin = val;
                xmin = x;
                ymin = y;
                zmin = z;
                tmin = t;
              }
            if( val > valmax )
              {
                valmax = val;
                xmax = x;
                ymax = y;
                zmax = z;
                tmax = t;
              }
            sum += val;
          }

  cout << "minimum : ";
  outHelper<T>(valmin, cout);
  cout << " at " 
       << "(" << xmin << "," << ymin << "," 
       << zmin << "," << tmin << ")" << endl;
  cout << "maximum : ";
  outHelper<T>(valmax, cout);
  cout << " at " 
       << "(" << xmax << "," << ymax << "," 
       << zmax << "," << tmax << ")" << endl;
  cout   << "mean : " 
       << sum / ( long( vol.getSizeX() ) * vol.getSizeY() * vol.getSizeZ() 
                  * vol.getSizeT() ) << endl;

  return true;
}


template<class T>
static bool extrema( Process & p, const string & filename, Finder & finder );


//	Processing class, with template operation function
class FileInfo : public Process
{
public:
  FileInfo( bool verb );

  template<class T>
  friend bool extrema( Process & p, const string & filename, Finder & finder );

private:
  bool	verbose;
};


template<class T>
bool extrema( Process & p, const string & filename, Finder & finder )
{
  // retrieve exact process type
  FileInfo  &fi = (FileInfo &) p;
  // instantiate data
  // cout << "extrema - alloc vol...\n";
  T	    vol;
  /*
  cout << "extream, vol allocated " << &vol << ", header: " 
       << dynamic_cast<PythonHeader *>( vol.header() ) 
       << endl;
  */
  return( printExtrema( vol, filename, finder, fi.verbose ) );
}


static bool header( Process &, const string &, Finder & f )
{
  return printHeader( f );
}


static bool printMotion( Process &, const string & filename, Finder & finder )
{
  printHeader( finder );
  if( !verbose )
    return( true );

  Motion m;
  Reader<Motion>	r( filename );
  string		format = finder.format();

  try
  {
    if( !r.read( m, 0, &format ) )
      return false;
  }
  catch( exception & e )
  {
    cerr << "read failed\n";
    cerr << e.what() << endl;
    throw;
  }
  cout << m << endl;
  return true;
}


static bool printGenericObject( Process &, const string & filename,
                                Finder & finder )
{
  printHeader( finder );
  if( !verbose )
    return true;

  Object x;
  Reader<Object> r( filename );
  string         format = finder.format();

  try
  {
    if( !r.read( x, 0, &format ) )
      return false;
  }
  catch( exception & e )
  {
    cerr << "read failed\n";
    cerr << e.what() << endl;
    throw;
  }
  PythonWriter pw;
  pw.attach( cout );
  pw.write( x );
  return true;
}


FileInfo::FileInfo( bool verb ) : Process(), verbose( verb )
{
  // Here we register processing functions for every possible type, 
  // not necessarily using the same kind of process for each type
  registerProcessType( "Volume", "S8", &extrema<VolumeRef<int8_t> > );
  registerProcessType( "Volume", "U8", &extrema<VolumeRef<uint8_t> > );
  registerProcessType( "Volume", "S16", &extrema<VolumeRef<int16_t> > );
  registerProcessType( "Volume", "U16", &extrema<VolumeRef<uint16_t> > );
  registerProcessType( "Volume", "S32", &extrema<VolumeRef<int32_t> > );
  registerProcessType( "Volume", "U32", &extrema<VolumeRef<uint32_t> > );
  registerProcessType( "Volume", "FLOAT", &extrema<VolumeRef<float> > );
  registerProcessType( "Volume", "DOUBLE", &extrema<VolumeRef<double> > );
  registerProcessType( "Volume", "CFLOAT", &header );
  registerProcessType( "Volume", "CDOUBLE", &header );
  registerProcessType( "Volume", "RGB", &header );
  registerProcessType( "Volume", "RGBA", &header );
  registerProcessType( "Volume", "POINT3DF", &header );
  registerProcessType( "Volume", "VECTOR_OF_3_S16", &header );
  registerProcessType( "Volume", "VECTOR_OF_6_FLOAT", &header );
  registerProcessType( "Mesh", "VOID", &header );
  registerProcessType( "Segments", "VOID", &header );
  registerProcessType( "Mesh4", "VOID", &header );
  registerProcessType( "Bucket", "VOID", &header );
  registerProcessType( "Bucket", "S16", &header );
  registerProcessType( "Bucket", "FLOAT", &header );
  registerProcessType( "Bucket", "DTITENSOR", &header );
  registerProcessType( "Texture", "FLOAT", &header );
  registerProcessType( "Texture", "S16", &header );
  registerProcessType( "Texture", "S32", &header );
  registerProcessType( "Texture", "unsigned integer", &header );
  registerProcessType( "Texture", "POINT2DF", &header );
  registerProcessType( "Graph", "VOID", &header );
  registerProcessType( "Bundles", "any", &header );
  registerProcessType( "Motion", "VOID", &printMotion );
  registerProcessType( "genericobject", "any", &printGenericObject );
}


int main( int argc, const char **argv )
{
  try
    {
      string		filein;
      // bool		verbose = false;
      AimsApplication	app( argc, argv, "Give information about a data file "
                           "(any file Aims can read)" );
      app.addOption( filein, "-i", "input data file" );
      // app.addOption( verbose, "-v", 
      //                "give more info if possible (reads data)", true );
      app.alias( "--input", "-i" );
      app.alias( "-v", "--verbose" );
      app.initialize();

      FileInfo	proc( verbose );
      bool	res = false;
      try
        {
          res = proc.execute( filein );
        }
      catch( exception & )
        {
          if( !res )
            {
              Finder	f;
              if( f.check( filein ) )
                printHeader( f );
            }
          throw;
        }
    }
  catch( user_interruption &e )
    {
    }
  catch( std::exception &e )
    {
      cerr << argv[ 0 ] << ": " << e.what() << endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
