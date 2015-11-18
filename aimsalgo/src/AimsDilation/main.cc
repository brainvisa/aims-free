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
#include <aims/data/data_g.h>
#include <aims/morphology/morphology_g.h>
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/finder.h>
#include <aims/getopt/getopt2.h>
#include <cstdlib>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;


template<class T>
static bool dilat( Process & p, const string & filename, Finder & f );

class Dilatation : public Process
{
public:
  Dilatation( const string & fout, int xm, int ym, int zm, float rad, float fac );

  template<class T> 
  bool dilat_m( T & data, const string & filename, Finder & f );

private:
  string	fileout;
  int		xmask;
  int		ymask;
  int		zmask;
  float		radius;
  float		factor;
};

Dilatation::Dilatation( const string & fout, int xm, int ym, int zm,
	      float rad,  float fac )
	      : Process(), fileout( fout ), xmask( xm ), 
			    ymask( ym ), zmask( zm ), radius( rad ), 
			    factor( fac )
{
  registerProcessType( "Volume", "S16", &dilat<AimsData<short> > );
  // add new types here when morphology operators become templates...
}

template<class T> 
bool dilat( Process & p, const string & filename, Finder & f )
{
  Dilatation	& dp = (Dilatation &) p;
  T		data;
  return( dp.dilat_m( data, filename, f ) );
}


template<class T> 
bool Dilatation::dilat_m( T & data, const string & filename, Finder & f )
{
  Reader<T>	reader( filename );
  string	format = f.format();
  if( !reader.read( data, 1, &format ) )
    return( false );

  T		dil;
  dil = AimsMorphoChamferDilation( data, radius, xmask, ymask, zmask, factor );

  Writer<T>	writer( fileout );
  return( writer.write( dil ) );
}


int main( int argc, const char **argv )
{
  string filein, fileout;
  int xmask = 3, ymask = 3, zmask = 3;
  float radius, factor = 50;

  AimsApplication app( argc, argv, "Morphological dilation" );
  app.addOption( filein, "-i", "source volume" );
  app.alias( "--input", "-i" );
  app.addOption( fileout, "-o", "destination volume" );
  app.alias( "--output", "-o" );
  app.addOption( radius, "-e", "radius of the structuring element" );
  app.alias( "--eradius", "-e" );
  app.alias( "-r", "-e" );
  app.alias( "--radius", "-e" );
  app.addOption( xmask, "-x", "X size of the distance mask [default=3]",
                 true );
  app.addOption( ymask, "-y", "Y size of the distance mask [default=3]",
                 true );
  app.addOption( zmask, "-z", "Z size of the distance mask [default=3]",
                 true );
  app.alias( "--xmask", "-x" );
  app.alias( "--ymask", "-y" );
  app.alias( "--zmask", "-z" );
  app.addOption( factor, "-f", "chamfer multiplication factor [default=50]",
                 true );
  app.alias( "--factor", "-f" );

  try
  {
    app.initialize();

    Dilatation	proc( fileout, xmask, ymask, zmask, radius, factor );
    if( !proc.execute( filein ) )
    {
      cerr << "Couldn't process\n";
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

  return( EXIT_SUCCESS );
}
