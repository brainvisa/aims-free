/*
 *  Copyright (C) 1999-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *  	CEA/DSV/SHFJ
 *  	4 place du General Leclerc
 *  	91401 Orsay cedex
 *  	France
 */ 

//#include <cstdlib>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
//#include <aims/math/math_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/vector/vector.h>
#include <aims/mesh/texture.h>
//#include <aims/utility/utility_g.h>

#include <iostream>
#include <limits>
#include <cartobase/config/verbose.h>

using namespace aims;
using namespace carto;
using namespace std;

template<class T> 
static bool doit( Process &p, const string &filename, Finder & finder);

class CxParcel: public Process
{
public:
  CxParcel( const string & meshfile, const string & intexfile,const string & outputfile, 
	    int gm_value );
  
  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  template<class T> 
  bool parcellation( AimsData<T> & data );

private:
  string        meshf, itexf, parcelf;
  int           gm_value;
};

CxParcel::CxParcel( const string & meshfile, const string & intexfile, const string & outputfile,
		    int gm) 
  	: Process(), meshf( meshfile ), itexf( intexfile ), parcelf(outputfile), gm_value( gm )
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
  CxParcel              & cx = (CxParcel&) p;
  AimsData<T>		data;
  Reader<AimsData<T> >	reader( fname );
  string		format = f.format();
  if( !reader.read( data, 0, &format) )
    return( false );
  return( cx.parcellation( data ) );
} 


template<class T> 
bool CxParcel::parcellation( AimsData<T> & gmdata )
{
  unsigned                          p, pmin;
  int                               x, y, z, xmax, ymax, zmax;
  long                              g, gmax;
  float                             sx, sy, sz;
  double                            d2, d2min;
  xmax = gmdata.dimX(); ymax = gmdata.dimY(); zmax = gmdata.dimZ();
  sx = gmdata.sizeX(); sy = gmdata.sizeY(); sz = gmdata.sizeZ();
  
  /* Loading input */
  if(verbose) {
      cout << "Starting CxParcel()" << endl;
  	cout << "reading mesh...";
  }
  Reader<AimsSurfaceTriangle>	rmesh( meshf );
  AimsSurfaceTriangle		mesh;
  if( !rmesh.read( mesh ) )
    return( false );
  if(verbose)
  	cout << "done.\n";

  if(verbose)
        cout << "reading input texture file...";
  TimeTexture<float>             itex;
  if( itexf.length() ) {
      Reader<TimeTexture<float> >    reader1(itexf);
      if( !reader1.read( itex, 0) )
            return(false);
  }
  if(verbose)
        cout << "done.\n";
  
  AimsData<short> parcel (xmax, ymax, zmax);
  parcel.setSizeXYZT(sx, sy, sz);
  if( parcelf .length() ==0 )
  {
    cerr<<"ERROR: output parcellation file has not been defined!" << flush;
    return(false);
  }    
  Writer< AimsData< short > > w( parcelf );

  /***
  Algorithm
  ***/
  const AimsSurface<3,Void>         & surface = mesh[0];
  const vector<Point3df>            & vert = surface.vertex();
  unsigned                          pmax = vert.size();
  vector<Point3df>                  gm = vector<Point3df>();

  if( verbose )
    cout << "Computing 3D cortical locations (in mm)" << endl;

  // number of cortical voxels
  gmax=0;
  for(z=0; z<zmax; z++)
    for(y=0; y<ymax; y++)
      for(x=0; x<xmax; x++)
        if(gmdata(x,y,z) == static_cast<T>(gm_value))
	       gmax++;
  gm.reserve(gmax);

  g=0;
  for(z=0; z<zmax; z++)
     for(y=0; y<ymax; y++)
       for(x=0; x<xmax; x++)
         if(gmdata(x,y,z) == static_cast<T>(gm_value))
         {
            gm[g][0]=x*sx;
            gm[g][1]=y*sy;
            gm[g][2]=z*sz;
            g++;
         }

  if( verbose )
    cout << "Computing closest vertex location" << endl;

  for( g=0; g<gmax; g++ )
  {
    if( g / (gmax/10) ==0)
      cout << g % (gmax/10) << "0 pct done" << endl;
    
    // searching for the closest vertex
    d2min = 1000.;
    pmin = numeric_limits<unsigned>::max();
    for( p=0; p<pmax; p++)
    {
      d2 = (vert[p][0]-gm[g][0])*(vert[p][0]-gm[g][0]) 
         + (vert[p][1]-gm[g][1])*(vert[p][1]-gm[g][1]) 
         + (vert[p][2]-gm[g][2])*(vert[p][2]-gm[g][2]);
      if( d2 < d2min )
      {  
        d2min = d2;
        pmin = p;
      }
    }
    if(pmin != numeric_limits<unsigned>::max())
    {
      x=(int)round(gm[g][0]/sx);
      y=(int)round(gm[g][1]/sy);
      z=(int)round(gm[g][2]/sz);
      parcel(x,y,z,0) = (short int)(round) (itex.item(pmin));
    }
    else
      printf("ERROR: could not find any single vertex closer than 1m to (%.0f, %.0f, %.f)!\n",
	     gm[g][0]/sx,gm[g][1]/sy,gm[g][2]/sz);
  }

  /* Writing ouput */
  if(verbose)
    cout << "Writing debug...";
  w.write( parcel );  
  
  if(verbose)
    cout << "End of the process." << endl;
  return( true );
}


int main( int argc, const char** argv )
{
  string			gmvolumefile;
  Reader<AimsSurfaceTriangle>   meshfile;
  string                        intexfile, outputfile;
  int                           gm_value=0;

  AimsApplication	app( argc, argv, "Compute cortical parcellation" );
  app.addOption( gmvolumefile, "-i", "Cortical segmentation" );
  app.addOption( meshfile, "-m", "input sulcal mesh" );
  app.addOption( intexfile, "-t", "input segmentation texture");
  app.addOption( gm_value, "-g", "cortical label [default=0]", 0);
  app.addOption( outputfile, "-o", "output cortical parcellation");
  app.alias( "--mesh", "-m" );
  app.alias( "--input", "-i" );
  app.alias( "--texture", "-t" );
  app.alias( "--output", "-o" );

  try
    {
	app.initialize();
	if( verbose )
	  cout << "Init program" << endl;
	CxParcel	proc( meshfile.fileName(), intexfile, outputfile, gm_value);
	if( verbose )
	  cout << "Starting program.." << endl;      
	if( !proc.execute( gmvolumefile ) )
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
