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
#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>

using namespace std;
using namespace carto;
using namespace aims;



class OverVolumeProcess : public Process
{

  public:

  OverVolumeProcess( const std::string& fileout,
                       int dimX,
                       int dimY,
                       int dimZ,
                       int dimT,
                       int x0,
                       int y0,
                       int z0,
                       int t0 );

    int dimX() const { return _dimX; }
    int dimY() const { return _dimY; }
    int dimZ() const { return _dimZ; }
    int dimT() const { return _dimT; }
    int x0() const { return _x0; }
    int y0() const { return _y0; }
    int z0() const { return _z0; }
    int t0() const { return _t0; }
    std::string fileout() const { return _fileout; }

  private:


    /* mes membres prives */
    std::string _fileout;
    int _dimX;
    int _dimY;
    int _dimZ;
    int _dimT;
    int _x0;
    int _y0;
    int _z0;
    int _t0;
    std::string _format;
    bool _ascii;
    bool _verbose;

};

template < class U >
bool OverVolumeDoIt(  Process& process, const string& fileNameIn,
		      Finder& )
{

  int x,y,z,t;
  OverVolumeProcess& 
    overVolumeProcess = dynamic_cast< OverVolumeProcess& >( process );


  cout << "reading input volume   : " << flush;
  Reader< U > volR( fileNameIn );
  U dataIn;
  volR >> dataIn;
  cout << "done" << endl;

  U dataOut( overVolumeProcess.dimX(),
	     overVolumeProcess.dimY(),
	     overVolumeProcess.dimZ(),
	     overVolumeProcess.dimT() );

  dataOut.setSizeX( dataIn.sizeX() );
  dataOut.setSizeY( dataIn.sizeY() );
  dataOut.setSizeZ( dataIn.sizeZ() );
  dataOut.setSizeT( dataIn.sizeT() );

//   cout << "min=" << dataIn.minimum() << endl;
  if ( dataIn.minimum() < 0 )  
    {
      dataOut = dataIn.minimum();
    }
  else 
    {
      dataOut = 0;
    }

  ForEach4d( dataIn, x, y, z, t )
  {

    if ( ( x + overVolumeProcess.x0() < dataOut.dimX() ) &&
         ( y + overVolumeProcess.y0() < dataOut.dimY() ) &&
         ( z + overVolumeProcess.z0() < dataOut.dimZ() ) &&
         ( t + overVolumeProcess.t0() < dataOut.dimT() ) )
    {
         
      dataOut( x + overVolumeProcess.x0(),
               y + overVolumeProcess.y0(),
               z + overVolumeProcess.z0(),
               t + overVolumeProcess.t0() ) = dataIn( x, y, z, t );

    }

  }

  cout << "writing volume : " << flush;
  Writer< U > volW( overVolumeProcess.fileout() );
  volW << dataOut;
  cout << "done" << endl;  

  return true;

}


OverVolumeProcess::OverVolumeProcess( const std::string& fileout,
				      int dimX,
				      int dimY,
				      int dimZ,
				      int dimT,
				      int x0,
				      int y0,
				      int z0,
				      int t0 )
  : _fileout( fileout), _dimX(dimX), _dimY(dimY), _dimZ(dimZ), _dimT(dimT), 
    _x0(x0), _y0(y0), _z0(z0), _t0(t0)
{
 
 registerProcessType( "Volume", "S8",
		       &OverVolumeDoIt< AimsData< int8_t > > );

  registerProcessType( "Volume", "S16",
		       &OverVolumeDoIt< AimsData< int16_t > > );
  
  registerProcessType( "Volume", "FLOAT",
		     &OverVolumeDoIt< AimsData< float > > );


}


int main(int argc, const char **argv)
{

  string filein, fileout;
  int dx = 0, dy = 0, dz = 0,dt = 1;
  int x0 = 0 ,y0 = 0 ,z0 = 0, t0 = 0;

  AimsApplication app( argc, argv, "Change the dimension of an image" );
  app.addOption( filein, "-i", "source volume" );
  app.alias( "--input", "-i" );
  app.addOption( fileout, "-o", "destination volume" );
  app.alias( "--output", "-o" );
  app.addOption( dx, "--dx", "size of the output volume" );
  app.addOption( dy, "--dy", "size of the output volume" );
  app.addOption( dz, "--dz", "size of the output volume" );
  app.addOption( dt, "--dt", "size of the output volume" );
  app.addOption( x0, "--x0",
                 "translation (int) of the input image (default = none)",
                 true );
  app.addOption( y0, "--y0",
                 "translation (int) of the input image (default = none)",
                 true );
  app.addOption( z0, "--z0",
                 "translation (int) of the input image (default = none)",
                 true );
  app.addOption( t0, "--t0",
                 "translation (int) of the input image (default = none)",
                 true );
  try
  {
    app.initialize();

    OverVolumeProcess process( fileout,
                                dx, dy, dz, dt,
                                x0, y0, z0, t0 );

    process.execute( filein );
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}
