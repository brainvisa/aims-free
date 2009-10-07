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
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <aims/utility/utility_g.h>
#include <aims/mesh/mesh_g.h>
#include <aims/vector/vector.h>
#include <aims/io/writer.h>
#include <string.h>
#include <errno.h>


using namespace std;
using namespace aims;


BEGIN_USAGE(usage)
  "--------------------------------------------------------------------------",
  "AimsSphereTriangulation  -d[irections]  <direction>                       ",
  "         [ -o[utput] <fileout> ]                                          ",
  "         [ -s[steps] <steps> ]                                            ",
  "         [ -r[adius] <radius>                                             ",
  "         [-h[elp]]                                                        ",
  "--------------------------------------------------------------------------",
  " Output a file with coordinates of discrete spheric distribution with any ",
  " number of directions.                                                    ",
  "                                                                          ",
  " See also : AimsIcosaList, AimsSphereTriangulation                        ",
  "--------------------------------------------------------------------------",
  "-d[direction]       : number of directions                                ",
  "-o[utput]           : output file name (default: standard output)         ",
  "-s[steps]           : maximum number of minimization steps (default 10000)",
  "-r[adius]           : radius of the sphere (default : 1)                  ",
  "-h[elp]             : display the current help message                    ",
  "--------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}

inline double frand(void){return ((rand()-(RAND_MAX/2))/(RAND_MAX/2.));}

double get_coulomb_energy( const vector<Point3dd> &p )
{
  double e = 0;
  for( vector<Point3dd>::size_type i = 0; i < p.size(); ++i ) {
    for( vector<Point3dd>::size_type j = i + 1; j < p.size(); ++j ) {
      e += 1 / (p[j] - p[i]).dnorm();
    }
  }
  return e;
}

void get_forces( vector<Point3dd> &f, const vector<Point3dd> &p )
{
  vector<Point3dd>::size_type i, j;
  for( i = 0; i < f.size(); ++i ) {
    f[ i ][ 0 ] = f[ i ][ 1 ] = f[ i ][ 2 ] = 0;
  }
  for( i = 0; i < f.size(); ++i ) {
    for( j = i + 1; j < f.size(); ++j ) {
      Point3dd r = p[i] - p[j];
      double l = r.dnorm();
      l = 1/(l*l*l);
      double ff;
      ff = l*r[0]; f[i][0] += ff; f[j][0] -= ff;
      ff = l*r[1]; f[i][1] += ff; f[j][1] -= ff;
      ff = l*r[2]; f[i][2] += ff; f[j][2] -= ff;
    }
  }
}


int main(int argc,char *argv[])
{
  char *fileOut = NULL;
  int direction = 0;
  int Nstep = 1000;
  double step=0.01,radius = 1;
  const double minimal_step=1.e-10;

  AimsOption opt[] = {
  { 'h',"help"      ,AIMS_OPT_FLAG,   ( void* )Usage, AIMS_OPT_CALLFUNC, 0},
  { 'd',"direction" ,AIMS_OPT_INT,    &direction ,    0                , 1},
  { 'o',"output"    ,AIMS_OPT_STRING, &fileOut,       0              ,   0},
  { 's',"steps"     ,AIMS_OPT_INT,    &Nstep,         0                , 0},
  { 'r',"radius"    ,AIMS_OPT_DOUBLE, &radius,        0                , 0},
  { 0  ,0           ,AIMS_OPT_END,    0,              0           ,      0}};

  AimsParseOptions( &argc, argv, opt, usage );

  vector< Point3dd > p0( direction );
  vector< Point3dd > p1( direction );
  vector< Point3dd > f( direction );
  vector< Point3dd > *pp0 = &p0, *pp1 = &p1;

  srand( time( NULL ) );
  
  for( int i = 0; i < direction; ++i ) {
    p0[ i ][ 0 ] = 2 * frand();
    p0[ i ][ 1 ] = 2 * frand();
    p0[ i ][ 2 ] = 2 * frand();
    double l = p0[ i ].dnorm();
    if( l != 0.0 ) {
      p0[ i ][ 0 ] /= l;
      p0[ i ][ 1 ] /= l;
      p0[ i ][ 2 ] /= l;
    } else {
      --i;
    }
  }

  double e0 = get_coulomb_energy( p0 );
  for( int k = 0; k < Nstep; ++k ) {
    get_forces( f, p0 );
    for( int i = 0; i < direction; ++i ) {
      const double d = f[i].dot( (*pp0)[i] );
      f[ i ][ 0 ]  -= (*pp0)[ i ][ 0 ] * d;
      f[ i ][ 1 ]  -= (*pp0)[ i ][ 1 ] * d;
      f[ i ][ 2 ]  -= (*pp0)[ i ][ 2 ] * d;
      (*pp1)[ i ][ 0 ] = (*pp0)[ i ][ 0 ] + f[ i ][ 0 ] * step;
      (*pp1)[ i ][ 1 ] = (*pp0)[ i ][ 1 ] + f[ i ][ 1 ] * step;
      (*pp1)[ i ][ 2 ] = (*pp0)[ i ][ 2 ] + f[ i ][ 2 ] * step;
      const double l = (*pp1)[ i ].dnorm();
      (*pp1)[ i ][ 0 ] /= l;
      (*pp1)[ i ][ 1 ] /= l;
      (*pp1)[ i ][ 2 ] /= l;
    }
    double e = get_coulomb_energy( *pp1 );
    if ( e >= e0 ){  // not successfull step
      step /= 2;
      if( step < minimal_step ) break;
      continue;
    } else {   // successfull step
      vector<Point3dd> *t = pp0;
      pp0 = pp1; 
      pp1 = t;      
      e0 = e;
      step *= 2;
    }
    // fprintf(stderr,"\rn: %5d, e = %18.8f step = %12.10f",k,e,step);
    // fflush(stderr);     
  }

  ostream *out;
  if ( fileOut ) {
    out = new ofstream( fileOut );
  } else {
    out = &cout;
  }
  *out << direction << endl;
  for( int i = 0; i < direction; ++i ) {
    *out << radius * p0[i][0] << " " << radius * p0[i][1] << " " << radius * p0[i][2] << endl;
  }
  if ( fileOut ) {
    delete out;
  }

  return EXIT_SUCCESS;
}

