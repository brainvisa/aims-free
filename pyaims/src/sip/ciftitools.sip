
namespace aims
{

  class CiftiTools
  {
%TypeHeaderCode
#include <aims/sparsematrix/ciftitools.h>
%End
%Docstring
CIFTI-2 shaped matrices manipulation tools.

This class eases manipulation of matrices (SparseOrDenseMatrix) with
CIFTI information in their header. CIFTI information specify the nature
of data contained in the matrix, and can bring ROIs or parcellations
information.

Mainly one can obtain textures to map matrix information onto meshes.
Textures may be ROI information (label textures, TimeTexture_S32),
or matrix values textures, possibly expanded onto brain regions
(TimeTexture_FLOAT in this case).
%End

  public:
    typedef std::map<std::string, int> BrainStuctureToMeshMap;
    typedef std::vector<carto::rc_ptr<TimeTexture<int> > > RoiTextureList;
    typedef std::vector<carto::rc_ptr<TimeTexture<float> > > TextureList;

    CiftiTools(
      rc_ptr_SparseOrDenseMatrix matrix,
      const aims::CiftiTools::BrainStuctureToMeshMap & smap
        = aims::CiftiTools::BrainStuctureToMeshMap() );
%Docstring
CiftiTools(matrix, smap={})

Parameters
----------
mat: rc_ptr_SparseOrDenseMatrix
    matrix with CIFTI header (typically, read from CIFTI format via
    aims.read())
smap: map_STRING_S32
    brain structure name to mesh number map: defaults to the result of
    defaultBrainStructureToMeshMap()
%End
    ~CiftiTools();

    rc_ptr_SparseOrDenseMatrix matrix();
    void setMatrix( rc_ptr_SparseOrDenseMatrix matrix );
    aims::CiftiTools::BrainStuctureToMeshMap & brainStructureMap();
    void setBrainStructureMap(
      const aims::CiftiTools::BrainStuctureToMeshMap & smap );

    carto::Object getDimensionObject( int dim ) const;
    SIP_PYLIST roiTextureFromDimension( int dim ) const;
%Docstring
roiTextureFromDimension(dim)

Get ROI information for a given matrix dimension.

If the matrix dimension defines ROIs (parcels, brain regions), textures
defining these regions are built.

If the matrix dimension defines labels, an empty texture will be
created, with appropriate colormap information

If the matrix dimensions defines scalars or series, this function will
probably not be useful.

Parameters
----------
dim: int
    dimension in the matrix to get info for

Returns
-------
textures: list of TimeTexture_S32
    textures of regions, possibly with colormaps in headers, for regions,
    parcels etc. One texture per mesh. For scalars or series, the list will be
    empty since no particular ROI description is contained in the header for
    this dimension.
%End
%MethodCode
  aims::CiftiTools::RoiTextureList* roilist
    = sipCpp->roiTextureFromDimension( a0 );
  if( (sipRes = PyList_New( roilist->size() )) != NULL )
  {
    aims::CiftiTools::RoiTextureList::iterator il, el = roilist->end();
    unsigned i = 0;
    for( il=roilist->begin(); il!=el; ++il, ++i )
    {
      PyObject *item = sipConvertFromType( il->get(),
                                           sipType_TimeTexture_S32, Py_None );
      if( !item || PyList_SetItem( sipRes, i, item ) < 0 )
      {
        Py_DECREF(sipRes);
        sipIsErr = 1;
        sipRes = 0;
        PyErr_SetString( PyExc_TypeError, "item conversion failed" );
        break;
      }
      else
      {
        il->release();
      }
    }
  }
  delete roilist;
%End

    SIP_PYLIST expandedValueTextureFromDimension(
      int dim,
      const vector_S32 & other_dim_index_pos //,
//      aims::CiftiTools::TextureList* existing_tex_list = 0
    ) const;
%Docstring
expandedValueTextureFromDimension(dim, other_dim_index_pos)

Get matrix data in textures, possibly expanded to regions if needed.

Parcels data will be expanded to all vertices of each parcel, etc.
The resulting textures can be mapped on appropriate brain meshes.

Parameters
----------
dim: int
    dimension in the matrix to get info for
other_dim_index_pos: vector_S32
    position in fixed dimensions. Actually all dimensions must be specified
    (the size of this list must match the number of dimensions) but the
    specified dimension position will not be used (since all values will be
    extracted in textures), and possibly another one (scalars, series, labels
    dimension will be extracted in time steps or several textures).

Returns
-------
textures: list of TimeTexture_FLOAT
    Scalar or time series data will be stored in texture timepoints.
    Labels data will be stored in separate textures because they provide
    possibly different colormaps. In this case the returned list will have
    the size meshes_number * labels_number, and will be stored in this
    order:
    [mesh0_label0, mesh1_label0, .. meshN_label0, mesh0_label1, ..
     meshN_label1, .. mesh0_label0, .. meshN_labelM]
%End
%MethodCode
  aims::CiftiTools::TextureList* roilist
    = sipCpp->expandedValueTextureFromDimension( a0, *a1, 0 );
  if( (sipRes = PyList_New( roilist->size() )) != NULL )
  {
    aims::CiftiTools::TextureList::iterator il, el = roilist->end();
    unsigned i = 0;
    for( il=roilist->begin(); il!=el; ++il, ++i )
    {
      PyObject *item = sipConvertFromType( il->get(),
                                           sipType_TimeTexture_FLOAT,
                                           Py_None );
      if( !item || PyList_SetItem( sipRes, i, item ) < 0 )
      {
        Py_DECREF(sipRes);
        sipIsErr = 1;
        sipRes = 0;
        PyErr_SetString( PyExc_TypeError, "item conversion failed" );
        break;
      }
      else
      {
        il->release();
      }
    }
  }
  delete roilist;
%End

    static aims::CiftiTools::BrainStuctureToMeshMap
      defaultBrainStructureToMeshMap();
%Docstring
defaultBrainStructureToMeshMap()

Define the brain structure to mesh mapping: several meshes may be
involved in CIFTI data mapping (typically a left hemisphere mesh and
a right

Returns
-------
mesh_map: map_STRING_S32
    mesh identifier to index mapping
%End

    int valuesDimNum();
    static int valuesDimNum( carto::Object cifti_info );
    static size_t valuesDimSize( const aims::SparseOrDenseMatrix & mat,
                                 int value_dim );
    static std::string dimensionType( carto::Object cifti_info, int dim );
    std::string dimensionType( int dim ) const;

    vector_S32 getIndicesForBrainStructure(
      int dim, const std::string & struct_name ) const;
%Docstring
getIndicesForBrainStructure(dim, struct_name)

Get the list of indices corresponding to a given brain structure in the matrix,
on a given dimension.
%End

    vector_S32 getIndicesForSurfaceIndices(
      int dim, int surface_num, const vector_S32 & roi_indices ) const;
%Docstring
getIndicesForSurfaceIndices(dim, surface_num, roi_indices)

Get the list of matrix indices corresponding to a given region on a
mesh, for a given dimension.

Parameters
----------
dim: int
    matrix dimension
surface_num: int
    surface number (same as given by brainStructureMap())
roi_indices: list or array of int
    indices in the surface mesh of vertices if interest

Returns
-------
indices: vector_S32
    list of indices in the matrix
%End

    list_STRING getBrainStructures( int dim, bool keepSurfaces=true,
                                    bool keepVoxels=true ) const;
%Docstring
getBrainStructures( dim, keepSurfaces=True, keepVoxels=True )

Retreive brain structures list in a brain models dimension

Parameters
----------
dim: int
    dimension in Cifti matrix
keepSurfaces: bool
    keep or filter out surface models
keepVoxels: bool
    keep or filter out voxesl models

Returns
-------
brain_structures: list of string
    brain structures names list
%End
  };

};

