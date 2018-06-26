/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

//--- aims -------------------------------------------------------------------
#include <aims/bucket/bucketMap.h>                                // BucketMap
#include <aims/data/data_g.h>                                      // AimsData
#include <aims/getopt/getopt2.h>                            // AimsApplication
#include <aims/getopt/getoptProcess.h>        // AimsApplication: ProcessInput
#include <aims/io/datatypecode.h>                           // DataTypeCode<T>
#include <aims/io/io_g.h>                          // Reader - Writer - Finder
#include <aims/mesh/surfaceOperation.h>            // SurfaceManip::meshVolume
#include <aims/transformation/affinetransformation3d.h> // AffineTransformation3d
#include <aims/transformation/transformation_chain.h> // TransformationChain3d
#include <aims/transform/transform_objects.h>
#include <aims/resampling/linearresampler.h>                // LinearResampler
#include <aims/resampling/cubicresampler.h>                  // CubicResampler
#include <aims/registration/ffd.h>                        // FfdTransformation
#include <aims/utility/progress.h>                       // aims::ProgressInfo
//--- carto ------------------------------------------------------------------
#include <cartobase/smart/rcptr.h>                                   // rc_ptr
#include <cartobase/object/object.h>                                 // Object
#include <cartobase/stream/fileutil.h>
#include <cartobase/exception/errno.h>
//--- std --------------------------------------------------------------------
#include <algorithm>
#include <iostream>
#include <exception>
#include <sstream>
//----------------------------------------------------------------------------

using namespace aims;
using namespace std;
using namespace carto;


class FFDApplyProc;

rc_ptr<Transformation3d>
load_transformation(const string &filename_arg,
                    const FFDApplyProc &ffdproc,
                    bool *successp,
                    const vector<int>& in_size,
                    const vector<float>& in_voxel_size);
rc_ptr<FfdTransformation>
load_ffd_deformation(const string &filename,
                     const FFDApplyProc &ffdproc,
                     bool *successp,
                     const vector<int>& in_size,
                     const vector<float>& in_voxel_size);
rc_ptr<Transformation3d> load_transformations(
  const FFDApplyProc& ffdproc,
  bool *successp,
  const std::vector<int>& in_size = std::vector<int>(),
  const std::vector<float>& in_voxel_size = std::vector<float>()
);
void convert_old_mode_deformation(rc_ptr<FfdTransformation> deformation,
                                  const std::vector<int>& in_size,
                                  const std::vector<float>& in_voxel_size);

struct FatalError : public runtime_error {
  explicit FatalError(const string& msg) : runtime_error(msg) {}
};

template <class T, class C>
bool doVolume( Process &, const string &, Finder & );
template <int D>
bool doMesh( Process &, const string &, Finder & );
bool doBucket( Process &, const string &, Finder & );
bool doBundles( Process &, const string &, Finder & );
bool doGraph( Process &, const string &, Finder & );

class FFDApplyProc : public Process
{
public:
  FFDApplyProc();

  string  inputref;
  vector<string> transform_list;
  string  affinemotion;
  string  output;
  string  type;
  string  reference;
  string  bucketout;
  string  gridout;
  string  compout;
  string  defaultval;
  int32_t dx;
  int32_t dy;
  int32_t dz;
  double  sx;
  double  sy;
  double  sz;
  bool old_mode;
  string vfinterp;
};


FFDApplyProc::FFDApplyProc()
  : Process(),
    defaultval("0"),
    dx(0), dy(0), dz(0),
    sx(0.), sy(0.), sz(0.),
    old_mode(false)
{
  registerProcessType( "Volume", "S8",      &doVolume<int8_t, int8_t> );
  registerProcessType( "Volume", "U8",      &doVolume<uint8_t, uint8_t> );
  registerProcessType( "Volume", "S16",     &doVolume<int16_t, int16_t> );
  registerProcessType( "Volume", "U16",     &doVolume<uint16_t, uint16_t> );
  registerProcessType( "Volume", "S32",     &doVolume<int32_t, int32_t> );
  registerProcessType( "Volume", "U32",     &doVolume<uint32_t, uint32_t> );
  registerProcessType( "Volume", "FLOAT",   &doVolume<float, float> );
  registerProcessType( "Volume", "DOUBLE",  &doVolume<double, double> );
  registerProcessType( "Volume", "RGB",     &doVolume<AimsRGB, AimsRGB::ChannelType> );
  registerProcessType( "Volume", "RGBA",    &doVolume<AimsRGBA, AimsRGBA::ChannelType> );
  registerProcessType( "Volume", "POINT3DF", &doVolume<Point3df, float> );
  registerProcessType( "Mesh",   "VOID",    &doMesh<3> );
  registerProcessType( "Mesh2",  "VOID",    &doMesh<2> );
  registerProcessType( "Mesh4",  "VOID",    &doMesh<4> );
  registerProcessType( "Bucket", "VOID",    &doBucket );
  registerProcessType( "Graph",  "VOID",    &doGraph );
  registerProcessType( "BundleMap", "VOID", &doBundles ); // just for doc
}


bool write_node_bucket(const FFDApplyProc& ffdproc,
                       const FfdTransformation& deformation);
bool doVolume_special_outputs(const FFDApplyProc &ffdproc,
                              const Transformation3d& transform);

template <class T, class C>
bool doVolume( Process & process, const string & fileref, Finder & )
{
  FFDApplyProc & ffdproc = (FFDApplyProc & ) process;

  cout << "Type " << DataTypeCode<T>::name()
                  << ", depth: " << toString(DataTypeInfo<T>::depth())
                  << ", samples: " << toString(DataTypeInfo<T>::samples()) << endl;


  //==========================================================================
  //
  //      Read input data
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  // Input image
  //--------------------------------------------------------------------------
  AimsData<T> in;
  aims::Reader<AimsData<T> > rdata( fileref );
  rdata >> in;

  //--------------------------------------------------------------------------
  // Background value
  //--------------------------------------------------------------------------
  // Parses the default background value to the used type
  T bv;
  stringTo( ffdproc.defaultval, bv );

  bool success = true;
  rc_ptr<Transformation3d> transform = load_transformations(
    ffdproc, &success, in.volume()->getSize(), in.volume()->getVoxelSize());

  //--------------------------------------------------------------------------
  // Output volume dimensions
  //--------------------------------------------------------------------------

  //--- Reference volume
  int rdx = 0, rdy = 0, rdz = 0;
  double rsx = 0., rsy = 0., rsz = 0.;
  if( !ffdproc.reference.empty() ) {
    Finder reffinder;
    if( !reffinder.check( ffdproc.reference ) ) {
      ostringstream s;
      s << "Failed to check the reference volume "
        << ffdproc.reference;
      throw FatalError(s.str());
    }
    Object refheader = reffinder.headerObject();
    try {
      rdx = refheader->getProperty( "volume_dimension" )->getArrayItem(0)->getScalar();
      rdy = refheader->getProperty( "volume_dimension" )->getArrayItem(1)->getScalar();
      rdz = refheader->getProperty( "volume_dimension" )->getArrayItem(2)->getScalar();
      rsx = refheader->getProperty( "voxel_size" )->getArrayItem(0)->getScalar();
      rsy = refheader->getProperty( "voxel_size" )->getArrayItem(1)->getScalar();
      rsz = refheader->getProperty( "voxel_size" )->getArrayItem(2)->getScalar();
    } catch(...) {
      ostringstream s;
      s << "Failed to retrieve volume_dimension or voxel_size from "
           "the reference volume "
        << ffdproc.reference;
      throw FatalError(s.str());
    }
  }

  //--- Dimensions and voxel size of the resampled volume: in decreasing order
  //--- of priority, use commandline flags, then the reference volume, then the
  //--- input volume.
  ffdproc.dx = ( ffdproc.dx > 0 ? ffdproc.dx : ( rdx > 0 ? rdx : in.dimX() ) );
  ffdproc.dy = ( ffdproc.dy > 0 ? ffdproc.dy : ( rdy > 0 ? rdy : in.dimY() ) );
  ffdproc.dz = ( ffdproc.dz > 0 ? ffdproc.dz : ( rdz > 0 ? rdz : in.dimZ() ) );
  ffdproc.sx = ( ffdproc.sx > 0 ? ffdproc.sx : ( rsx > 0 ? rsx : in.sizeX() ) );
  ffdproc.sy = ( ffdproc.sy > 0 ? ffdproc.sy : ( rsy > 0 ? rsy : in.sizeY() ) );
  ffdproc.sz = ( ffdproc.sz > 0 ? ffdproc.sz : ( rsz > 0 ? rsz : in.sizeZ() ) );

  cout << "Output dimensions: "
       << ffdproc.dx << ", "
       << ffdproc.dy << ", "
       << ffdproc.dz << endl;
  cout << "Output voxel size: "
       << ffdproc.sx << ", "
       << ffdproc.sy << ", "
       << ffdproc.sz << " mm" << endl;


  //==========================================================================
  //
  //      Prepare output volumes
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  // Output resampled volume
  //--------------------------------------------------------------------------
  AimsData<T> out( ffdproc.dx, ffdproc.dy, ffdproc.dz, in.dimT() );
  out.setSizeXYZT( ffdproc.sx, ffdproc.sy, ffdproc.sz, in.sizeT() );
  out = bv;

  //==========================================================================
  //
  //      Prepare resampler
  //
  //==========================================================================

  // Choose resampling type
  carto::rc_ptr<aims::Resampler<T> > rsp;
  if (ffdproc.type == "nearest" || ffdproc.type == "n") {
    rsp.reset(new NearestNeighborResampler<T>);
  } else if( ffdproc.type == "linear" || ffdproc.type == "l" ) {
    rsp.reset(new LinearResampler<T>);
  } else {
    rsp.reset(new CubicResampler<T>);
  }

  //==========================================================================
  //
  //      Prepare the composed transformation
  //
  //==========================================================================

  cout << "Initialization done." << endl;

  cout << "Resampling... ";
  rsp->resample_inv( in, *transform, bv, out, true );
  cout << endl;

  if(!ffdproc.compout.empty()
     || !ffdproc.gridout.empty()) {
    success = doVolume_special_outputs(ffdproc, *transform) && success;
  }

  //--------------------------------------------------------------------------
  // Output resampled volume
  //--------------------------------------------------------------------------
  Writer<AimsData<T> > w3(ffdproc.output);
  success = w3.write(out) && success;

  return success;
}


bool write_node_bucket(const FFDApplyProc& ffdproc,
                       const FfdTransformation& deformation)
{
  bool success = true;
  //--------------------------------------------------------------------------
  // Write bucket of ???
  //--------------------------------------------------------------------------
  // Create bucket (= nodes)  list
  BucketMap< Void > * subBucketMapVoid = 0;
  if( !ffdproc.bucketout.empty() ) {
    //--- Field of view (in which it is useful to compte a ffd motion)
    //
    // FIXME: assigning the voxel size for flat dimensions is almost certainly
    // wrong (fdx represents voxels, not mm)
    int32_t fdx, fdy, fdz;
    fdx = deformation.isFlat(0) ? ffdproc.sx : (int32_t)( double(deformation.dimX()) * deformation.sizeX() / ffdproc.sx );
    fdy = deformation.isFlat(1) ? ffdproc.sy : (int32_t)( double(deformation.dimY()) * deformation.sizeY() / ffdproc.sy );
    fdz = deformation.isFlat(2) ? ffdproc.sz : (int32_t)( double(deformation.dimZ()) * deformation.sizeZ() / ffdproc.sz );
    fdx = std::min( fdx, ffdproc.dx );
    fdy = std::min( fdy, ffdproc.dy );
    fdz = std::min( fdz, ffdproc.dz );

    subBucketMapVoid = new BucketMap< Void >;
    subBucketMapVoid->setSizeXYZT( ffdproc.sx,
                                   ffdproc.sy,
                                   ffdproc.sz,
                                   1);
    int dfx, dfy, dfz;
    int szx = deformation.dimX(), szy = deformation.dimY(),
      szz = deformation.dimZ();
    for(int k = 0; k < szz; ++k)
      for(int j = 0; j < szy; ++j)
        for(int i = 0; i < szx; ++i)
        {
          dfx = int( i*(float(fdx) / float((szx - 1)) ));
          dfy = int( j*(float(fdy) / float((szy - 1)) ));
          dfz = int( k*(float(fdz) / float((szz - 1)) ));

          subBucketMapVoid->insert( Point3d(dfx, dfy, dfz), Void() );
          subBucketMapVoid->insert( Point3d(dfx - 1, dfy, dfz), Void() );
          subBucketMapVoid->insert( Point3d(dfx + 1, dfy, dfz), Void() );
          subBucketMapVoid->insert( Point3d(dfx, dfy - 1, dfz), Void() );
          subBucketMapVoid->insert( Point3d(dfx, dfy + 1, dfz), Void() );
          subBucketMapVoid->insert( Point3d(dfx, dfy, dfz - 1), Void() );
          subBucketMapVoid->insert( Point3d(dfx, dfy, dfz + 1), Void() );
        }
    Writer< BucketMap< Void > > w2 ( ffdproc.bucketout );
    success = w2.write(*subBucketMapVoid) && success;
    delete subBucketMapVoid;
  }
  return success;
}


bool doVolume_special_outputs(const FFDApplyProc &ffdproc,
                              const Transformation3d& transform_chain)
{
  bool success = true;

  //--------------------------------------------------------------------------
  // Prepare output grid of deformations
  //--------------------------------------------------------------------------
  AimsData<float> grid;
  if( !ffdproc.gridout.empty() ) {
    grid = AimsData<float>( ffdproc.dx, ffdproc.dy, ffdproc.dz, 3 );
    grid.setSizeXYZT( ffdproc.sx, ffdproc.sy, ffdproc.sz, 1. );
    grid = 0.;
  }

  //--------------------------------------------------------------------------
  // Prepare output compression volume
  //--------------------------------------------------------------------------
  AimsData<float> compression;
  if( !ffdproc.compout.empty() ) {
    compression = AimsData<float>( ffdproc.dx, ffdproc.dy, ffdproc.dz );
    compression.setSizeXYZT( ffdproc.sx, ffdproc.sy, ffdproc.sz );
    compression = 0.;
  }

  Point3df size( ffdproc.sx, ffdproc.sy, ffdproc.sz );
  float v_unity = ffdproc.sx * ffdproc.sy * ffdproc.sz, v_deform;
  float dx = .5 * ffdproc.sx;
  float dy = .5 * ffdproc.sy;
  float dz = .5 * ffdproc.sz;

  Point3df v;
  Point3df p_ext;

  cout << "Computing special outputs (--grid and/or --compression)" << endl;

  if(!ffdproc.compout.empty()) {
    cout << "WARNING: the output of --compression is almost certainly wrong "
         << "when the output voxel size is different from 1mm. See FIXME "
         << "comments in the source code."
         << endl;
  }

  if( ffdproc.dz > 1)
    std::cout << "Progress: ";

  aims::Progression progress(ffdproc.dx * ffdproc.dy * ffdproc.dz);

  for( long z = 0; z < ffdproc.dz; ++z )
  for( long y = 0; y < ffdproc.dy; ++y )
  for( long x = 0; x < ffdproc.dx; ++x )
  {
    (++progress).print();

    Point3df porg(x, y, z);
    porg[0] *= ffdproc.sx;
    porg[1] *= ffdproc.sy;
    porg[2] *= ffdproc.sz;
    Point3df p = transform_chain.transform(porg);

    {
      p[0] /= ffdproc.sx;
      p[1] /= ffdproc.sy;
      p[2] /= ffdproc.sz;

      if( !ffdproc.gridout.empty() ) {
        grid( x, y, z, 0 ) = p[0] - x;
        grid( x, y, z, 1 ) = p[1] - y;
        grid( x, y, z, 2 ) = p[2] - z;
      }

      if( !ffdproc.compout.empty() )
      {
        // Generate a deformed mesh
        AimsSurfaceTriangle  vol;
        AimsSurface<3, Void>  & vo = vol[0];
        vector<Point3df>     ogv;
        vector<Point3df>     & vert = vo.vertex();
        vector< AimsVector<uint, 3> > & poly = vo.polygon();

        ogv.reserve( 8 );
        vert.reserve( 8 );
        poly.reserve( 12 );

        ogv.push_back( Point3df( x - dx, y - dy, z - dz ) );
        ogv.push_back( Point3df( x + dx, y - dy, z - dz ) );
        ogv.push_back( Point3df( x - dx, y + dy, z - dz ) );
        ogv.push_back( Point3df( x + dx, y + dy, z - dz ) );
        ogv.push_back( Point3df( x - dx, y - dy, z + dz ) );
        ogv.push_back( Point3df( x + dx, y - dy, z + dz ) );
        ogv.push_back( Point3df( x - dx, y + dy, z + dz ) );
        ogv.push_back( Point3df( x + dx, y + dy, z + dz ) );

        poly.push_back( AimsVector<uint,3>( 0, 2, 3 ) );
        poly.push_back( AimsVector<uint,3>( 0, 3, 1 ) );
        poly.push_back( AimsVector<uint,3>( 4, 7, 6 ) );
        poly.push_back( AimsVector<uint,3>( 4, 5, 7 ) );
        poly.push_back( AimsVector<uint,3>( 0, 6, 2 ) );
        poly.push_back( AimsVector<uint,3>( 0, 4, 6 ) );
        poly.push_back( AimsVector<uint,3>( 1, 3, 7 ) );
        poly.push_back( AimsVector<uint,3>( 1, 7, 5 ) );
        poly.push_back( AimsVector<uint,3>( 4, 0, 1 ) );
        poly.push_back( AimsVector<uint,3>( 4, 1, 5 ) );
        poly.push_back( AimsVector<uint,3>( 6, 3, 2 ) );
        poly.push_back( AimsVector<uint,3>( 6, 7, 3 ) );

        // Apply vertices deformation
        vector<Point3df>::iterator it, ie = ogv.end();
        for( it = ogv.begin(); it < ie; it++ )
        {
          v = *it;
          // FIXME: v is assumed to be in voxels, but it is not really the case
          // because it is the sum of an int voxel coordinate, and a float
          // representing half the voxel size
          v[0] *= size[0];
          v[1] *= size[1];
          v[2] *= size[2];
          v = transform_chain.transform( v );
          v[0] /= size[0];
          v[1] /= size[1];
          v[2] /= size[2];
          vert.push_back( v );
        }

        v_deform = SurfaceManip::meshVolume( vol );
        // FIXME: porg is in mm, so it is definitely not the correct variable
        // to use here for indexing the compression volume
        compression( porg[0], porg[1], porg[2] ) =  (-1) * 100 * ( v_unity - v_deform ) / v_unity;
      }
    }

  }

  cout << endl;

  //--------------------------------------------------------------------------
  // Output grid
  //--------------------------------------------------------------------------
  if( ! ffdproc.gridout.empty() )
  {
    Writer< AimsData<float> > w4(ffdproc.gridout);
    success = w4.write(grid) && success;
  }

  //--------------------------------------------------------------------------
  // Output compression
  //--------------------------------------------------------------------------
  if( !ffdproc.compout.empty() )
  {
    Writer< AimsData<float> > w5(ffdproc.compout);
    success = w5.write(compression) && success;
  }

  return success;
}


//
// MESH RESAMPLING
//

template <int D>
bool doMesh( Process & process, const string & fileref, Finder & )
{
  FFDApplyProc & ffdproc = (FFDApplyProc & ) process;

  cout << "Type Mesh <" << D << ">" << endl;

  //==========================================================================
  //
  //      Read input data
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  // Input mesh
  //--------------------------------------------------------------------------
  AimsTimeSurface<D, Void> in;
  aims::Reader<AimsTimeSurface<D, Void> > rdata( fileref );
  rdata >> in;

  //--------------------------------------------------------------------------
  // FFD motion
  //--------------------------------------------------------------------------
  bool success = true;
  rc_ptr<Transformation3d> transform = load_transformations(ffdproc, &success);

  //==========================================================================
  //
  //      Do resampling
  //
  //==========================================================================
  cout << "Resampling ";

  transformMesh( in, *transform );

  cout << endl;

  //==========================================================================
  //
  //      Write output data
  //
  //==========================================================================

  Writer<AimsTimeSurface<D, Void> > w3(ffdproc.output);
  bool res = w3.write(in);

  return res;
}


//
// BUCKET RESAMPLING
//

bool doBucket( Process & process, const string & fileref, Finder & )
{
  FFDApplyProc & ffdproc = (FFDApplyProc & ) process;

  cout << "Type Bucket" << endl;

  //==========================================================================
  //
  //      Read input data
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  // Input bucket
  //--------------------------------------------------------------------------
  BucketMap<Void> in;
  aims::Reader<BucketMap<Void> > rdata( fileref );
  rdata >> in;

  //--- Hard dimensions
  ffdproc.sx = ( ffdproc.sx > 0 ? ffdproc.sx : in.sizeX() );
  ffdproc.sy = ( ffdproc.sy > 0 ? ffdproc.sy : in.sizeY() );
  ffdproc.sz = ( ffdproc.sz > 0 ? ffdproc.sz : in.sizeZ() );

  //--------------------------------------------------------------------------
  // FFD motion
  //--------------------------------------------------------------------------
  bool success = true;
  rc_ptr<Transformation3d> transform = load_transformations(ffdproc, &success);

  //==========================================================================
  //
  //      Do resampling
  //
  //==========================================================================
  cout << "Resampling ";

  rc_ptr<BucketMap<Void> > out
    = transformBucket( in, *transform,
                       Point3df( ffdproc.sx, ffdproc.sy, ffdproc.sz ) );

  cout << endl;

  //==========================================================================
  //
  //      Write output data
  //
  //==========================================================================

  Writer<BucketMap<Void> > w3(ffdproc.output);
  bool res = w3.write(*out);

  return res;
}


//
// BUNDLES RESAMPLING
//

bool doBundles( Process & process, const string & fileref, Finder & )
{
  FFDApplyProc & ffdproc = (FFDApplyProc & ) process;

  cout << "Type Bundles" << endl;

  //==========================================================================
  //
  //      Read input data
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  // Input bundles reader
  //--------------------------------------------------------------------------
  aims::BundleReader rdata( fileref );

  //--------------------------------------------------------------------------
  // FFD motion
  //--------------------------------------------------------------------------
  bool success = true;
  rc_ptr<Transformation3d> transform = load_transformations(ffdproc, &success);

  //==========================================================================
  //
  //      Do read, resampling and write
  //
  //==========================================================================
  cout << "Resampling ";

  BundleTransformer btrans( transform );
  rdata.addBundleListener( btrans );
  BundleWriter bw;
  bw.setFileString( ffdproc.output );
  btrans.addBundleListener( bw );

  rdata.read();

  cout << endl;

  return true;
}

//
// GRAPH RESAMPLING
//

bool doGraph( Process & process, const string & fileref, Finder & f )
{
  // check bundles objects, which are reported as graphs by Finder
  // (since they can be read as graphs, which is the only data structure to
  // represent them internally)
  const PythonHeader *ph = dynamic_cast<const PythonHeader *>( f.header() );
  if( ph && ph->getProperty( "object_type" )->getString() == "BundleMap" )
    return doBundles( process, fileref, f );

  // other cases are "real" graphs

  FFDApplyProc & ffdproc = (FFDApplyProc & ) process;

  cout << "Type Graph" << endl;

  //==========================================================================
  //
  //      Read input data
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  // Input graph
  //--------------------------------------------------------------------------
  rc_ptr<Graph> in;
  aims::Reader<Graph> rdata( fileref );
  in.reset( rdata.read() );

  //--- Hard dimensions
  vector<float> vs;
  if( !in->getProperty( "voxel_size", vs ) )
    vs = vector<float>( 3, 0.f );

  if( vs.size() < 3 )
    vs.resize( 3, 1. );
  ffdproc.sx = ( ffdproc.sx > 0 ? ffdproc.sx : vs[0] );
  ffdproc.sy = ( ffdproc.sy > 0 ? ffdproc.sy : vs[1] );
  ffdproc.sz = ( ffdproc.sz > 0 ? ffdproc.sz : vs[2] );

  //--------------------------------------------------------------------------
  // FFD motion
  //--------------------------------------------------------------------------
  bool success = true;
  rc_ptr<Transformation3d> transform = load_transformations(ffdproc, &success);

  //==========================================================================
  //
  //      Do resampling
  //
  //==========================================================================
  cout << "Resampling ";

  transformGraph( *in, *transform,
                  Point3df( ffdproc.sx, ffdproc.sy, ffdproc.sz ) );

  cout << endl;

  //==========================================================================
  //
  //      Write output data
  //
  //==========================================================================

  Writer<Graph> w3(ffdproc.output);
  in->setProperty("filename_base", "*" );
  bool res = w3.write(*in);

  return res;
}


//
//  POINTS RESAMPLING
//

bool doPoints( FFDApplyProc & ffdproc, const string & filename )
{
  cout << "Type points\n";

  istream *s = 0;
  ifstream f;
  stringstream sf;
  if( FileUtil::fileStat( filename ).find( '+' ) != string::npos )
  {
    f.open( filename.c_str() );
    if( !f )
      throw errno_error();
    s = &f;
  }
  else
  {
    sf.str( filename );
    s = &sf;
  }

  //--------------------------------------------------------------------------
  // FFD motion
  //--------------------------------------------------------------------------
  bool success = true;
  rc_ptr<Transformation3d> transform = load_transformations(ffdproc, &success);

  vector<Point3df> transformed;

  while( !s->eof() && !s->fail() )
  {
    Point3df p( 1e38, 1e38, 1e38 ), q;
    (*s) >> p;
    if( p == Point3df( 1e38, 1e38, 1e38 ) )
      break;
    q = transform->transform( p );
    if( ffdproc.output.empty() )
      cout << "p : " << p << " -> " << q << endl;
    else
      transformed.push_back( q );
  }

  if( !ffdproc.output.empty() )
  {
    ofstream g( ffdproc.output.c_str() );
    vector<Point3df>::iterator ip, ep = transformed.end();
    for( ip=transformed.begin(); ip!=ep; ++ip )
      g << *ip << endl;
  }

  return true;
}


// FatalError is thrown if the transformations cannot be loaded properly. In
// case of a non-fatal error, the success parameter is set to false. When
// passed volume dimensions and voxel size in the in_size and in_voxel_size
// parameters, this function will work in "volume mode", which has two special
// behaviours:
// - it can read --old-mode deformation fields
// - it will invert the affine transformation passed as -m / --motion.
rc_ptr<Transformation3d>
load_transformations(const FFDApplyProc& ffdproc,
                     bool *successp,
                     const std::vector<int>& in_size,
                     const std::vector<float>& in_voxel_size)
{
  const bool volume_mode = !in_size.empty() || !in_voxel_size.empty();

  //--------------------------------------------------------------------------
  // Legacy affine motion
  //--------------------------------------------------------------------------
  rc_ptr<AffineTransformation3d> affine(new AffineTransformation3d);
  if( !ffdproc.affinemotion.empty() )
  {
    aims::Reader<AffineTransformation3d> rmotion(ffdproc.affinemotion);
    bool read_success = rmotion.read(*affine);
    if(!read_success) {
      ostringstream s;
      s << "Failed to load affine transformation from "
        << ffdproc.affinemotion << ", aborting.";
      throw FatalError(s.str());
    }
    if(volume_mode) {
      *affine = affine->inverse();
    }
  }
  cout << "Affine : " << *affine << endl;

  rc_ptr<TransformationChain3d> transform_chain(new TransformationChain3d);
  if(!affine->isIdentity()) {
    transform_chain->push_back(affine);
  }

  for(vector<string>::const_iterator filename_it = ffdproc.transform_list.begin();
      filename_it != ffdproc.transform_list.end();
      ++filename_it)
  {
    rc_ptr<Transformation3d> transform = load_transformation(
      *filename_it, ffdproc, successp, in_size, in_voxel_size
    );
    transform_chain->push_back(transform);
  }

  return rc_ptr<Transformation3d>(transform_chain.release());
}


rc_ptr<Transformation3d>
load_transformation(const string &filename_arg,
                    const FFDApplyProc &ffdproc,
                    bool *successp,
                    const vector<int>& in_size,
                    const vector<float>& in_voxel_size)
{
  bool invert;
  string filename;

  if(filename_arg.size() > 4
     && filename_arg.substr(filename_arg.size() - 4) == ":inv") {
    filename = filename_arg.substr(0, filename_arg.size() - 4);
    invert = true;
  } else {
    filename = filename_arg;
    invert = false;
  }

  Finder finder;
  if( !finder.check( filename ) ) {
    ostringstream s;
    s << "Failed to check the transformation file " << filename;
    throw FatalError(s.str());
  }

  if(finder.objectType() == "Volume" && finder.dataType() == "POINT3DF")
  {
    if(invert) {
      ostringstream s;
      s << "Cannot invert a deformation field )"
        << filename_arg << "), aborting.";
      throw FatalError(s.str());
    }
    rc_ptr<FfdTransformation> deformation = load_ffd_deformation(
      filename, ffdproc, successp, in_size, in_voxel_size
      );
    return rc_ptr<Transformation3d>(deformation.release());
  }
  else if(finder.objectType() == "AffineTransformation3d")
  {
    rc_ptr<AffineTransformation3d> affine(new AffineTransformation3d);
    aims::Reader<AffineTransformation3d> reader(filename);
    bool read_success = reader.read(*affine);
    if(!read_success) {
      ostringstream s;
      s << "Failed to load affine transformation from "
        << filename << ", aborting.";
      throw FatalError(s.str());
    }
    if(invert) {
      *affine = affine->inverse();
    }
    return rc_ptr<Transformation3d>(affine.release());
  }

  ostringstream s;
  s << "Unsupported object type " << finder.objectType()
    << "(of " << finder.dataType() << ") passed in " << filename;
  throw FatalError(s.str());
}


// Throws FatalError in case of failure
rc_ptr<FfdTransformation>
load_ffd_deformation(const string &filename,
                     const FFDApplyProc &ffdproc,
                     bool *successp,
                     const std::vector<int>& in_size,
                     const std::vector<float>& in_voxel_size)
{
  rc_ptr<FfdTransformation> deformation;

  if( ffdproc.vfinterp == "linear" || ffdproc.vfinterp == "l" )
    deformation.reset( new TrilinearFfd );
  else
    deformation.reset( new SplineFfd );

  aims::Reader<FfdTransformation> rdef(filename);
  bool read_success = rdef.read(*deformation);
  if(!read_success) {
    ostringstream s;
    s << "Failed to load a deformation field from "
      << filename << ", aborting.";
    throw FatalError(s.str());
  }

  if( ffdproc.old_mode )
  {
    convert_old_mode_deformation(deformation, in_size, in_voxel_size);
  }

  if( !ffdproc.bucketout.empty() ) {
    if( ffdproc.transform_list.size() == 1 ) {
      *successp = write_node_bucket(ffdproc, *deformation) && *successp;
    } else {
      clog << "Error: --bucket only works if a single transformation is used"
           << endl;
      *successp = false;
    }
  }

  return deformation;
}

// Before 2015, FFD deformations were in voxels of the input image.
// They are now stored in mm.
// A hint is that old ffd motions have a voxel size of 1. mm.
// Old motions can be used with old_mode set to true.
void convert_old_mode_deformation(rc_ptr<FfdTransformation> deformation,
                                  const std::vector<int>& in_size,
                                  const std::vector<float>& in_voxel_size)
{
  if(in_size.size() < 3 || in_voxel_size.size() < 3) {
    throw FatalError("--old-mode needs to determine the size and voxel size "
                     "of an input *volume*, it cannot work with other types "
                     "of inputs");
  }

  // The grid can be updated in-place
  AimsData<Point3df>& grid = static_cast<AimsData<Point3df>&>(*deformation);

  grid.setSizeXYZT(
    double(in_size[0] - 1) / double(grid.dimX() - 1) * in_voxel_size[0],
    double(in_size[1] - 1) / double(grid.dimY() - 1) * in_voxel_size[1],
    double(in_size[2] - 1) / double(grid.dimZ() - 1) * in_voxel_size[2] );
  int dx = grid.dimX(), dy = grid.dimY(), dz = grid.dimZ();
  float sx = grid.sizeX(), sy = grid.sizeY(), sz = grid.sizeZ();
  for( int z = 0; z < dz; ++z )
    for( int y = 0; y < dy; ++y )
      for( int x = 0; x < dx; ++x )
      {
        grid(x, y, z)[0] *= sx;
        grid(x, y, z)[1] *= sy;
        grid(x, y, z)[2] *= sz;
      }
}

int main( int argc, const char **argv )

{
  int result = EXIT_SUCCESS;

  try {
    FFDApplyProc ffdproc;
    ProcessInput  ffdpi( ffdproc );

    //
    // Collect arguments.
    //
    AimsApplication application(
      argc, argv,
      "Apply FFD (vector field) transformation on an image, a mesh, a 'bucket' "
      "(voxels list file), fiber tracts, a graph, or to points.\n"
      "\n"
      "Note that when resampling an image, the transformations must represent "
      "the inverse direction (for a destination point we seek the source "
      "position of the point), but for meshes, buckets, fibers, or points the "
      "transformations are applied directly (a vertex is moved according to "
      "the vector field). Similarly, the transformations are applied in the "
      "order that they are passed on the command-line. As a result:\n"
      "\n"
      "- when resampling an image, inverse transformations must be given, "
      "ordered from the target space to the input space. This also applies to "
      "affine transformations (this is the opposite convention to "
      "AimsResample, use the :inv suffix to invert affine transformations on "
      "the fly).\n"
      "\n"
      "- when resampling meshes, buckets, fibers, or points, the direct "
      "transformations should be given, ordered from the input space to the "
      "target space.\n"
      "\n"
      "\n"
      "In points mode, the -i options either specifies an ASCII file "
      "containing point coordinates, or is directly one or several points "
      "coordinates. Points shoud be in the shape (x, y, z) with parentheses.\n"
      "\n"
      "Note also that for meshes or points, the dimensions, voxel sizes, grid, "
      "reference, and resampling options are pointless and are unused. Only "
      "the vector field interpolation (--vi) option is used.\n\n"
      "In Buckets and graph mode, the options --sx, --sy, --sz allow to "
      "specify the output voxel size, but the reference (-r) is not used so "
      "far. The transformation field is applied independently on each bucket "
      "voxel with no resampling, so objects may end up with holes."
    );
    application.addOption( ffdpi, "-i", "Input image" );
    application.addOptionSeries( ffdproc.transform_list, "-d",
                                 "transformations to be applied. Multiple "
                                 "transformation will be composed in the "
                                 "order that they are passed on the "
                                 "command-line. Affine transformations may be "
                                 "suffixed with :inv to use their inverse.");
    application.addOption( ffdproc.affinemotion, "-m",
                           "Input affine transformation (deprecated: you can "
                           "now pass affine transformations with -d)",
                           true );
    application.addOption( ffdproc.type, "-t",
                           "Voxel values resampling type : n[earest], "
                           "l[inear], c[ubic] [default = cubic]", true );
    application.addOption( ffdproc.defaultval, "-bv",
                           "Background value to use", true );
    application.addOption( ffdproc.dx, "--dx",
                           "Output X dimension [default: same as input]",
                           true );
    application.addOption( ffdproc.dy, "--dy",
                           "Output Y dimension [default: same as input]",
                           true );
    application.addOption( ffdproc.dz, "--dz",
                           "Output Z dimension [default: same as input]",
                           true );
    application.addOption( ffdproc.sx, "--sx",
                           "Output X voxel size [default: same as input]",
                           true );
    application.addOption( ffdproc.sy, "--sy",
                           "Output Y voxel size [default: same as input]",
                           true );
    application.addOption( ffdproc.sz, "--sz",
                           "Output Z voxel size [default: same as input]",
                           true );
    application.addOption( ffdproc.reference, "-r", "Volume used to define "
                           "output voxel size and volume dimension (values "
                           "are overrided by --dx, --dy, --dz, --sx, --sy and "
                           "--sz)", true );
    application.addOption( ffdproc.output, "-o", "Output image" );
    application.addOption( ffdproc.bucketout, "-b", "Output bucket knots",
                           true );
    application.addOption( ffdproc.gridout, "-g",
                           "Output a deformation field corresponding to the "
                           "composed transformation", true );
    application.addOption( ffdproc.compout, "-c", "Output compression volume",
                           true );
    application.addOption( ffdproc.old_mode, "--old-mode", "Make this command "
                           "work with pre-2015 FFD motions (which are in "
                           "voxels of the input image, instead of millimetres)"
                           " [default: false]", true );
    application.addOption( ffdproc.vfinterp, "--vi", "Vector field "
                           "interpolation type: l[inear], c[ubic] "
                           "[default = cubic]", true );
    application.alias( "--input", "-i" );
    application.alias( "--transform", "-d" );
    application.alias( "--motion", "-m" );
    application.alias( "--type", "-t" );
    application.alias( "--reference", "-r" );
    application.alias( "--output", "-o" );
    application.alias( "--bucket", "-b" );
    application.alias( "--grid", "-g" );
    application.alias( "--compression", "-c" );
    application.alias( "--vectorinterpolation", "--vi" );
    application.initialize();

    bool ok = false;
    try {
      ok = ffdproc.execute( ffdpi.filename );
    } catch(const FatalError& exc) {
      clog << "Failed to run in File mode, will now try Points mode ("
           << exc.what() << ")."<< endl;
    }
    if( !ok )
    {
      ok = doPoints( ffdproc, ffdpi.filename );
    }
    if( !ok )
      result = EXIT_FAILURE;

  }
  catch( const FatalError &exc )
  {
    clog << exc.what() << endl;
    result = EXIT_FAILURE;
  }
  catch( user_interruption &e ) {
    result = EXIT_FAILURE;
  }
  catch( std::exception &e ) {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }
  return result;
}
