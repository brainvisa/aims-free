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

/*
 *  Converts standard data formats
 */
/*	This new command 18/05/01 makes a bunch of older commands obsolete:
	AimsBckOpenMode
	AimsDicom2Gis
	AimsDicom2Vida
	AimsGis2Vida
	AimsMesh2Tri
	AimsMeshOpenMode
	AimsTri2Mesh
	AimsTypeConverter
	AimsVida2Gis
	AimsBucket2Mask
	AimsMask2Bucket

	and possibly in the near future some cross-object type conversions:
	AimsDtiBck2Vol
	AimsTensor2Bucket
	...
*/

#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/utility/converter_g.h>
#include <aims/mesh/surface.h>
#include <aims/bucket/bucket.h>
#include <aims/mesh/texture.h>
#include <aims/resampling/motion.h>
#include <aims/io/datatypecode.h>
#include <aims/math/dtitensor.h>
#include <aims/rgb/rgb.h>

using namespace aims;
using namespace carto;
using namespace std;

template<class T>
static bool readAndWrite( Process &, const string &, Finder & );
template<class T>
static bool convert( Process &, const string &, Finder & );


//	Processing class, with template operation function
class FileConvert : public Process
{
public:
  FileConvert();
  template<class T>
  friend bool readAndWrite( Process &, const string &, Finder & );
  template<class T>
  friend bool convert( Process &, const string &, Finder & );

  string	file;
  bool		ascii;
  string	form;
  string	type;
  string	otype;
  bool		rescale;
  unsigned	xdim;
  unsigned	ydim;
  unsigned	zdim;
  RescalerInfo  info;
};


FileConvert::FileConvert()
  : Process(), ascii( false ), rescale( false ), xdim( 0 ), ydim( 0 ), 
    zdim( 0 ), info()
{
  registerProcessType( "Volume", "S8", &convert<AimsData<int8_t> > );
  registerProcessType( "Volume", "U8", &convert<AimsData<uint8_t> > );
  registerProcessType( "Volume", "S16", &convert<AimsData<int16_t> > );
  registerProcessType( "Volume", "U16", &convert<AimsData<uint16_t> > );
  registerProcessType( "Volume", "S32", &convert<AimsData<int32_t> > );
  registerProcessType( "Volume", "U32", &convert<AimsData<uint32_t> > );
  registerProcessType( "Volume", "FLOAT", &convert<AimsData<float> > );
  registerProcessType( "Volume", "DOUBLE", &convert<AimsData<double> > );
  registerProcessType( "Volume", "CFLOAT", 
		       &readAndWrite<AimsData<cfloat> > );
  registerProcessType( "Volume", "CDOUBLE", 
		       &readAndWrite<AimsData<cdouble> > );
  registerProcessType( "Volume", "RGB", 
		       &convert<AimsData<AimsRGB> > );
  registerProcessType( "Volume", "RGBA", 
		       &convert<AimsData<AimsRGBA> > );
  registerProcessType( "Volume", "HSV", 
		       &convert<AimsData<AimsHSV> > );
  registerProcessType( "Volume", "POINT3DF", 
		       &readAndWrite<AimsData<Point3df> > );
  registerProcessType( "Volume", "VECTOR_OF_3_SHORT", 
		       &readAndWrite<AimsData<Point3d> > );
  registerProcessType( "Volume", "VECTOR_OF_6_FLOAT", 
		       &readAndWrite<AimsData<AimsVector<float,6> > > );
  registerProcessType( "Mesh", "VOID", 
		       &readAndWrite<AimsSurfaceTriangle> );
  registerProcessType( "Segments", "VOID", 
		       &readAndWrite<AimsTimeSurface<2, Void> > );
  registerProcessType( "Mesh4", "VOID", 
		       &readAndWrite<AimsSurfaceFacet> );
  registerProcessType( "Bucket", "VOID", 
		       &convert<BucketMap<Void> > );
  registerProcessType( "Bucket", "S16", 
		       &convert<BucketMap<int16_t> > );
  registerProcessType( "Bucket", "DTITENSOR", 
		       &readAndWrite<BucketMap<DtiTensor> > );
  registerProcessType( "Texture", "FLOAT", &convert<Texture1d> );
  registerProcessType( "Texture", "S16", &convert<TimeTexture<short> > );
  registerProcessType( "Texture", "S32", &convert<TimeTexture<int> > );
  registerProcessType( "Texture", "unsigned integer", 
		       &convert<TimeTexture<unsigned> > );
  registerProcessType( "Texture", "U32", &convert<TimeTexture<uint32_t> > );
  registerProcessType( "Texture", "POINT2DF", &readAndWrite<Texture2d> );
  registerProcessType( "Motion", "VOID", &readAndWrite<Motion> );
}


#if ( __GNUC__-0 >= 3 ) || ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 > 91 )
template<class T1, class U>
static bool convData( Process&, const string &, Finder & );
#endif

template<typename T>
class DataConverter : public Process
{
public:
  DataConverter( T & dat, const string & fileout, bool a, 
		 const string & format, bool resc, unsigned dx, unsigned dy, 
		 unsigned dz, RescalerInfo & info );

  template<class T1, class U>
  friend bool convData( Process&, const string &, Finder & );

 private:
#if ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 <= 91 )
  template<class U>
  static bool convInternal( Process&, const string &, Finder & );
#endif
  static bool noConvert( Process &, const string &, Finder & );

  T		& data;
  string	file;
  bool		ascii;
  const string	form;
  bool		rescale;
  unsigned	xdim;
  unsigned	ydim;
  unsigned	zdim;
  RescalerInfo  info;
};


template<typename T>
bool DataConverter<T>::noConvert( Process & p, const string &, Finder & )
{
  DataConverter	& dc = (DataConverter &) p;
  return( write( dc.data, dc.file, dc.ascii, dc.form ) );
}

// partial specializations

template<class T>
class DataConverter<AimsData<T> > : public Process
{
public:
  DataConverter( AimsData<T> & dat, const string & fileout, bool a, 
		 const string & format, bool resc, unsigned dx, unsigned dy, 
		 unsigned dz, RescalerInfo & info );

  template<class T1, class U>
  friend bool convData( Process&, const string &, Finder & );

 private:
#if ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 <= 91 )
  template<class U>
  static bool convInternal( Process&, const string &, Finder & );
#endif

  AimsData<T>	& data;
  string	file;
  bool		ascii;
  const string	form;
  bool		rescale;
  unsigned	xdim;
  unsigned	ydim;
  unsigned	zdim;
  RescalerInfo  info;
};

template<class T>
class DataConverter<TimeTexture<T> > : public Process
{
public:
  DataConverter( TimeTexture<T> & dat, const string & fileout, bool a, 
		 const string & format, bool resc, unsigned dx, unsigned dy, 
		 unsigned dz, RescalerInfo & info );

  template<class T1, class U>
  friend bool convData( Process&, const string &, Finder & );

 private:
#if ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 <= 91 )
  template<class U>
  static bool convInternal( Process&, const string &, Finder & );
#endif

  TimeTexture<T>	& data;
  string		file;
  bool			ascii;
  const string		form;
  bool			rescale;
  unsigned		xdim;
  unsigned		ydim;
  unsigned		zdim;
  RescalerInfo  info;
};


template<class T>
class DataConverter<BucketMap<T> > : public Process
{
public:
  DataConverter( BucketMap<T> & dat, const string & fileout, bool a, 
		 const string & format, bool resc, unsigned dx, unsigned dy, 
		 unsigned dz, RescalerInfo  & info );

  template<class T1, class U>
  friend bool convData( Process&, const string &, Finder & );

 private:
  BucketMap<T>		& data;
  string		file;
  bool			ascii;
  const string		form;
  bool			rescale;
  unsigned		xdim;
  unsigned		ydim;
  unsigned		zdim;
  RescalerInfo  & info;
};


//	definitions

template<class T>
DataConverter<AimsData<T> >::DataConverter( AimsData<T> & dat, 
					    const string & fileout,
					    bool a, const string & format, 
					    bool resc, unsigned dx, 
					    unsigned dy, unsigned dz, RescalerInfo & info )
  : Process(), data( dat ), file( fileout ), ascii( a ), form( format ),
    rescale( resc ), xdim( dx ), ydim( dy ), zdim( dz ), info(info)
{
#if ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 <= 91 )
  registerProcessType( "Volume", "S8", &convInternal<AimsData<int8_t> > );
  registerProcessType( "Volume", "U8", &convInternal<AimsData<uint8_t> > );
  registerProcessType( "Volume", "S16", &convInternal<AimsData<int16_t> > );
  registerProcessType( "Volume", "U16", &convInternal<AimsData<uint16_t> > );
  registerProcessType( "Volume", "S32", &convInternal<AimsData<int32_t> > );
  registerProcessType( "Volume", "U32", &convInternal<AimsData<uint32_t> > );
  registerProcessType( "Volume", "FLOAT", &convInternal<AimsData<float> > );
  registerProcessType( "Volume", "DOUBLE", &convInternal<AimsData<double> > );
  registerProcessType( "Volume", "RGB", &convInternal<AimsData<AimsRGB> > );
  registerProcessType( "Volume", "RGBA", &convInternal<AimsData<AimsRGBA> > );
  registerProcessType( "Volume", "HSV", &convInternal<AimsData<AimsHSV> > );
  registerProcessType( "Bucket", "VOID", &convInternal<BucketMap<Void> > );
  registerProcessType( "Bucket", "S16", &convInternal<BucketMap<int16_t> > );
#else
  registerProcessType( "Volume", "S8", 
		       &convData<AimsData<T>,AimsData<int8_t> > );
  registerProcessType( "Volume", "U8", 
		       &convData<AimsData<T>,AimsData<uint8_t> > );
  registerProcessType( "Volume", "S16", 
		       &convData<AimsData<T>,AimsData<int16_t> > );
  registerProcessType( "Volume", "U16", 
		       &convData<AimsData<T>,AimsData<uint16_t> > );
  registerProcessType( "Volume", "S32", 
		       &convData<AimsData<T>,AimsData<int32_t> > );
  registerProcessType( "Volume", "U32", 
		       &convData<AimsData<T>,AimsData<uint32_t> > );
  registerProcessType( "Volume", "FLOAT", 
		       &convData<AimsData<T>,AimsData<float> > );
  registerProcessType( "Volume", "DOUBLE", 
		       &convData<AimsData<T>,AimsData<double> > );
  registerProcessType( "Volume", "RGB", 
		       &convData<AimsData<T>,AimsData<AimsRGB> > );
  registerProcessType( "Volume", "RGBA", 
		       &convData<AimsData<T>,AimsData<AimsRGBA> > );
  registerProcessType( "Volume", "HSV", 
		       &convData<AimsData<T>,AimsData<AimsHSV> > );
  registerProcessType( "Bucket", "VOID", 
		       &convData<AimsData<T>,BucketMap<Void> > );
  registerProcessType( "Bucket", "S16", 
		       &convData<AimsData<T>,BucketMap<int16_t> > );
#endif
}


template<class T>
DataConverter<TimeTexture<T> >::DataConverter( TimeTexture<T> & dat, 
					       const string & fileout,
					       bool a, const string & format, 
					       bool resc, unsigned dx, 
					       unsigned dy, unsigned dz, RescalerInfo & info )
  : Process(), data( dat ), file( fileout ), ascii( a ), form( format ),
    rescale( resc ), xdim( dx ), ydim( dy ), zdim( dz ), info( info )
{
#if ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 <= 91 )
  registerProcessType( "Texture", "FLOAT", 
		       &convInternal<TimeTexture<float> > );
  registerProcessType( "Texture", "S16", &convInternal<TimeTexture<short> > );
  registerProcessType( "Texture", "integer", &convInternal<TimeTexture<int> > );
  registerProcessType( "Texture", "unsigned integer", 
		       &convInternal<TimeTexture<unsigned> > );
  registerProcessType( "Texture", "U32", 
		       &convInternal<TimeTexture<uint32_t> > );
#else
  registerProcessType( "Texture", "S16", 
		       &convData<TimeTexture<T>,TimeTexture<short> > );
  registerProcessType( "Texture", "integer", 
		       &convData<TimeTexture<T>,TimeTexture<int> > );
  registerProcessType( "Texture", "unsigned integer", 
		       &convData<TimeTexture<T>,TimeTexture<unsigned> > );
  registerProcessType( "Texture", "U32", 
		       &convData<TimeTexture<T>,TimeTexture<uint32_t> > );
  registerProcessType( "Texture", "FLOAT", 
		       &convData<TimeTexture<T>,TimeTexture<float> > );
#endif
}


template<typename T>
DataConverter<BucketMap<T> >::DataConverter( BucketMap<T> & dat, 
                                             const string & fileout,
                                             bool a, const string & format, 
                                             bool resc, unsigned dx, 
                                             unsigned dy, unsigned dz, RescalerInfo & info )
  : Process(), data( dat ), file( fileout ), ascii( a ), form( format ),
    rescale( resc ), xdim( dx ), ydim( dy ), zdim( dz ), info( info )
{
  registerProcessType( "Volume", "S16", 
		       &convData<BucketMap<T>,AimsData<short> > );
  registerProcessType( "Bucket", "VOID", 
                       &convData<BucketMap<T>,BucketMap<Void> > );
  registerProcessType( "Bucket", "S16", 
                       &convData<BucketMap<T>,BucketMap<int16_t> > );
}


template<class T>
bool read( T & data, const string & filename, const Finder & f, bool rw )
{
  Reader<T>	r( filename );
  if( rw )
    r.setAllocatorContext( AllocatorContext( AllocatorStrategy::InternalModif, 
                                             DataSource::none(), false, 
                                             0.01 ) );
  else
    r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly, 
                                             DataSource::none(), false, 
                                             0.01 ) );
  string	format = f.format();
  if( !r.read( data, 0, &format ) )
    return( false );
  cout << "reading done\n";
  return( true );
}


template<class T>
bool write( T & data, const string & file, bool ascii, const string & form )
{
  cout << "writing " << file << "...\n";
  Object options = Object::value( Dictionary() );
  options->setProperty( "exact_format", true );
  Writer<T>	w( file, options );
  const string	*wf = 0;
  if( !form.empty() )
    wf = &form;
  if( !w.write( data, ascii, wf ) )
    return( false );
  cout << "done\n";
  return( true );
}


template<class T>
bool readAndWrite( Process & p, const string & filename, Finder & f )
{
  FileConvert	&fc = (FileConvert &) p;
  T		data;
  if( !read( data, filename, f, filename == fc.file ) )
    return( false );

  return( write( data, fc.file, fc.ascii, fc.form ) );
}


template<class T>
bool convert( Process & p, const string & filename, Finder & f )
{
  FileConvert		&fc = (FileConvert &) p;
  T			data;
  //DataTypeCode<T>	dtc;
  cout << "reading " << filename << " as " << f.objectType() << " / " 
       << f.dataType() << "...\n";
  if( !read( data, filename, f, filename == fc.file ) )
    return( false );

  //	second layer of processes
  DataConverter<T>	proc( data, fc.file, fc.ascii, fc.form, fc.rescale, 
			      fc.xdim, fc.ydim, fc.zdim, fc.info );
  Finder		f2;
  if( fc.otype.empty() )
    f2.setObjectType( f.objectType() );
  else
    f2.setObjectType( fc.otype );
  if( fc.type.empty() )
    f2.setDataType( f.dataType() );
  else
    f2.setDataType( fc.type );
  //if( !fc.otype.empty() || !fc.type.empty() )
  cout << "convert to " << f2.objectType() << " / " << f2.dataType() << "\n";
  bool x = proc.execute( f2, "toto" );
  return( x );
}


template<class T> static 
void allocThing( T *& obj, unsigned, unsigned, unsigned, unsigned )
{
  obj = new T;
}


template<class T> static 
unsigned sizeT( T & )
{
  return( 1 );
}


template<class T> static 
void allocThing( AimsData<T> *& obj, unsigned xdim, unsigned ydim, 
		 unsigned zdim, unsigned tdim )
{
  obj = new AimsData<T>( xdim, ydim, zdim, tdim );
}


template<class T> static 
unsigned sizeT( AimsData<T> & obj )
{
  return( obj.dimT() );
}


template<class T> static 
unsigned sizeT( BucketMap<T> & obj )
{
  return( obj.size() );
}


template<class T> static 
unsigned sizeT( TimeTexture<T> & obj )
{
  return( obj.size() );
}


template<int D, class T> static 
void sizeT( AimsTimeSurface<D,T> & obj )
{
  return( obj.size() );
}


#if ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 <= 91 )
template<class T> template<class U>
bool DataConverter<T>::convInternal( Process& p, const string &, Finder & )
{
#else


template<class T,class U>
bool convData( Process & p, const string &, Finder & )
{
#endif
  DataConverter<T>	& dc = (DataConverter<T> &) p;
  cout << "converting data...\n";

  ShallowConverter<T,U>	conv( dc.rescale, dc.info );
  auto_ptr<U>		vol2;
  if( dc.xdim != 0 && dc.ydim != 0 && dc.zdim != 0 )
    {
      U		*data;
      allocThing( data, dc.xdim, dc.ydim, dc.zdim, sizeT( dc.data ) );
      vol2.reset( data );
      conv.convert( dc.data, *vol2 );
    }
  else
    vol2.reset( conv( dc.data ) );

  cout << "convert done\n";

  return( write( *vol2, dc.file, dc.ascii, dc.form ) );
}


int main( int argc, const char **argv )
{
  FileConvert	proc;
  ProcessInput	pi( proc );
  string	filein, fileout, format, type, otype;
  bool		ascii = false, rescale = false;
  unsigned	xdim = 0, ydim = 0, zdim = 0;

  AimsApplication	app( argc, argv, 
			     "Performs file format and data conversion" );
  app.addOption( pi, "-i", "input data filename to convert" );
  app.alias( "--input", "-i" );
  app.addOption( proc.file, "-o", "output filename" );
  app.alias( "--output", "-o" );
  app.addOption( proc.ascii, "-a", "write in ASCII mode if output format " 
                 "supports it (default: binary)", true );
  app.alias( "--ascii", "-a" );
  app.addOption( proc.rescale, "-r", "rescale output values to amplitude of " 
                 "output format (if output format is an integer variant)", 
                 true );
  app.alias( "--rescale", "-r" );
  app.addOption( proc.info.vmin, "--imin",
                 "specify input minimum value for rescaling.",
                 true );
  app.addOption( proc.info.vmax, "--imax", 
                 "specify input maximum value for rescaling.",
                 true );
  app.addOption( proc.info.omin, "--omin",
                 "specify output minimum value for rescaling.",
                 true );
  app.addOption( proc.info.omax, "--omax",
                 "specify output maximum value for rescaling.",
                 true );
  app.addOption( proc.info.usevtypelimits, "--itypelimits",
                 "uses input type limits instead of dynamic min max to rescale data dynamic.",
                 true );
  app.addOption( proc.form, "-f", "force a specific output format (GIS, "
		 "VIDA, ...) (default: guessed by the output filename " 
		 "extension)", true );
  app.alias( "--format", "-f" );
  app.addOption( proc.type, "-t", "output data type (only applicable to " 
                 "volumes or textures of scalars, default: same as input)", 
                 true );
  app.alias( "--type", "-t" );
  app.addOption( proc.otype, "-c", "change object type in addition to data " 
                 "type, currently only applicable to Bucket of VOID <-> " 
                 "Volume", true );
  app.alias( "--change", "-c" );
  app.alias( "--otype", "-c" );
  app.addOption( proc.xdim, "-x", "(for output volumes only) forces output " 
                 "volume dimension", true );
  app.alias( "--xdim", "-x" );
  app.addOption( proc.ydim, "-y", "(for output volumes only) forces output " 
                 "volume dimension", true );
  app.alias( "--ydim", "-y" );
  app.addOption( proc.zdim, "-z", "(for output volumes only) forces output " 
                 "volume dimension", true );
  app.alias( "--zdim", "-z" );

  try
    {
      app.initialize();

      //	Our specific process
      cout << "filein  : " << filein << endl;
      cout << "fileout : " << fileout << endl;
      cout << "ascii   : " << ascii << endl;
      cout << "rescale : " << rescale << endl;
      cout << "format  : " << format << endl;
      cout << "type    : " << type << endl;
      cout << "objtype : " << otype << endl;
      cout << "xdim    : " << xdim << endl;
      cout << "ydim    : " << ydim << endl;
      cout << "zdim    : " << zdim << endl;
      ostringstream vminstream;
      vminstream << (double)proc.info.vmin;
      cout << "imin    : " << (isnan<double>( proc.info.vmin ) ? "default" : vminstream.str()) << endl;
      ostringstream vmaxstream;
      vmaxstream << (double)proc.info.vmax;
      cout << "imax    : " << (isnan<double>( proc.info.vmax ) ? "default" : vmaxstream.str()) << endl;
      ostringstream ominstream;
      ominstream << (double)proc.info.omin;
      cout << "omin    : " << (isnan<double>( proc.info.omin ) ? "default" : ominstream.str()) << endl;
      ostringstream omaxstream;
      omaxstream << (double)proc.info.omax;
      cout << "omax    : " << (isnan<double>( proc.info.omax ) ? "default" : omaxstream.str()) << endl;

      //	Use the Process mechanism to switch on types
      if( !proc.execute( pi.filename ) )
	cout << "Couldn't convert file - aborted";
    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << argv[ 0 ] << ": " << e.what() << endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

