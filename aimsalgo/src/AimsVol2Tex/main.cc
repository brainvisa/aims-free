/*
 *  Copyright (C) 1999-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *  	CEA/DSV/SHFJ
 *  	4 place du General Leclerc
 *  	91401 Orsay cedex
 *  	France
 */

#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/getopt/getopt2.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>

#include <iostream>
#include <cartobase/config/verbose.h>
#include <cfloat>

using namespace aims;
using namespace carto;
using namespace std;

template<class T> 
static bool doit( Process &p, const string &filename, Finder & finder);

class VoxelMapTexture : public Process
{
public:
  VoxelMapTexture( const string & meshfile, const string & brainfile, const string & outexfile,
                   int radius, int height, bool asciiFlag, int mod,
                   const string & volumefilep, const string & outexfilep );
  
  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  template<class T> 
  bool voxelMap( AimsData<T> & data );

private:
  string        meshf, brainf, otexf, otexfp, volfp;
  int          radius, height, mode;
  bool         ascii;
};


VoxelMapTexture::VoxelMapTexture( const string & meshfile, const string & brainfile,  const string & outexfile, 
                            int rad, int hei, bool asciiFlag, int mod, const string & volumefilep, const string & outexfilep ) 
  	: Process(), meshf( meshfile ), brainf(brainfile), otexf( outexfile ),
          radius( rad ), height( hei ), ascii( asciiFlag ), mode( mod ),
          volfp( volumefilep ), otexfp( outexfilep )
   {
    registerProcessType( "Volume", "S8", &doit<int8_t> );
    registerProcessType( "Volume", "U8", &doit<uint8_t> );
    registerProcessType( "Volume", "S16", &doit<int16_t> );
    registerProcessType( "Volume", "U16", &doit<uint16_t> );
    registerProcessType( "Volume", "S32", &doit<int32_t> );
    registerProcessType( "Volume", "U32", &doit<uint32_t> );
    registerProcessType( "Volume", "FLOAT", &doit<float> );
}


template<class T> 
bool doit( Process & p, const string & fname, Finder & f)
{
  if( verbose )
    cout << "Starting doit()" << endl;
  VoxelMapTexture               & tp = (VoxelMapTexture &) p;
  AimsData<T>		data;
  Reader<AimsData<T> >	reader( fname );
  string		format = f.format();
  if( !reader.read( data, 0, &format) )
    return( false );

  return( tp.voxelMap( data ) );
} 


template<class T> 
bool VoxelMapTexture::voxelMap( AimsData<T> & data )
{
  /* Loading input */
  if(verbose) {
    cout << "Starting VoxelMapTexture()" << endl;
  	 cout << "Reading mesh...";
  }
  Reader<AimsSurfaceTriangle>	rmesh( meshf );
  AimsSurfaceTriangle		mesh;
  if( !rmesh.read( mesh ) )
    return( false );
  if(verbose)
  	cout << " done.\n";

  if(verbose)
     cout << "Reading brain mask...";
  AimsData<short>               brainData;
  bool BRAIN_MASK = 0;
  if( brainf.length() ) {
      Reader<AimsData<short> >      reader3( brainf );
      if(!reader3.read( brainData, 0) )
        return(false);
      BRAIN_MASK = 1;
  }
  if(verbose)
        cout << " done.\n";

  if(verbose)
     cout << "Reading parallel brain...";
  AimsData<float>               datap(0, 0, 0, 0);
  if( !volfp.empty() ) {
      Reader<AimsData<float> >      readerp( volfp );
      if(!readerp.read( datap, 0) )
        return(false);
  }
  if(verbose)
        cout << " done.\n";

  TimeTexture<float>             otex, otexp;  /* NEW VAR */

  /***
  Algorithm
  ***/

  set<size_t>                           instants;
  AimsSurfaceTriangle::iterator         itm, etm = mesh.end();

  //    count mesh times
  for( itm=mesh.begin(); itm!=etm; ++itm )
    instants.insert( (*itm).first );
  //    add remaining volume times
  size_t                                vt, evt = data.dimT();

  for( vt = *instants.rbegin() + 1; vt<evt; ++vt )
    instants.insert( vt );

  set<size_t>::iterator             ins, endt = instants.end();
  unsigned                          p;
  int                               x, y, z, i, j, k, kmax, xmin, ymin, zmin, nval;
  float                             val=0, meanVal, mval, Mval, Fval;
  float                             valp=0, meanValp, mvalp, Mvalp;

  for( ins=instants.begin(); ins!=endt; ++ins )
    {
      size_t                             t;

      itm = mesh.lower_bound( *ins );
      if( itm == etm )
        t = (*mesh.rbegin()).first;
      else
        t = (*itm).first;

      t = *ins;
      if( t >= (size_t) data.dimT() )
        t = (size_t) ( data.dimT() - 1 );

      const AimsSurface<3,Void>         & surface = mesh[t];
      const vector<Point3df>            & vert = surface.vertex();
      const vector<Point3df>            & u = surface.normal();
      unsigned                         n = vert.size();
      vector<Point3df>                  v = vector<Point3df>( n );
      vector<Point3df>                  w = vector<Point3df>( n );
      
      if( verbose )
        cout << "Computing sets of normal vectors at each vertex" << endl;

      for( p=0; p<n; ++p )
        {
          // cout << i << ' ' << flush;
          if( u[p][0]!=1 && u[p][0]!=-1 ) /* main case */
            {
                w[p][0] = (float) sqrt( 1 - u[p][0]*u[p][0] );
                w[p][1] = -u[p][0] * u[p][1] / w[p][0];
                w[p][2] = -u[p][0] * u[p][2] / w[p][0];

                v[p][0] = 0;
                v[p][1] = -u[p][2] / w[p][0];
                v[p][2] =  u[p][1] / w[p][0];
            }
          else /* u[p][1] = u[p][2] = 0 */
            {
                v[p][1] = 1;
                v[p][0] = v[p][2] = 0;
                w[p][2] = 1;
                w[p][0] = w[p][1] = 0;
            }
        }

      if( verbose ) {
          cout << "Computing texture, radius " << radius << " height " << height  << endl;
      }

      otex.reserve( n );
      if(!datap.empty())
        otexp.reserve( n );

      for( p=0; p<n; p++ ) 
      {
        // voxel initialization
        nval = 0;
        mval = FLT_MAX;
        Mval = -FLT_MAX;
        meanVal=0;
        meanValp=0;

        // cylinder (radius, 2*height+1)
        for( i=-height; i<=height; i++ )
            {
              for( j=-radius; j<=radius; j++)
                {
                  kmax = (short) (sqrt(radius*radius-j*j)+.5);
                  for( k=-kmax; k<=kmax; k++ )
                    {
                      x = round(vert[p][0]/data.sizeX() + i*u[p][0] + j*v[p][0] + k*w[p][0]);
                      y = round(vert[p][1]/data.sizeY() + i*u[p][1] + j*v[p][1] + k*w[p][1]);
                      z = round(vert[p][2]/data.sizeZ() + i*u[p][2] + j*v[p][2] + k*w[p][2]);
                      if(x<data.dimX() && y<data.dimY() && z<data.dimZ())
                      {
                        val = (float) data( x, y, z, t );
                        if( val!=0 && (!BRAIN_MASK || brainData(x, y, z, t)))
                          {
                          meanVal=meanVal+val;
                          nval=nval+1;
                          if(!datap.empty()) {
                            valp = (float) datap( x, y, z, t );
                            meanValp += valp;
                          }
                          if( mode==2 )
                            {
                              if( mval>val )
                                {
                                  mval=val;
                                  mvalp=valp;
                                }
                            }
                            else if( mode==3 )
                            {
                              if( Mval<val )
                                {
                                  Mval=val;
                                  Mvalp=valp;
                                }
                            }
                          }
                      }
                    }
                }
            }
        if( nval>0 )
        {
          meanVal=meanVal/nval;
          meanValp=meanValp/nval;
        }
        if( mode==1 )
        {
          otex.push_back( meanVal );
          if(!datap.empty())
            otexp.push_back( meanValp );
        }
        else if( mode==2 )
        {
          if( mval==FLT_MAX )
          {
            mval=meanVal;
            mvalp=meanValp;
          }
          otex.push_back( mval );
          if(!datap.empty())
            otexp.push_back( mvalp );
        }
        else if( mode==3 )
        {
          if( Mval==-FLT_MAX )
          {
            Mval=meanVal;
            Mvalp=meanValp;
          }
          otex.push_back( Mval );
          if(!datap.empty())
            otexp.push_back( Mvalp );
        }
      }      
    }

  /* Writing ouput */
  if(verbose)
    cout << "writing texture(s)..." << endl;
  Writer<TimeTexture<float> >   w2( otexf );
  if( !w2.write( otex, ascii ) )
    return( false );

  if(!datap.empty()) {
    Writer<TimeTexture<float> >   w3( otexfp );
    if( !w3.write( otexp, ascii ) )
      return( false );
  }

  if(verbose)
    cout << "End of the process." << endl;
  return( true );
}


int main( int argc, const char** argv )
{
  string			                 volumefile, outexfile;
  string                                         volumefilep, outexfilep;
  Reader<AimsSurfaceTriangle>   meshfile;
  string                        brainfile="";
  int                          radius=1, height=1, mod = 1;
  bool                         asciiFlag = false;

  AimsApplication	app( argc, argv, "Compute label close to mesh and build texture file" );
  app.addOption( volumefile, "-i", "input Label volume" );
  app.addOption( volumefilep, "-ip", "input parallel Label volume", true );
  app.addOption( meshfile, "-m", "input mesh" );
  app.addOption( outexfile, "-o", "texture output");
  app.addOption( outexfilep, "-op", "parallel texture output", true );
  app.addOption( height, "-height", "cylinder half height (voxels) [default=1]", 1 );
  app.addOption( radius, "-radius", "cylinder radius (voxels) [default=1]", 1 );
  app.addOption( brainfile, "-b", "brain mask file [default=no brain mask]", "");
  app.addOption( asciiFlag, "--ascii", "write texture file in ASCII [default=false]", true );
  app.addOption( mod, "-mod", "values to assign to the texture (1:mean, 2:min, 3:max)", 1);
  app.alias( "--mesh", "-m" );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );

  try
    {
      app.initialize();
      if(!volumefilep.empty() && outexfilep.empty()) {
        cerr << "-op must be specified if -ip is given" << endl;
        return 2;  // usage error
      }
      if( verbose )
        cout << "Init program" << endl;
      VoxelMapTexture	proc( meshfile.fileName(), brainfile, outexfile,
                              radius, height, asciiFlag, mod,
                              volumefilep, outexfilep );
      if( verbose )
        cout << "Starting program..." << endl;      
      if( !proc.execute( volumefile ) )
      {
            cout << "Couldn't process file - aborted\n";
            return( 1 );
      }
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
