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

struct ApplyTransformProc;

struct FatalError : public runtime_error {
  explicit FatalError(const string& msg) : runtime_error(msg) {}
};

template <class T, class C>
bool doVolume(Process &, const string &, Finder &);
template <int D>
bool doMesh(Process &, const string &, Finder &);
bool doBucket(Process &, const string &, Finder &);
bool doBundles(Process &, const string &, Finder &);
bool doGraph(Process &, const string &, Finder &);


struct ApplyTransformProc : public Process
{
public:
  ApplyTransformProc();

  string  output;
  vector<string> direct_transform_list;
  vector<string> inverse_transform_list;
  string  interp_type;
  string  background_value;
  string  reference;
  int32_t dx;
  int32_t dy;
  int32_t dz;
  double  sx;
  double  sy;
  double  sz;
  string  vfinterp; // TODO find a good way of choosing the vector field interpolation (URL param?)
};


ApplyTransformProc::ApplyTransformProc()
  : Process(),
    background_value("0"),
    dx(0), dy(0), dz(0),
    sx(0.), sy(0.), sz(0.)
{
  registerProcessType("Volume", "S8",      &doVolume<int8_t, int8_t>);
  registerProcessType("Volume", "U8",      &doVolume<uint8_t, uint8_t>);
  registerProcessType("Volume", "S16",     &doVolume<int16_t, int16_t>);
  registerProcessType("Volume", "U16",     &doVolume<uint16_t, uint16_t>);
  registerProcessType("Volume", "S32",     &doVolume<int32_t, int32_t>);
  registerProcessType("Volume", "U32",     &doVolume<uint32_t, uint32_t>);
  registerProcessType("Volume", "FLOAT",   &doVolume<float, float>);
  registerProcessType("Volume", "DOUBLE",  &doVolume<double, double>);
  registerProcessType("Volume", "RGB",     &doVolume<AimsRGB, AimsRGB::ChannelType>);
  registerProcessType("Volume", "RGBA",    &doVolume<AimsRGBA, AimsRGBA::ChannelType>);
  registerProcessType("Volume", "POINT3DF", &doVolume<Point3df, float>);
  registerProcessType("Mesh",   "VOID",    &doMesh<3>);
  registerProcessType("Mesh2",  "VOID",    &doMesh<2>);
  registerProcessType("Mesh4",  "VOID",    &doMesh<4>);
  registerProcessType("Bucket", "VOID",    &doBucket);
  registerProcessType("Graph",  "VOID",    &doGraph);

  // just for doc (bundles are handled by a special case in doGraph)
  registerProcessType("BundleMap", "VOID", &doBundles);
}


template<class T>
unique_ptr<Resampler<T> >
get_resampler(const std::string& interp_type)
{
  map<string, int> resamplers;
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

  typename map<string, int>::iterator	i = resamplers.find( interp_type );
  if( i == resamplers.end() ) {
    throw invalid_argument( "invalid resampler type: " + interp_type );
  }
  return ResamplerFactory<T>::getResampler( i->second );
}


// Throws FatalError in case of failure
rc_ptr<FfdTransformation>
load_ffd_deformation(const string &filename,
                     const ApplyTransformProc &proc)
{
  rc_ptr<FfdTransformation> deformation;

  // TODO find a better way of choosing the vector field interpolation (URL param?)
  if(proc.vfinterp == "cubic" || proc.vfinterp == "c")
    deformation.reset(new SplineFfd);
  else
    deformation.reset(new TrilinearFfd);

  aims::Reader<FfdTransformation> rdef(filename);
  bool read_success = rdef.read(*deformation);
  if(!read_success) {
    ostringstream s;
    s << "Failed to load a deformation field from "
      << filename << ", aborting.";
    throw FatalError(s.str());
  }

  return deformation;
}


rc_ptr<Transformation3d>
load_transformation(const string &filename_arg,
                    const ApplyTransformProc &proc)
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
  if(!finder.check(filename)) {
    ostringstream s;
    s << "Failed to check the transformation file " << filename;
    throw FatalError(s.str());
  }

  if(finder.objectType() == "Volume" && finder.dataType() == "POINT3DF")
  {
    if(invert) {
      ostringstream s;
      s << "Cannot invert a deformation field)"
        << filename_arg << "), aborting.";
      throw FatalError(s.str());
    }
    rc_ptr<FfdTransformation> deformation = load_ffd_deformation(
      filename, proc
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


// FatalError is thrown if the transformations cannot be loaded properly.
std::pair<rc_ptr<Transformation3d>, rc_ptr<Transformation3d> >
load_transformations(const ApplyTransformProc& proc)
{
  std::pair<rc_ptr<Transformation3d>, rc_ptr<Transformation3d> > ret;

  if(!proc.direct_transform_list.empty()) {
    ret.first.reset(new TransformationChain3d);
    TransformationChain3d & direct_chain
      = dynamic_cast<TransformationChain3d&>(*ret.first);

    for(vector<string>::const_iterator filename_it = proc.direct_transform_list.begin();
        filename_it != proc.direct_transform_list.end();
        ++filename_it)
    {
      rc_ptr<Transformation3d> transform
        = load_transformation(*filename_it, proc);
      direct_chain.push_back(transform);
    }
  }

  if(!proc.inverse_transform_list.empty()) {
    ret.second.reset(new TransformationChain3d);
    TransformationChain3d & inverse_chain
      = dynamic_cast<TransformationChain3d&>(*ret.second);

    for(vector<string>::const_iterator filename_it = proc.inverse_transform_list.begin();
        filename_it != proc.inverse_transform_list.end();
        ++filename_it)
    {
      rc_ptr<Transformation3d> transform
        = load_transformation(*filename_it, proc);
      inverse_chain.push_back(transform);
    }
  }


  if(!ret.first && !ret.second) {
    clog << "No transformation provided, identity will be used." << endl;
    ret.first.reset(new TransformationChain3d);
    ret.second.reset(new TransformationChain3d);
  }

  if(!ret.first && ret.second->invertible()) {
    ret.first = ret.second->getInverse();
  }

  if(!ret.second && ret.first->invertible()) {
    ret.second = ret.first->getInverse();
  }

  return ret;
}


template <class T, class C>
bool doVolume(Process & process, const string & fileref, Finder &)
{
  ApplyTransformProc & proc = (ApplyTransformProc &) process;

  // Read the input image
  Volume<T> input_image;
  aims::Reader<Volume<T> > input_reader(fileref);
  input_reader.read(input_image);

  // Parse the background value to the data type
  T background_value;
  stringTo(proc.background_value, background_value);

  // Load the transformation
  rc_ptr<Transformation3d> inverse_transform;
  {
    std::pair<rc_ptr<Transformation3d>, rc_ptr<Transformation3d> > transforms
      = load_transformations(proc);
    inverse_transform = transforms.second;
    if(!inverse_transform) {
      clog << "Error: no inverse transform provided" << endl;
      return false;
    }
  }

  // Compute dimensions of the output volume
  if(!(proc.dx > 0 && proc.dy > 0 && proc.dz > 0
       && proc.sx > 0 && proc.sy > 0 && proc.sz > 0))
  {
    // Reference volume
    int rdx = 0, rdy = 0, rdz = 0;
    double rsx = 0., rsy = 0., rsz = 0.;
    if(!proc.reference.empty()) {
      Finder reffinder;
      if(!reffinder.check(proc.reference)) {
        ostringstream s;
        s << "Failed to check the reference volume "
          << proc.reference;
        throw FatalError(s.str());
      }
      Object refheader = reffinder.headerObject();
      try {
        rdx = refheader->getProperty("volume_dimension")->getArrayItem(0)->getScalar();
        rdy = refheader->getProperty("volume_dimension")->getArrayItem(1)->getScalar();
        rdz = refheader->getProperty("volume_dimension")->getArrayItem(2)->getScalar();
        rsx = refheader->getProperty("voxel_size")->getArrayItem(0)->getScalar();
        rsy = refheader->getProperty("voxel_size")->getArrayItem(1)->getScalar();
        rsz = refheader->getProperty("voxel_size")->getArrayItem(2)->getScalar();
      } catch(...) {
        ostringstream s;
        s << "Failed to retrieve volume_dimension and voxel_size from "
          "the reference volume "
          << proc.reference;
        throw FatalError(s.str());
      }
    }
    // Dimensions and voxel size of the resampled volume: in decreasing order
    // of priority, use commandline flags, else the reference volume, else the
    // input volume.
    vector<int> isize = input_image.getSize();
    vector<float> ivoxel_size = input_image.getVoxelSize();
    proc.dx = (proc.dx > 0 ? proc.dx : (rdx > 0 ? rdx : isize[0]));
    proc.dy = (proc.dy > 0 ? proc.dy : (rdy > 0 ? rdy : isize[1]));
    proc.dz = (proc.dz > 0 ? proc.dz : (rdz > 0 ? rdz : isize[2]));
    proc.sx = (proc.sx > 0 ? proc.sx : (rsx > 0 ? rsx : ivoxel_size[0]));
    proc.sy = (proc.sy > 0 ? proc.sy : (rsy > 0 ? rsy : ivoxel_size[1]));
    proc.sz = (proc.sz > 0 ? proc.sz : (rsz > 0 ? rsz : ivoxel_size[2]));
  }
  cout << "Output dimensions: "
       << proc.dx << ", " << proc.dy << ", " << proc.dz << endl;
  cout << "Output voxel size: "
       << proc.sx << ", " << proc.sy << ", " << proc.sz << " mm" << endl;

  // Prepare the output resampled volume
  Volume<T> out(proc.dx, proc.dy, proc.dz, input_image.getSizeT());
  vector<float> vs(4);
  vs[0] = proc.sx;
  vs[1] = proc.sy;
  vs[2] = proc.sz;
  vs[3] = input_image.getVoxelSize()[3];
  out.header().setProperty("voxel_size", vs);
  out.fill(background_value);

  // Prepare the resampler
  unique_ptr<aims::Resampler<T> > resampler
    = get_resampler<T>(proc.interp_type);

  // Perform resampling
  cout << "Resampling " << DataTypeCode<Volume<T> >::name() << "... ";
  resampler->resample_inv(input_image, *inverse_transform,
                          background_value, out, true);
  cout << endl;

  // Write the resampled volume
  Writer<Volume<T> > output_writer(proc.output);
  bool success = output_writer.write(out);

  return success;
}


template <int D>
bool doMesh(Process & process, const string & fileref, Finder &)
{
  ApplyTransformProc & proc = (ApplyTransformProc &) process;
  typedef AimsTimeSurface<D, Void> MeshType;

  // Read the input mesh
  AimsTimeSurface<D, Void> mesh;
  aims::Reader<MeshType> input_reader(fileref);
  input_reader.read(mesh);

  // Load the transformation
  rc_ptr<Transformation3d> direct_transform;
  {
    std::pair<rc_ptr<Transformation3d>, rc_ptr<Transformation3d> > transforms
      = load_transformations(proc);
    direct_transform = transforms.first;
    if(!direct_transform) {
      clog << "Error: no direct transform provided" << endl;
      return false;
    }
  }

  // Perform the transformation
  cout << "Transforming " << DataTypeCode<MeshType>::name() << "... ";
  transformMesh(mesh, *direct_transform);
  cout << endl;

  // Write the transformed mesh
  Writer<AimsTimeSurface<D, Void> > w3(proc.output);
  bool success = w3.write(mesh);

  return success;
}


bool doBucket(Process & process, const string & fileref, Finder &)
{
  ApplyTransformProc & proc = (ApplyTransformProc &) process;

  typedef BucketMap<Void> BucketType;

  BucketType input_bucket;
  aims::Reader<BucketType> input_reader(fileref);
  input_reader.read(input_bucket);

  // Prepare the output dimensions
  proc.sx = (proc.sx > 0 ? proc.sx : input_bucket.sizeX());
  proc.sy = (proc.sy > 0 ? proc.sy : input_bucket.sizeY());
  proc.sz = (proc.sz > 0 ? proc.sz : input_bucket.sizeZ());
  cout << "Output voxel size: "
       << proc.sx << ", " << proc.sy << ", " << proc.sz << " mm" << endl;

  // Load the transformation
  rc_ptr<Transformation3d> direct_transform, inverse_transform;
  {
    std::pair<rc_ptr<Transformation3d>, rc_ptr<Transformation3d> > transforms
      = load_transformations(proc);
    direct_transform = transforms.first;
    inverse_transform = transforms.second;
    if(!direct_transform) {
      clog << "Error: no direct transform provided" << endl;
      return false;
    }
  }

  // Perform the transformation
  // TODO: add an option to use pushforward, pullback, or both
  rc_ptr<BucketMap<Void> > out;
  if(inverse_transform) {
    cout << "Resampling " << DataTypeCode<BucketType>::name()
         << " with the combined pushforward and pullback methods... ";
    out = resampleBucket(input_bucket, *direct_transform, *inverse_transform,
                         Point3df(proc.sx, proc.sy, proc.sz));
  } else {
    cout << "Transforming " << DataTypeCode<BucketType>::name()
         << " with the low-quality pushforward method... ";
    out = transformBucketDirect(input_bucket, *direct_transform,
                                Point3df(proc.sx, proc.sy, proc.sz));
  }
  cout << endl;

  // Write the resampled volume
  Writer<BucketMap<Void> > w3(proc.output);
  bool success = w3.write(*out);

  return success;
}


bool doBundles(Process & process, const string & fileref, Finder &)
{
  ApplyTransformProc & proc = (ApplyTransformProc &) process;

  // Prepare the Bundle reader
  aims::BundleReader bundle_reader(fileref);

  // Load the transformation
  rc_ptr<Transformation3d> direct_transform;
  {
    std::pair<rc_ptr<Transformation3d>, rc_ptr<Transformation3d> > transforms
      = load_transformations(proc);
    direct_transform = transforms.first;
    if(!direct_transform) {
      clog << "Error: no direct transform provided" << endl;
      return false;
    }
  }

  // Prepare the Bundles processing chain
  BundleTransformer transformer(direct_transform);
  bundle_reader.addBundleListener(transformer);
  BundleWriter bundle_writer;
  bundle_writer.setFileString(proc.output);
  transformer.addBundleListener(bundle_writer);

  cout << "Transforming Bundles... ";
  bundle_reader.read();
  cout << endl;

  return true;
}


bool doGraph(Process & process, const string & fileref, Finder & f)
{
  // check bundles objects, which are reported as graphs by Finder
  // (since they can be read as graphs, which is the only data structure to
  // represent them internally)
  const PythonHeader *ph = dynamic_cast<const PythonHeader *>(f.header());
  if(ph && ph->getProperty("object_type")->getString() == "BundleMap")
    return doBundles(process, fileref, f);

  // other cases are "real" graphs

  ApplyTransformProc & proc = (ApplyTransformProc &) process;

  // Read the input Graph
  unique_ptr<Graph> graph;
  aims::Reader<Graph> input_reader(fileref);
  graph.reset(input_reader.read());

  // Deduce the voxel size of the output Graph
  {
    vector<float> vs;
    if(!graph->getProperty("voxel_size", vs))
      vs = vector<float>(3, 0.f);

    if(vs.size() < 3)
      vs.resize(3, 1.);
    proc.sx = (proc.sx > 0 ? proc.sx : vs[0]);
    proc.sy = (proc.sy > 0 ? proc.sy : vs[1]);
    proc.sz = (proc.sz > 0 ? proc.sz : vs[2]);
  }
  cout << "Output voxel size: "
       << proc.sx << ", " << proc.sy << ", " << proc.sz << " mm" << endl;

  // Load the transformation
  // TODO: add an option to use pushforward, pullback, or both for Buckets
  rc_ptr<Transformation3d> direct_transform, inverse_transform;
  {
    std::pair<rc_ptr<Transformation3d>, rc_ptr<Transformation3d> > transforms
      = load_transformations(proc);
    direct_transform = transforms.first;
    inverse_transform = transforms.second;
    if(!direct_transform) {
      clog << "Error: no direct transform provided" << endl;
      return false;
    }
  }

  // Perform the graph transformation
  cout << "Resampling Graph... ";
  transformGraph(*graph, *direct_transform, inverse_transform.get(),
                  Point3df(proc.sx, proc.sy, proc.sz));
  cout << endl;

  // Write the output Graph
  Writer<Graph> output_writer(proc.output);
  graph->setProperty("filename_base", "*");
  bool success = output_writer.write(*graph);

  return success;
}


bool doPoints(ApplyTransformProc & proc, const string & filename)
{
  istream *s = 0;
  ifstream f;
  stringstream sf;
  if(FileUtil::fileStat(filename).find('+') != string::npos)
  {
    f.open(filename.c_str());
    if(!f)
      throw errno_error();
    s = &f;
  }
  else
  {
    sf.str(filename);
    s = &sf;
  }

  // Load the transformation
  rc_ptr<Transformation3d> direct_transform;
  {
    std::pair<rc_ptr<Transformation3d>, rc_ptr<Transformation3d> > transforms
      = load_transformations(proc);
    direct_transform = transforms.first;
    if(!direct_transform) {
      clog << "Error: no direct transform provided" << endl;
      return false;
    }
  }

  vector<Point3df> transformed;

  while(!s->eof() && !s->fail())
  {
    Point3df p(1e38, 1e38, 1e38), q;
    (*s) >> p;
    if(p == Point3df(1e38, 1e38, 1e38))
      break;
    q = direct_transform->transform(p);
    if(proc.output.empty())
      cout << "p : " << p << " -> " << q << endl;
    else
      transformed.push_back(q);
  }

  if(!proc.output.empty())
  {
    ofstream g(proc.output.c_str());
    vector<Point3df>::iterator ip, ep = transformed.end();
    for(ip=transformed.begin(); ip!=ep; ++ip)
      g << *ip << endl;
  }

  return true;
}


int main(int argc, const char **argv)
{
  int result = EXIT_SUCCESS;

  try {
    ApplyTransformProc proc;
    ProcessInput  proc_input(proc);

    //
    // Collect arguments.
    //
    AimsApplication app(
      argc, argv, // TODO rework help message
      "Apply a spatial transformation on an image, a mesh, a 'bucket' "
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
    app.addOption(proc_input, "--input", "Input image");
    app.addOptionSeries(proc.direct_transform_list,
                        "--direct-transform",
                        "direct transformation (from input space to "
                        "output space). Multiple transformations will "
                        "be composed in the order that they are "
                        "passed on the command-line. Affine "
                        "transformations may be suffixed with :inv to "
                        "use their inverse.");
    app.addOptionSeries(proc.inverse_transform_list,
                        "--inverse-transform",
                        "inverse transformation (from output space to "
                        "input space). Multiple transformations will "
                        "be composed in the order that they are "
                        "passed on the command-line. Affine "
                        "transformations may be suffixed with :inv to "
                        "use their inverse.");
    app.addOption(proc.interp_type, "--interp",
                  "Voxel values resampling type : n[earest], "
                  "l[inear], c[ubic] [default = cubic]", true);
    app.addOption(proc.background_value, "-bv",
                  "Background value to use", true);
    app.addOption(proc.dx, "--dx",
                  "Output X dimension [default: same as input]", true);
    app.addOption(proc.dy, "--dy",
                  "Output Y dimension [default: same as input]", true);
    app.addOption(proc.dz, "--dz",
                  "Output Z dimension [default: same as input]", true);
    app.addOption(proc.sx, "--sx",
                  "Output X voxel size [default: same as input]", true);
    app.addOption(proc.sy, "--sy",
                  "Output Y voxel size [default: same as input]", true);
    app.addOption(proc.sz, "--sz",
                  "Output Z voxel size [default: same as input]", true);
    app.addOption(proc.reference, "--reference",
                  "Volume used to define output voxel size and volume "
                  "dimension (values are overrided by --dx, --dy, "
                  "--dz, --sx, --sy and --sz)", true);
    app.addOption(proc.output, "--output", "Output image");
    app.addOption(proc.vfinterp, "--vectorinterpolation",
                  "Vector field interpolation type: l[inear], c[ubic] "
                  "[default = linear]", true); // TODO remove?
    app.alias("-i", "--input");
    app.alias("-d", "--direct-transform");
    app.alias("-m", "--direct-transform");
    app.alias("-I", "--inverse-transform");
    app.alias("--type", "--interp");
    app.alias("-t", "--interp");
    app.alias("-r", "--reference");
    app.alias("-o", "--output");
    app.alias("--vi", "--vectorinterpolation"); // TODO remove?
    app.initialize();


    // TODO improve the switch to Points mode
    bool ok = false;
    try {
      ok = proc.execute(proc_input.filename);
    } catch(const FatalError& exc) {
      clog << "Failed to run in File mode, will now try Points mode ("
           << exc.what() << ")."<< endl;
    }
    if(!ok)
    {
      ok = doPoints(proc, proc_input.filename);
    }
    if(!ok)
      result = EXIT_FAILURE;

  }
  catch(const FatalError &exc)
  {
    clog << exc.what() << endl;
    result = EXIT_FAILURE;
  }
  catch(user_interruption &e) {
    result = EXIT_FAILURE;
  }
  catch(std::exception &e) {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }
  return result;
}
