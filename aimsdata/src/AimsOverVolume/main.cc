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
#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
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
                       int t0);

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

  registerProcessType( "Volume", "S16",
		       &OverVolumeDoIt< AimsData< int16_t > > );
  
  registerProcessType( "Volume", "FLOAT",
		     &OverVolumeDoIt< AimsData< float > > );


}



BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsOverVolume       -i[nput] <filein>                                   ",
  "                     -o[utput] <fileout>                                 ",
  "                    [--x0 <x0> --y0 <y0> --z0 <z0> --t0 <t0> ]           ",
  "                    --dx <dimx> --dy <dimy> --dz <dimz> --dt <dimT>       ",
  "                    [-h[elp]]                                            ",
  "-------------------------------------------------------------------------",
  "Change the dimension of an image.                                         ",
  "-------------------------------------------------------------------------",
  "     filein  : source volume                                             ",
  "     fileout : destination volume                                        ",
  "     dimx, dimy, dimz, dimT : size of the output volume                  ",
  "     x0, y0, z0, t0 : translation (int) of the input image (default = none)    ",
  "-------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}

int main(int argc, char **argv)
{

  char *filein, *fileout;
  int dx = 0, dy = 0, dz = 0,dt = 1;
  int x0 = 0 ,y0 = 0 ,z0 = 0, t0 = 0;

  const AimsOption opt[] = {
  { 'h',"help"    ,AIMS_OPT_FLAG  ,( void* )Usage      ,AIMS_OPT_CALLFUNC,0},
  { ' ',"dx"      ,AIMS_OPT_LONG  ,&dx        ,0                ,1},
  { ' ',"dy"      ,AIMS_OPT_LONG  ,&dy        ,0                ,1},
  { ' ',"dz"      ,AIMS_OPT_LONG  ,&dz        ,0                ,1},
  { ' ',"dt"      ,AIMS_OPT_LONG  ,&dt        ,0                ,0},
  { ' ',"x0"      ,AIMS_OPT_LONG ,&x0        ,0                ,0},
  { ' ',"y0"      ,AIMS_OPT_LONG ,&y0        ,0                ,0},
  { ' ',"z0"      ,AIMS_OPT_LONG ,&z0        ,0                ,0},
  { ' ',"z0"      ,AIMS_OPT_LONG ,&t0        ,0                ,0},
  { 'i',"input"   ,AIMS_OPT_STRING,&filein    ,0                ,1},
  { 'o',"output"  ,AIMS_OPT_STRING,&fileout   ,0                ,1},
  { 0  ,0         ,AIMS_OPT_END   ,0          ,0                ,0}};
  
  AimsParseOptions( &argc, argv, opt, usage );


   OverVolumeProcess process( fileout,
                              dx, dy, dz, dt,
                              x0, y0, z0, t0 );

   process.execute( filein );

  return EXIT_SUCCESS;


}
