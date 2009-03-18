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


#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/io/reader.h>
#include <aims/io/finder.h>
#include <aims/io/process.h>

using namespace aims;
using namespace carto;
using namespace std;

template<typename T> 
static bool masscenter( Process &, const string &, Finder & );

class MassCenter : public Process
{
public:
  MassCenter( bool bin = false );
  virtual ~MassCenter();

  bool	binary;
};


MassCenter::MassCenter( bool bin ) : binary( bin )
{
  registerProcessType( "Volume", "S8", &masscenter<int8_t> );
  registerProcessType( "Volume", "U8", &masscenter<uint8_t> );
  registerProcessType( "Volume", "S16", &masscenter<int16_t> );
  registerProcessType( "Volume", "FLOAT", &masscenter<float> );
}


MassCenter::~MassCenter()
{
}


template<typename T>
bool masscenter( Process & p, const string & filein, Finder & f )
{
  AimsData<T>		data;
  Reader<AimsData<T> >	r( filein );
  string		format = f.format();
  r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly, 
                                           DataSource::none(), false, 0.01 ) );
  r.read( data, 0, &format );
  bool			bin = ((MassCenter &) p).binary;

  int	x, y, z, t;
  double	sum, val, totsum = 0, np, totnp = 0;
  double	vol = data.sizeX() * data.sizeY() * data.sizeZ();
  Point3df	pos, totpos;
  for( t=0; t<data.dimT(); ++t )
    {
      sum = 0;
      np = 0;
      pos = Point3df( 0, 0, 0 );
      ForEach3d( data, x, y, z )
	{
	  if( bin )
	    {
	      if( data( x, y, z, t ) == 0 )
		val = 0;
	      else
		val = 1;
	    }
	  else
	    val = (double) data( x, y, z, t );
	  sum += val;
	  if( val > 0 )
	    ++np;
	  pos += Point3df( val * x, val * y, val * z );
	}
      totpos += pos;
      pos /= sum;
      cout << t << "\t\t" << pos[0] << "\t" << pos[1] << "\t" << pos[2] 
	   << "\tMass: " << vol * sum << "\tVol: " << vol * np << endl;
      totsum += sum;
      totnp += np;
    }
  totpos /= totsum;
  cout << "---\nGeneral:\t" << totpos[0] << "\t" << totpos[1] << "\t" 
       << totpos[2] << "\tMass: " << vol * totsum << "\tVol: " << vol * totnp 
       << endl;
  return( true );
}


int main( int argc, const char **argv )
{
  bool		bin = false;
  MassCenter	proc;
  ProcessInput	filein( proc );

  AimsApplication	app( argc, argv, "Computes position of the mass "
			     "center of the image" );
  app.addOption( filein, "-i", "input data" );
  app.alias( "--input", "-i" );
  app.addOption( bin, "-b", "consider input image as binary data", true );
  app.alias( "--binary", "-b" );

  try
    {
      app.initialize();
      proc.binary = bin;
      if( !proc.execute( filein.filename ) )
	cout << "Couldn't process file - aborted\n";
    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return( 1 );
    }

  return EXIT_SUCCESS;
}


