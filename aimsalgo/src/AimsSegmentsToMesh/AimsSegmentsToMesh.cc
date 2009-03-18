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
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <aims/utility/utility_g.h>
#include <aims/mesh/mesh_g.h>
#include <aims/vector/vector.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/mesh/texture.h>
#include <aims/io/defaultItemR.h>

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "--------------------------------------------------------------------------",
  "AimsSegmentsToMesh  -i[nput] <filein>                                     ",
  "           -o[utput] <fileout>                                            ",
  "         [ -w[idth] ] <size>                                              ",
  "         [ -v[erbose] ]                                                   ",
  "         [ -h[elp] ]                                                      ",
  "--------------------------------------------------------------------------",
  " Take a list of segments (pairs of points) and build a 3D composed of     ",
  " cylinders. Each cylinder joins two points of a segment.                  ",
  "                                                                          ",
  "--------------------------------------------------------------------------",
  "-i[nput]            : Serie of pair of 3D points                          ",
  "-o[utput]           : Name of the output mesh file                        ",
  "-w[idth]            : Width of the cylinder (default 1)                   ",
  "-v[erbose]          : display information on standard error output        ",
  "-h[elp]             : display the current help message                    ",
  "--------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


int main(int argc,char *argv[])
{
  char *fileIn = NULL;
  char *fileOut = NULL;
  int verbose = 0;
  float cylinderRadius = 1;

  AimsOption opt[] = {
    { 'h', "help",   AIMS_OPT_FLAG  ,(void *) Usage, AIMS_OPT_CALLFUNC, 0 },
    { 'i', "input",  AIMS_OPT_STRING, &fileIn, 0, 1 },
    { 'o', "output", AIMS_OPT_STRING, &fileOut, 0, 1 },
    { 'w', "width",  AIMS_OPT_FLOAT,  &cylinderRadius, 0, 0 },
    { 'v', "verbose",     AIMS_OPT_FLAG,   &verbose, 0, 0 },
    { 0  ,0, AIMS_OPT_END, 0, 0, 0 }
  };

  AimsParseOptions( &argc, argv, opt, usage );

  AimsSurfaceTriangle result;

  // Read segments
  vector< Point3df > points;
  {
    if ( verbose ) cout << "Read " << fileIn << flush;
    ifstream in( fileIn );
    DefaultAsciiItemReader< Point3df > pointReader;
    size_t segmentCount;
    in >> segmentCount;
    points.resize( 2 * segmentCount );
    for( size_t i = 0; i < points.size(); ++i ) {
      pointReader.read( in, points[ i ] );
    }
    if ( verbose ) cout << " done" << endl;
  }

  // Build cylinders
  for( size_t i = 0; i < points.size(); i += 2 ) {
    Point3df v( points[i+1] - points[ i ] );
    v /= v.norm();
    Point3df vBorders[ 4 ];
    if ( v[ 0 ] ) {
      vBorders[ 0 ] = crossed( v, Point3df( 0, 1, 0 ) );
    } else {
      vBorders[ 0 ] = crossed( v, Point3df( 1, 0, 0 ) );
    }
    vBorders[ 0 ] *= cylinderRadius / vBorders[ 0 ].norm();
    vBorders[ 1 ] = crossed( v, vBorders[ 0 ] );
    vBorders[ 1 ] *= cylinderRadius / vBorders[ 1 ].norm();
    vBorders[ 2 ] = crossed( v, vBorders[ 1 ] );
    vBorders[ 2 ] *= cylinderRadius / vBorders[ 2 ].norm();
    vBorders[ 3 ] = crossed( v, vBorders[ 2 ] );
    vBorders[ 3 ] *= cylinderRadius / vBorders[ 3 ].norm();

    // Add points
    size_t p = result.vertex().size();
    for( int j = 0; j < 4; ++j ) {
      result.vertex().push_back( points[ i ] + vBorders[ j ] );
      result.normal().push_back( vBorders[ j ] / vBorders[ j ].norm() );
    }
    for( int j = 0; j < 4; ++j ) {
      result.vertex().push_back( points[ i + 1 ] + vBorders[ j ] );
      result.normal().push_back( vBorders[ j ] / vBorders[ j ].norm() );
    }

    // Add triangles
    result.polygon().push_back( AimsVector<uint,3>( p + 3, p + 1, p ) );
    result.polygon().push_back( AimsVector<uint,3>( p + 3, p + 2, p + 1 ) );

    result.polygon().push_back( AimsVector<uint,3>( p + 4, p + 5, p + 7 ) );
    result.polygon().push_back( AimsVector<uint,3>( p + 5, p + 6, p + 7 ) );

    for( int j = 0; j < 4; ++j ) {
      result.polygon().push_back( AimsVector<uint,3>( p + (j+1) % 4 + 4, 
						       p + j + 4, 
						       p + j ) );
      result.polygon().push_back( AimsVector<uint,3>( p + j, 
						       p + ( j + 1 ) % 4, 
						       p + (j+1) % 4 + 4 ) );
    }
  }

  // Write result
  if ( verbose ) cout << "Write " << fileOut << flush;
  Writer< AimsSurfaceTriangle > writer( fileOut );
  writer.write( result );
  if ( verbose ) cout << " done" << endl;

  return EXIT_SUCCESS;
}
