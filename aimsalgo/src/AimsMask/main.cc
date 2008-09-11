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


#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/rgb/rgb.h>
#include <aims/resampling/resampling_g.h>

using namespace aims;
using namespace carto;
using namespace std;


template<class T>
static bool doit( Process &, const string &, Finder & );


class Masker : public Process
{
public:
  Masker( const string & maskf, const string & fout, bool doInverse);

  template<class T>
  friend bool doit( Process &, const string &, Finder & );
  bool getdoInverse() {return _doInverse;}

private:
  string	filemask;
  string	fileout;
  bool _doInverse;
};


Masker::Masker( const string & maskf, const string & fout, bool doInverse ) 
  : Process(), filemask( maskf ), fileout( fout ), _doInverse(doInverse)
{
  registerProcessType( "Volume", "S8", &doit<int8_t> );
  registerProcessType( "Volume", "U8", &doit<uint8_t> );
  registerProcessType( "Volume", "S16", &doit<int16_t> );
  registerProcessType( "Volume", "U16", &doit<uint16_t> );
  registerProcessType( "Volume", "S32", &doit<int32_t> );
  registerProcessType( "Volume", "U32", &doit<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &doit<float> );
  registerProcessType( "Volume", "DOUBLE", &doit<double> );
  registerProcessType( "Volume", "RGB", &doit<AimsRGB> );
}


template<class T> bool 
doit( Process & p, const string & fname, Finder & f )
{
  Masker		& mp = (Masker &) p;
  AimsData<T>		data;
  string		format = f.format();
  Reader<AimsData<T> >	r( fname );
  cout << "reading volume...\n";
  if( !r.read( data, 0, &format ) )
    return( false );
  cout << "done\n";

  Reader<AimsData<short> > reader2( mp.filemask );
  AimsData<short> mask;
  if( !reader2.read( mask ) )
    return( false );

  int x,y,z,t;
  if( mp.getdoInverse() )
  {
  		ForEach4d( mask ,x ,y ,z ,t )
		{
    		if ( mask(x,y,z,t) == 0 )	mask(x,y,z,t) = 1;
      	else mask(x,y,z,t) = 0;
		}
  }   


// Resampling if differences of voxels sizes
  if( data.sizeX()!=mask.sizeX() ||
		data.sizeY()!=mask.sizeY() ||
		data.sizeZ()!=mask.sizeZ() ||
		data.sizeT()!=mask.sizeT() )
  {
		unsigned dimz = 1;
		if( data.dimZ()!=1 || mask.dimZ()!=1 ) 
		dimz = unsigned(mask.dimZ()*mask.sizeZ()/(1.0*data.sizeZ()) + .5);
		Motion identity;
		identity.setToIdentity();
		LinearResampler<short> reech;
	  	reech.setRef( mask );
	  	mask = reech.doit( identity, 
								 unsigned (mask.dimX()*mask.sizeX()/(1.0*data.sizeX()) + .5),
								 unsigned (mask.dimY()*mask.sizeY()/(1.0*data.sizeY()) + .5),
								 dimz,
 				      		 Point3df(data.sizeX(),data.sizeY(),data.sizeZ()));
  }


// Maskage...
  ForEach4d( data ,x ,y ,z ,t ) {
    int minT = min( t, mask.dimT()-1 );
    if ( x<mask.dimX() &&
         y<mask.dimY() &&
         z<mask.dimZ() &&
         mask(x,y,z,minT) == 0  
         )
      data( x, y, z, t ) = 0; 
  }

  Writer<AimsData<T> > writer( mp.fileout );
  return( writer.write( data ) );
}


int main( int argc, const char **argv )
{
  int result = EXIT_SUCCESS;

  try	{

  string filein = "", fileout = "", filemask = "";
  bool doInverse = false;


  AimsApplication app( argc, argv, "Mask an image with another one" );
  app.addOption( filein,  "-i", "origin file" );
  app.addOption( fileout, "-o", "output file" ); 
  app.addOption( filemask, "-m", "S16 mask" ); 
  app.addOption( doInverse, "--inv", "use inverse mask image (default=no)" ,true);
  app.initialize(); 


  Masker	proc( filemask, fileout, doInverse);
  if( !proc.execute( filein ) )
    {
      cerr << "Could not process\n";
      return( 1 );
    }


  }
  catch(user_interruption &e) 
  {
  }
  catch(std::exception &e)
  {
	cerr<<argv[0]<<":"<<e.what()<<endl;
   result = EXIT_FAILURE;
  }
  
  return result;
}
