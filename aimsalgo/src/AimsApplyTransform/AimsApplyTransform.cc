//--- aims -------------------------------------------------------------------
#include <aims/bucket/bucketMap.h>                                // BucketMap
#include <aims/data/data_g.h>                                      // AimsData
#include <aims/getopt/getopt2.h>                            // AimsApplication
#include <aims/getopt/getoptProcess.h>        // AimsApplication: ProcessInput
#include <aims/io/datatypecode.h>                           // DataTypeCode<T>
#include <aims/io/io_g.h>                          // Reader - Writer - Finder
#include <aims/mesh/surfaceOperation.h>            // SurfaceManip::meshVolume
#include <aims/graph/graphmanip.h>                           // storeTalairach
#include <aims/transformation/affinetransformation3d.h> // AffineTransformation3d
#include <aims/transformation/transformation_chain.h> // TransformationChain3d
#include <aims/transform/transform_objects.h>
#include <aims/resampling/linearresampler.h>                // LinearResampler
#include <aims/resampling/cubicresampler.h>                  // CubicResampler
#include <aims/resampling/resamplerfactory.h>
#include <aims/resampling/standardreferentials.h>
#include <aims/registration/ffd.h>                        // FfdTransformation
//--- carto ------------------------------------------------------------------
#include <cartobase/smart/rcptr.h>                                   // rc_ptr
#include <cartobase/object/object.h>                                 // Object
#include <cartobase/stream/fileutil.h>
#include <cartobase/exception/errno.h>
//--- soma -------------------------------------------------------------------
#include <soma-io/allocator/allocator.h>
//--- std --------------------------------------------------------------------
#include <algorithm>
#include <iostream>
#include <exception>
#include <sstream>
//--- boost ------------------------------------------------------------------
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
//----------------------------------------------------------------------------

using namespace aims;
using namespace std;
using namespace carto;
using namespace soma;

namespace {

static const int EXIT_USAGE_ERROR = 2;

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
  string  input_coords;
  bool    points_mode;
  string  interp_type;
  string  background_value;
  bool    keep_transforms;
  bool    ignore_reference_transforms;
  string  reference;
  int32_t dx;
  int32_t dy;
  int32_t dz;
  double  sx;
  double  sy;
  double  sz;
  string  vfinterp;
  bool    mmap_fields;
  string  progress_file;
  bool    volume_id;
};


ApplyTransformProc::ApplyTransformProc()
  : Process(),
    input_coords("AIMS"),
    points_mode(false),
    interp_type("linear"),
    background_value("0"),
    keep_transforms(false),
    ignore_reference_transforms(false),
    dx(0), dy(0), dz(0),
    sx(0.), sy(0.), sz(0.),
    vfinterp("linear"),
    mmap_fields(false),
    volume_id( false )
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
  registerProcessType("Mesh4",  "VOID",    &doMesh<4>);
  registerProcessType("Segments", "VOID",  &doMesh<2>);
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
  resamplers[ "maj" ] = 101;
  resamplers[ "majority" ] = 101;
  resamplers[ "mef" ] = 201;
  resamplers[ "median" ] = 201;
  int order = 0;

  typename map<string, int>::iterator	i = resamplers.find( interp_type );
  if( i != resamplers.end() )
    order = i->second;
  else
  {
    // try another numeric value
    stringstream s( interp_type );
    try
    {
      s >> order;
    }
    catch( ... )
    {
      throw invalid_argument( "invalid resampler type: " + interp_type );
    }
    if( order == 0 )
      throw invalid_argument( "invalid resampler type: " + interp_type );
  }
  return ResamplerFactory<T>::getResampler( order );
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


// Return the header of the reference object, or a null reference if there is
// no reference object.
carto::Object read_reference_header(const std::string reference_filename)
{
  carto::Object reference_header;
  if(!reference_filename.empty()) {
    Finder finder;
    if(!finder.check(reference_filename)) {
      throw FatalError("failed to read the header of the reference object "
                       + reference_filename);
    }
    if( finder.objectType() == "Graph" )
    {
      // graphs are their own header, thus we have to read them entirely
      Reader<Graph> r( reference_filename );
      Graph *g = r.read();
      reference_header.reset( g );
    }
    else
      reference_header = finder.headerObject();
  }
  return reference_header;
}


// Set dimensions and voxel size of the resampled object: in decreasing order
// of priority, use commandline flags, else the reference object, else the
// input object passed as fallback_header.
void set_geometry(ApplyTransformProc& proc,
                  const carto::Object& reference_header,
                  const DictionaryInterface& fallback_header,
                  bool set_dimensions)
{
  bool dimension_ok = !set_dimensions || (proc.dx > 0 && proc.dy > 0 && proc.dz > 0);
  bool voxel_size_ok = proc.sx > 0 && proc.sy > 0 && proc.sz > 0;
  if(dimension_ok && voxel_size_ok)
    return;

  // Reference volume
  if(!reference_header.isNull()) {
    try {
      set_geometry_from_header(proc, *reference_header, dimension_ok,
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

void copy_referentials_and_transformations(const carto::Object& referentials,
                                           const carto::Object& transformations,
                                           carto::Object& referentials2,
                                           carto::Object& transformations2)
{
  // Initialize new objects with empty vectors that they own
  transformations2 = carto::Object::value<std::vector<std::vector<float> > >();
  referentials2 = carto::Object::value<std::vector<std::string> >();

  // Obtain a reference to the new vectors owned by the objects
  std::vector<std::vector<float> > & t2
    = transformations2->value<std::vector<std::vector<float> > >();
  std::vector<std::string> & r2
    = referentials2->value<std::vector<std::string> >();

  // Copy the elements one by one
  size_t nt = std::min(referentials->size(), transformations->size());
  r2.reserve(nt);
  t2.reserve(nt);
  for( size_t i=0; i<nt; ++i ) {
    AffineTransformation3d mat(transformations->getArrayItem(i));
    t2.push_back(mat.toVector());
    r2.push_back(referentials->getArrayItem(i)->getString());
  }
}

void
insert_transformation_to_old_referential(DictionaryInterface& header,
                                         const AffineTransformation3d& inverse_transform,
                                         std::string old_referential_id)
{
  if(old_referential_id.empty()) {
    old_referential_id = "Coordinates aligned to another file or to "
      "anatomical truth";
  }

  typedef std::vector<std::string> ReferentialVectorType;
  typedef std::vector<std::vector<float> > TransformVectorType;
  carto::Object new_referentials_obj
    = carto::Object::value<ReferentialVectorType>();
  carto::Object new_transforms_obj
    = carto::Object::value<TransformVectorType>();
  ReferentialVectorType& new_referentials
    = new_referentials_obj->value<ReferentialVectorType>();
  TransformVectorType& new_transforms
    = new_transforms_obj->value<TransformVectorType>();

  // Copy the existing referentials and transformations
  try {
    carto::Object old_referentials, old_transforms;
    old_referentials = header.getProperty("referentials");
    old_transforms = header.getProperty("transformations");
    new_referentials.reserve(old_referentials->size() + 1);
    new_transforms.reserve(old_transforms->size() + 1);

    for( carto::Object tit = old_transforms->objectIterator(),
           rit = old_referentials->objectIterator();
         tit->isValid() && rit->isValid();
         tit->next(), rit->next() ) {
      new_referentials.push_back(rit->currentValue()->getString());
      AffineTransformation3d transform(tit->currentValue());
      new_transforms.push_back(transform.toVector());
    }
  } catch(...) {
    // An error occured while reading the existing transformations, discard
    // what has been read until now.
    new_referentials.clear();
    new_transforms.clear();
  }

  // Insert a transformation to the old referential at the end
  new_referentials.push_back(old_referential_id);
  new_transforms.push_back(inverse_transform.toVector());
  header.setProperty("referentials", new_referentials_obj);
  header.setProperty("transformations", new_transforms_obj);
}


void adjust_header_transforms(const ApplyTransformProc& proc,
                              DictionaryInterface& header,
                              const Transformation3d * inverse_transform,
                              const carto::Object reference_header)
{
  if(proc.keep_transforms) {
    // Case 1: the user explicitly requests to keep the input header transforms
    return;
  }
  if(inverse_transform && inverse_transform->isIdentity())
    return;  // the transformations remain valid

  std::string old_referential;
  try {
    old_referential = header.getProperty("referential")->getString();
  } catch(...) {}
  // The referential UUID used by Axon is no longer valid
  try {
    header.removeProperty("referential");
  } catch(...) {}

  const AffineTransformation3d * affine_inverse_transform
    = dynamic_cast<const AffineTransformation3d*>(inverse_transform);

  if(!proc.ignore_reference_transforms && !reference_header.isNull()) {
    // Case 2: copy the referential and transformations from the reference
    // object if enabled and available.

    // TODO: specify the behaviour w.r.t. on-disk data orientation (i.e.
    // storage_to_memory header field). We should probably copy it from the
    // reference header, but only for Volumes (it is not used for other data
    // types). Also, we should specify the behaviour w.r.t. preservation of the
    // anatomical orientation (i.e. the allow_orientation_change flag of the
    // somanifti writer).

    // Copy the referential uuid from the reference header if present
    std::string reference_referential;
    try {
      reference_referential =
        reference_header->getProperty("referential")->getString();
    } catch(...) {
    }
    if(!reference_referential.empty()) {
      header.setProperty("referential",
                         carto::Object::value(reference_referential));
    }

    // Copy the list of referentials and transformations from the reference
    carto::Object new_referentials_obj, new_transforms_obj;
    try {
      copy_referentials_and_transformations(
        reference_header->getProperty("referentials"),
        reference_header->getProperty("transformations"),
        new_referentials_obj,
        new_transforms_obj
      );
    } catch(...) {
      new_referentials_obj.reset();
      new_transforms_obj.reset();
    }
    if(!new_referentials_obj.isNull()) {
      header.setProperty("referentials", new_referentials_obj);
      header.setProperty("transformations", new_transforms_obj);

      if(affine_inverse_transform) {
        insert_transformation_to_old_referential(header,
                                                 *affine_inverse_transform,
                                                 old_referential);
      }
      return; // success
    }
    // in case of failure, fall through to reach case 4
  }

  if(affine_inverse_transform) {
    // Case 3: reuse the  referential and transformations from the input object
    // and compose them with the inverse affine transformation
    try {
      carto::Object old_transforms = header.getProperty("transformations");

      typedef std::vector<std::vector<float> > TransformVectorType;
      carto::Object new_transforms_obj
        = carto::Object::value<TransformVectorType>();
      TransformVectorType& new_transforms
        = new_transforms_obj->value<TransformVectorType>();
      new_transforms.reserve(old_transforms->size());

      for( carto::Object it = old_transforms->objectIterator();
           it->isValid();
           it->next() ) {
        carto::Object old_transform_obj = it->currentValue();
        AffineTransformation3d transform(old_transform_obj);
        transform *= *affine_inverse_transform;
        new_transforms.push_back(transform.toVector());
      }

      header.setProperty("transformations", new_transforms_obj);

      insert_transformation_to_old_referential(header,
                                               *affine_inverse_transform,
                                               old_referential);
      return; // success
    } catch( ... ) {
      // remove all transformations by falling through to reach case 4
    }
  }

  // Case 4: if neither case 1 nor case 2 applies, or if an error occurs while
  // updating the transformations, we fail gracefully by removing all
  // pre-existing transformations from the input object.
  try {
    header.removeProperty("transformations");
  } catch(...) {}
  try {
    header.removeProperty("referentials");
  } catch(...) {}

  if(affine_inverse_transform) {
    insert_transformation_to_old_referential(header, *affine_inverse_transform,
                                             old_referential);
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

  AllocatorContext requested_allocator(AllocatorStrategy::ReadOnly);
  if(proc.mmap_fields) {
    // The "use factor" is used calculate a threshold size above which
    // memory-mapping is attempted, based on the currently available memory. By
    // setting it to zero, we can force memory-mapping to be always attempted.
    requested_allocator.setUseFactor(0.f);
  }

  aims::Reader<FfdTransformation> rdef(filename);
  rdef.setAllocatorContext(requested_allocator);
  bool read_success = rdef.read(*deformation);
  if(!read_success) {
    ostringstream s;
    s << "Failed to load a deformation field from "
      << filename << ", aborting.";
    throw FatalError(s.str());
  }

  const AllocatorContext & actual_allocator =
    static_cast<const VolumeRef<Point3df> & >(
      *deformation.get()).allocatorContext();
  if(proc.mmap_fields
     && actual_allocator.allocatorType() != AllocatorStrategy::ReadOnlyMap)
  {
    std::clog << "Warning: memory-mapping was requested but could not be used"
              << std::endl;
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

  // FIXME
  // Denis 2022/09/21 this if.. block should not be needed any longer
  // now that we have a Reader<Transformation3d> (used only as a fallback at
  // the end, up to now)

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
      affine = affine->inverse();
    }
    return rc_ptr<Transformation3d>(affine.release());
  }

  try
  {
    Reader<Transformation3d> r( filename );
    rc_ptr<Transformation3d> tr( r.read() );
    if( !tr )
    {
      ostringstream s;
      s << "Failed to load 3D transformation from " << filename
        << ", aborting.";
      throw FatalError(s.str());
    }
    if( invert )
    {
      if( !tr->invertible() )
      {
        ostringstream s;
        s << "3D transformation from " << filename
          << " is not invertible, aborting.";
        throw FatalError(s.str());
      }

      unique_ptr<Transformation> ti = tr->getInverse();
      Transformation3d *ti3 = dynamic_cast<Transformation3d *>( ti.get() );
      if( !ti3 )
        throw FatalError( "Inverse is not a 3D transform" );
      ti.release();
      return rc_ptr<Transformation3d>( ti3 );
    }
    return tr;
  }
  catch( io_error & )
  {
    ostringstream s;
    s << "cannot read file " << filename
      << " as a 3D transformation (object type: " << finder.objectType()
      << "(of " << finder.dataType() << "))";
    throw FatalError(s.str());
  }
}


// FatalError is thrown if the transformations cannot be loaded properly.
std::pair<const_ref<Transformation3d>, const_ref<Transformation3d> >
load_transformations(const ApplyTransformProc& proc,
                     const DictionaryInterface* input_header = nullptr,
                     const Finder & finder = Finder())
{
  std::pair<const_ref<Transformation3d>, const_ref<Transformation3d> > ret;
  rc_ptr<AffineTransformation3d> aims_to_input_space_transform; // null

  // boost::iequals is used for case-insensitive comparison
  using boost::iequals;
  string effective_input_coords = boost::trim_copy(proc.input_coords);
  if(proc.points_mode) {
    if(iequals(effective_input_coords, "auto"))
      effective_input_coords = "AIMS";
    else if(!iequals(effective_input_coords, "AIMS"))
      throw FatalError("--input-coords cannot be used in points mode");
  }

  if(!iequals(effective_input_coords, "AIMS")) {
    if(!input_header || !input_header->isDictionary()) {
      throw FatalError("--input-coords cannot be used, because no header "
                       "could be read for the input data");
    }

    carto::Object referentials;
    carto::Object transformations;
    try
    {
      referentials = input_header->getProperty( "referentials" );
      transformations = input_header->getProperty( "transformations" );
    }
    catch( ... )
    {
      throw FatalError("--input-coords cannot be used, because no "
                       "transformations could be read in the input header");
    }
    if(referentials.isNull() || transformations.isNull()) {
      throw FatalError("--input-coords cannot be used, because no "
                       "transformations could be read in the input header");
    }

    // Valid values are >= 0
    int transform_position = -1;
    // Detect special values
    if(iequals(effective_input_coords, "first")) {
      transform_position = 0;
    } else if(iequals(effective_input_coords, "last")
              || iequals(effective_input_coords, "auto")) {
      transform_position = std::min(referentials->size(),
                                    transformations->size()) - 1;
    } else if(iequals(effective_input_coords, "qform")
       || iequals(effective_input_coords, "ITK")
       || iequals(effective_input_coords, "ANTS")) {
      // TODO determine transform_position
      throw FatalError("--input-coords qform not implemented yet");
    } else if(iequals(effective_input_coords, "sform")) {
      // TODO determine transform_position
      throw FatalError("--input-coords sform not implemented yet");
    } else {
      try {
        transform_position = boost::lexical_cast<int>(effective_input_coords);
      } catch(const boost::bad_lexical_cast &) {}
    }

    if(transform_position < 0) {
      string referential_name, referential_id;

      if(iequals(effective_input_coords, "mni")
         || iequals(effective_input_coords, "mni152")
         || iequals(effective_input_coords, "NIFTI_XFORM_MNI_152")) {
        referential_name = StandardReferentials::mniTemplateReferential();
        referential_id = StandardReferentials::mniTemplateReferentialID();
      } else if(iequals(effective_input_coords, "scanner")
                || iequals(effective_input_coords, "NIFTI_XFORM_SCANNER_ANAT")) {
        referential_name = StandardReferentials::commonScannerBasedReferential();
        referential_id = StandardReferentials::commonScannerBasedReferentialID();
      } else if(iequals(effective_input_coords, "acpc")) {
        referential_name = StandardReferentials::acPcReferential();
        referential_id = StandardReferentials::acPcReferentialID();
      } else if(iequals(effective_input_coords, "talairach")
                || iequals(effective_input_coords, "NIFTI_XFORM_TALAIRACH")) {
        referential_name = StandardReferentials::talairachReferential();
      } else if(iequals(effective_input_coords, "aligned")
                || iequals(effective_input_coords, "NIFTI_XFORM_ALIGNED_ANAT")) {
        referential_name = "Coordinates aligned to another file or to anatomical truth";
      } else if(iequals(effective_input_coords, "NIFTI_XFORM_TEMPLATE_OTHER")) {
        referential_name = "Other template";
      } else {
        referential_name = proc.input_coords;
      }

      // Look for the target referential or id in header['referentials']
      size_t i = 0;
      for( carto::Object it = referentials->objectIterator();
           it->isValid();
           it->next(), ++i ) {
        const carto::Object referential_obj = it->currentValue();
        if(!referential_obj.isNone() && referential_obj->isString()) {
          const string current_ref_name = referential_obj->getString();
          if(current_ref_name == referential_name
             || (!referential_id.empty()
                 && current_ref_name == referential_id)) {
            transform_position = i;
            if(carto::verbose)
            break;
          }
        }
      }
    }

    if(transform_position >= 0) {
      carto::Object transformation_object;
      if(!transformations->isArray())
        throw FatalError("--input-coords cannot find a valid 'transformations'"
                         "header field");
      if(!(transform_position < transformations->size())) {
        stringstream error_message;
        error_message << "--input-coords cannot use transform in position "
                      << transform_position
                      << " (0-based), the header contains only "
                      << transformations->size() << " transformations.";
        throw FatalError(error_message.str());
      }
      transformation_object = transformations->getArrayItem(transform_position);
      if(!transformation_object.isNull()) {
        aims_to_input_space_transform.reset(
          new AffineTransformation3d(transformation_object));
        if(carto::verbose) {
          std::cout << "--input-coords will use the referential in position "
                    << transform_position
                    << " (0-based) which is named '"
                    << referentials->getArrayItem(transform_position)->getString()
                    << "'\n";
          std::cout << "--input-coords transformation:\n"
                    << *aims_to_input_space_transform
                    << "\n";
        }
      } else {
        throw FatalError("Error in the input header, --input-coords failed");
      }
    } else if(iequals(effective_input_coords, "auto")) {
      // Do nothing: fall back to AIMS coordinates
      // (aims_to_input_space_transform is a null pointer)
    } else {
      throw FatalError("Cannot find the header transformation corresponding "
                       "to --input-coords '" + proc.input_coords + "'");
    }
  }

  if(!proc.direct_transform_list.empty()) {
    TransformationChain3d direct_chain;
    if(!aims_to_input_space_transform.isNull())
      direct_chain.push_back(aims_to_input_space_transform);

    for(vector<string>::const_iterator filename_it = proc.direct_transform_list.begin();
        filename_it != proc.direct_transform_list.end();
        ++filename_it)
    {
      rc_ptr<Transformation3d> transform
        = load_transformation(*filename_it, proc);
      direct_chain.push_back(transform);
    }
    ret.first = direct_chain.simplify();
  }

  if(!proc.inverse_transform_list.empty()) {
    TransformationChain3d inverse_chain;

    for(vector<string>::const_iterator filename_it = proc.inverse_transform_list.begin();
        filename_it != proc.inverse_transform_list.end();
        ++filename_it)
    {
      rc_ptr<Transformation3d> transform
        = load_transformation(*filename_it, proc);
      inverse_chain.push_back(transform);
    }
    if(!aims_to_input_space_transform.isNull()) {
      if(!aims_to_input_space_transform->invertible()) {
        throw FatalError("Error using --input-coords: the transformation "
                         "is not invertible");
      }
      unique_ptr<Transformation> ti
        = aims_to_input_space_transform->getInverse();
      Transformation3d *ti3 = dynamic_cast<Transformation3d *>( ti.get() );
      if( !ti3 )
        throw FatalError( "An inverse transformation is not a 3D "
                          "transformation" );
      inverse_chain.push_back( rc_ptr<Transformation3d>( ti3 ) );
      ti.release();
    }
    ret.second = inverse_chain.simplify();
  }

  if(ret.first.isNull() && ret.second.isNull()) {
    if(aims_to_input_space_transform.isNull()) {
      clog << "No transformation provided, identity will be used." << endl;
      ret.first = const_ref<Transformation3d>(new TransformationChain3d);
      ret.second = const_ref<Transformation3d>(new TransformationChain3d);
      // if --volume_id is specified, compensate the difference in voxel size
      // between input and output origins
      if( proc.volume_id )
      {
        cout << "VOLUME ID\n";
        Point3df vs( 1., 1., 1. );
        const PythonHeader *ph
          = dynamic_cast<const PythonHeader *>( finder.header() );
        if( ph )
        {
          try
          {
            Object ovs = ph->getProperty( "voxel_size" );
            if( ovs )
            {
              if( ovs->size() >= 1 )
              {
                vs[0] = float( ovs->getArrayItem( 0 )->getScalar() );
                {
                  if( ovs->size() >= 2 )
                  {
                    vs[1] = float( ovs->getArrayItem( 1 )->getScalar() );
                    if( ovs->size() >= 3 )
                      vs[2] = float( ovs->getArrayItem( 2 )->getScalar() );
                  }
                }
              }
            }
          }
          catch( ... )
          {
          }
        }
        vs[0] = vs[0] - proc.sx;
        vs[1] = vs[1] - proc.sy;
        vs[2] = vs[2] - proc.sz;
        cout << "translation: " << vs << endl;

        AffineTransformation3d *t = new AffineTransformation3d;
        rc_ptr<Transformation3d> tid( t );
        t->setTranslation( vs / 2 );
        ret.first = tid;
        ret.second = rc_ptr<Transformation3d>(
          new AffineTransformation3d( *t->inverse() ) );
      }
    }
    else
    {
      TransformationChain3d direct_chain;
      direct_chain.push_back(aims_to_input_space_transform);
      ret.first = direct_chain.simplify();
    }
  }

  if(ret.first.isNull() && ret.second->invertible()) {
    unique_ptr<Transformation> ti = ret.second->getInverse();
    Transformation3d *ti3 = dynamic_cast<Transformation3d *>( ti.get() );
    if( !ti3 )
      throw FatalError( "Inverse is not a 3D transform" );
    ti.release();
    ret.first = const_ref<Transformation3d>( ti3 );
  }

  if(ret.second.isNull() && ret.first->invertible()) {
    unique_ptr<Transformation> ti = ret.first->getInverse();
    Transformation3d *ti3 = dynamic_cast<Transformation3d *>( ti.get() );
    if( !ti3 )
      throw FatalError( "Inverse is not a 3D transform" );
    ti.release();
    ret.second = const_ref<Transformation3d>( ti3 );
  }

  return ret;
}


template <class T, class C>
bool doVolume(Process & process, const string & fileref, Finder & finder)
{
  ApplyTransformProc & proc = (ApplyTransformProc &) process;

  // Read the input image
  aims::Reader<Volume<T> > input_reader(fileref);
  input_reader.setAllocatorContext(AllocatorStrategy::ReadOnly);
  Volume<T> input_image;
  input_reader.read(input_image);

  // Parse the background value to the data type
  T background_value;
  stringTo(proc.background_value, background_value);

  // Compute dimensions of the output volume
  const carto::Object reference_header = read_reference_header(proc.reference);
  set_geometry(proc, reference_header, input_image.header(), true);

  // Load the transformation
  const_ref<Transformation3d> inverse_transform;
  {
    std::pair<const_ref<Transformation3d>, const_ref<Transformation3d> > transforms
      = load_transformations(proc, &input_image.header(), finder);
    inverse_transform = transforms.second;
    if(inverse_transform.isNull()) {
      clog << "Error: no inverse transform provided" << endl;
      return false;
    }
  }

  cout << "Output dimensions: "
       << proc.dx << ", " << proc.dy << ", " << proc.dz << endl;
  cout << "Output voxel size: "
       << proc.sx << ", " << proc.sy << ", " << proc.sz << " mm" << endl;

  // Prepare the output resampled volume
  Volume<T> out(proc.dx, proc.dy, proc.dz, input_image.getSizeT());
  out.copyHeaderFrom(input_image.header());
  vector<float> vs(4);
  vs[0] = proc.sx;
  vs[1] = proc.sy;
  vs[2] = proc.sz;
  vs[3] = input_image.getVoxelSize()[3];
  out.header().setProperty("voxel_size", vs);

  // Prepare the resampler
  unique_ptr<aims::Resampler<T> > resampler
    = get_resampler<T>(proc.interp_type);

  unique_ptr<ostream> progress_stream;
  if( !proc.progress_file.empty() )
  {
    progress_stream.reset( new ofstream( proc.progress_file.c_str(),
                                         ios::app ) );
    resampler->setVerboseStream( *progress_stream );
  }

  adjust_header_transforms(proc, out.header(), inverse_transform.pointer(),
                           reference_header);
  // The UUID should NOT be preserved, because the output is a different file
  // from the input. Keeping it triggers the infamous "duplicate UUID" problem
  // in BrainVISA/Axon...
  try {
    out.header().removeProperty("uuid");
  } catch(...) {}

  // Perform resampling
  cout << "Resampling " << DataTypeCode<Volume<T> >::name() << "... ";
  resampler->resample_inv(input_image, *inverse_transform,
                          background_value, out, true);
  cout << endl;

  // Write the resampled volume. The allow_orientation_change instructs the
  // NIfTI writer to avoid writing dummy transformations to the header of the
  // output image.
  Writer<Volume<T> > output_writer(
    proc.output,
    carto::Object::value(std::map<std::string, bool>{
      {"allow_orientation_change", true} })
  );
  bool success = output_writer.write(out);

  return success;
}


template <int D>
bool doMesh(Process & process, const string & fileref, Finder & finder)
{
  ApplyTransformProc & proc = (ApplyTransformProc &) process;
  typedef AimsTimeSurface<D, Void> MeshType;

  // Read the input mesh
  aims::Reader<MeshType> input_reader(fileref);
  input_reader.setAllocatorContext(AllocatorStrategy::InternalModif);
  AimsTimeSurface<D, Void> mesh;
  input_reader.read(mesh);

  // Load the transformation
  const_ref<Transformation3d> direct_transform, inverse_transform;
  {
    std::pair<const_ref<Transformation3d>, const_ref<Transformation3d> > transforms
      = load_transformations(proc, &mesh.header(), finder);
    direct_transform = transforms.first;
    inverse_transform = transforms.second; // allowed to be null
    if(direct_transform.isNull()) {
      clog << "Error: no direct transform provided" << endl;
      return false;
    }
  }

  const carto::Object reference_header = read_reference_header(proc.reference);
  adjust_header_transforms(proc, mesh.header(), inverse_transform.pointer(),
                           reference_header);
  // The UUID should NOT be preserved, because the output is a different file
  // from the input. Keeping it triggers the infamous "duplicate UUID" problem
  // in BrainVISA/Axon...
  try {
    mesh.header().removeProperty("uuid");
  } catch(...) {}


  // Perform the transformation
  cout << "Transforming " << DataTypeCode<MeshType>::name() << "... ";
  transformMesh(mesh, *direct_transform);
  cout << endl;

  // Write the transformed mesh
  Writer<AimsTimeSurface<D, Void> > w3(proc.output);
  bool success = w3.write(mesh);

  return success;
}


bool doBucket(Process & process, const string & fileref, Finder & finder)
{
  ApplyTransformProc & proc = (ApplyTransformProc &) process;

  typedef BucketMap<Void> BucketType;

  aims::Reader<BucketType> input_reader(fileref);
  input_reader.setAllocatorContext(AllocatorStrategy::ReadOnly);
  BucketType input_bucket;
  input_reader.read(input_bucket);

  // Prepare the output dimensions
  const carto::Object reference_header = read_reference_header(proc.reference);
  set_geometry(proc, reference_header, input_bucket.header(), false);
  cout << "Output voxel size: "
       << proc.sx << ", " << proc.sy << ", " << proc.sz << " mm" << endl;

  // Load the transformation
  const_ref<Transformation3d> direct_transform, inverse_transform;
  {
    std::pair<const_ref<Transformation3d>, const_ref<Transformation3d> > transforms
      = load_transformations(proc, &input_bucket.header(), finder);
    direct_transform = transforms.first;
    inverse_transform = transforms.second; // allowed to be null
    if(direct_transform.isNull()) {
      clog << "Error: no direct transform provided" << endl;
      return false;
    }
  }

  // Perform the transformation
  // TODO: add an option to use pushforward, pullback, or both
  rc_ptr<BucketMap<Void> > out;

  if(!inverse_transform.isNull()) {
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

  out->setHeader(input_bucket.header());
  // The UUID should NOT be preserved, because the output is a different file
  // from the input. Keeping it triggers the infamous "duplicate UUID" problem
  // in BrainVISA/Axon...
  try {
    out->header().removeProperty("uuid");
  } catch(...) {}

  adjust_header_transforms(proc, out->header(), inverse_transform.pointer(),
                           reference_header);

  // Write the resampled volume
  Writer<BucketMap<Void> > w3(proc.output);
  bool success = w3.write(*out);

  return success;
}


bool doBundles(Process & process, const string & fileref, Finder & finder)
{
  ApplyTransformProc & proc = (ApplyTransformProc &) process;

  // Prepare the Bundle reader
  aims::BundleReader bundle_reader(fileref);

  // Load the transformation
  const_ref<Transformation3d> direct_transform, inverse_transform;
  {
    std::pair<const_ref<Transformation3d>, const_ref<Transformation3d> > transforms
      = load_transformations(proc, bundle_reader.readHeader().get(), finder);
    direct_transform = transforms.first;
    inverse_transform = transforms.second; // allowed to be null
    if(direct_transform.isNull()) {
      clog << "Error: no direct transform provided" << endl;
      return false;
    }
  }

  // FIXME: there is no API to set the output header, so the transformations
  // will just be ignored for Bundles.
  //
  // carto::Object header = bundle_reader.readHeader();
  // const carto::Object reference_header
  //   = read_reference_header(proc.reference);
  // adjust_header_transforms(proc, header, inverse_transform.pointer(),
  //                          reference_header);

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
  if(ph
     && ph->hasProperty("format")
     && ph->getProperty("format")->isString()
     && ph->getProperty("format")->getString().substr(0, 8) == "bundles_")
    return doBundles(process, fileref, f);

  // other cases are "real" graphs

  ApplyTransformProc & proc = (ApplyTransformProc &) process;

  // Read the input Graph
  aims::Reader<Graph> input_reader(fileref);
  input_reader.setAllocatorContext(AllocatorStrategy::InternalModif);
  unique_ptr<Graph> graph(input_reader.read());

  // Deduce the voxel size of the output Graph
  const carto::Object reference_header = read_reference_header(proc.reference);
  set_geometry(proc, reference_header, *graph, false);
  cout << "Output voxel size: "
       << proc.sx << ", " << proc.sy << ", " << proc.sz << " mm" << endl;

  // Load the transformation
  // TODO: add an option to use pushforward, pullback, or both for Buckets
  const_ref<Transformation3d> direct_transform, inverse_transform;
  {
    std::pair<const_ref<Transformation3d>, const_ref<Transformation3d> > transforms
      = load_transformations(proc, graph.get(), f);
    direct_transform = transforms.first;
    inverse_transform = transforms.second; // allowed to be null
    if(direct_transform.isNull()) {
      clog << "Error: no direct transform provided" << endl;
      return false;
    }
  }

  adjust_header_transforms(proc, *graph,
                           inverse_transform.pointer(),
                           reference_header);
  // Update the transformation to Talairach stored in the old attributes
  // (Talairach_rotation, Talairach_translation, and Talairach_scale) to
  // reflect the updated transformations.
  GraphManip::storeTalairach(*graph, GraphManip::talairach(*graph));

  // The UUID should NOT be preserved, because the output is a different file
  // from the input. Keeping it triggers the infamous "duplicate UUID" problem
  // in BrainVISA/Axon...
  try {
    graph->removeProperty("uuid");
  } catch(...) {}
  try {
    graph->getProperty("header")->removeProperty("uuid");
  } catch(...) {}

  // Perform the graph transformation
  cout << "Resampling Graph... ";
  transformGraph(*graph, *direct_transform, inverse_transform.pointer(),
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
  // Open the input stream
  istream *input_stream = 0;
  ifstream input_file_stream;
  stringstream input_string_stream;
  if(filename == "-")
  {
    input_stream = &std::cin;  // standard input stream
  }
  else if(FileUtil::fileStat(filename).find('+') != string::npos)
  {
    input_file_stream.open(filename.c_str());
    if(!input_file_stream)
      throw errno_error();
    input_stream = &input_file_stream;
  }
  else
  {
    input_string_stream.str(filename);
    input_stream = &input_string_stream;
  }

  // Open the output stream
  const bool print_on_stdout = proc.output.empty() || proc.output == "-";
  ostream * output_stream;
  ofstream output_file_stream;
  if(print_on_stdout)
  {
    output_stream = &std::cout;
  }
  else
  {
    output_file_stream.open(proc.output.c_str());
    if(!output_file_stream)
      throw errno_error();
    output_stream = &output_file_stream;
  }

  // Load the transformation
  const_ref<Transformation3d> direct_transform;
  {
    std::pair<const_ref<Transformation3d>, const_ref<Transformation3d> > transforms
      = load_transformations(proc);
    direct_transform = transforms.first;
    if(direct_transform.isNull()) {
      clog << "Error: no direct transform provided" << endl;
      return false;
    }
  }

  // Transform all points in the input stream and write to the output stream
  while(input_stream->good())
  {
    Point3df p(1e38, 1e38, 1e38), q;
    *input_stream >> p;
    if(p == Point3df(1e38, 1e38, 1e38))
      break;
    q = direct_transform->transform(p);
    if(proc.output.empty())
      *output_stream << "p : " << p << " -> ";
    *output_stream << q << "\n";
    if(output_stream->fail()) {
      std::clog << "Error writing to the output stream" << std::endl;
      return false;
    }
  }
  if(input_stream->bad()) {
    // We should check for fail(), but it is also set when everything goes
    // well...
    std::clog << "Error reading the input stream of points" << std::endl;
    return false;
  }

  return true;
}

} // end of anonymous namespace


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
      "The meaning of coordinates in the input image can be specified with \n"
      "the --input-coords option. This option specifies how to interpret the\n"
      "transformations contained in the image header. --input-coords can\n"
      "take the following values:\n"
      "- 'AIMS' (default): internal coordinate system of the AIMS library.\n"
      "  For images, this is defined as 'physical' coordinates in\n"
      "  millimetres, starting from zero at the centre of the rightmost,\n"
      "  most anterior, most superior voxel in the image. Note that AIMS\n"
      "  determines the anatomical orientation based on the header in a\n"
      "  format-dependent manner. For NIfTI it uses first qform then sform\n"
      "  if they point to a referential of known orientation, and falls back\n"
      "  to assuming a RAS+ on-disk orientation.\n"
      "- 'auto': use the last transformation defined in the AIMS metadata\n"
      "  (i.e. the last or qform or sform to be defined) or fallback to\n"
      "  AIMS coordinates if no transformation is found.\n"
      "- 'first': use the first transformation defined in the AIMS metadata,\n"
      "  i.e. the first or qform or sform to be defined.\n"
      "- 'last': use the last transformation defined in the AIMS metadata,\n"
      "  i.e. the last or qform or sform to be defined.\n"
      "- '0', '1'... or any non-negative integer: use the transformation\n"
      "  in that position (0-based) in the AIMS metadata field\n"
      "  'transformations' (0 is a synonym of 'first').\n"
      /* These options are not implemented yet. TODO(ylep)
      "- 'qform', 'ITK', or 'ANTS': use the target space of the qform stored\n"
      "  in a NIfTI file. This corresponds to the physical space used in\n"
      "  tools based on the ITK library, such as ANTS.\n"
      "- sform: use the target space of the sform stored in a NIfTI file.\n"
      // TODO: do some tools use sform?
      */
      "- a referential name or UUID: use that referential from the\n"
      "  'referentials' field of the AIMS image header, which is read from\n"
      "  the image header or the sidecar .minf file (the .minf takes\n"
      "  precedence). There are shortcuts for common cases:\n"
      "  - 'mni', or 'mni152': use MNI coordinates, which correspond to the\n"
      "    NIFTI_XFORM_MNI_152 intent, a.k.a. 'Talairach-MNI template-SPM',\n"
      "    in AIMS.\n"
      "  - 'scanner': use 'Scanner-based anatomical coordinates', which\n"
      "    correspond to the NIFTI_XFORM_SCANNER_ANAT intent.\n"
      "  - 'acpc': use the 'Talairach-AC/PC-Anatomist' referential.\n"
      "  - 'aligned': use the referential that corresponds to\n"
      "    the NIFTI_XFORM_ALIGNED_ANAT intent, a.k.a. 'Coordinates aligned\n"
      "    to another file or to anatomical truth' in AIMS.\n"
      "  - 'talairach': use the referential that corresponds to the\n"
      "    NIFTI_XFORM_TALAIRACH intent.\n"
      "  - 'NIFTI_XFORM_TEMPLATE_OTHER': use the referential that\n"
      "    corresponds to this intent ('Other template' in AIMS).\n"
      "\n"
      "The header transformations of the output are set according to the\n"
      "first rule that applies:\n"
      "1. If --keep-transforms is passed, or if the applied transformation\n"
      "   is identity, the header transformations of the input object are\n"
      "   copied verbatim to the output. This mode should be used when\n"
      "   applying a corrective transformation.\n"
      "2. If --reference is given and unless --ignore-reference-transform is\n"
      "   passed, the transformations of the reference object are copied to\n"
      "   the output.\n"
      "3. If the applied transformation is affine, the output transformations\n"
      "   are set to the header transforms of the input, composed with the\n"
      "   applied transformation.\n"
      "4. If none of the rules above apply, no transformations are set in\n"
      "   the output header.\n"
      "\n"
      "Points mode is activated by passing the --points option.\n"
      "In this mode, the --input option either specifies an ASCII file\n"
      "containing point coordinates, or is directly one or several points\n"
      "coordinates on the command-line. Points should be formatted as\n"
      "\"(x, y, z)\", with parentheses and commas. Points will be written\n"
      "to the output stream in the same format, one point per line.\n"
      "The input or output file names can also be -, which means standard\n"
      "input and standard output, respectively. If --output is omitted,\n"
      "the points are written on standard output in a user-friendly format.\n"
      "\n"
      "Note also that for meshes or points, the dimensions, voxel sizes,\n"
      "reference, and resampling options are not needed and are ignored."
   );
    app.addOption(proc_input, "--input", "Input image");
    app.addOption(proc.output, "--output", "Output image", true);
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
    app.addOption(proc.input_coords, "--input-coords",
                  "How to interpret coordinates in the input image w.r.t. "
                  "the transformations written in the image header. See above."
                  " [default: AIMS]", true);
    app.addOption(proc.points_mode, "--points",
                  "Points mode: transform point coordinates (see above).", true);
    app.addOption(proc.interp_type, "--interp",
                  "Type of interpolation used for Volumes: n[earest], "
                  "l[inear], q[uadratic], c[cubic], quartic, quintic, "
                  "six[thorder], seven[thorder], maj[ority], med[ian] "
                  "[default=linear]. Modes may "
                  "also be specified as order number: 0=nearest, 1=linear... "
                  "Additional values: 101=majority, 201=median",
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
    app.addOption(proc.keep_transforms, "--keep-transforms",
                  "Preserve the transformations of the input image", true);
    app.addOption(proc.ignore_reference_transforms,
                  "--ignore-reference-transforms",
                  "Do not try to copy the transformations from the reference "
                  "image", true);
    app.addOption(proc.vfinterp, "--vectorinterpolation",
                  "Interpolation used for vector field transformations "
                  "(a.k.a. FFD): l[inear], c[ubic] [default = linear]", true);
    app.addOption(proc.mmap_fields, "--mmap-fields",
                  "Try to memory-map the deformation fields instead of"
                  "loading them entirely in memory. This may improve the "
                  "performance for transforming sparse point sets.", true);
    app.addOption(proc.progress_file, "--progress",
                  "write progress info in this file. The file is opened in "
                  "append mode, so that it can be an existing file, "
                  "already opened by a monitoring application.", true );
    app.addOption(proc.volume_id, "--vol_id",
                  "When no tansform is specified (identity) and "
                  "transformation applies between volumes, then add a "
                  "translation to compensate the difference in voxel sizes "
                  "between input and output geometries, and keep the same "
                  "field of view instead of the same origin. This is normally "
                  "what we do when just changing an image resolution.", true);
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
    try
    {
      app.initialize();
    }
    catch(const carto::user_interruption &)
    {
      // Exit after printing e.g. help
      return EXIT_SUCCESS;
    }
    catch(const std::runtime_error &e)
    {
      clog << argv[0] << ": error processing command-line options: "
           << e.what() << endl;
      return EXIT_USAGE_ERROR;
    }


    bool ok = false;
    if(proc.points_mode)
    {
      ok = doPoints(proc, proc_input.filename);
    }
    else
    {
      if(proc.output.empty()) {
        std::clog << argv[0] << ": error: --output must be provided" << std::endl;
        return EXIT_USAGE_ERROR;
      }
      ok = proc.execute(proc_input.filename);
    }
    if(!ok)
      result = EXIT_FAILURE;

  }
  catch(std::exception &e) {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }
  return result;
}
