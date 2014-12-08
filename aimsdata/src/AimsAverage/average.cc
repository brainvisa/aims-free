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
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/utility/converter_volume.h>

using namespace aims;
using namespace carto;
using namespace std;


template<class T>
static bool doit( Process & p, const string & filename, Finder & finder );


//	Processing class, with template operation function
class Averager : public Process
{
public:
  Averager( AimsData<float> & d, const string & dr, const vector<unsigned> & n );
  string type() const { return( _type ); }

  template<class T>
  bool average( T & dat, const string & filename, Finder & finder );

private:
  AimsData<float>	& data;
  string		_type;
  bool			first;
  string		dir;
  vector<unsigned>	num;
};


template<class T>
bool doit( Process & p, const string & filename, Finder & finder )
{
  Averager	& ap = (Averager &) p;
  T		dat;
  return( ap.average( dat, filename, finder ) );
}


template<class T> bool
Averager::average( T & dat, const string & filename, Finder & finder )
{
  string	format = finder.format();
  Reader<T>	r( filename );

  _type.erase();
  r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly, 
                                           DataSource::none(), false, 0.01 ) );
  cout << "reading " << filename << "...\n";
  if( !r.read( dat, 0, &format ) )
    return( false );

  _type = finder.dataType();

  int	dx = dat.dimX(), dy = dat.dimY(), dz = dat.dimZ(), dt = dat.dimT();
  int	axis = 0;

  if( dir == "x" )
    axis = 1;
  else if( dir == "y" )
    axis = 2;
  else if( dir == "z" )
    axis = 3;
  else if( dir == "t" )
    axis = 4;

  if( first )
    {
      int	dxx = dx, dyy = dy, dzz = dz, dtt = dt;
      switch( axis )
	{
	case 1:
	  dxx = 1;
	  break;
	case 2:
	  dyy = 1;
	  break;
	case 3:
	  dzz = 1;
	  break;
	case 4:
	  dtt = 1;
	  break;
	default:
	  break;
	}
      data = AimsData<float>( dxx, dyy, dzz, dtt );
      data.setSizeXYZT( dat.sizeX(), dat.sizeY(), dat.sizeZ(), dat.sizeT() );
      data = 0;
      first = false;
    }
  else
    {
      bool		nomatch = false;
      if( axis != 1 && data.dimX() != dx )
	{
	  nomatch = true;
	  if( data.dimX() < dx )
	    dx = data.dimX();
	}
      if( axis != 2 && data.dimY() != dy )
	{
	  nomatch = true;
	  if( data.dimY() < dy )
	    dy = data.dimY();
	}
      if( axis != 3 && data.dimZ() != dz )
	{
	  nomatch = true;
	  if( data.dimZ() < dz )
	    dz = data.dimZ();
	}
      if( axis != 4 && data.dimT() != dt )
	{
	  nomatch = true;
	  if( data.dimT() < dt )
	    dt = data.dimT();
	}
      if( nomatch )
	cerr << "warning, unmatching image dimensions\n";
    }

  int		x, y, z, t, x1, y1, z1, t1, x2, y2, z2, t2, i;
  double	fac = 1;

  switch( axis )
    {
    case 1:
      if( num.empty() )
	for( i=0; i<dx; ++i )
	  num.push_back( i );
      else
	dx = num.size();
      fac = 1. / dx;
      break;
    case 2:
      if( num.empty() )
	for( i=0; i<dy; ++i )
	  num.push_back( i );
      else
	dy = num.size();
      fac = 1. / dy;
      break;
    case 3:
      if( num.empty() )
	for( i=0; i<dz; ++i )
	  num.push_back( i );
      else
	dz = num.size();
      fac = 1. / dz;
      break;
    case 4:
      if( num.empty() )
	for( i=0; i<dt; ++i )
	  num.push_back( i );
      else
	dt = num.size();
      fac = 1. / dt;
      break;
    default:
      break;
    }

  for( t=0; t<dt; ++t )
    {
      if( axis == 4 )
	{
	  t1 = num[t];
	  t2 = 0;
	}
      else
	{
	  t1 = t;
	  t2 = t;
	}
      for( z=0; z<dz; ++z )
	{
	  if( axis == 3 )
	    {
	      z1 = num[z];
	      z2 = 0;
	    }
	  else
	    {
	      z1 = z;
	      z2 = z;
	    }
	  for( y=0; y<dy; ++y )
	    {
	      if( axis == 2 )
		{
		  y1 = num[y];
		  y2 = 0;
		}
	      else
		{
		  y1 = y;
		  y2 = y;
		}
	      for( x=0; x<dx; ++x )
		{
		  if( axis == 1 )
		    {
		      x1 = num[x];
		      x2 = 0;
		    }
		  else
		    {
		      x1 = x;
		      x2 = x;
		    }
		  data( x2, y2, z2, t2 ) += fac * dat( x1, y1, z1, t1 );
		}
	    }
	}
    }

  return( true );
}


Averager::Averager( AimsData<float> & d, const string & dr, const vector<unsigned> & n ) 
    : Process(), data( d ), first( true ), dir( dr ), num( n )
{
    registerProcessType( "Volume", "S8", &doit<AimsData<int8_t> > );
    registerProcessType( "Volume", "U8", &doit<AimsData<uint8_t> > );
    registerProcessType( "Volume", "S16", &doit<AimsData<int16_t> > );
    registerProcessType( "Volume", "U16", &doit<AimsData<uint16_t> > );
    registerProcessType( "Volume", "S32", &doit<AimsData<int32_t> > );
    registerProcessType( "Volume", "U32", &doit<AimsData<uint32_t> > );
    registerProcessType( "Volume", "FLOAT", &doit<AimsData<float> > );
    registerProcessType( "Volume", "DOUBLE", &doit<AimsData<double> > );
}


template<class T>
static bool saver( Process & p, const string & filename, Finder & finder );


class Saver : public Process
{
public:
  Saver( AimsData<float> & d );

  template<class T>
  friend bool saver( Process & p, const string & filename, Finder & finder );

private:
  AimsData<float>	& data;
};


template<class T>
bool saver( Process & p, const string & filename, Finder & )
{
  Saver				& sp = (Saver &) p;
  ShallowConverter<AimsData<float>, AimsData<T> >	conv;
  AimsData<T>			*d2 = conv( sp.data );
  Writer<AimsData<T> >		w( filename );

  bool	res = w.write( *d2 );
  delete d2;
  return( res );
}


Saver::Saver( AimsData<float> & d ) : Process(), data( d )
{
    registerProcessType( "Volume", "S8", &saver<int8_t> );
    registerProcessType( "Volume", "U8", &saver<uint8_t> );
    registerProcessType( "Volume", "S16", &saver<int16_t> );
    registerProcessType( "Volume", "U16", &saver<uint16_t> );
    registerProcessType( "Volume", "S32", &saver<int32_t> );
    registerProcessType( "Volume", "U32", &saver<uint32_t> );
    registerProcessType( "Volume", "FLOAT", &saver<float> );
    registerProcessType( "Volume", "DOUBLE", &saver<double> );
}


vector<unsigned> parseNums( const string & str )
{
  cout << "parseNums...\n";
  vector<unsigned>	num;
  const char		*p = str.c_str(), *pe = p + str.length();
  char			c;
  string		n1, n2;
  bool			first = true;
  unsigned		un1, un2, i;

  while( p < pe )
    {
      c = *p++;
      if( c == ',' )
	{
	  //cout << "virgule\n";
	  if( n1.empty() || ( !first && n2.empty() ) )
	    throw runtime_error( "Invalid options" );
	  sscanf( n1.c_str(), "%d", &un1 );
	  //cout << "n1 : " << un1 << endl;
	  if( !first )
	    {
	      //cout << "read n2\n";
	      sscanf( n2.c_str(), "%d", &un2 );
	      //cout << "n2 : " << un2 << endl;
	      if( un2 < un1 )
          throw runtime_error( "Invalid options" );
	    }
	  else
	    un2 = un1;
	  first = true;
	  for( i=un1; i<=un2; ++i )
	    num.push_back( i );
	  n1.erase();
	  n2.erase();
	}
      else if( c == '-' )
	{
	  //cout << "-" << endl;
	  if( !first || !n2.empty() )
      throw runtime_error( "Invalid options" );
	  first = false;
	}
      else if( c >= '0' && c <= '9' )
	{
	  //cout << "chiffre : " << c << "\n";
	  if( first )
	    n1 += c;
	  else
	    n2 += c;
	}
      else
        throw runtime_error( "Invalid options" );
    }

  if( !n1.empty() )
    {
      sscanf( n1.c_str(), "%d", &un1 );
      if( !first )
	{
	  sscanf( n2.c_str(), "%d", &un2 );
	  if( un2 < un1 )
      throw runtime_error( "Invalid options" );
	}
      else
	un2 = un1;
      for( i=un1; i<=un2; ++i )
	num.push_back( i );
    }

  cout << "slices numbers :\n";
  for( i=0; i<num.size(); ++i )
    cout << num[i] << " ";
  cout << endl;
  return( num );
}


int main(int argc, const char **argv)
{
  string 		fileout;
  vector<string>	filein;
  string		type;
  string		dir, nums;

  AimsApplication	app( argc, argv, "Averages several volume files or several "
                       "images / slices of a file" );

  app.addOption( fileout, "-o", "output data" );
  app.addOption( type, "-t", "output data type (S16, U8, FLOAT, ...)", true );
  app.addOption( dir, "-d", "if specified, perform averaging inside "
                 "each file along this direction (x, y, z or t)", true );
  app.addOption( nums, "-n", "if a direction is specified, gives the "
                 "volume or slice numbers to process. ex: 2-20,24,26 "
                 "(no space between numbers)", true );
  app.addOptionSeries( filein, "-i", "input volumes", 1 );
  app.alias( "--output", "-o" );
  app.alias( "--type", "-t" );
  app.alias( "--dir", "-d" );
  app.alias( "--num", "-n" );
  app.alias( "--input", "-i" );

  try
    {
      app.initialize();

      if( !dir.empty() && dir != "x" && dir != "y" && dir != "z" 
          && dir != "t" )
        throw runtime_error( "Bad direction" );
      if( !nums.empty() && dir.empty() )
        throw runtime_error( "must use -n with -d option" );

      vector<unsigned>	num;
      if( !dir.empty() )
        num = parseNums( nums );

      AimsData<float>	aver;
      Averager		proc( aver, dir, num );
      unsigned		i, n = filein.size();

      for( i=0; i<n; ++i )
        {
          if( !proc.execute( filein[i] ) )
            {
              cerr << "Couldn't read " << filein[i] << "\n";
              return( 1 );
            }
          if( type.empty() )
            type = proc.type();
        }

      int		x, y, z, t;
      ForEach4d( aver, x, y, z, t )
        aver( x, y, z, t ) /= n;

      if( type.empty() )
        type = proc.type();

      Finder	f;
      f.setObjectType( "Volume" );
      f.setDataType( type );
      Saver		proc2( aver );
      if( !proc2.execute( f, fileout ) )
        {
          cerr << "couldn't save " << fileout << endl;
          return( 1 );
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
