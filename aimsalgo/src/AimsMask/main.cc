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


#include <cmath>
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

// Process 1: read mask

template <typename M>
bool doitMask( Process &, const string &, Finder & );

class MaskProcess: public Process
{
public:
  MaskProcess():
    filein(),
    fileout(),
    doInverse(false),
    defaultval("0")
  {
    registerProcessType( "Volume", "S8",     &doitMask<int8_t> );
    registerProcessType( "Volume", "U8",     &doitMask<uint8_t> );
    registerProcessType( "Volume", "S16",    &doitMask<int16_t> );
    registerProcessType( "Volume", "U16",    &doitMask<uint16_t> );
    registerProcessType( "Volume", "S32",    &doitMask<int32_t> );
    registerProcessType( "Volume", "U32",    &doitMask<uint32_t> );
    registerProcessType( "Volume", "FLOAT",  &doitMask<float> );
    registerProcessType( "Volume", "DOUBLE", &doitMask<double> );
  }

  string  filein;
  string  fileout;
  bool    doInverse;
  string  defaultval;
};

// Process 2: read data

template <typename M, typename T>
bool doitData( Process &, const string &, Finder & );

template <typename M>
class DataProcess: public Process
{
public:
  DataProcess():
    fileout(),
    doInverse(false),
    defaultval("0"),
    vsData(1., 1., 1.),
    vsMask(1., 1., 1.),
    dimData(1, 1, 1),
    dimMask(1, 1, 1)
  {
    registerProcessType( "Volume", "S8",      &doitData<M,int8_t> );
    registerProcessType( "Volume", "U8",      &doitData<M,uint8_t> );
    registerProcessType( "Volume", "S16",     &doitData<M,int16_t> );
    registerProcessType( "Volume", "U16",     &doitData<M,uint16_t> );
    registerProcessType( "Volume", "S32",     &doitData<M,int32_t> );
    registerProcessType( "Volume", "U32",     &doitData<M,uint32_t> );
    registerProcessType( "Volume", "FLOAT",   &doitData<M,float> );
    registerProcessType( "Volume", "DOUBLE",  &doitData<M,double> );
    registerProcessType( "Volume", "RGB",     &doitData<M,AimsRGB> );
    registerProcessType( "Volume", "RGBA",    &doitData<M,AimsRGBA> );
  }

  Point3dl data2Mask( const Point3dl & ) const;

  string       fileout;
  bool         doInverse;
  string       defaultval;

  AimsData<M>  mask;
  Point3df     vsData;
  Point3df     vsMask;
  Point3dl     dimData;
  Point3dl     dimMask;
};

// Process 1: doit
template <typename M>
bool doitMask( Process & p, const string & fname, Finder & /*f*/ )
{
  MaskProcess & mp = (MaskProcess &) p;
  DataProcess<M> ip;
  ip.fileout = mp.fileout;
  ip.doInverse = mp.doInverse;
  ip.defaultval = mp.defaultval;

  Reader<AimsData<M> > maskReader( fname );
  cout << "reading mask..." << endl;
  maskReader >> ip.mask;
  cout << "done" << endl;
  ip.vsMask[0] = ip.mask.sizeX();
  ip.vsMask[1] = ip.mask.sizeY();
  ip.vsMask[2] = ip.mask.sizeZ();
  ip.dimMask[0] = ip.mask.dimX();
  ip.dimMask[1] = ip.mask.dimY();
  ip.dimMask[2] = ip.mask.dimZ();

  return ip.execute( mp.filein );
}

// Process 2: helper
template <typename M>
Point3dl DataProcess<M>::data2Mask( const Point3dl & p ) const
{
  Point3dl out(p);
  if( dimMask[0] != 1 || dimData[0] != 1 )
    out[0] = int( floor( ( float(p[0]) + .5 ) * vsData[0] / vsMask[0] ) );
  if( dimMask[1] != 1 || dimData[2] != 1 )
    out[1] = int( floor( ( float(p[1]) + .5 ) * vsData[1] / vsMask[1] ) );
  if( dimMask[2] != 1 || dimData[2] != 1 )
    out[2] = int( floor( ( float(p[2]) + .5 ) * vsData[2] / vsMask[2] ) );
  return out;
}

// Process 2: doit
template <typename M, typename T>
bool doitData( Process & p, const string & fname, Finder & f )
{
  DataProcess<M> & ip = (DataProcess<M> &) p;
  T dv;
  stringTo( ip.defaultval, dv );

  AimsData<T> data;
  string format = f.format();
  Reader<AimsData<T> > inputReader( fname );

  cout << "reading volume..." << endl;
  inputReader >> data;
  cout << "done" << endl;
  ip.vsData[0] = data.sizeX();
  ip.vsData[1] = data.sizeY();
  ip.vsData[2] = data.sizeZ();
  ip.dimData[0] = data.dimX();
  ip.dimData[1] = data.dimY();
  ip.dimData[2] = data.dimZ();

  int x, y, z, t;
  if( ip.doInverse )
  {
    ForEach4d( ip.mask, x, y, z, t )
    {
      if ( ip.mask(x, y, z, t) == 0 )
        ip.mask(x, y, z, t) = 1;
      else
        ip.mask(x, y, z, t) = 0;
    }
  }

  // Maskage
  aims::Progression progress(data.dimT() * data.dimZ() * data.dimY() * data.dimX() - 1);

  if (carto::verbose)
    std::cout << std::endl << "Masking progress: ";

  ForEach4d( data, x, y, z, t )
  {
    if (carto::verbose)
      std::cout << ++progress << std::flush;

    Point3dl pData(x, y, z);
    Point3dl pMask = ip.data2Mask( pData );
    int minT = min( t, ip.mask.dimT()-1 );

    if( 0 <= pMask[0] && pMask[0] < ip.mask.dimX() &&
        0 <= pMask[1] && pMask[1] < ip.mask.dimY() &&
        0 <= pMask[2] && pMask[2] < ip.mask.dimZ() )
    {
      if( ip.mask( pMask[0], pMask[1], pMask[2], minT ) == 0 )
        data( x, y, z, t ) = dv;
    }
    else if( !ip.doInverse )
      data( x, y, z, t ) = dv;
  }

  if (carto::verbose)
    std::cout << std::endl;

  Writer<AimsData<T> > writer( ip.fileout );
  return( writer.write( data ) );
}


int main( int argc, const char **argv )
{
  int result = EXIT_SUCCESS;
  try
  {

    MaskProcess proc;
    string filemask("");

    AimsApplication app( argc, argv, "Mask an image with another one" );
    app.addOption( proc.filein,  "-i", "origin file" );
    app.addOption( proc.fileout, "-o", "output file" );
    app.addOption( filemask, "-m", "mask" );
    app.addOption( proc.doInverse, "--inv", "use inverse mask image (default=no)" , true);
    app.addOption( proc.defaultval, "-d", "Default values for masked pixels [default=0]", true );
    app.initialize();

    if( !proc.execute( filemask ) )
    {
      cerr << "Could not process" << endl;
      result = EXIT_FAILURE;
    }

  }
  catch(user_interruption &e)
  {
  }
  catch(std::exception &e)
  {
    cerr<< argv[0] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }

  return result;
}
