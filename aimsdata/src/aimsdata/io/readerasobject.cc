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


