/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#include <aims/distancemap/meshdistance.h>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/math/math_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
#include <aims/utility/utility_g.h>
#include <iomanip>
#include <set>

using namespace aims;
using namespace carto;
using namespace std;

typedef float float3[3];


template<class T, class U>
static bool texvol( Process &, const string &, Finder & );


class Texturer : public Process
{
public:
  Texturer( const string & meshfile, const string & texfile, bool asciif );

  template<class T, class U>
  friend bool texvol( Process &, const string &, Finder & );
  template<class T, class U>
  bool texvol_m( AimsData<T> & data, TimeTexture<U> & );

private:
  string	meshf;
  string	texf;
  bool		ascii;
};


Texturer::Texturer( const string & meshfile, const string & texfile, bool asciif ) 
  : Process(), meshf( meshfile ), texf( texfile ), ascii( asciif )
{
  registerProcessType( "Volume", "S8", texvol<int8_t,float> );
  registerProcessType( "Volume", "U8", texvol<uint8_t,float> );
  registerProcessType( "Volume", "S16", texvol<int16_t,float> );
  registerProcessType( "Volume", "U16", texvol<uint16_t,float> );
  registerProcessType( "Volume", "S32", texvol<int32_t,float> );
  registerProcessType( "Volume", "U32", texvol<uint32_t,float> );
  registerProcessType( "Volume", "FLOAT", texvol<float,float> );
  registerProcessType( "Volume", "DOUBLE", texvol<double,float> );
  registerProcessType( "Volume", "CFLOAT", texvol<cfloat,Point2df> );
  registerProcessType( "Volume", "CDOUBLE", texvol<cdouble,Point2df> );
}



template<class T, class U>
bool texvol( Process & p, const string & fname, Finder & f )
{
  Texturer		& tp = (Texturer &) p;
  cout << "reading volume...";
  Reader<AimsData<T> >	reader( fname );
  AimsData<T>		data;
  string		format = f.format();
  if( !reader.read( data, 0, &format ) )
    return( false );
  cout << "done.\n";

  TimeTexture<U>			tex;

  return( tp.texvol_m( data, tex ) );
}


template<class U> class Conv
{
public:
  template<class T> U conv( const T & x )
  {
    return( (U) x );
  }
};

template<> class Conv<Point2df>
{
public:
  template<class T> Point2df conv( const std::complex<T> & x )
  {
    return( Point2df( x.real(), x.imag() ) );
  }
};


template<class T, class U> bool
Texturer::texvol_m( AimsData<T> & data, TimeTexture<U> & tex )
{
  cout << "reading mesh...\n";
  Reader<AimsSurfaceTriangle>	r( meshf );
  AimsSurfaceTriangle		mesh;
  if( !r.read( mesh ) )
    return( false );
  cout << "done.\n";

  set<size_t>				instants;
  AimsSurfaceTriangle::iterator		itm, etm = mesh.end();

  //	count mesh times
  for( itm=mesh.begin(); itm!=etm; ++itm )
    instants.insert( (*itm).first );
  //	add remaining vomule times
  size_t				vt, evt = data.dimT();
  for( vt = *instants.rbegin() + 1; vt<evt; ++vt )
    instants.insert( vt );

  set<size_t>::iterator			ins, endt = instants.end();
  size_t				t;
  Conv<U>				c;

  for( ins=instants.begin(); ins!=endt; ++ins )
    {
      itm = mesh.lower_bound( *ins );
      if( itm == etm )
	t = (*mesh.rbegin()).first;
      else
	t = (*itm).first;
      const AimsSurface<3,Void>		& surface = mesh[t];
      const vector<Point3df>		& vert = surface.vertex();
      const vector<Point3df>		& norm = surface.normal();
      unsigned				i, n = vert.size();
      int x,y,z;
      float depla=1.;

      t = *ins;
      if( t >= (size_t) data.dimT() )
	t = (size_t) ( data.dimT() - 1 );

      Texture<U>	& texi = tex[ *ins ];
      texi.reserve( n );

      // This could be done without any conversion towards float volume...
      cout << "Computing texture, time " << *ins << "..." << endl;
      for( i=0; i<n; ++i )
	{
	  // cout << i << ' ' << flush;
	  x = (int)floor((vert[i][0]+norm[i][0]*depla) / data.sizeX() + 0.5);
	  y = (int)floor((vert[i][1]+norm[i][1]*depla) / data.sizeY() + 0.5);
	  z = (int)floor((vert[i][2]+norm[i][2]*depla) / data.sizeZ() + 0.5);
	  // test!
	  // cout << x << ' ' << y << ' ' << z << ' ' <<vol(x,y,z) << endl;
	  tex.push_back( c.conv( data( x, y, z, t ) ) );
	}
    }

  cout << "done.\nwriting texture...\n";

  Writer<TimeTexture<U> >	w( texf );
  if( !w.write( tex, ascii ) )
    return( false );
  cout << "done.\n";
  return( true );
}


int main( int argc, const char** argv )
{
  Reader<AimsSurfaceTriangle>	meshfile;
  string			volumefile;
  bool 				asciiFlag = false;
  string			texfile;

  AimsApplication	app( argc, argv, "Map a volume on the mesh in the " 
                             "normal direction" );
  app.addOption( meshfile, "-m", "input_mesh" );
  app.addOption( volumefile, "-v", "input_volume" );
  app.addOption( texfile, "-t", "output_texture" );
  app.addOption( asciiFlag, "--ascii", 
                 "write texture file in ASCII [default=false]", true );
  app.alias( "--mesh", "-m" );
  app.alias( "--volume", "-v" );
  app.alias( "--texture", "-t" );
  app.alias( "-o", "-t" );

  try
    {
      app.initialize();
      Texturer	proc( meshfile.fileName(), texfile, asciiFlag );
      if( proc.execute( volumefile ) )
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
