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
  string  vfinterp;
};


ApplyTransformProc::ApplyTransformProc()
  : Process(),
    interp_type("linear"),
    background_value("0"),
    dx(0), dy(0), dz(0),
    sx(0.), sy(0.), sz(0.),
    vfinterp("linear")
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


void set_geometry_from_header(ApplyTransformProc& proc,
                              const DictionaryInterface &header,
                              bool dimension_ok,
                              bool voxel_size_ok)
{
  if(!voxel_size_ok) {
    Object voxel_size = header.getProperty("voxel_size");
    if(!(proc.sx > 0))
      proc.sx = voxel_size->getArrayItem(0)->getScalar();
    if(!(proc.sy > 0))
      proc.sy = voxel_size->getArrayItem(1)->getScalar();
    if(!(proc.sz > 0))
      proc.sz = voxel_size->getArrayItem(2)->getScalar();
  }
  if(!dimension_ok) {
    Object volume_dimension = header.getProperty("volume_dimension");
    if(!(proc.dx > 0))
      proc.dx = volume_dimension->getArrayItem(0)->getScalar();
    if(!(proc.dy > 0))
      proc.dy = volume_dimension->getArrayItem(1)->getScalar();
    if(!(proc.dz > 0))
      proc.dz = volume_dimension->getArrayItem(2)->getScalar();
  }
}


// Set dimensions and voxel size of the resampled object: in decreasing order
// of priority, use commandline flags, else the reference object, else the
// input object passed as fallback_header.
void set_geometry(ApplyTransformProc& proc,
                  const DictionaryInterface& fallback_header,
                  bool set_dimensions)
{
  bool dimension_ok = !set_dimensions || (proc.dx > 0 && proc.dy > 0 && proc.dz > 0);
  bool voxel_size_ok = proc.sx > 0 && proc.sy > 0 && proc.sz > 0;
  if(dimension_ok && voxel_size_ok)
    return;

  // Reference volume
  if(!proc.reference.empty()) {
    Finder reffinder;
    if(!reffinder.check(proc.reference)) {
      throw FatalError("failed to read the header of the reference object "
                       + proc.reference);
    }
    Object refheader = reffinder.headerObject();
    try {
      set_geometry_from_header(proc, *refheader, dimension_ok,
                               voxel_size_ok);
    } catch(...) {
      throw FatalError("Failed to retrieve volume_dimension and voxel_size "
                       "from the reference object (" + proc.reference + ")");
    }
  }

  dimension_ok = !set_dimensions || (proc.dx > 0 && proc.dy > 0 && proc.dz > 0);
  voxel_size_ok = proc.sx > 0 && proc.sy > 0 && proc.sz > 0;
  if(dimension_ok && voxel_size_ok)
    return;

  try {
    set_geometry_from_header(proc, fallback_header, dimension_ok,
                             voxel_size_ok);
  } catch(...) {
    throw FatalError("Failed to retrieve volume_dimension and voxel_size "
                     "from the fallback object");
  }
}


// Throws FatalError in case of failure
rc_ptr<FfdTransformation>
load_ffd_deformation(const string &filename,
                     const ApplyTransformProc &proc)
{
  rc_ptr<FfdTransformation> deformation;

  // Maybe find a better way of choosing the vector field interpolation for
  // each transformation (URL parameter?)
  if(proc.vfinterp == "cubic" || proc.vfinterp == "c")
    deformation.reset(new SplineFfd);
  else if(proc.vfinterp == "linear" || proc.vfinterp == "l")
    deformation.reset(new TrilinearFfd);
  else
    throw invalid_argument("invalid vector field interpolation type: "
                           + proc.vfinterp + ", aborting.");

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

  if(filename_arg.substr(0, 4) == "inv:") {
    filename = filename_arg.substr(4, std::string::npos);
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
    ret.first = rc_ptr<Transformation3d>(ret.second->getInverse());
  }

  if(!ret.second && ret.first->invertible()) {
    ret.second = rc_ptr<Transformation3d>(ret.first->getInverse());
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
  set_geometry(proc, input_image.header(), true);

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
  set_geometry(proc, input_bucket.header(), false);
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
  set_geometry(proc, *graph, false);
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
      argc, argv,
      "Apply a spatial transformation on an image, a mesh, a 'bucket'\n"
      "(voxels list file), fiber tracts, a graph, or to points.\n"
      "\n"
      "Depending on the type of input, the direct transformation and/or the\n"
      "inverse transformation are needed:\n"
      "- Images need the inverse transformation, because they are resampled\n"
      "  using pullback interpolation;\n"
      "- Meshes and Bundles need the direct transformation only;\n"
      "- Buckets and Graphs need the direct transformation (pushforward), but\n"
      "  a better interpolation method will be used for Buckets if the\n"
      "  inverse transformation is also available (combined pullback and\n"
      "  pushforward).\n"
      "\n"
      "These rules apply for passing the transformations:\n"
      "- An arbitrary number of transformations can be specified, they will\n"
      "  be composed in the order that they appear on the command line\n"
      "  (e.g. for flags '-m A.trm -m B.trm', A will be applied before B,\n"
      "  in other words, the matrix product BA will be used);\n"
      "- Each transformation can be an affine transform (in .trm format) or\n"
      "  a displacement field (in .ima/.dim GIS format, a.k.a. \"FFD\" for\n"
      "  Free-Form Deformation);\n"
      "- Direct transformations must be given with --direct-transform/-d/-m,\n"
      "  ordered from the input space to the target space;\n"
      "- Inverse transformations must be given with --inverse-transform/-I/-M,\n"
      "  ordered from the target space to the input space;\n"
      "- If only one transformation is specified (direct or inverse), the\n"
      "  other will be computed automatically if the full transformation\n"
      "  chain can be inverted. Otherwise, both transformation chains must\n"
      "  be specified completely.\n"
      "\n"
      "In points mode, the --input option either specifies an ASCII file\n"
      "containing point coordinates, or is directly one or several points\n"
      "coordinates on the command-line. Points should be formatted as\n"
      "\"(x, y, z)\", with parentheses and commas.\n"
      "\n"
      "Note also that for meshes or points, the dimensions, voxel sizes,\n"
      "reference, and resampling options are not needed and are ignored."
   );
    app.addOption(proc_input, "--input", "Input image");
    app.addOption(proc.output, "--output", "Output image");
    app.addOptionSeries(proc.direct_transform_list,
                        "--direct-transform",
                        "direct transformation (from input space to "
                        "output space). Multiple transformations will "
                        "be composed in the order that they are "
                        "passed on the command-line. The file name may be "
                        "prefixed with 'inv:', in which case the inverse of "
                        "the transformation is used.");
    app.addOptionSeries(proc.inverse_transform_list,
                        "--inverse-transform",
                        "inverse transformation (from output space to "
                        "input space). Multiple transformations will "
                        "be composed in the order that they are "
                        "passed on the command-line. The file name may be "
                        "prefixed with 'inv:', in which case the inverse of "
                        "the transformation is used.");
    app.addOption(proc.interp_type, "--interp",
                  "Type of interpolation used for Volumes: n[earest], "
                  "l[inear], q[uadratic], c[cubic], quartic, quintic, "
                  "six[thorder], seven[thorder] [default=linear]. Modes may "
                  "also be specified as order number: 0=nearest, 1=linear...",
                  true);
    app.addOption(proc.background_value, "--background",
                  "Value used for the background of Volumes", true);
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
                  "dimension (values are overridden by --dx, --dy, "
                  "--dz, --sx, --sy and --sz)", true);
    app.addOption(proc.vfinterp, "--vectorinterpolation",
                  "Interpolation used for vector field transformations "
                  "(a.k.a. FFD): l[inear], c[ubic] [default = linear]", true);
    app.alias("-i",             "--input");
    app.alias("-m",             "--direct-transform");
    app.alias("--motion",       "--direct-transform");
    app.alias("-d",             "--direct-transform");
    app.alias("-I",             "--inverse-transform");
    app.alias("-M",             "--inverse-transform");
    app.alias("--type",         "--interp");
    app.alias("-t",             "--interp");
    app.alias("--bg",           "--background");
    app.alias("-bv",            "--background");
    app.alias("--defaultvalue", "--background");
    app.alias("-r",             "--reference");
    app.alias("-o",             "--output");
    app.alias("--vi",           "--vectorinterpolation");
    app.initialize();


    // TODO improve the switch to Points mode
    bool ok = false;
    try {
      ok = proc.execute(proc_input.filename);
    } catch(const exception& exc) {
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
