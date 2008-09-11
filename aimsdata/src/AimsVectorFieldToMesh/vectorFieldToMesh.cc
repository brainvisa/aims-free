/* Copyright (c) 1995-2005 CEA
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

#include <aims/data/data.h>
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/mesh/surfaceOperation.h>
#include <aims/mesh/surfacegen.h>
#include <aims/math/mathelem.h>
#include <aims/rgb/rgb.h>
#include <cartobase/exception/ioexcept.h>

using namespace aims;
using namespace carto;
using namespace std;


class VF2Mesh : public Process
{
public:
  VF2Mesh( const string & out , float s);
  virtual ~VF2Mesh();
  template<typename T> static bool doit( Process & p, const string & filename, 
                                         Finder & finder );

  string	fileout;
  float scale;
};


VF2Mesh::VF2Mesh( const string & out , float s)
  : Process(), fileout( out ), scale ( s )
{
  registerProcessType( "Volume", "S16", &doit<short> );
  registerProcessType( "Volume", "FLOAT", &doit<float> );
  registerProcessType( "Volume", "DOUBLE", &doit<double> );
  registerProcessType( "Volume", "RGB", &doit<AimsRGB> );
  registerProcessType( "Volume", "POINT3DF", &doit<Point3df> );
  registerProcessType( "Volume", "VECTOR_OF_3_S16", &doit<Point3d> );
}


VF2Mesh::~VF2Mesh()
{
}


template<typename T> void meshit( const AimsData<T> & data, 
                                  AimsSurfaceTriangle & mesh, float vsm, 
                                  const vector<float> & vs , float s)
{
  int			x, y, z;
  Point3df		pos, dir;
  float			size, msz = 0;

  if(s<=0)
  {
		cout<<"Vectors are normalised"<<endl;
  		ForEach3d( data, x, y, z )
    	{
      	size = sqrt( (float) sqr( data( x, y, z, 0 ) ) 
                   + sqr( data( x, y, z, 1 ) )
                   + sqr( data( x, y, z, 2 ) ) );
      	if( size > msz )
        	msz = size;
    	}
  		msz = vsm / msz;
  }

  else msz=s;

  ForEach3d( data, x, y, z )
    {
      dir = Point3df( ((float) data( x, y, z, 0 )) * msz, 
                      ((float) data( x, y, z, 1 )) * msz,
                      ((float) data( x, y, z, 2 )) * msz );
      pos = Point3df( vs[0] * x, vs[1] * y, vs[2] * z );
      SurfaceManip::meshMerge( mesh, 
                               *SurfaceGenerator::cone( pos + dir, pos, 
                                                        0.1*dir.norm(), 4, 
                                                        true ) );
    }
}


template<typename T> void meshvector( const AimsData<T> & data, 
                        AimsSurfaceTriangle & mesh, float vsm, 
                        const vector<float> & vs , float s)
{
  int			x, y, z;
  Point3df		pos, dir;
  float			size, msz = 0;
  T				col;

  if(s<=0)
  {
		cout<<"Vectors are normalised"<<endl;
  		ForEach3d( data, x, y, z )
    	{
      	col = data( x, y, z );
      	size = sqrt( (float) sqr( col[0] ) + sqr( col[1] ) + sqr( col[2] ) );
      	if( size > msz )
        	msz = size;
    	}
  		msz = vsm / msz;
  }
  else msz=s;


  ForEach3d( data, x, y, z )
    {
      col = data( x, y, z );
      dir = Point3df( ((float) col[0]) * msz, 
                      ((float) col[1]) * msz,
                      ((float) col[2]) * msz );
      pos = Point3df( vs[0] * x, vs[1] * y, vs[2] * z );
      SurfaceManip::meshMerge( mesh, 
                               *SurfaceGenerator::cone( pos + dir, pos, 
                                                        0.1*dir.norm(), 4, 
                                                        true ) );
    }
}



template<> void meshit( const AimsData<AimsRGB> & data, 
                        AimsSurfaceTriangle & mesh, float vsm, 
                        const vector<float> & vs , float s )
{
  meshvector( data, mesh, vsm, vs , s );
}


template<typename U> void meshit( const AimsData<AimsVector<U,3> > & data, 
                        AimsSurfaceTriangle & mesh, float vsm, 
                        const vector<float> & vs , float s )
{
  meshvector( data, mesh, vsm, vs , s );
}


void test_dims2( const vector<int> & dims, const string & filename )
{
  if( dims.size() < 4 || dims[3] != 3 )
    throw format_mismatch_error( "need 3 timesteps in volume", filename );
}

template<typename T> void test_dims( const vector<int> &, const string & )
{
}


template<> void test_dims<short>( const vector<int> & dims, const string & filename )
{
  test_dims2( dims, filename );
}


template<> void test_dims<float>( const vector<int> & dims, const string & filename )
{
  test_dims2( dims, filename );
}


template<> void test_dims<double>( const vector<int> & dims, const string & filename )
{
  test_dims2( dims, filename );
}


template<typename T> bool VF2Mesh::doit( Process & p, const string & filename, 
                                         Finder & finder )
{
  VF2Mesh	& proc = (VF2Mesh &) p;
  Reader<AimsData<T> >	vr( filename );
  AimsData<T>		data;
  vector<int>		dims;
  vector<float>		vs;
  const PythonHeader 
    *ph = dynamic_cast<const PythonHeader *>( finder.header() );

  if( !ph )
    throw format_mismatch_error( "no header in image", filename );

  ph->getProperty( "volume_dimension", dims );
  test_dims<T>( dims, filename );
  ph->getProperty( "voxel_size", vs );
  float	vsm = 1;
  if( vs.size() > 0 )
    vsm = vs[0];
  else
    vs.push_back( 1 );
  if( vs.size() > 1 && vs[1] < vsm )
    vsm = vs[1];
  else
    vs.push_back( 1 );
  if( vs.size() > 2 && vs[2] < vsm )
    vsm = vs[2];
  else
    vs.push_back( 1 );

  vsm *= 0.7;

  string	f = finder.format();
  vr.read( data, 0, &f );

  cout << "generating mesh...\n";
  AimsSurfaceTriangle	mesh;
  meshit( data, mesh, vsm, vs , proc.scale);

  cout << "writing mesh...\n";
  Writer<AimsSurfaceTriangle>	mw( proc.fileout );
  mw.write( mesh );
  cout << "done.\n";
  return true;
}


int main( int argc, const char **argv )
{
  try
    {
      string	filein, fileout;
		float scale=-1;
      AimsApplication	app( argc, argv, "Converts a vector field volume to " 
                             "a mesh of arrows" );
      app.addOption( filein, "-i", "input vector field (either one RGB " 
                     "image or a 4D image with 3 components)" );
      app.addOption( fileout, "-o", "output mesh" );
      app.addOption( scale, "-s", "scale factor (default=normalization)",true );
      app.alias( "--input", "-i" );
      app.alias( "--output", "-o" );
      app.initialize();

      VF2Mesh	proc( fileout , scale );
      proc.execute( filein );
    }
  catch( user_interruption &e )
    {
    }
  catch( exception &e )
    {
      cerr << argv[ 0 ] << ": " << e.what() << endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

