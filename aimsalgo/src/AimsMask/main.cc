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
#include <cartobase/config/verbose.h>
#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/utility/utility_g.h>
#include <aims/rgb/rgb.h>
#include <aims/resampling/resampling_g.h>
#include <aims/utility/progress.h>

using namespace aims;
using namespace carto;
using namespace std;

template<class T>
static bool doit( Process &, const string &, Finder & );


class Masker : public Process
{
public:
  Masker( const string & maskf, const string & fout, bool doInverse, string defaultval);

  template<class T>
  friend bool doit( Process &, const string &, Finder & );
  bool getdoInverse() {return _doInverse;}

private:
  string	filemask;
  string	fileout;
  bool          _doInverse;
  string        _defaultval;
};


Masker::Masker( const string & maskf, const string & fout, bool doInverse, string defaultval ) 
  : Process(), filemask( maskf ), fileout( fout ), _doInverse(doInverse), _defaultval(defaultval)
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
  registerProcessType( "Volume", "RGBA", &doit<AimsRGBA> );
}


template<class T> bool 
doit( Process & p, const string & fname, Finder & f )
{
  Masker		& mp = (Masker &) p;
  T                     dv;
  stringTo( mp._defaultval, dv );

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
      if ( mask(x,y,z,t) == 0 ) mask(x,y,z,t) = 1;
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
		NearestNeighborResampler<short> reech;
    reech.setRef( mask );
    mask = reech.doit( identity, 
                       unsigned (mask.dimX()*mask.sizeX()/(1.0*data.sizeX()) + .5),
                       unsigned (mask.dimY()*mask.sizeY()/(1.0*data.sizeY()) + .5),
                       dimz,
                       Point3df(data.sizeX(),data.sizeY(),data.sizeZ()));
  }

  // Maskage
  aims::Progression progress(data.dimT() * data.dimZ() * data.dimY() * data.dimX() - 1);
  
  if (carto::verbose)
    std::cout << std::endl << "Masking progress: ";
  
  ForEach4d( data ,x ,y ,z ,t ) {
    if (carto::verbose){
      ++progress;
      // Display progression
      std::cout << progress << std::flush;
    }
    int minT = min( t, mask.dimT()-1 );
    if( x<mask.dimX() && y<mask.dimY() && z<mask.dimZ() ) {
      if( mask(x,y,z,minT) == 0 )
        data( x, y, z, t ) = dv;
    } else if( !mp.getdoInverse() )
      data( x, y, z, t ) = dv;
  }

  if (carto::verbose)
    std::cout << std::endl;

  Writer<AimsData<T> > writer( mp.fileout );
  return( writer.write( data ) );
}


int main( int argc, const char **argv )
{
  int result = EXIT_SUCCESS;

  try	{

  string filein = "", fileout = "", filemask = "";
  bool doInverse = false;
  string defaultval = "0";


  AimsApplication app( argc, argv, "Mask an image with another one" );
  app.addOption( filein,  "-i", "origin file" );
  app.addOption( fileout, "-o", "output file" ); 
  app.addOption( filemask, "-m", "S16 mask" ); 
  app.addOption( doInverse, "--inv", "use inverse mask image (default=no)" ,true);
  app.addOption( defaultval, "-d", "Default values for masked pixels [default=0]", true );
  app.initialize(); 


  Masker	proc( filemask, fileout, doInverse, defaultval );
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
