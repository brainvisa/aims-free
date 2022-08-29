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
#include <aims/io/finder.h>
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/io/motionR.h>
#include <aims/io/writer.h>
#include <aims/vector/vector.h>
#include <aims/resampling/resampling_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <cartobase/smart/rcptr.h>
#include <iomanip>

using namespace aims;
using namespace carto;
using namespace std;


class Resamp : public Process
{
public:
  Resamp();

  string	motionfile;
  int		dx, dy, dz;
  Point3df	size;
  string	fout;
  string	resampler;
  string        defaultval;
};


template<class T> 
bool doit( Process & p, const string & fname, Finder & f )
{
  Resamp		& rp = (Resamp &) p;
  T                     dv;
  stringTo( rp.defaultval, dv );

  // resamplers map
  map<string, int>	resamplers;
  resamplers[ "n" ] = 0;
  resamplers[ "nearest" ] = 0;
  resamplers[ "0" ] = 0;
  resamplers[ "l" ] = 1;
  resamplers[ "linear" ] = 1;
  resamplers[ "1" ] = 1;
  resamplers[ "q" ] = 2;
  resamplers[ "quadratic" ] = 2;
  resamplers[ "2" ] = 2;
  resamplers[ "c" ] = 3;
  resamplers[ "cubic" ] = 3;
  resamplers[ "3" ] = 3;
  resamplers[ "quadric" ] = 4;
  resamplers[ "4" ] = 4;
  resamplers[ "quintic" ] = 5;
  resamplers[ "5" ] = 5;
  resamplers[ "six" ] = 6;
  resamplers[ "sixthorder" ] = 6;
  resamplers[ "6" ] = 6;
  resamplers[ "seven" ] = 7;
  resamplers[ "seventhorder" ] = 7;
  resamplers[ "7" ] = 7;
  resamplers[ "maj" ] = 101;
  resamplers[ "majority" ] = 101;
  resamplers[ "mef" ] = 201;
  resamplers[ "median" ] = 201;

  int order = 0;

  typename map<string, int>::iterator	i = resamplers.find( rp.resampler );
  if( i != resamplers.end() )
    order = i->second;
  else
  {
    // try another numeric value
    stringstream s( rp.resampler );
    try
    {
      s >> order;
    }
    catch( ... )
    {
      throw invalid_argument( "invalid resampler type: " + rp.resampler );
    }
    if( order == 0 )
      throw invalid_argument( "invalid resampler type: " + rp.resampler );
  }

  ResamplerFactory<T>	rf;
  rc_ptr<Resampler<T> >	resamp( rf.getResampler( order ) );
  VolumeRef<T>		data;
  string		format = f.format();
  Reader<Volume<T> >	r( fname );
  cout << "reading volume...\n";
  data.reset( r.read( 0, &format ) );
  if( data.isNull() )
    return false;

  cout << "done\n";

  Motion motion;
  if( ! rp.motionfile.empty() ) {
    MotionReader mReader( rp.motionfile );
    mReader.read( motion );
  }

  Resampler<T> & resampler = *resamp;
  resampler.setRef( data );
  resampler.setDefaultValue( dv );
  
  if ( rp.dx == 0 )
    rp.dx = data.getSizeX();
  if ( rp.dy == 0 )
    rp.dy = data.getSizeY();
  if ( rp.dz == 0 )
    rp.dz = data.getSizeZ();

  vector<float> vs = data.getVoxelSize();
  if ( rp.size[0] == 0 )
    rp.size[0] = vs[0];
  if ( rp.size[1] == 0 )
    rp.size[1] = vs[1];
  if ( rp.size[2] == 0 )
    rp.size[2] = vs[2];

  cout << "resampling...\n";
  VolumeRef<T> resampled = resampler.doit( motion, rp.dx, rp.dy, rp.dz,
                                           rp.size );

  cout << "done\n";

  Writer<VolumeRef<T> >	w( rp.fout );
  cout << "writing result...\n";
  return w.write( resampled );
}


Resamp::Resamp() 
  : Process(), dx( 0 ), dy( 0 ), dz( 0 )
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


int main( int argc, const char **argv )
{
  try {
    string	fileout, motionfile, rt = "l";
    int		dx = 0, dy = 0, dz = 0;
    Point3df	size(0, 0, 0);
    string	tr = "linear";
    Resamp	proc;
    ProcessInput	filein( proc );
    string 	referenceFile;
    string	defaultval = "0";
    bool force = false;
    
    AimsApplication	app(
      argc, argv,
      "This command is obsolete. Please use AimsApplyTransform instead. The "
      "same options and arguments will just work in most cases, except for "
      "the \"-d\" option, which should be replaced with \"--bg\" instead. "
      "This old command is now disabled, unless the \"--force\" option is "
      "passed."
    );
    app.addOption( filein, "-i", "source volume" );
    app.addOption( fileout, "-o", "destination volume" );
    app.addOption( motionfile, "-m", "motion file [default=identity]", true );
    app.addOption( dx, "--dx", "dimx of the resampled volume", true );
    app.addOption( dy, "--dy", "dimy of the resampled volume", true );
    app.addOption( dz, "--dz", "dimz of the resampled volume", true );
    app.addOption( size[0], "--sx", 
                   "voxel x dimension of the resampled volume", 
                   true );
    app.addOption( size[1], 
                   "--sy", "voxel y dimension of the resampled volume", 
                   true );
    app.addOption( size[2], 
                   "--sz", "voxel z dimension of the resampled volume", 
                   true );
    app.addOption( rt, "-t", 
                   "Type of interpolation used: n[earest], "
                   "l[inear], q[uadratic], c[cubic], quartic, quintic, "
                   "six[thorder], seven[thorder], maj[ority], med[ian] "
                   "[default=linear]. Modes may "
                   "also be specified as order number: 0=nearest, 1=linear... "
                   "Additional values: 101=majority, 201=median",
                   true );
    app.addOption( referenceFile, "-r", "Volume used to define output voxel "
                   "size and volume dimension (values are overrided by --dx, "
                   "--dy, --dz, --sx, --sy and --sz)",
                   true );
    app.addOption( defaultval, "-d", "Default value for borders [default=0]", true );
    app.addOption( force, "--force",
                   "force use of AimsResample despite its obsolescence (it "
                   "will refuse to work otherwise)", true );
    app.alias( "--input", "-i" );
    app.alias( "--output", "-o" );
    app.alias( "--motion", "-m" );
    app.alias( "--type", "-t" );
    app.alias( "--reference", "-r" );
    app.alias( "--defaultvalue", "-d" );
    app.initialize();

    if( !force )
    {
      cerr << "This command is obsolete and is disabled. Please use "
           << "AimsApplyTransform instead. The \"--force\" option will run it "
           << "anyway (thus re-enable it), you can use it if really needed "
           << "but the command will be removed in a future version of AIMS\n";
      return EXIT_FAILURE;
    }
    
    if ( ! referenceFile.empty() ) {
      Finder f;
      if( ! f.check( referenceFile ) ) {
        throw runtime_error( string( "cannot load " ) + referenceFile );
      }
      const PythonHeader *pheader = 
        static_cast<const PythonHeader *>( f.header() );
      vector< int > vd;
      if( pheader->getProperty( "volume_dimension", vd ) ) {
        if ( ! dx ) dx = vd[ 0 ];
        if ( ! dy ) dy = vd[ 1 ];
        if ( ! dz ) dz = vd[ 2 ];
      }
      else {
        throw runtime_error( string( "cannot get volume_dimenstion attribute "
                                     "of file " ) + referenceFile );
      }
      vector< float > vs;
      if( pheader->getProperty( "voxel_size", vs ) ) {
        if ( ! size[ 0 ] ) size[ 0 ] = vs[ 0 ];
        if ( ! size[ 1 ] ) size[ 1 ] = vs[ 1 ];
        if ( ! size[ 2 ] ) size[ 2 ] = vs[ 2 ];
      }
      else {
        throw runtime_error(  string( "cannot get voxel_size attribute of "
                                      "file " ) + referenceFile );
      }
    }

    proc.motionfile = motionfile;
    proc.fout = fileout;
    proc.dx = dx;
    proc.dy = dy;
    proc.dz = dz;
    proc.size = size;
    proc.resampler = rt;
    proc.defaultval = defaultval;

    proc.execute( filein.filename );

    return EXIT_SUCCESS;
  }
  catch( user_interruption & ) {}
  catch( exception & e ) {
    cerr << e.what() << endl;
  }
  return EXIT_FAILURE;
}
