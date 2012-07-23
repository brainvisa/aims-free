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
#include <aims/data/headerutil.h>
#include <locale.h>

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
  int		encoding;
  bool		normal;
  string	form;
  string	type;
  string	otype;
  bool		rescale;
  unsigned	xdim;
  unsigned	ydim;
  unsigned	zdim;
  RescalerInfo  info;
  string        orient;
};


FileConvert::FileConvert()
  : Process(), encoding( 0 ), normal( false ), rescale( false ), xdim( 0 ),
    ydim( 0 ), zdim( 0 ), info()
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
  registerProcessType( "Bucket", "S32",
                       &convert<BucketMap<int32_t> > );
  registerProcessType( "Bucket", "U16",
                       &convert<BucketMap<uint16_t> > );
  registerProcessType( "Bucket", "U32",
                       &convert<BucketMap<uint32_t> > );
  registerProcessType( "Bucket", "FLOAT",
                       &convert<BucketMap<float> > );
  registerProcessType( "Bucket", "DOUBLE",
                       &convert<BucketMap<double> > );
  registerProcessType( "Bucket", "DTITENSOR", 
		       &readAndWrite<BucketMap<DtiTensor> > );
  registerProcessType( "Texture", "FLOAT", &convert<Texture1d> );
  registerProcessType( "Texture", "DOUBLE", &convert<TimeTexture<double> > );
  registerProcessType( "Texture", "S16", &convert<TimeTexture<short> > );
  registerProcessType( "Texture", "S32", &convert<TimeTexture<int> > );
  registerProcessType( "Texture", "unsigned integer", 
		       &convert<TimeTexture<unsigned> > );
  registerProcessType( "Texture", "U32", &convert<TimeTexture<uint32_t> > );
  registerProcessType( "Texture", "POINT2DF", &readAndWrite<Texture2d> );
  registerProcessType( "Motion", "VOID", &readAndWrite<Motion> );
}


template <typename T1, typename U>
static bool convData( Process &, const string &, Finder & );


template<typename T>
class DataConverter : public Process
{
public:
  DataConverter( T & dat, const string & fileout, bool a, bool n,
		 const string & format, bool resc, unsigned dx, unsigned dy, 
		 unsigned dz, RescalerInfo & info );

  template<class T1, class U>
  friend bool convData( Process&, const string &, Finder & );

 private:
  static bool noConvert( Process &, const string &, Finder & );

  T		& data;
  string	file;
  int		encoding;
  bool      normal;
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
  return( write( p, dc.data, dc.file, dc.encoding, dc.normal , dc.form ) );
}

// partial specializations

template<class T>
class DataConverter<AimsData<T> > : public Process
{
public:
  DataConverter( AimsData<T> & dat, const string & fileout, int enc, bool n,
		 const string & format, bool resc, unsigned dx, unsigned dy, 
		 unsigned dz, RescalerInfo & info );

  template<class T1, class U>
  friend bool convData( Process&, const string &, Finder & );

 private:
  AimsData<T>	& data;
  string	file;
  int		encoding;
  bool      normal;
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
  DataConverter( TimeTexture<T> & dat, const string & fileout, int enc, bool n,
		 const string & format, bool resc, unsigned dx, unsigned dy, 
		 unsigned dz, RescalerInfo & info );

  template<class T1, class U>
  friend bool convData( Process&, const string &, Finder & );

 private:
  TimeTexture<T>	& data;
  string		file;
  int			encoding;
  bool          normal;
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
  DataConverter( BucketMap<T> & dat, const string & fileout, int enc, bool n,
		 const string & format, bool resc, unsigned dx, unsigned dy, 
		 unsigned dz, RescalerInfo  & info );

  template<class T1, class U>
  friend bool convData( Process&, const string &, Finder & );

 private:
  BucketMap<T>		& data;
  string		file;
  int			encoding;
  bool          normal;
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
					    int enc, bool n, const string & format,
					    bool resc, unsigned dx, 
					    unsigned dy, unsigned dz, RescalerInfo & info )
  : Process(), data( dat ), file( fileout ), encoding( enc ), normal(n), form( format ),
    rescale( resc ), xdim( dx ), ydim( dy ), zdim( dz ), info(info)
{
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
  registerProcessType( "Bucket", "U16",
                       &convData<AimsData<T>,BucketMap<uint16_t> > );
  registerProcessType( "Bucket", "S32",
                       &convData<AimsData<T>,BucketMap<int32_t> > );
  registerProcessType( "Bucket", "U32",
                       &convData<AimsData<T>,BucketMap<uint32_t> > );
  registerProcessType( "Bucket", "FLOAT",
                       &convData<AimsData<T>,BucketMap<float> > );
  registerProcessType( "Bucket", "DOUBLE",
                       &convData<AimsData<T>,BucketMap<double> > );
}


template<class T>
DataConverter<TimeTexture<T> >::DataConverter( TimeTexture<T> & dat, 
					       const string & fileout,
					       int enc, bool n, const string & format,
					       bool resc, unsigned dx, 
					       unsigned dy, unsigned dz, RescalerInfo & info )
  : Process(), data( dat ), file( fileout ), encoding( enc ), normal( n ), form( format ),
    rescale( resc ), xdim( dx ), ydim( dy ), zdim( dz ), info( info )
{
  registerProcessType( "Texture", "S16", 
		       &convData<TimeTexture<T>,TimeTexture<short> > );
  registerProcessType( "Texture", "S32",
  		       &convData<TimeTexture<T>,TimeTexture<int> > );
  registerProcessType( "Texture", "integer", 
		       &convData<TimeTexture<T>,TimeTexture<int> > );
  registerProcessType( "Texture", "unsigned integer", 
		       &convData<TimeTexture<T>,TimeTexture<unsigned> > );
  registerProcessType( "Texture", "U32", 
		       &convData<TimeTexture<T>,TimeTexture<uint32_t> > );
  registerProcessType( "Texture", "FLOAT", 
		       &convData<TimeTexture<T>,TimeTexture<float> > );
  registerProcessType( "Texture", "DOUBLE",
                       &convData<TimeTexture<T>,TimeTexture<double> > );
}


template<typename T>
DataConverter<BucketMap<T> >::DataConverter( BucketMap<T> & dat, 
                                             const string & fileout,
                                             int enc, bool n, const string & format,
                                             bool resc, unsigned dx, 
                                             unsigned dy, unsigned dz, RescalerInfo & info )
  : Process(), data( dat ), file( fileout ), encoding( enc ), normal ( n ), form( format ),
    rescale( resc ), xdim( dx ), ydim( dy ), zdim( dz ), info( info )
{
  registerProcessType( "Volume", "S16", 
		       &convData<BucketMap<T>,AimsData<short> > );
  registerProcessType( "Volume", "U16",
                       &convData<BucketMap<T>,AimsData<uint16_t> > );
  registerProcessType( "Volume", "S32",
                       &convData<BucketMap<T>,AimsData<int32_t> > );
  registerProcessType( "Volume", "U32",
                       &convData<BucketMap<T>,AimsData<uint32_t> > );
  registerProcessType( "Volume", "FLOAT",
                       &convData<BucketMap<T>,AimsData<float> > );
  registerProcessType( "Volume", "DOUBLE",
                       &convData<BucketMap<T>,AimsData<double> > );
  registerProcessType( "Bucket", "VOID", 
                       &convData<BucketMap<T>,BucketMap<Void> > );
  registerProcessType( "Bucket", "S16", 
                       &convData<BucketMap<T>,BucketMap<int16_t> > );
  registerProcessType( "Bucket", "U16",
                       &convData<BucketMap<T>,BucketMap<uint16_t> > );
  registerProcessType( "Bucket", "S32",
                       &convData<BucketMap<T>,BucketMap<int32_t> > );
  registerProcessType( "Bucket", "U32",
                       &convData<BucketMap<T>,BucketMap<uint32_t> > );
  registerProcessType( "Bucket", "FLOAT",
                       &convData<BucketMap<T>,BucketMap<float> > );
  registerProcessType( "Bucket", "DOUBLE",
                       &convData<BucketMap<T>,BucketMap<double> > );
}


template <typename T>
Object getHeader( T & )
{
  return none();
}

template <typename T>
Object getHeader( AimsData<T> & data )
{
  return Object::reference( *dynamic_cast<GenericObject *>( data.header() ) );
}


template<class T>
bool read( Process & p, T & data, const string & filename, const Finder & f,
           bool rw, const string & orient )
{
  FileConvert	&fc = (FileConvert &) p;

  Reader<T>	r( filename );

  Object options = Object::value( Dictionary() );
  options->setProperty( "encoding", fc.encoding );
  options->setProperty( "normal", fc.normal );
  options->setProperty( "exact_format", true );
  r.setOptions(options);

  if( rw )
    r.setAllocatorContext( AllocatorContext( AllocatorStrategy::InternalModif, 
                                             DataSource::none(), false, 
                                             0.01 ) );
  else
    r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly, 
                                             DataSource::none(), false, 
                                             0.01 ) );
  string	format = f.format();

  if( !r.read( data, 0, &format) )
    return( false );

  cout << "reading done\n";
  cout << "orient: " << orient << endl;
  cout << "header: " << getHeader( data ).get() << endl;
  if( !orient.empty() )
    setOrientationInformation( getHeader( data ), orient );
  return( true );
}


template<class T>
bool write( Process & p, T & data, const string & file, int encoding, const string & form )
{
  FileConvert	&fc = (FileConvert &) p;

  cout << "writing " << file << "...\n";
  Object options = Object::value( Dictionary() );
  options->setProperty( "exact_format", true );
  options->setProperty( "normal", fc.normal );
  options->setProperty( "encoding", fc.encoding );

  Writer<T>	w( file, options );
  const string	*wf = 0;
  if( !form.empty() )
    wf = &form;

  w.setOptions(options);

  bool ascii;
  if (encoding == 1) ascii = true; else ascii = false;

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

  if( !read( p, data, filename, f, filename == fc.file, fc.orient ) )
    return( false );

  return( write( p, data, fc.file, fc.encoding, fc.form ) );
}


template<class T>
bool convert( Process & p, const string & filename, Finder & f )
{
  FileConvert		&fc = (FileConvert &) p;
  T			data;
  //DataTypeCode<T>	dtc;
  cout << "reading " << filename << " as " << f.objectType() << " / " 
       << f.dataType() << "...\n";
  if( !read( p, data, filename, f, filename == fc.file, fc.orient ) )
    return( false );

  //	second layer of processes
  DataConverter<T>	proc( data, fc.file, fc.encoding, fc.normal, fc.form, fc.rescale,
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


template<class T,class U>
bool convData( Process & p, const string &, Finder & )
{
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

  return( write( p, *vol2, dc.file, dc.encoding, dc.form ) );
}


int main( int argc, const char **argv )
{
  FileConvert	proc;
  ProcessInput	pi( proc );

//  bool		ascii = false, rescale = false;
//  unsigned	xdim = 0, ydim = 0, zdim = 0;

  AimsApplication	app( argc, argv, 
			     "Performs file format and data conversion" );
  app.addOption( pi, "-i", "input data filename to convert" );
  app.alias( "--input", "-i" );
  app.addOption( proc.file, "-o", "output filename" );
  app.alias( "--output", "-o" );

  app.addOption( proc.encoding, "-e", "set the data encoding for any output file.\n"
		  "0 : Binary (default)\n"
		  "1 : ASCII \n"
		  "2 : Base64 binary (for gifti format only)\n"
		   "3 : Base64 compressed binary (for gifti format only)\n", true );
  app.alias( "--encoding", "-e" );
  app.addOption( proc.normal, "-n", "write normal in output file",
                 true );
    app.alias( "--normal", "-n" );
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
  app.addOption( proc.orient, "--orient", "change/force output volume voxels "
      "orientation on disk (if the output format supports it), with different "
      "syntaxes:\n"
      "--orient \"flipx yflip\": flips orientations of x and y axes.\n"
      "--orient neuro or --orient neurological or --orient left-to-right: "
      "force neuro orientation.\n"
      "--orient radio or --orient radiological or --orient right-to-left: "
      "force radio orientation.\n"
      "--orient \"-1 1 -1\": set a flip matrix flipping x and z axes.\n"
      "--orient \"-1 0 0  0 1 0  0 0 -1\": same by specifying the full "
      "rotation matrix.\n"
      "--orient \"-1 0 0 255  0 1 0 0  0 0 -1 123  0 0 0 1\": same by "
      "specifying the full 4x4 matrix.", true );

  try
    {
	  app.initialize();

      setlocale(LC_ALL,"C");

      cout << "fileTestGifti  : " << endl;

      //	Our specific process
      cout << "filein  : " << pi.filename << endl;
      cout << "fileout : " << proc.file << endl;
      if (proc.encoding == 0)
    	  cout << "encoding: " << "Binary"  << endl;
      if (proc.encoding == 1)
          cout << "encoding: " << "ASCII"  << endl;
      if (proc.encoding == 2)
          cout << "encoding: " << "Base64 binary"  << endl;
      if (proc.encoding == 3)
          cout << "encoding: " << "Base64 compressed binary"  << endl;

      cout << "normal  : " << proc.normal << endl;
      cout << "rescale : " << proc.rescale << endl;
      cout << "format  : " << proc.form << endl;
      cout << "type    : " << proc.type << endl;
      cout << "objtype : " << proc.otype << endl;
      cout << "xdim    : " << proc.xdim << endl;
      cout << "ydim    : " << proc.ydim << endl;
      cout << "zdim    : " << proc.zdim << endl;
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
      //cout << "filename process  : " << pi.filename << endl;

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

