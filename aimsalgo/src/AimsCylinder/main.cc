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


#include <cstdlib>
#include <aims/math/math_g.h>
#include <aims/vector/vector.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/surface.h>
#include <math.h>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;

void getCylinder( int x, int y, int z,
                  const AimsData< Point3df >& eigen,
                  vector< Point3df >& coord,
                  vector< Point3df >& normal,
                  vector< AimsVector< uint, 3 > >& triang,
                  float length, int n, int l, float radius, int which )
{
  float teta, teta0;
  int i, j;
  int begin = which * ( 2 + n * l + 2 * n );
  int end = ( which + 1 ) * ( 2 + n * l + 2 * n );
  int countCoord  = begin;
  int countNormal = begin;
  int countTriang = which * ( 2 * n + ( l - 1 ) * n * 2 );

  Point3df u = eigen(x,y,z),v,w;
 
  Point3df xyz( x * eigen.sizeX() - u[ 0 ] / 2.0 * length,
                y * eigen.sizeY() - u[ 1 ] / 2.0 * length,
                z * eigen.sizeZ() - u[ 2 ] / 2.0 * length );

  float dlength = length / ( l - 1 );

  coord[ countCoord++ ] = xyz;
  normal[ countNormal++ ] = -u;  

  v[ 0 ] =  u[ 1 ]; 
  v[ 1 ] = -u[ 0 ];
  v[ 2 ] =  0.0;
  float nor = norm2( v );
  if ( nor < 0.00001 )
  {
    v[ 0 ] = u[ 2 ];
    v[ 1 ] = 0.0;
    v[ 2 ] = - u[ 0 ];
    nor = norm2( v );
    v /= nor;
  }  
  v /= sqrt( nor );

  w[ 0 ] = u[ 1 ] * v[ 2 ] - u[ 2 ] * v[ 1 ];
  w[ 1 ] = v[ 0 ] * u[ 2 ] - u[ 0 ] * v[ 2 ];
  w[ 2 ] = u[ 0 ] * v[ 1 ] - u[ 1 ] * v[ 0 ];

  teta0 = 2 * M_PI / n;

  for ( i = 0; i < n; i++ )
  {
    teta = i * teta0;
    coord[ countCoord++ ]   = xyz + ( v * (float)cos( teta ) +
                                      w * (float)sin( teta ) ) * radius;
    normal[ countNormal++ ] = -u;
  }
  for ( i = 0; i < n - 1; i++ )
  {
    triang[ countTriang   ][ 0 ] = begin + i + 2;
    triang[ countTriang   ][ 1 ] = begin + i + 1;
    triang[ countTriang++ ][ 2 ] = begin + 0;
  }
  triang[ countTriang   ][ 0 ] = begin + 1;
  triang[ countTriang   ][ 1 ] = begin + n;
  triang[ countTriang++ ][ 2 ] = begin + 0;

  for ( i = 0; i < n; i++ )
  {
    teta = i * teta0;
    coord[ countCoord++ ]   = xyz + ( v * (float)cos( teta ) +
                                      w * (float)sin( teta ) ) * radius;
    normal[ countNormal++ ] = v * (float)cos( teta ) + w * (float)sin( teta );
  }

  for ( j = 1; j < l; j++ )
  {
    for ( i = 1; i <= n; i++ )
    { 
      teta = i * teta0;
      coord[ countCoord++ ] = coord[ begin + i + n ] +
                              ( u * ( (float)j * dlength ) );
      normal[ countNormal++ ] = v * (float)cos( teta ) 
	+ w * (float)sin( teta );
    }
    for ( i = 1; i <= n - 1; i++ )
    {
      triang[ countTriang   ][ 0 ] = begin + i + 1 + n * j + n;
      triang[ countTriang   ][ 1 ] = begin + i + n * j + n;
      triang[ countTriang++ ][ 2 ] = begin + i + n * ( j - 1 ) + n;
    }
    triang[ countTriang   ][ 0 ] = begin + n * j + 1 + n;
    triang[ countTriang   ][ 1 ] = begin + n * j + n + n;
    triang[ countTriang++ ][ 2 ] = begin + n + n * ( j - 1 ) + n;
 
    for ( i = 1; i <= n - 1; i++ )
    {
      triang[ countTriang   ][ 0 ] = begin + i + 1 + n * ( j - 1 ) + n;
      triang[ countTriang   ][ 1 ] = begin + i + 1 + n * j + n;
      triang[ countTriang++ ][ 2 ] = begin + i + n * ( j - 1 ) + n;
    }
    triang[ countTriang   ][ 0 ] = begin + n * ( j - 1 ) + 1 + n;
    triang[ countTriang   ][ 1 ] = begin + n * j + 1 + n;
    triang[ countTriang++ ][ 2 ] = begin + n + n * ( j - 1 ) + n;
  }
  j = l-1;

  for ( i = 1; i <= n; i++ )
  {
    teta = i * teta0;
    coord[ countCoord++ ] = coord[ begin + i + n ] + u * ( (float)j * dlength);
    normal[ countNormal++ ] = u;
  }
  coord[ countCoord++ ] = xyz + u * length;
  normal[ countNormal++ ] = u;

  for ( i = 1; i <= n - 1; i++ )
  {
    triang[ countTriang   ][ 0 ] = end - 1 - i - 1;
    triang[ countTriang   ][ 1 ] = end - 1 - i;
    triang[ countTriang++ ][ 2 ] = end - 1;
  }
  triang[ countTriang   ][ 0 ] = end - 1 - 1;
  triang[ countTriang   ][ 1 ] = end - 1 - n;
  triang[ countTriang++ ][ 2 ] = end - 1;
}


int main( int argc, const char **argv )
{
  Reader<AimsData<Point3df> >	fileEigen;
  Writer<AimsSurfaceTriangle>	fileOut;
  Reader<AimsData<byte> >	fileMask;
  Reader<AimsData<float> >	fileAniso;
  int				xmin = -1, ymin = -1, zmin = -1;
  int				xmax = -1, ymax = -1, zmax = -1;
  float				topLength = 3.0, radius = 0.2;
  int				n = 6;
  int				l = 2;
  float				threshAniso = 0.5;

  AimsApplication	app( argc, argv, "2D curvature of an intensity image " 
                             "f(x,y) = I" );
  app.addOption( fileEigen, "-e", "source eigen vectors file" );
  app.addOption( fileOut, "-o", "output mesh file [default=<eigen>.mesh]", 
                 true );
  app.addOption( fileMask, "-m", "mask on the eigen vectors file", true );
  app.addOption( fileAniso, "-a", "anisotropy file to give a length", true );
  app.addOption( xmin, "-x", 
                 "(x,y,z),(X,Y,Z) defines a sub-volume of interest", true );
  app.addOption( ymin, "-y", "", true );
  app.addOption( zmin, "-z", "", true );
  app.addOption( xmax, "-X", "", true );
  app.addOption( ymax, "-Y", "", true );
  app.addOption( zmax, "-Z", "", true );
  app.addOption( topLength, "-t", "maximum length of the cylinders "
                 "[default=3.0]", true );
  app.addOption( radius, "-r", "radius of the cylinders [default=0.2]", true );
  app.addOption( n, "-n", "number of division on 2 disk faces of the " 
                 "cylinders [default=6]", true );
  app.addOption( l, "-l", "number of triangles on each division [default=2]", 
                 true );
  app.addOption( threshAniso, "-g", "lower threshold for anisotropy; an eigen "
                 "vector is valid if its anisotropy is greater or equal to " 
                 "<g> [default=0.5]", true );

  app.alias( "--eigen", "-e" );
  app.alias( "--output", "-o" );
  app.alias( "--mask", "-m" );
  app.alias( "--aniso", "-a" );
  app.alias( "--topLength", "-t" );
  app.alias( "--radius", "-r" );

  try
    {
      app.initialize();

      //
      // Examining arguments
      //
      cout << "examining line arguments : " << flush;

      if ((xmin!=-1 || ymin!=-1 || zmin!=-1 || xmax!=-1 || ymax!=-1 
           || zmax!=-1) && (xmin==-1 || ymin==-1 || zmin==-1 || xmax==-1 
                            || ymax==-1 || zmax==-1) )
        AimsError( "AimsCylinder : you must specify all coordinates of " 
                   "sub-volume" );

      AimsData< Point3df > eigen;
      fileEigen.read( eigen );

      if( fileOut.fileName().empty() )
        fileOut.setFileName( eigen.header()->removeExtension
                             ( fileEigen.fileName() ) );

      if( xmin != -1 && ymin != -1 && zmin != -1 &&
          xmax != -1 && ymax != -1 && zmax != -1   )
        {
          ASSERT( xmin < eigen.dimX() && xmax < eigen.dimX() &&
                  ymin < eigen.dimY() && ymax < eigen.dimY() &&
                  zmin < eigen.dimZ() && zmax < eigen.dimZ()   );
        }
      else
        {
          xmin = ymin = zmin = 0;
          xmax = eigen.dimX() - 1;
          ymax = eigen.dimY() - 1;
          zmax = eigen.dimZ() - 1;
        }

      AimsData<byte> mask;
      if( !fileMask.fileName().empty() )
        fileMask.read( mask );

      AimsData< float > anisotropy;
      if( !fileAniso.fileName().empty() )
        fileAniso.read( anisotropy );

      ASSERT( n >= 3 && l >= 2 && radius >= 0.0001 );
      ASSERT( threshAniso >= 0.0 && threshAniso <= 1.0 );

      cout << "done" << endl << flush;

      //
      // Looking for valid points
      //
      int npoints = 0, x, y, z;
      Point3df nullPt(0,0,0);

      cout << "detecting valid points : " << flush;
      ForEach3d( eigen, x, y, z )
        if ( x >= xmin && x <= xmax &&
             y >= ymin && y <= ymax &&
             z >= zmin && z <= zmax    )
        {
          if( !fileMask.fileName().empty() )
            {
              if( mask( x, y, z) && eigen( x, y, z ) != nullPt )
                {
                  if( !fileAniso.fileName().empty() )
                    {
                      if ( anisotropy( x, y, z ) >= threshAniso )
                        npoints++;
                    }
                  else
                    {
                      if ( topLength >= 0.01 )
                        npoints++;
                    }
                }
            }
          else 
            {
              if ( eigen( x, y, z ) != nullPt )
                {
                  if( !fileAniso.fileName().empty() )
                    {
                      if ( anisotropy( x, y, z ) >= threshAniso )
                        npoints++;
                    }
                  else
                    {
                      if ( topLength >= 0.01 )
                        npoints++;
                    }
                }
            }
        }
      cout << "done" << endl << flush;
      cout << "number of valid points : " << npoints << endl << flush;

      int cPoints = 2 + n*l + 2*n;
      int cTriang = 2*n + (l-1)*n*2;
      int nPoints = npoints * cPoints;
      int nTriang = npoints * cTriang;

      //
      // Allocation of Coordinates, Normals and Triangles
      //
      cout << "allocating coord., normals and triangles : " << flush;
      AimsSurfaceTriangle	surf;
      vector< Point3df >	& coord ( surf.vertex() );
      vector< Point3df >	& normal( surf.normal() );
      vector< AimsVector< uint, 3 > >	& triang( surf.polygon() );

      coord.reserve( nPoints );
      normal.reserve( nPoints );
      triang.reserve( nTriang );
      coord.insert( coord.end(), nPoints, Point3df( 0, 0, 0 ) );
      normal.insert( normal.end(), nPoints, Point3df( 0, 0, 0 ) );
      triang.insert( triang.end(), nPoints, 
                     AimsVector< uint, 3 >( 0, 0, 0 ) );
      cout << "done" << endl <<flush;

      //
      // Builds cylinders
      //
      cout << "building cylinders" << endl << flush;
      cout << "cylinders : " << setw(6) << 0 << flush;
      int which = 0;
      ForEach3d( eigen, x, y, z )
        if ( x >= xmin && x <= xmax &&
             y >= ymin && y <= ymax &&
             z >= zmin && z <= zmax    )
        {
          if( !fileMask.fileName().empty() )
            {
              if ( mask( x, y, z ) && eigen( x, y, z ) != nullPt )
                {
                  if( !fileAniso.fileName().empty() )
                    {
                      if( anisotropy( x, y, z ) >= threshAniso )
                        {
                          getCylinder( x, y, z, eigen, coord, normal, triang,
                                       topLength * anisotropy( x, y, z), n, l, 
                                       radius, which++ );
                        }
                    }
                  else
                    {
                      if ( topLength >= 0.01 )
                        {
                          getCylinder( x, y, z, eigen, coord, normal, triang,
                                       topLength, n, l, radius, which++ );
                        }
                    }
                }
            }
          else 
            {
              if( eigen( x, y, z ) != nullPt )
                {
                  if( !fileAniso.fileName().empty() )
                    {
                      if( anisotropy( x, y, z ) >= threshAniso )
                        {
                          getCylinder( x, y, z, eigen, coord, normal, triang,
                                       topLength * anisotropy( x, y, z ), n, 
                                       l, radius, which++ );
                        }
                    }
                  else
                    {
                      if ( topLength >= 0.01 )
                        {
                          getCylinder( x, y, z, eigen, coord, normal, triang,
                                       topLength, n, l, radius, which++ ); 
                        }
                    }
                }
            }
        }
      cout << "\b\b\b\b\b\b" << setw(6) << which << endl << flush;

      //
      // Saves the data to disk in a *.tri file
      //
      cout << "writing data to disk : " << flush;
      fileOut.write( surf );

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
