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
#include <aims/registration/ffd.h>  // SplineFfd - SplineFfdResampler
#include <aims/utility/progress.h>                       // aims::ProgressInfo
//--- carto ------------------------------------------------------------------
#include <cartobase/object/object.h>                                 // Object
#include <cartobase/stream/fileutil.h>
#include <cartobase/exception/errno.h>
//--- std --------------------------------------------------------------------
#include <cstdio>
#include <algorithm>
//----------------------------------------------------------------------------

using namespace aims;
using namespace std;
using namespace carto;

template <class T, class C>
bool doit( Process &, const string &, Finder & );
template <int D>
bool doMesh( Process &, const string &, Finder & );
bool doBucket( Process &, const string &, Finder & );
bool doBundles( Process &, const string &, Finder & );
bool doGraph( Process &, const string &, Finder & );

class FFDApplyProc : public Process
{
public:
  FFDApplyProc();

  template <class T, class C>
  friend bool doit( Process &, const string &, Finder & );

  string  inputref;
  string  inputmotion;
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


FFDApplyProc::FFDApplyProc() : Process(),
  dx(0), dy(0), dz(0),
  sx(0.), sy(0.), sz(0.),
  old_mode(false), defaultval("0")
{
  registerProcessType( "Volume", "S8",      &doit<int8_t, int8_t> );
  registerProcessType( "Volume", "U8",      &doit<uint8_t, uint8_t> );
  registerProcessType( "Volume", "S16",     &doit<int16_t, int16_t> );
  registerProcessType( "Volume", "U16",     &doit<uint16_t, uint16_t> );
  registerProcessType( "Volume", "S32",     &doit<int32_t, int32_t> );
  registerProcessType( "Volume", "U32",     &doit<uint32_t, uint32_t> );
  registerProcessType( "Volume", "FLOAT",   &doit<float, float> );
  registerProcessType( "Volume", "DOUBLE",  &doit<double, double> );
  registerProcessType( "Volume", "RGB",     &doit<AimsRGB, AimsRGB::ChannelType> );
  registerProcessType( "Volume", "RGBA",    &doit<AimsRGBA, AimsRGBA::ChannelType> );
  registerProcessType( "Volume", "POINT3DF", &doit<Point3df, float> );
  registerProcessType( "Mesh",   "VOID",    &doMesh<3> );
  registerProcessType( "Mesh2",  "VOID",    &doMesh<2> );
  registerProcessType( "Mesh4",  "VOID",    &doMesh<4> );
  registerProcessType( "Bucket", "VOID",    &doBucket );
  registerProcessType( "Graph",  "VOID",    &doGraph );
  registerProcessType( "BundleMap", "VOID", &doBundles ); // just for doc
}

template <class T, class C>
bool doit( Process & process, const string & fileref, Finder & )
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

  //--------------------------------------------------------------------------
  // FFD motion
  //--------------------------------------------------------------------------
  rc_ptr<FfdTransformation> deformation;
  if( ffdproc.vfinterp == "linear" || ffdproc.vfinterp == "l" )
    deformation.reset( new TrilinearFfd );
  else
    deformation.reset( new SplineFfd );

  // Before 2015, FFD deformations were in voxels. They are now stored in mm.
  // A hint is that old ffd motions have a voxel size of 1. mm.
  // Old motions can be used with old_mode set to true.
  if( ffdproc.old_mode )
  {
    AimsData<Point3df> grid;
    aims::Reader<AimsData<Point3df> > rdef(ffdproc.inputmotion);
    rdef >> grid;
    grid.setSizeXYZT(
      double(in.dimX() - 1) / double(grid.dimX() - 1) * in.sizeX(),
      double(in.dimY() - 1) / double(grid.dimY() - 1) * in.sizeY(),
      double(in.dimZ() - 1) / double(grid.dimZ() - 1) * in.sizeZ() );
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
    deformation->updateAllCtrlKnot(grid);
  }
  else
  {
    aims::Reader<FfdTransformation> rdef(ffdproc.inputmotion);
    rdef >> *deformation;
  }

  //--------------------------------------------------------------------------
  // Affine motion
  //--------------------------------------------------------------------------
  Motion affine;
  if( !ffdproc.affinemotion.empty() )
  {
    aims::Reader<Motion> rmotion(ffdproc.affinemotion);
    rmotion >> affine;
    affine = affine.inverse();
  }
  cout << "Affine : " << affine << endl;

  //--------------------------------------------------------------------------
  // Output volume dimensions
  //--------------------------------------------------------------------------

  //--- Reference volume
  int rdx = 0, rdy = 0, rdz = 0;
  double rsx = 0., rsy = 0., rsz = 0.;
  if( !ffdproc.reference.empty() ) {
    Finder reffinder;
    if( reffinder.check( ffdproc.reference ) ) {
      Object refheader = reffinder.headerObject();
      if( refheader->hasProperty( "volume_dimension" ) ) {
        rdx = refheader->getProperty( "volume_dimension" )->getArrayItem(0)->getScalar();
        rdy = refheader->getProperty( "volume_dimension" )->getArrayItem(1)->getScalar();
        rdz = refheader->getProperty( "volume_dimension" )->getArrayItem(2)->getScalar();
      }
      if( refheader->hasProperty( "voxel_size" ) ) {
        rsx = refheader->getProperty( "voxel_size" )->getArrayItem(0)->getScalar();
        rsy = refheader->getProperty( "voxel_size" )->getArrayItem(1)->getScalar();
        rsz = refheader->getProperty( "voxel_size" )->getArrayItem(2)->getScalar();
      }
    } else {
      cout << "Failed to check " << ffdproc.reference << endl;
      cout << "Continue anyway..." << endl;
    }
  }

  //--- Hard dimensions
  ffdproc.dx = ( ffdproc.dx > 0 ? ffdproc.dx : ( rdx > 0 ? rdx : in.dimX() ) );
  ffdproc.dy = ( ffdproc.dy > 0 ? ffdproc.dy : ( rdy > 0 ? rdy : in.dimY() ) );
  ffdproc.dz = ( ffdproc.dz > 0 ? ffdproc.dz : ( rdz > 0 ? rdz : in.dimZ() ) );
  ffdproc.sx = ( ffdproc.sx > 0 ? ffdproc.sx : ( rsx > 0 ? rsx : in.sizeX() ) );
  ffdproc.sy = ( ffdproc.sy > 0 ? ffdproc.sy : ( rsy > 0 ? rsy : in.sizeY() ) );
  ffdproc.sz = ( ffdproc.sz > 0 ? ffdproc.sz : ( rsz > 0 ? rsz : in.sizeZ() ) );

  //--- Field of view (in which it is useful to compte a ffd motion)
  int32_t fdx, fdy, fdz;
  fdx = deformation->isFlat(0) ? ffdproc.sx : (int32_t)( double(deformation->dimX()) * deformation->sizeX() / ffdproc.sx );
  fdy = deformation->isFlat(1) ? ffdproc.sy : (int32_t)( double(deformation->dimY()) * deformation->sizeY() / ffdproc.sy );
  fdz = deformation->isFlat(2) ? ffdproc.sz : (int32_t)( double(deformation->dimZ()) * deformation->sizeZ() / ffdproc.sz );
  fdx = std::min( fdx, ffdproc.dx );
  fdy = std::min( fdy, ffdproc.dy );
  fdz = std::min( fdz, ffdproc.dz );

  cout << "Output dimensions: "
       << ffdproc.dx << ", "
       << ffdproc.dy << ", "
       << ffdproc.dz << endl;
  cout << "Output voxel size: "
       << ffdproc.sx << ", "
       << ffdproc.sy << ", "
       << ffdproc.sz << endl;
  cout << "Output field of view: "
       << fdx << ", "
       << fdy << ", "
       << fdz << endl;


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

  //--------------------------------------------------------------------------
  // Output grid of deformations
  //--------------------------------------------------------------------------
  AimsData<float> grid;
  if( !ffdproc.gridout.empty() ) {
    grid = AimsData<float>( ffdproc.dx, ffdproc.dy, ffdproc.dz, 3 );
    grid.setSizeXYZT( ffdproc.sx, ffdproc.sy, ffdproc.sz, 1. );
    grid = 0.;
  }

  //--------------------------------------------------------------------------
  // Output compression volume
  //--------------------------------------------------------------------------
  AimsData<float> compression;
  if( !ffdproc.compout.empty() ) {
    compression = AimsData<float>( ffdproc.dx, ffdproc.dy, ffdproc.dz );
    compression.setSizeXYZT( ffdproc.sx, ffdproc.sy, ffdproc.sz );
    compression = 0.;
  }

  //==========================================================================
  //
  //      Prepare resampler
  //
  //==========================================================================

  // Choose resampling type
  carto::rc_ptr<FfdResampler<T> > rsp;
  if ((ffdproc.type == "nearest") || (ffdproc.type == "n")) {
    rsp = carto::rc_ptr<FfdResampler<T> >(
      new NearestNeighborFfdResampler<T, C>(*deformation, affine, bv)
    );
  }
  else if( ffdproc.type == "linear" || ffdproc.type == "l" )
  {
    rsp = carto::rc_ptr<FfdResampler<T> >(
      new TrilinearFfdResampler<T, C>(*deformation, affine, bv) );
  }
  else {
    rsp = carto::rc_ptr<FfdResampler<T> >(
      new SplineFfdResampler<T, C>(*deformation, affine, bv)
    );
  }
  rsp->setRef(in);

  //--------------------------------------------------------------------------
  // Write bucket of ???
  //--------------------------------------------------------------------------
  // Create bucket (= nodes)  list
  BucketMap< Void > * subBucketMapVoid = 0;
  if( !ffdproc.bucketout.empty() ) {
    subBucketMapVoid = new BucketMap< Void >;
    subBucketMapVoid->setSizeXYZT( ffdproc.sx,
                                   ffdproc.sy,
                                   ffdproc.sz,
                                   in.sizeT() );
    int dfx, dfy, dfz;
    int szx = deformation->dimX(), szy = deformation->dimY(),
      szz = deformation->dimZ();
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
    w2 << *subBucketMapVoid;
    delete subBucketMapVoid;
  }

  cout << "Initialization done." << endl;


  //==========================================================================
  //
  //      Do resampling
  //
  //==========================================================================
  cout << "Resampling ";

  Point3df size( ffdproc.sx, ffdproc.sy, ffdproc.sz );
  float v_unity = ffdproc.sx * ffdproc.sy * ffdproc.sz, v_deform;
  float dx = .5 * ffdproc.sx;
  float dy = .5 * ffdproc.sy;
  float dz = .5 * ffdproc.sz;

  Point3df v;
  Point3df p_ext;

  if( ffdproc.dz > 1)
    std::cout << "Progress: ";

  aims::Progression progress(ffdproc.dx * ffdproc.dy * ffdproc.dz * in.dimT());

  for( int t = 0; t < in.dimT(); ++t )
  for( long z = 0; z < ffdproc.dz; ++z )
  for( long y = 0; y < ffdproc.dy; ++y )
  for( long x = 0; x < ffdproc.dx; ++x )
  {
    (++progress).print();

    Point3df porg(x, y, z);
    porg[0] *= ffdproc.sx;
    porg[1] *= ffdproc.sy;
    porg[2] *= ffdproc.sz;
    Point3df p = rsp->resample( porg, out(x, y, z, t), t );

    if( t == 0 )
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
          if (! affine.isIdentity() )
          {
            // Apply inverted affine transformation
            v[0] *= size[0];
            v[1] *= size[1];
            v[2] *= size[2];
            v = affine.transform( v );
            v[0] /= size[0];
            v[1] /= size[1];
            v[2] /= size[2];
          }
          v = deformation->transform( v );
          vert.push_back( v );
        }

        v_deform = SurfaceManip::meshVolume( vol );
        compression( porg[0], porg[1], porg[2] ) =  (-1) * 100 * ( v_unity - v_deform ) / v_unity;
      }

    } // if t == 0
  }

  cout << endl;


  //==========================================================================
  //
  //      Write output data
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  // Output resampled volume
  //--------------------------------------------------------------------------
  Writer<AimsData<T> > w3(ffdproc.output);
  bool res = w3.write(out);

  //--------------------------------------------------------------------------
  // Output grid
  //--------------------------------------------------------------------------
  if( ! ffdproc.gridout.empty() )
  {
    Writer< AimsData<float> > w4(ffdproc.gridout);
    w4 << grid;
  }

  //--------------------------------------------------------------------------
  // Output compression
  //--------------------------------------------------------------------------
  if( !ffdproc.compout.empty() )
  {
    Writer< AimsData<float> > w5(ffdproc.compout);
    w5 << compression;
  }

  return res;
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
  rc_ptr<FfdTransformation> deformation;
  if( ffdproc.vfinterp == "linear" || ffdproc.vfinterp == "l" )
    deformation.reset( new TrilinearFfd );
  else
    deformation.reset( new SplineFfd );

  aims::Reader<FfdTransformation> rdef(ffdproc.inputmotion);
  rdef >> *deformation;

  //--------------------------------------------------------------------------
  // Affine motion
  //--------------------------------------------------------------------------
  Motion affine;
  if( !ffdproc.affinemotion.empty() )
  {
    aims::Reader<Motion> rmotion(ffdproc.affinemotion);
    rmotion >> affine;
  }
  cout << "Affine : " << affine << endl;

  //==========================================================================
  //
  //      Do resampling
  //
  //==========================================================================
  cout << "Resampling ";

  ffdTransformMesh( in, *deformation, affine );

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
  rc_ptr<FfdTransformation> deformation;
  if( ffdproc.vfinterp == "linear" || ffdproc.vfinterp == "l" )
    deformation.reset( new TrilinearFfd );
  else
    deformation.reset( new SplineFfd );

  aims::Reader<FfdTransformation> rdef(ffdproc.inputmotion);
  rdef >> *deformation;

  //--------------------------------------------------------------------------
  // Affine motion
  //--------------------------------------------------------------------------
  Motion affine;
  if( !ffdproc.affinemotion.empty() )
  {
    aims::Reader<Motion> rmotion(ffdproc.affinemotion);
    rmotion >> affine;
  }
  cout << "Affine : " << affine << endl;

  //==========================================================================
  //
  //      Do resampling
  //
  //==========================================================================
  cout << "Resampling ";

  rc_ptr<BucketMap<Void> > out
    = ffdTransformBucket( in, *deformation, affine,
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
  rc_ptr<FfdTransformation> deformation;
  if( ffdproc.vfinterp == "linear" || ffdproc.vfinterp == "l" )
    deformation.reset( new TrilinearFfd );
  else
    deformation.reset( new SplineFfd );

  aims::Reader<FfdTransformation> rdef(ffdproc.inputmotion);
  rdef >> *deformation;

  //--------------------------------------------------------------------------
  // Affine motion
  //--------------------------------------------------------------------------
  Motion affine;
  if( !ffdproc.affinemotion.empty() )
  {
    aims::Reader<Motion> rmotion(ffdproc.affinemotion);
    rmotion >> affine;
  }
  cout << "Affine : " << affine << endl;

  //==========================================================================
  //
  //      Do read, resampling and write
  //
  //==========================================================================
  cout << "Resampling ";

  BundleFFDTransformer btrans( deformation, affine );
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
  if( !in->getProperty( "voxel_size" ) )
    vs = vector<float>( 3, 0.f );
  ffdproc.sx = ( ffdproc.sx > 0 ? ffdproc.sx : vs[0] );
  ffdproc.sy = ( ffdproc.sy > 0 ? ffdproc.sy : vs[1] );
  ffdproc.sz = ( ffdproc.sz > 0 ? ffdproc.sz : vs[2] );

  //--------------------------------------------------------------------------
  // FFD motion
  //--------------------------------------------------------------------------
  rc_ptr<FfdTransformation> deformation;
  if( ffdproc.vfinterp == "linear" || ffdproc.vfinterp == "l" )
    deformation.reset( new TrilinearFfd );
  else
    deformation.reset( new SplineFfd );

  aims::Reader<FfdTransformation> rdef(ffdproc.inputmotion);
  rdef >> *deformation;

  //--------------------------------------------------------------------------
  // Affine motion
  //--------------------------------------------------------------------------
  Motion affine;
  if( !ffdproc.affinemotion.empty() )
  {
    aims::Reader<Motion> rmotion(ffdproc.affinemotion);
    rmotion >> affine;
  }
  cout << "Affine : " << affine << endl;

  //==========================================================================
  //
  //      Do resampling
  //
  //==========================================================================
  cout << "Resampling ";

  ffdTransformGraph( *in, *deformation, affine,
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
  rc_ptr<FfdTransformation> deformation;
  if( ffdproc.vfinterp == "linear" || ffdproc.vfinterp == "l" )
    deformation.reset( new TrilinearFfd );
  else
    deformation.reset( new SplineFfd );

  aims::Reader<FfdTransformation> rdef(ffdproc.inputmotion);
  rdef >> *deformation;

  //--------------------------------------------------------------------------
  // Affine motion
  //--------------------------------------------------------------------------
  Motion affine;
  if( !ffdproc.affinemotion.empty() )
  {
    aims::Reader<Motion> rmotion(ffdproc.affinemotion);
    rmotion >> affine;
  }
  cout << "Affine : " << affine << endl;

  bool idaffine = affine.isIdentity();
  vector<Point3df> transformed;

  while( !s->eof() && !s->fail() )
  {
    Point3df p( 1e38, 1e38, 1e38 ), q;
    (*s) >> p;
    if( p == Point3df( 1e38, 1e38, 1e38 ) )
      break;
    if( !idaffine )
      q = affine.transform( p );
    else
      q = p;
    q = deformation->transform( p );
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
      "Note that on images the vector field normally represents the inverse "
      "transformation (for a destination point we seek the source position of "
      "the point), but for meshes, buckets, fibers, or points the vector field "
      "is applied directly (a vertex is moved according to the vector field), "
      "so the transformation should be the inverse of the one applied to an "
      "image. Similarly, any affine transform given as -m option is from the "
      "source object to the input deformation space."
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
    application.addOption( ffdproc.inputmotion, "-d", "Input control knots grid", true );
    application.addOption( ffdproc.affinemotion, "-m", "Input affine transformation [Test_TO_Ref.trm]", true );
    application.addOption( ffdproc.type, "-t", "Voxel values resampling type : n[earest], l[inear], c[ubic] [default = cubic]", true );
    application.addOption( ffdproc.defaultval, "-bv", "Background value to use", true );
    application.addOption( ffdproc.dx, "--dx", "Output X dimension [default: same as input]", true );
    application.addOption( ffdproc.dy, "--dy", "Output Y dimension [default: same as input]", true );
    application.addOption( ffdproc.dz, "--dz", "Output Z dimension [default: same as input]", true );
    application.addOption( ffdproc.sx, "--sx", "Output X voxel size [default: same as input]", true );
    application.addOption( ffdproc.sy, "--sy", "Output Y voxel size [default: same as input]", true );
    application.addOption( ffdproc.sz, "--sz", "Output Z voxel size [default: same as input]", true );
    application.addOption( ffdproc.reference, "-r", "Volume used to define output voxel size and "
                                                    "volume dimension (values are overrided by "
                                                    "--dx, --dy, --dz, --sx, --sy and --sz)", true );
    application.addOption( ffdproc.output, "-o", "Output image" );
    application.addOption( ffdproc.bucketout, "-b", "Output bucket knots", true );
    application.addOption( ffdproc.gridout, "-g", "Output grid mesh", true );
    application.addOption( ffdproc.compout, "-c", "Output compression volume", true );
    application.addOption( ffdproc.old_mode, "--old-mode", "Make this command work with pre-2015 FFD motions (which are in voxels instead of millimetres) [default: false]", true );
    application.addOption( ffdproc.vfinterp, "--vi", "Vector field interpolation type: l[inear], c[ubic] [default = cubic]", true );
    application.alias( "--input", "-i" );
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
    try
    {
      ok = ffdproc.execute( ffdpi.filename );
    }
    catch( ... )
    {
    }
    if( !ok )
    {
      try
      {
        ok = doPoints( ffdproc, ffdpi.filename );
      }
      catch( ... )
      {
      }
    }
    if( !ok )
      result = EXIT_FAILURE;

  }
  catch( user_interruption &e ) {
  }
  catch( std::exception &e ) {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }
  return result;
}
