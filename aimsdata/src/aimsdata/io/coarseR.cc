// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/coarseR.h>
#include <aims/io/finder.h>
#include <aims/io/reader.h>
#include <aims/io/process.h>
#include <aims/utility/converter_volume.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace aims
{
  namespace internal
  {
    class CoarseConverter : public Process
    {
    public:
      CoarseConverter( int b, int frm, VolumeRef<int16_t> & d )
	: Process(), border( b ), frame( frm ), data( d ) {}
      virtual ~CoarseConverter() {}
      int		border;
      int		frame;
      VolumeRef<int16_t>	& data;
    };
  }
}

CoarseReader::CoarseReader( const std::string & f )
  : _filename( f )
{
}


CoarseReader::~CoarseReader()
{
}


static bool direct( Process & p, const string & fname, Finder & f )
{
  string			fmt = f.format();
  Reader<VolumeRef<int16_t> >	r( fname );
  aims::internal::CoarseConverter & cc = (aims::internal::CoarseConverter &) p;
  return( r.read( cc.data, cc.border, &fmt, cc.frame ) );
}


template<typename T>
static bool convert( Process & p, const string & fname, Finder & f )
{
  string			fmt = f.format();
  Reader<T>			r( fname );
  aims::internal::CoarseConverter	& cc = (aims::internal::CoarseConverter &) p;
  T				data;
  r.read( data, cc.border, &fmt, cc.frame );
  cc.data = VolumeRef<int16_t>( data.getSize() );
  ShallowConverter<T, VolumeRef<int16_t> >	conv;
  conv.convert( data, cc.data );
  return( true );
}


void CoarseReader::read( VolumeRef<int16_t> & data, int border,
                         const std::string*, int frame )
{
  internal::CoarseConverter	p( border, frame, data );
  p.registerProcessType( "Volume", "S16", &direct );
  p.registerProcessType( "Volume", "S8", &convert<VolumeRef<int8_t> > );
  p.registerProcessType( "Volume", "U8", &convert<VolumeRef<uint8_t> > );
  p.registerProcessType( "Volume", "U16", &convert<VolumeRef<uint16_t> > );
  p.registerProcessType( "Volume", "S32", &convert<VolumeRef<int32_t> > );
  p.registerProcessType( "Volume", "U32", &convert<VolumeRef<uint32_t> > );
  p.registerProcessType( "Volume", "FLOAT", &convert<VolumeRef<float> > );
  p.registerProcessType( "Volume", "DOUBLE", &convert<VolumeRef<double> > );

  // ### remove after everything has been moved to intN_t/uintN_t
  p.registerProcessType( "Volume", "S8", &convert<VolumeRef<char> > );
  p.registerProcessType( "Volume", "S64", &convert<VolumeRef<int64_t> > );
  p.registerProcessType( "Volume", "U64", &convert<VolumeRef<uint64_t> > );

  p.execute( _filename );
}
