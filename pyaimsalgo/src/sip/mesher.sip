class Mesher
{
%TypeHeaderCode
#include <aims/mesh/mesher.h>
%End
%TypeCode
#ifndef AIMS_MAP_SIZE_T_list_AimsTimeSurface_3_VOID_DEFINED
typedef std::map<size_t, std::list<AimsTimeSurface<3,Void> > > map_SIZE_T_list_AimsTimeSurface_3_VOID;
#endif
%End

  public:
    enum SmoothingType
    {
      LAPLACIAN,
      SIMPLESPRING,
      POLYGONSPRING,
      LOWPASS,
    };

%Docstring
Mesh binary objects in a volume and produce surface meshes.
%End

  public:
    Mesher();
    virtual ~Mesher();

    void doit( const AimsData_S16&, const std::string&,
               const std::string& = "binar" ) /ReleaseGIL/;
%Docstring
doit(object_to_mesh, filename_base, write_mode="binar")

Mesh every interface of objects in the input label volume. Each mesh is written in a separate file. Files are numbered according to objects interfaces (label1_label2) and an interface number for this pair of objects.
write_mode is an old flag to write files in ascii or binary modes. It's obsolete.

object_to_mesh may be a volume of int16 values (labels), or a bucket.
When meshing a volume, the input volume should have a border of at least
1 voxel, filled with the value -1. If not, a new one will be allocated to perform the operation, thus using more memory and a copy overhead.
%End

    void doit( const BucketMap_VOID&, const std::string&,
               const std::string& = "binar" ) /ReleaseGIL/;

    void setSmoothing( SmoothingType smoothType, int nIteration, float smoothRate );
%Docstring
    setSmoothing(smoothType, nIteration, smoothRate)

    Smoothing parameters

    Parameters
    ----------
    smoothType: enum
        LOWPASS, LAPLACIAN, SIMPLESPRING, POLYGONSPRING
    smoothIt: int
        30
    smoothRate: float
      in [0.0;1.0] (instance : 0.4)
%End

/*
    map_SIZE_T_list_AimsTimeSurface_3_VOID doit(
      const rc_ptr_Volume_S16 & thing ) /Factory/;
%Docstring
  mesh_dict = mesher.doit(object_to_mesh)

  Mesh every interface of objects in the input label volume or bucket.
  The result is a map (dict-like) which keys are labels, and values are lists
  of meshes.
%End
%MethodCode
  std::map<size_t, std::list<AimsSurfaceTriangle> > *surf_map
    = new std::map<size_t, std::list<AimsSurfaceTriangle> >;
  sipCpp->doit( *a0, *surf_map );

  sipRes = surf_map;
%End
*/

    SIP_PYDICT doit(
      const rc_ptr_Volume_S16 & thing );
%MethodCode
  std::map<size_t, std::list<AimsSurfaceTriangle> > surf_map;
  sipCpp->doit( *a0, surf_map );

  PyObject *res = PyDict_New();
  std::map<size_t, std::list<AimsSurfaceTriangle> >::iterator i, e = surf_map.end();
  std::list<AimsSurfaceTriangle>::iterator il, el;
  size_t n;

  for( i=surf_map.begin(); i!=e; ++i )
  {
    PyObject *p = PyList_New( i->second.size() );
#if PY_VERSION_HEX >= 0x03000000
    PyDict_SetItem( res, PyLong_FromLong( i->first ), p );
#else
    PyDict_SetItem( res, PyInt_FromLong( i->first ), p );
#endif

    for( il=i->second.begin(), el=i->second.end(), n=0; il!=el; ++il, ++n )
    {
      AimsSurfaceTriangle *mesh = new AimsSurfaceTriangle( *il );
      PyObject *m = sipConvertFromType( mesh, sipFindType( "AimsTimeSurface_3_VOID" ), Py_None );
      PyList_SetItem( p, n, m );
    }
  }

  sipRes = res;
%End

    map_SIZE_T_list_AimsTimeSurface_3_VOID *doit(
      const BucketMap_VOID & thing ) /Factory/;
%MethodCode
  std::map<size_t, std::list<AimsSurfaceTriangle> > *surf_map
    = new std::map<size_t, std::list<AimsSurfaceTriangle> >;
  sipCpp->doit( *a0, *surf_map );

  sipRes = surf_map;
%End

    void setSmoothingLaplacian( float featureAngle );
%Docstring
    setSmoothingLaplacian(featureAngle)

    good value: 180. degrees
%End

    void setSmoothingSpring( float smoothForce );
%Docstring
    setSmoothingSpring(smoothForce)

    in [0.0;1.0]. Good value: 0.2
%End

    void unsetSmoothing();
    void smooth( AimsSurfaceTriangle& surface ) /ReleaseGIL/;

    void setDecimation( float, float, float, float );
%Docstring
    setDecimation(deciReductionRate, deciMaxClearance, deciMaxError,
                  deciFeatureAngle)

    good values:
    deciReductionRate: 99.0 (%)
    deciMaxClearance: 3.0
    deciMaxError: 0.2
    deciFeatureAngle: 180.0 (deg)
%End

    void unsetDecimation();

    float decimate( AimsSurfaceTriangle& surface );
    float decimate( AimsSurfaceTriangle& surface,
                    const vector_FLOAT & precthresholds,
                    const TimeTexture_FLOAT & precisionmap );

    void setSplitting();
    void unsetSplitting();

    void setLabelInf( int );
    void setLabelSup( int );

    void setMinSurface( float );

    void setMinFacetNumber( unsigned int );
    bool verbose() const;
    void setVerbose( bool);

    void getBrain( const AimsData_S16&, AimsSurfaceTriangle&, 
      bool insideinterface=false )
      throw ( std::runtime_error, carto::assert_error ) /ReleaseGIL/;
%Docstring
    getBrain(object_to_mesh, mesh, insideinterface=False)

    Get a smoothed mesh of the external (unless insideinterface is True) interface of the biggest object.

object_to_mesh may be a volume of int16 values (labels), or a bucket.
When meshing a volume, the input volume should have a border of at least
1 voxel, filled with the value -1. If not, a new one will be allocated to perform the operation, thus using more memory and a copy overhead.
%End

    void getBrain( const BucketMap_VOID&, AimsSurfaceTriangle&,
      bool insideinterface=false )
      throw ( std::runtime_error, carto::assert_error ) /ReleaseGIL/;

    void getWhite( const AimsData_S16& , AimsSurfaceTriangle& ) /ReleaseGIL/;
%Docstring
    getWhite(object_to_mesh, mesh)

    Get a smoothed mesh of the intternal interface of the biggest object.

object_to_mesh may be a volume of int16 values (labels), or a bucket.
When meshing a volume, the input volume should have a border of at least
1 voxel, filled with the value -1. If not, a new one will be allocated to perform the operation, thus using more memory and a copy overhead.
%End

    void getWhite( const BucketMap_VOID & thing,
                   AimsSurfaceTriangle & surface ) /ReleaseGIL/;

    void getSingleLabel(const AimsData_S16&, AimsSurfaceTriangle &)
      /ReleaseGIL/;
};
