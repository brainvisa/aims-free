/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#include <aims/data/data_g.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/mesh/surface.h>

using namespace aims;   
using namespace carto;
using namespace std;

class ElevationMap : public Process
{
public:
  ElevationMap();
  template <typename T> static 
  bool doit( Process &, const std::string &, Finder & );

  float scale;
  bool doInverse;
  Writer<AimsSurfaceTriangle>	outWriter1, outWriter2, outWriter3;
};


ElevationMap::ElevationMap() : scale( 1. ), doInverse( false )
{
  registerProcessType( "Volume", "S8", &ElevationMap::doit<int8_t> );
  registerProcessType( "Volume", "U8", &ElevationMap::doit<uint8_t> );
  registerProcessType( "Volume", "S16", &ElevationMap::doit<int16_t> );
  registerProcessType( "Volume", "U16", &ElevationMap::doit<uint16_t> );
  registerProcessType( "Volume", "S32", &ElevationMap::doit<int32_t> );
  registerProcessType( "Volume", "U32", &ElevationMap::doit<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &ElevationMap::doit<float> );
  registerProcessType( "Volume", "DOUBLE", &ElevationMap::doit<double> );
}


template <typename T>
bool ElevationMap::doit( Process & p, const std::string & filename, 
                         Finder & finder )
{
  AimsData<T> dataIn;
  Reader< AimsData<T> > fileIn( filename );
  fileIn.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly, 
                                                DataSource::none(), 
                                                false, 0.01 ) );
  string		format = finder.format();
  if( !fileIn.read( dataIn, 0, &format ) )
    return false;

  ElevationMap	& em = (ElevationMap &) p;
  //   Surface correspondant a l image    
  AimsSurfaceTriangle		surf;
  AimsSurface<3,Void>		& s = surf[0];
  vector<Point3df>		& vert = s.vertex();
  vector< AimsVector<uint,3> >	& poly = s.polygon();

  //   Surface correspondant a l image aplatie (pour generer la texture ds anatomist)    
  AimsSurfaceTriangle		surf_flat;
  AimsSurface<3,Void>		& s_flat = surf_flat[0];
  vector<Point3df>		& vert_flat = s_flat.vertex();
  vector< AimsVector<uint,3> >	& poly_flat = s_flat.polygon();

  //   Surface correspondant aux bords de l image    
  AimsSurfaceTriangle		surf_border;
  AimsSurface<3,Void>		& s_border = surf_border[0];
  vector<Point3df>		& vert_border = s_border.vertex();
  vector< AimsVector<uint,3> >	& poly_border = s_border.polygon();

  Point3df v;
  AimsVector<uint, 3> p1, p2;
    
  int x, y;
  int nb_vertex, nb_polygon;
  int nb_vertex_border, nb_polygon_border;
  int dim_x, dim_y;
  float size_x, size_y;
  float scale = em.scale;
  double max;
  bool doInverse = em.doInverse;


  max = dataIn.maximum();
    
  dim_x = dataIn.dimX();
  dim_y = dataIn.dimY();
    
  size_x = dataIn.sizeX();
  size_y = dataIn.sizeY();
    
  //   Vertex       image        +       base
  nb_vertex = dim_x * dim_y;
  nb_vertex_border = 4 * ( dim_x + dim_y );

  //   Polygones        image        +          bords            dessous 
  nb_polygon = ((dim_x-1)*(dim_y-1))*2;
  nb_polygon_border = 4 * ((dim_x-1)+(dim_y-1)) + 2;
    
    
  vert.reserve( nb_vertex );
  poly.reserve( nb_polygon );
    
  vert_flat.reserve( nb_vertex );
  poly_flat.reserve( nb_polygon );
        
  vert_border.reserve( nb_vertex_border );
  poly_border.reserve( nb_polygon_border );
        

  //   Creation de la liste des vertex
  //
  //   a) Image

  ForEach2d( dataIn, x, y )
    {
      //   Relief
      v[0] = x * size_x;
      v[1] = y * size_y;

      if(!doInverse)
        {
          v[2] = dataIn( x, y ) * scale;
        }
      else
        {
          v[2] = ( max - dataIn( x, y ) ) * scale;
        }

      vert.push_back( v );

      //  Plat
      v[2] = 0;
	
      vert_flat.push_back( v );
    }
    
  //   b) Bords
  for( x=0; x<dim_x; ++x ) 
    {
      v[0] = x * size_x;
      v[1] = 0;
      v[2] = 0;
      
      vert_border.push_back( v );
    }

  for( x=0; x<dim_x; ++x ) 
    {
      v[0] = x * size_x;
      v[1] = 0;

      if(!doInverse)
        {
          v[2] = dataIn( x, 0 ) * scale;
        }
      else
        {
          v[2] = ( max - dataIn( x, 0 ) ) * scale;
        }
	
      vert_border.push_back( v );
    }

  for( x=0; x<dim_x; ++x )
    {
      v[0] = x * size_x;
      v[1] = (dim_y-1) * size_y;
      v[2] = 0;
      
      vert_border.push_back( v );
    }

  for( x=0; x<dim_x; ++x ) 
    {
      v[0] = x * size_x;
      v[1] = (dim_y-1) * size_y;

      if(!doInverse)
        {
          v[2] = dataIn( x, dim_y-1 ) * scale;
        }
      else
        {
          v[2] = ( max - dataIn( x, dim_y-1 ) ) * scale;
        }

      vert_border.push_back( v );
    }

  for( y=0; y<dim_y; ++y)
    {
      v[0] = 0;
      v[1] = y * size_y;
      v[2] = 0;
      
      vert_border.push_back( v );
    }

  for( y=0; y<dim_y; ++y)
    {
      v[0] = 0;
      v[1] = y * size_y;

      if(!doInverse)
        {
          v[2] = dataIn( 0, y ) * scale;
        }
      else
        {
          v[2] = ( max - dataIn( 0, y ) ) * scale;
        }

      vert_border.push_back( v );
    }

  for( y=0; y<dim_y; ++y)
    {
      v[0] = (dim_x-1) * size_x;
      v[1] = y * size_y;
      v[2] = 0;
	
      vert_border.push_back( v );
    }
    
  for( y=0; y<dim_y; ++y)
    {
      v[0] = (dim_x-1) * size_x;
      v[1] = y * size_y;

      if(!doInverse)
        {
          v[2] = dataIn( dim_x-1, y ) * scale;
        }
      else
        {
          v[2] = ( max - dataIn( dim_x-1, y ) ) * scale;
        }

      vert_border.push_back( v );
    }
    

  //   Creation de la liste des polygones
  //    
  //   a) Image

  for( y=0; y<dim_y-1; ++y )
    for( x=0; x<dim_x-1; ++x )
      {
        //   Relief
        p1[0] = x + y*dim_x;
        p1[1] = x + y*dim_x +1;
        p1[2] = x + (y+1)*dim_x + 1;
	  
        poly.push_back( p1 );
        poly_flat.push_back( p1 );
	  
        p2[0] = x + y*dim_x;
        p2[1] = x + (y+1)*dim_x + 1;
        p2[2] = x + (y+1)*dim_x;
	  
        poly.push_back( p2 );
        poly_flat.push_back( p2 );
      }

    
  //   b) Bords
    
  for( x=0; x<dim_x-1; ++x )
    {
      //   Face 1
      p1[0] = x;
      p1[1] = x+1;
      p1[2] = x+dim_x;
	
      poly_border.push_back( p1 );
	
      p2[0] = x+1;
      p2[1] = x+dim_x+1;
      p2[2] = x+dim_x;
	
      poly_border.push_back( p2 );
	

      //   Face 2
      p1[0] = x + 2*dim_x;
      p1[1] = x + 3*dim_x;
      p1[2] = x + 2*dim_x + 1;
	
      poly_border.push_back( p1 );
	
      p2[0] = x + 2*dim_x + 1;
      p2[1] = x + 3*dim_x;
      p2[2] = x + 3*dim_x + 1;
	
      poly_border.push_back( p2 );

    }

  for( y=0; y<dim_y-1; ++y )
    {
      //   Face 1
      p1[0] = y + 4*dim_x;
      p1[1] = y + 4*dim_x + dim_y;
      p1[2] = y + 4*dim_x + 1;
	
      poly_border.push_back( p1 );
	
      p2[0] = y + 4*dim_x + 1;
      p2[1] = y + 4*dim_x + dim_y;
      p2[2] = y + 4*dim_x + dim_y + 1;
	
      poly_border.push_back( p2 );


      //   Face 2
      p1[0] = y + 4*dim_x + 2*dim_y;
      p1[1] = y + 4*dim_x + 2*dim_y + 1;
      p1[2] = y + 4*dim_x + 3*dim_y;
	
      poly_border.push_back( p1 );
	
      p2[0] = y + 4*dim_x + 2*dim_y + 1;
      p2[2] = y + 4*dim_x + 3*dim_y;
      p2[1] = y + 4*dim_x + 3*dim_y + 1;
	
      poly_border.push_back( p2 );
    }
    
  // Base de la surface
  p1[0] = 0;
  p1[1] = 2*dim_x;
  p1[2] = dim_x-1;
    
  poly_border.push_back( p1 );
    
  p2[0] = dim_x-1;
  p2[1] = 2*dim_x;
  p2[2] = 3*dim_x-1;
    
  poly_border.push_back( p2 );

  Dictionary	material;
  material[ "face_culling" ] = Object::value( (int) 0 );
  surf.header().setProperty( "material", material );
  surf_flat.header().setProperty( "material", material );
  surf_border.header().setProperty( "material", material );


  //   Ecriture des images
  em.outWriter1 << surf;

  if( !em.outWriter2.fileName().empty() )
    em.outWriter2 << surf_flat;
    
  if( !em.outWriter3.fileName().empty() )
    em.outWriter3 << surf_border;

  return true;
}


int main( int argc, const char **argv )
  
{
  int result = EXIT_SUCCESS;
  try {
    
    //
    // Default values
    //
    ElevationMap	em;
    ProcessInput	fileIn( em );

    //
    // Collecte des arguments.
    //
    AimsApplication application( argc, argv, "Convert 2D data in surface" );
    application.addOption( fileIn,  "-i", "Input 2D short image" );
    application.addOption( em.outWriter1, "-o", "Output surface" );
    application.addOption( em.outWriter2, "-of", "Output flat surface " 
                           "[default: not written]", true );
    application.addOption( em.outWriter3, "-ob", "Output border surface " 
                           "[default: not written]", true );
    application.addOption( em.scale, "-s", "Scale factor (intensity)", true );
    application.addOption( em.doInverse, "--inv", "Inverse grey level " 
                           "[default=no]" ,true);
    application.initialize();

    if( !em.execute( fileIn.filename ) )
      result = EXIT_FAILURE;
  }
  catch( user_interruption &e ) {
  }
  catch( std::exception &e ) {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }
  return result;  
}
