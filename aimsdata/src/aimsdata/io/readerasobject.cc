// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/readerasobject.h>
#include <cartodata/volume/volume.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/bucket/bucket.h>
#include <aims/rgb/rgb.h>
#include <aims/math/dtitensor.h>
#include <graph/graph/graph.h>

using namespace aims;
using namespace carto;
using namespace std;

ReaderAsObject::ReaderAsObject( const string & fname )
  : Process(), _filename( fname )
{
  registerProcessType( "Volume", "S8", 
                       &ReaderAsObject::readAsObject<VolumeRef<int8_t> > );
  registerProcessType( "Volume", "U8", 
                       &ReaderAsObject::readAsObject<VolumeRef<uint8_t> > );
  registerProcessType( "Volume", "S16", 
                       &ReaderAsObject::readAsObject<VolumeRef<int16_t> > );
  registerProcessType( "Volume", "U16", 
                       &ReaderAsObject::readAsObject<VolumeRef<uint16_t> > );
  registerProcessType( "Volume", "S32", 
                       &ReaderAsObject::readAsObject<VolumeRef<int32_t> > );
  registerProcessType( "Volume", "U32", 
                       &ReaderAsObject::readAsObject<VolumeRef<uint32_t> > );
  registerProcessType( "Volume", "FLOAT", 
                       &ReaderAsObject::readAsObject<VolumeRef<float> > );
  registerProcessType( "Volume", "DOUBLE", 
                       &ReaderAsObject::readAsObject<VolumeRef<double> > );
  registerProcessType( "Volume", "RGB", 
                       &ReaderAsObject::readAsObject<VolumeRef<AimsRGB> > );
  registerProcessType( "Volume", "RGBA", 
                       &ReaderAsObject::readAsObject<VolumeRef<AimsRGBA> > );
  registerProcessType( "Volume", "CFLOAT", 
                       &ReaderAsObject::readAsObject<VolumeRef<cfloat> > );
  registerProcessType( "Volume", "CDOUBLE", 
                       &ReaderAsObject::readAsObject<VolumeRef<cdouble> > );
  registerProcessType( "Volume", "POINT3DF", 
                       &ReaderAsObject::readAsObject<VolumeRef<Point3df> > );
  registerProcessType( "Volume", DataTypeCode<Point3d>::name(), 
                       &ReaderAsObject::readAsObject<VolumeRef<Point3d> > );
  registerProcessType( "Volume", DataTypeCode<AimsVector<float,6> >::name(), 
                       &ReaderAsObject::readAsObject
                       <VolumeRef<AimsVector<float,6> > > );

  registerProcessType( "Mesh", "VOID", 
                       &ReaderAsObject::readAsObject<AimsSurfaceTriangle> );
  registerProcessType( "Mesh4", "VOID", 
                       &ReaderAsObject::readAsObject
                       <AimsTimeSurface<4,Void> > );
  registerProcessType( "Segments", "VOID", 
                       &ReaderAsObject::readAsObject
                       <AimsTimeSurface<2,Void> > );

  registerProcessType( "Texture", "FLOAT", 
                       &ReaderAsObject::readAsObject<TimeTexture<float> > );
  registerProcessType( "Texture", "S16", 
                       &ReaderAsObject::readAsObject<TimeTexture<int16_t> > );
  registerProcessType( "Texture", "S32", 
                       &ReaderAsObject::readAsObject<TimeTexture<int32_t> > );
  registerProcessType( "Texture", "U32", 
                       &ReaderAsObject::readAsObject<TimeTexture<uint32_t> > );
  registerProcessType( "Texture", DataTypeCode<Point2df>::name(), 
                       &ReaderAsObject::readAsObject<TimeTexture<Point2df> > );
  registerProcessType( "Texture", DataTypeCode<Point2d>::name(), 
                       &ReaderAsObject::readAsObject<TimeTexture<Point2d> > );

  registerProcessType( "Bucket", "VOID", 
                       &ReaderAsObject::readAsObject<BucketMap<Void> > );
  registerProcessType( "Bucket", "FLOAT", 
                       &ReaderAsObject::readAsObject<BucketMap<float> > );
  registerProcessType( "Bucket", "DOUBLE", 
                       &ReaderAsObject::readAsObject<BucketMap<double> > );
  registerProcessType( "Bucket", "S16", 
                       &ReaderAsObject::readAsObject<BucketMap<int16_t> > );
  registerProcessType( "Bucket", "DTITENSOR", 
                       &ReaderAsObject::readAsObject<BucketMap<DtiTensor> > );

  //registerProcessType( "Graph", "VOID", &ReaderAsObject::readAsObject<Graph> );

}


ReaderAsObject::~ReaderAsObject()
{
}


Object ReaderAsObject::read( int border, const string* format, int frame )
{
  _border = border;
  _format = format;
  _frame = frame;
  execute( _filename );
  return _result;
}


