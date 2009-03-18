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
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/surfaceOperation.h>
#include <iomanip>


using namespace aims;
using namespace carto;
using namespace std;



float normOf( const vector< Point3df >& vertices, uint k1, uint k2 )
{

  const Point3df& p1 = vertices[ k1 ];
  const Point3df& p2 = vertices[ k2 ];

  return norm( p2 - p1 );

}


bool findLongEdge( list< AimsVector< uint, 3 > >& triangles,
                   vector< Point3df >& vertices,
                   list< AimsVector< uint, 3 > >& removed,
                   uint& k1, uint& k2,
                   float maximumEdgeLength )
{

  removed.clear();

  list< list< AimsVector< uint, 3 > >::iterator > toBeRemoved;

  list< AimsVector< uint, 3 > >::iterator t = triangles.begin(),
                                          te = triangles.end();

  bool found = false;
  while ( t != te )
  {

    AimsVector< uint, 3 > triangle = *t;

    if ( !found )
    {

      if ( normOf( vertices, triangle.item( 0 ), triangle.item( 1 ) ) >
           maximumEdgeLength )
      {

         k1 = triangle.item( 0 );
         k2 = triangle.item( 1 );
         toBeRemoved.push_back( t );
         removed.push_back( triangle );
         found = true;

      }
      else if ( normOf( vertices, triangle.item( 0 ), triangle.item( 2 ) ) >
                maximumEdgeLength )
      {

         k1 = triangle.item( 0 );
         k2 = triangle.item( 2 );
         toBeRemoved.push_back( t );
         removed.push_back( triangle );
         found = true;

      }
      else if ( normOf( vertices, triangle.item( 1 ), triangle.item( 2 ) ) >
                maximumEdgeLength )
      {

         k1 = triangle.item( 1 );
         k2 = triangle.item( 2 );
         toBeRemoved.push_back( t );
         removed.push_back( triangle );
         found = true;

      }

    }
    else
    {

      if ( ( triangle.item( 0 ) == k1 ||
             triangle.item( 1 ) == k1 ||
             triangle.item( 2 ) == k1 ) &&
           ( triangle.item( 0 ) == k2 ||
             triangle.item( 1 ) == k2 ||
             triangle.item( 2 ) == k2 ) )
      {

         toBeRemoved.push_back( t );
         removed.push_back( triangle );

      }

    }
    
    ++ t;

  }

  list< list< AimsVector< uint, 3 > >::iterator >::const_iterator
    r = toBeRemoved.begin(), re = toBeRemoved.end();
  while ( r != re )
  {

    triangles.erase( *r );
    ++ r;

  }

  return !removed.empty();

}


void addNewTriangles( list< AimsVector< uint, 3 > >& triangles,
                      const list< AimsVector< uint, 3 > >& removed,
                      uint k1, uint k2, uint newK )
{

  int k3;
  list< AimsVector< uint, 3 > >::const_iterator r = removed.begin(),
                                                re = removed.end();
  while ( r != re )
  {

    AimsVector< uint, 3 > triangle = *r;
    
    if ( ( triangle.item( 0 ) == k1 ) && ( triangle.item( 1 ) == k2 ) )
    {

      k3 = triangle.item( 2 );

    }
    else if ( ( triangle.item( 0 ) == k1 ) && ( triangle.item( 2 ) == k2 ) )
    {

      k3 = triangle.item( 1 );

    }
    else if ( ( triangle.item( 0 ) == k2 ) && ( triangle.item( 1 ) == k1 ) )
    {

      k3 = triangle.item( 2 );

    }
    else if ( ( triangle.item( 2 ) == k1 ) && ( triangle.item( 0 ) == k2 ) )
    {

      k3 = triangle.item( 1 );

    }
    else if ( ( triangle.item( 1 ) == k1 ) && ( triangle.item( 2 ) == k2 ) )
    {

      k3 = triangle.item( 0 );

    }
    else if ( ( triangle.item( 2 ) == k1 ) && ( triangle.item( 1 ) == k2 ) )
    {

      k3 = triangle.item( 0 );

    }

    triangles.push_back( AimsVector< uint, 3 >( k1, newK, k3 ) );
    triangles.push_back( AimsVector< uint, 3 >( k2, newK, k3 ) );

    ++ r;

  }

}


int main( int argc, const char **argv )
{

  try
  {

    Reader< AimsSurfaceTriangle > meshReader;
    Writer< AimsSurfaceTriangle > meshWriter;
    bool ascii = false;
    float maximumEdgeLength;

    AimsApplication application( argc, argv, 
                                 "Mesh refinement" );
    application.addOption( meshReader, "-i", "input mesh" );
    application.addOption( meshWriter, "-o", "output mesh" );
    application.addOption( ascii, "-a", "write in ascii format", true );
    application.addOption( maximumEdgeLength, "-l",
                           "maximum edge length in mm" );

    application.initialize();

    // read triangulation
    cout << "reading mesh : " << flush;
    AimsSurfaceTriangle mesh;
    meshReader >> mesh;
    cout << "done" << endl;

    // processing inverse decimation
    cout << "processing inverse decimation : " << flush;
    AimsSurfaceTriangle::iterator s = mesh.begin(),
                                  se = mesh.end();

    int rank = 0;
    while ( s != se )
    {

      AimsSurface< 3, Void >& surface = s->second;

      vector< Point3df >& vertices = surface.vertex();
      vector< Point3df >& normals = surface.normal();

      // copy triangles to temporary list
      list< AimsVector< uint, 3 > > triangles( surface.polygon().begin(),
                                               surface.polygon().end() );

      size_t initialTriangleCount = triangles.size();

      if ( rank )
      {

        cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
             << setw( 3 ) << rank << " -> "
             << setw( 6 ) << initialTriangleCount << " : "
             << setw( 6 ) << triangles.size() << flush;

      }
      else
      {

        cout << "rank = " << setw( 3 ) << rank << " -> "
             << setw( 6 ) << initialTriangleCount << " : "
             << setw( 6 ) << triangles.size() << flush;

      }

      // loop until no more long edge exist
      list< AimsVector< uint, 3 > > removed;
      uint k1, k2;

      while ( findLongEdge( triangles, vertices,
                            removed, k1, k2, maximumEdgeLength ) )
      {

        Point3df newVertex = ( vertices[ k1 ] + vertices[ k2 ] );
        newVertex /= 2.0;
        Point3df newNormal = ( normals[ k1 ] + normals[ k2 ] );
        newNormal /= 2.0;
        uint newK = ( uint )vertices.size();

        vertices.push_back( newVertex );
        normals.push_back( newNormal );

        addNewTriangles( triangles, removed, k1, k2, newK );

        std::cout << "\b\b\b\b\b\b" << setw( 6 ) <<  triangles.size() << flush;

      }


      // copy back triangles
      surface.polygon() = vector< AimsVector< uint, 3 > >( triangles.begin(),
                                                           triangles.end() );
      ++ s;
      ++ rank;

    }
    cout << endl;

    // save triangulation
    cout << "saving mesh    : " << flush;
    meshWriter.write( mesh, ascii );
    cout << "done" << endl;

  }
  catch( user_interruption & )
  {
  }

  return EXIT_SUCCESS;

}
