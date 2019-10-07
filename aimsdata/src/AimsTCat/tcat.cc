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

#include <aims/getopt/getopt2.h>
#include <cartodata/volume/volume.h>
#include <aims/io/finder.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/data/pheader.h>
#include <aims/rgb/rgb.h>
#include <aims/mesh/surface.h>
#include <aims/bucket/bucket.h>
#include <aims/mesh/texture.h>
#include <soma-io/allocator/mappingcopy.h>

using namespace aims;
using namespace carto;
using namespace std;


template<class T>
static bool tcat( Process & p, const string & filename, Finder & finder );
template<class T>
static unsigned cat( T & out, const T & in, unsigned t );
template<class T>
static unsigned cat( Volume<T> & out, const Volume<T> & in, unsigned t );

class TCat : public Process
{
 public:
  TCat( const vector<string> & in, const string & out );
  virtual ~TCat() {}

  const vector<string>	& inputs;
  string		output;
};


TCat::TCat( const vector<string> & in, const string & out )
  : Process(), inputs( in ), output( out )
{
  registerProcessType( "Volume", "S8", &tcat<Volume<int8_t> > );
  registerProcessType( "Volume", "U8", &tcat<Volume<uint8_t> > );
  registerProcessType( "Volume", "S16", &tcat<Volume<int16_t> > );
  registerProcessType( "Volume", "U16", &tcat<Volume<uint16_t> > );
  registerProcessType( "Volume", "S32", &tcat<Volume<int32_t> > );
  registerProcessType( "Volume", "U32", &tcat<Volume<uint32_t> > );
  registerProcessType( "Volume", "FLOAT", &tcat<Volume<float> > );
  registerProcessType( "Volume", "DOUBLE", &tcat<Volume<double> > );
  registerProcessType( "Volume", "CFLOAT", &tcat<Volume<cfloat> > );
  registerProcessType( "Volume", "CDOUBLE", &tcat<Volume<cdouble> > );
  registerProcessType( "Volume", "RGB", &tcat<Volume<AimsRGB> > );
  registerProcessType( "Volume", "RGBA", &tcat<Volume<AimsRGBA> > );
  registerProcessType( "Volume", "POINT3DF", &tcat<Volume<Point3df> > );
  registerProcessType( "Mesh", "VOID", &tcat<AimsSurfaceTriangle> );
  registerProcessType( "Segments", "VOID", &tcat<AimsTimeSurface<2, Void> > );
  registerProcessType( "Mesh4", "VOID", &tcat<AimsSurfaceFacet> );
  registerProcessType( "Texture", "FLOAT", &tcat<Texture1d> );
  registerProcessType( "Texture", "S16", &tcat<TimeTexture<short> > );
  registerProcessType( "Texture", "U32", &tcat<TimeTexture<uint32_t> > );
  registerProcessType( "Texture", "POINT2DF", &tcat<Texture2d> );
  registerProcessType( "Texture", "POINT2D", &tcat<TimeTexture<Point2d> > );
  registerProcessType( "Bucket", "VOID", &tcat<BucketMap<Void> > );
}


template<class T> static
T* allocObject( T &, unsigned, TCat &, Finder & )
{
  return( new T );
}


template<class T> static
Volume<T>* allocObject( Volume<T> & in0, unsigned n,
                        TCat & /*tp*/, Finder & )
{
  /* AllocatorContext mwalloc 
    = AllocatorContext( AllocatorStrategy::ReadWrite, 
                        new FileDataSource( tp.output, 0, 
                                            DataSource::ReadWrite ), 
                                            ??diskok??, 0 ); */

  Volume<T>	*din = new Volume<T>( in0.getSizeX(), in0.getSizeY(),
                                      in0.getSizeZ(), n /*, 0, mwalloc*/ );

  din->copyHeaderFrom( in0.header() );

  return din;
}


template<>  /* static */
// static removed for gcc 4.3: see http://gcc.gnu.org/gcc-4.3/porting_to.html
BucketMap<Void>* allocObject( BucketMap<Void> &in0, unsigned, TCat &, 
			      Finder & )
{
  BucketMap<Void>	*obj = new BucketMap<Void>;
  obj->setSizeXYZT( in0.sizeX(), in0.sizeY(), in0.sizeZ(), in0.sizeT() );
  return( obj );
}


unsigned checkDims( Finder & f1, Finder & f2 )
{
  if( f1.objectType() != f2.objectType() || f1.dataType() != f2.dataType() )
    {
      cerr << "Differing file types : " << f2.objectType() << " / " 
	   << f2.dataType() << ", should be " << f1.objectType() << " / " 
	   << f1.dataType() << endl;
      return( 0 );
    }

  const PythonHeader 
    *ph1 = dynamic_cast<const PythonHeader *>( f1.header() );
  const PythonHeader 
    *ph2 = dynamic_cast<const PythonHeader *>( f2.header() );
  unsigned	nt = 1;

  if( !ph1 || !ph2 )
    {
      cerr << "header missing (can't be read ?)\n";
      return( 0 );
    }

  vector<int>	d1, d2;
  if( ph1->getProperty( "volume_dimension", d1 ) )
    {
      if( !ph2->getProperty( "volume_dimension", d2 ) )
	{
	  cerr << "missing dimensions\n";
	  return( 0 );
	}
      bool dimOk = false ;
      if( d1[0] == d2[0] )
        if( d1.size() >= 2 )
	  if( d1[1] == d2[1] )
	    if( d1.size() >= 3 )
	      if( d1[2] == d2[2] )
	        dimOk = true ;
	      else
	        dimOk = false ;
	    else
	      dimOk = true ;
	  else
	    dimOk = false ;
	else
	  dimOk = true ;
      else
        dimOk = false ;
	       
      if( !dimOk )
	{
	  cerr  << "differing dimensions : (" << d2[0] << ", " << d2[1] 
	        << ", " << d2[2] << ") instead of (" << d1[0] << ", " 
		<< d1[1] << ", " << d2[1] << ")\n";
	  return( 0 );
	}

      if( d2.size() >= 4 )
	nt = d2[3];
    }
  int	x, n;
  if( ph2->getProperty( "nb_t_pos", x ) )
    nt = x;

  vector<float>	vs1, vs2;
  if( ph1->getProperty( "voxel_size", vs1 ) )
    {
      if( !ph2->getProperty( "voxel_size", vs2 ) )
	cout << "warning, concatenated object has no voxel size" << endl;
      else 
      {
        while( vs1.size() < vs2.size() )
          vs1.push_back(1) ;
        while( vs1.size() > vs2.size() )
          vs2.push_back(1) ;
        for( x=0, n=vs1.size(); x<n; ++x )
          if( fabs( vs1[x] - vs2[x] ) > 1e-5 )
            cout << "warning, concatenated objects have differing voxel sizes" 
                 << endl;
      }
    }


  //Checking for specific Nuclear Medicine Specific fields
  int pc1, pc2;
  if( ph1->getProperty( "process_code", pc1 ) )
    {
      if( ph2->getProperty( "process_code", pc2 ) )
	{
	  if (pc1 != pc2)
	    {
	      cerr << "process_code discrepancy in the image serie.\n";
	      //Just a warning
	      //return( 0 );
	    }
	}
      else
	{
	  cerr << "Can't find process_code for one image. See HeaderInfo\n";
	  return( 0 );
	}
    }
  int ih1, ih2;
  if( ph1->getProperty( "isotope_halflife", ih1 ) )
    {
      if( ph2->getProperty( "isotope_halflife", ih2 ) )
	{
	  if (ih1 != ih2)
	    {
	      cerr << "isotope_halflife discrepancy in the image serie.\n";
	      return( 0 );
	    }
	}
      else
	{
	  cerr << "Can't find isotope_halflife for one image.See HeaderInfo\n";
	  return( 0 );
	}
    }

  return( nt );
}


template<class T>
static bool tcat( Process & p, const string &, Finder & finder )
{
  TCat		& tp = (TCat &) p;
  unsigned	n = tp.inputs.size(), i, t = 0, nt = 1;
  T		*in0;	// for now all data must be of the same type
  Reader<T>	r( tp.inputs[0] );
  string	format = finder.format();

  const PythonHeader 
    *h = dynamic_cast<const PythonHeader *>( finder.header() );
  if( h )
    {
      vector<int>	dim;
      if( h->getProperty( "volume_dimension", dim ) && dim.size() >= 4 )
	nt = dim[3];
      else
	{
	  int		x;
	  if( h->getProperty( "nb_t_pos", x ) )
	    nt = x;
	}
    }

  cout << "checking dimensions..." << endl;
  Finder	f2;
  for( i=1; i<n; ++i )
    {
      cout << tp.inputs[i] << "...\n";
      f2.check( tp.inputs[i] );
      t = checkDims( finder, f2 );
     if( !t )
	{
	  cerr << "Dimensions don't agree or files can't be read\n";
	  return( false );
	}
      nt += t;
    }
  cout << "OK\n";

  cout << "reading " << tp.inputs[0] << "...\n";
  r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly, 
                                           DataSource::none(), false, 0.01 ) );
  in0 = r.read( 0, &format );

  try
    {
      cout << "appending...\n";
      T		*din = allocObject( *in0, nt, tp, finder );
      cout << "OK\n";
      
      t = cat( *din, *in0, 0 );
      
      
      delete in0;
      
      for( i=1; i<n; ++i )
	{
	  cout << "reading " << tp.inputs[i] << endl;
	  T *in;	// for now all data must be of the same type
	  Reader<T>	r( tp.inputs[i] );
	  //	check dimensions ....
          r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly,
                                                   DataSource::none(), false, 
                                                   0.01 ) );
          in = r.read();
	  t = cat( *din, *in, t );
          delete in;
	}
      cout << "done\nWriting result...";
      
      Writer<T>	w( tp.output );
      int r = w.write( *din );
      delete din;
      return( r );
      
    }
  catch( std::exception &e )
    {
      cerr <<  "Debug : " << e.what() << endl;
      return EXIT_FAILURE;
    }
}


template<class T>
unsigned cat( Volume<T> & out, const Volume<T> & in, unsigned t0 )
{
  int	x, y, z, t;


  // Append information for nuclear medicine images
   int in_zst;
   in_zst = 0;
   vector<int> in_st, in_dt;
   const PropertySet &inph = in.header();

  if ( inph.getProperty("zero_start_time", in_zst) &&
       inph.getProperty("start_time", in_st) &&
       inph.getProperty("duration_time", in_dt) )
    {
      int out_zst;
      vector<int> out_st, out_dt;

      PropertySet &outph = out.header();

      // Check first file of the concatenation list
      if( !outph.getProperty("zero_start_time", out_zst) )
	{ // first file
	  outph.setProperty("zero_start_time", in_zst);
	  outph.setProperty("start_time", in_st);
	  outph.setProperty("duration_time", in_dt);
 	}
      else
	{ // following files
	  unsigned int i;
	  outph.getProperty("zero_start_time", out_zst);
	  outph.getProperty("start_time", out_st);
	  outph.getProperty("duration_time", out_dt);
	  for(i=0;i<in_st.size(); ++i)
	    {
	      out_st.push_back( in_st[i] + ( in_zst - out_zst)*1000 );
	      out_dt.push_back( in_dt[i] );
	    }
	  outph.setProperty("start_time", out_st);
	  outph.setProperty("duration_time", out_dt);
	}
    }


  vector<int> dims = in.getSize();
  int dx = dims[0], dy = dims[1], dz = dims[2], dt = dims[3];

  for( t=0; t<dt; ++t )
    for( z=0; z<dz; ++z )
      for( y=0; y<dy; ++y )
        for( x=0; x<dx; ++x )
          out.at( x, y, z, t0 + t ) = in.at( x, y, z, t );
  return( t0 + t );
}


template<class T>
unsigned cat( T & out, const T & in, unsigned )
{
  typename T::const_iterator	is;
  typename T::const_iterator	es = in.end();
  for( is=in.begin(); is!=es; ++is )
    out[out.size()] = is->second;
  return( out.size() );
}


int main( int argc, const char **argv )
{
  string		fileout;
  vector<string>	in;
  string		help = "Concatenates files along time.\n"
    "Works on homogen input data (volumes of S16, ...). Currently supports:\n";

  {
    TCat			pc( in, fileout );
    const std::map<std::string, std::map<std::string, Process::ProcFunc> > & 
      procs = pc.processTypes();
    std::map<std::string, std::map<std::string, Process::ProcFunc> >
      ::const_iterator	ipl, epl = procs.end();
    std::map<std::string, Process::ProcFunc>::const_iterator	ip, ep;
    unsigned	count = 0;

    for( ipl=procs.begin(); ipl!=epl; ++ipl )
      for( ip=ipl->second.begin(), ep=ipl->second.end(); ip!=ep; ++ip )
	{
	  if( count != 0 )
	    help += std::string( "\n" );
	  ++count;
	  help += ipl->first + " of " + ip->first;
	}
  }

  AimsApplication	app( argc, argv, help );
  app.addOption( fileout, "-o", "output data" );
  app.alias( "--output", "-o" );
  app.addOptionSeries( in, "-i", "input files", 1 );
  app.alias( "--input", "-i" );

  try
    {
      app.initialize();

      cout << "inputs : " << in.size() << endl;

      TCat	proc( in, fileout );
      if( !proc.execute( in[0] ) )
	{
	  cout << "Failure\n";
	  return EXIT_FAILURE;
	}
      cout << "OK\n";
    }
  catch( user_interruption &e )
    {
    }
  catch( std::exception &e )
    {
      cerr << argv[ 0 ] << ": " << e.what() << endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}


