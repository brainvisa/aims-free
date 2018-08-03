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

using namespace aims;
using namespace carto;
using namespace std;

template<class T> 
static bool doit( Process &p, const string &filename, Finder & finder);

class LabelMapTexture : public Process
{
public:
  LabelMapTexture( const string & meshfile, const string & brainfile, const string & outexfile,
                int radius, int height, int mode, bool asciiFlag );
  
  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  template<class T> 
  bool labelMap( AimsData<T> & data );

private:
  string        meshf, brainf, otexf;
  int          radius, height, mode;
  bool         ascii;
};

LabelMapTexture::LabelMapTexture( const string & meshfile, const string & brainfile,  const string & outexfile, 
                            int rad, int hei, int mod, bool asciiFlag) 
  	: Process(), meshf( meshfile ), brainf(brainfile), otexf( outexfile ), radius( rad ), height( hei ), mode( mod ), ascii( asciiFlag )
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
  LabelMapTexture               & tp = (LabelMapTexture &) p;
  AimsData<T>		data;
  Reader<AimsData<T> >	reader( fname );
  string		format = f.format();
  if( !reader.read( data, 0, &format) )
    return( false );
  return( tp.labelMap( data ) );
} 


template<class T> 
bool LabelMapTexture::labelMap( AimsData<T> & data )
{
  /* Loading input */
  if(verbose) {
    cout << "Starting LabelMapTexture()" << endl;
  	 cout << "reading mesh...";
  }
  Reader<AimsSurfaceTriangle>	rmesh( meshf );
  AimsSurfaceTriangle		mesh;
  if( !rmesh.read( mesh ) )
    return( false );
  if(verbose)
  	cout << "done.\n";

  if(verbose)
     cout << "reading brain mask...";
  AimsData<short>               brainData;
  bool BRAIN_MASK = 0;
  if( brainf.length() ) {
      Reader<AimsData<short> >      reader3( brainf );
      if(!reader3.read( brainData, 0) )
        return(false);
      BRAIN_MASK = 1;
  }
  if(verbose)
        cout << "done.\n";
  
  TimeTexture<float>             otex;

  /***
  Algorithm
  ***/

  set<size_t>                           instants;
  AimsSurfaceTriangle::iterator         itm, etm = mesh.end();

  //    count mesh times
  for( itm=mesh.begin(); itm!=etm; ++itm )
    instants.insert( (*itm).first );
  //    add remaining vomule times
  size_t                                vt, evt = data.dimT();
  for( vt = *instants.rbegin() + 1; vt<evt; ++vt )
    instants.insert( vt );

  set<size_t>::iterator             ins, endt = instants.end();
  unsigned                         p;
  int                               x, y, z, i, j, k, kmax, min_cyl, max_cyl;
  short                             val=0, nlabel, nlabel_max, label_max, cur_label;
  vector< short >::iterator         lab, endlab;
  vector< short >                   label;

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
      unsigned                          n = vert.size();
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
          cout << "Computing texture, radius " << radius << " height " << height  << " mode " << mode << endl;
      }

      otex.reserve( n );

      for( p=0; p<n; p++ ) 
      {
        // label initialization
        label.clear();

        // cylinder (radius, alpha*height+1)
	switch ( mode )
      	{
         case 1:
            min_cyl = -height;
	    max_cyl = 0;
            break;
         case 2:
            min_cyl = 0;
	    max_cyl = height;
            break;
         case 3:
            min_cyl = -height;
	    max_cyl = height;
	 default:
            min_cyl = -height;
	    max_cyl = height;
      	}
        for( i=min_cyl; i<=max_cyl; i++ )
            {
              for( j=-radius; j<=radius; j++)
                {
                  kmax = (short) (sqrt(radius*radius-j*j)+.5);
                  for( k=-kmax; k<=kmax; k++ )
                    {
                      x = round(vert[p][0]/data.sizeX() + i*u[p][0] + j*v[p][0] + k*w[p][0]);
                      y = round(vert[p][1]/data.sizeY() + i*u[p][1] + j*v[p][1] + k*w[p][1]);
                      z = round(vert[p][2]/data.sizeZ() + i*u[p][2] + j*v[p][2] + k*w[p][2]);
                      if(x >= 0 && x<data.dimX() && y >= 0 && y<data.dimY()
                         && z >= 0 && z<data.dimZ())
                      {
                        val = (short) data( x, y, z, t );
                        if( val>0 && (!BRAIN_MASK || brainData(x, y, z, t)))
                          label.push_back( val );
                      }
                    }
                }
            }
        if( !label.empty() )
        {
           sort(label.begin(), label.end());
           cur_label=-1;
           nlabel=0;
           nlabel_max=0;
           endlab = label.end();
           label_max = 0;
           for(lab=label.begin(); lab!=endlab; ++lab )
           {
             if( *lab != cur_label ) 
             { 
                if( nlabel > nlabel_max )
                {  // the largest label
                   nlabel_max = nlabel;
                   label_max = cur_label; 
                }
                // new label
                cur_label = *lab;
                nlabel = 1;
             }
             else
               nlabel ++;
           }

           if( nlabel > nlabel_max )
           {  // the largest label
              nlabel_max = nlabel;
              label_max = cur_label;
           }
           otex.push_back( label_max );
        }
        else
           otex.push_back( 0 );
      }
    }

  /* Writing ouput */
  if(verbose)
    cout << "writing texture..." << endl;
  Writer<TimeTexture<float> >   w2( otexf );
  if( !w2.write( otex, ascii ) )
    return( false );

  if(verbose)
    cout << "End of the process." << endl;
  return( true );
}


int main( int argc, const char** argv )
{
  string			                 volumefile, outexfile;
  Reader<AimsSurfaceTriangle>   meshfile;
  string                        brainfile="";
  int                          radius=1, height=1, mode=3;
  bool                         asciiFlag = false;

  AimsApplication	app( argc, argv, "Compute label close to mesh and build texture file" );
  app.addOption( volumefile, "-i", "input Label volume" );
  app.addOption( meshfile, "-m", "input mesh" );
  app.addOption( outexfile, "-o", "texture output");
  app.addOption( height, "-height", "cylinder half height (voxels) [default=1]", 1 );
  app.addOption( radius, "-radius", "cylinder radius (voxels) [default=1]", 1 );
  app.addOption( mode, "-mode", "cylinder direction (1:interior, 2:exterior, 3:both) [default=3]", 1 );
  app.addOption( brainfile, "-b", "brain mask file [default=no brain mask]", "");
  app.addOption( asciiFlag, "--ascii", "write texture file in ASCII [default=false]", true );
  app.alias( "--mesh", "-m" );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );

  try
    {
      app.initialize();
      if( verbose )
        cout << "Init program" << endl;
      LabelMapTexture	proc( meshfile.fileName(), brainfile, outexfile, radius, height, mode, asciiFlag );
      if( verbose )
        cout << "Starting program.." << endl;      
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
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
