
%ModuleHeaderCode
#include <aims/transformation/transformation_chain.h>
%End


namespace soma
{

  class Transformation : carto::RCObject
  {
%TypeHeaderCode
#include <aims/transformation/transformation.h>
%End
%Docstring
Base class for spatial transformations.
%End

  public:
    virtual ~Transformation();

    virtual bool isIdentity() const;
%Docstring
Test if the transformation can safely be omitted

This method must only return true if the transformation behaves exactly
like an identity transformation (notably, the transform methods will always
return the input coordinates unchanged).

NOTE: Implementors of derived classes may choose to always return false if
a test would be difficult to implement or expensive to run.
%End

    carto::Object header();
    void setHeader( carto::Object ph );
    virtual vector_S32 transform( const vector_S32 & pos ) const;
    virtual vector_FLOAT transform( const vector_FLOAT & pos /Constrained/ ) const;
    virtual vector_DOUBLE transform( const vector_DOUBLE & pos ) const = 0;
    virtual vector_S32 transformVector( const vector_S32 & pos ) const;
    virtual vector_FLOAT transformVector( const vector_FLOAT & pos /Constrained/ ) const;
    virtual vector_DOUBLE transformVector( const vector_DOUBLE & pos ) const;

    virtual bool invertible() const;
    rc_ptr_Transformation getInverse() const;

    virtual bool isDirect() const = 0;

    static vector_S32 vadd( const vector_S32 & v1 /Constrained/,
                            const vector_S32 & v2 /Constrained/ );
    static vector_FLOAT vadd( const vector_FLOAT & v1 /Constrained/,
                              const vector_FLOAT & v2 /Constrained/ );
    static vector_DOUBLE vadd( const vector_DOUBLE & v1,
                               const vector_DOUBLE & v2 );
%Docstring
    vector arithmetics, as convenience static functions
%End

    static vector_S32 vsub( const vector_S32 & v1 /Constrained/,
                            const vector_S32 & v2 /Constrained/);
    static vector_FLOAT vsub( const vector_FLOAT & v1 /Constrained/,
                              const vector_FLOAT & v2 /Constrained/);
    static vector_DOUBLE vsub( const vector_DOUBLE & v1 ,
                               const vector_DOUBLE & v2 );
%Docstring
    vector arithmetics, as convenience static functions
%End

    static vector_S32 vadd( const vector_S32 & v1 /Constrained/,
                            const int & v2 );
    static vector_FLOAT vadd( const vector_FLOAT & v1 /Constrained/,
                              const float & v2 );
    static vector_DOUBLE vadd( const vector_DOUBLE & v1,
                               const double & v2 );
%Docstring
    vector arithmetics, as convenience static functions
%End

    static vector_S32 vsub( const vector_S32 & v1 /Constrained/,
                            const int & v2 );
    static vector_FLOAT vsub( const vector_FLOAT & v1 /Constrained/,
                              const float & v2 );
    static vector_DOUBLE vsub( const vector_DOUBLE & v1,
                               const double & v2 );
%Docstring
    vector arithmetics, as convenience static functions
%End

    static vector_S32 vadd( const int & v1,
                            const vector_S32 & v2 /Constrained/ );
    static vector_FLOAT vadd( const float & v1,
                              const vector_FLOAT & v2 /Constrained/ );
    static vector_DOUBLE vadd( const double & v1,
                               const vector_DOUBLE & v2 );
%Docstring
    vector arithmetics, as convenience static functions
%End

    static vector_S32 vsub( const int & v1,
                            const vector_S32 & v2 /Constrained/ );
    static vector_FLOAT vsub( const float & v1,
                              const vector_FLOAT & v2 /Constrained/ );
    static vector_DOUBLE vsub( const double & v1,
                               const vector_DOUBLE & v2 );
%Docstring
    vector arithmetics, as convenience static functions
%End

  protected:
    Transformation();
  };

};

%Include rcptr_Transformation3d.sip
%Include rcptr_Transformation.sip

namespace soma
{

  class Transformation3d : soma::Transformation
  {
%TypeHeaderCode
#include <soma-io/transformation/transformation.h>
%End

%TypeCode
#include <pyaims/vector/numpyarrayfunc.h>
%End
%Docstring
Base class for spatial transformations in 3D.
%End

  public:
    virtual ~Transformation3d();

    Point3d transform( const Point3d & p /Constrained/ ) const;
    Point3df transform( const Point3df & dir /Constrained/ ) const;
    Point3dd transform( const Point3dd & pos /Constrained/ ) const;
    virtual vector_S32 transform( const vector_S32 & pos ) const;
    virtual vector_FLOAT transform( const vector_FLOAT & pos /Constrained/ ) const;
    virtual vector_DOUBLE transform( const vector_DOUBLE & pos ) const;
    Point3dd transform( double x, double y, double z ) const;
    virtual vector_S32 transformVector( const vector_S32 & pos ) const;
    virtual vector_FLOAT transformVector( const vector_FLOAT & pos /Constrained/ ) const;
    virtual vector_DOUBLE transformVector( const vector_DOUBLE & pos ) const;

    SIP_PYOBJECT transformPoints( SIP_PYOBJECT ) const /Factory/;
%MethodCode
    if( PyArray_Check(a0) )
    {
      PyArrayObject *arr = (PyArrayObject *) a0;
      if( PyArray_NDIM( arr ) != 2 )
      {
        sipIsErr = 1;
        PyErr_SetString(PyExc_TypeError, "Array dimensions != 2");
      }
      else if( PyArray_DESCR( arr )->type_num != NPY_DOUBLE )
      {
        sipIsErr = 1;
        PyErr_SetString( PyExc_TypeError, "wrong array data type: should be double" );
      }
      else if( PyArray_DIMS( arr )[1] != 3 )
      {
        sipIsErr = 1;
        PyErr_SetString(PyExc_TypeError, "Array width != 3");
      }
      else
      {
        npy_int stride0 = PyArray_STRIDE( arr, 0 );
        npy_int stride1 = PyArray_STRIDE( arr, 1 );
        npy_int npoints = PyArray_DIMS( arr )[0], i;
        char *point = (char *) PyArray_DATA( arr );
        sipRes = PyArray_SimpleNew( 2, PyArray_DIMS( arr ), NPY_DOUBLE );
        double *opoint = (double *) PyArray_DATA((PyArrayObject *) sipRes);
        for( i=0; i<npoints; ++i )
        {
          Point3dd tvec = sipCpp->transform( *(double *)point,
            *(double *) (point+stride1), *(double *) (point+stride1*2) );
          *opoint++ = tvec[0];
          *opoint++ = tvec[1];
          *opoint++ = tvec[2];
          point += stride0;
        }

        if(sipRes) sipRes = PyArray_Return((PyArrayObject *) sipRes);
      }
    }
    else
    {
      sipIsErr = 1;
      PyErr_SetString(PyExc_TypeError, "wrong argument type");
    }
%End

    // conversion from Object
    static  rc_ptr_Transformation3d
    fromObject( carto::GenericObject ) /Factory/;
%MethodCode
    try
    {
      carto::rc_ptr<soma::Transformation3d> & obj
        = a0->value<carto::rc_ptr<soma::Transformation3d> >();
      sipRes = new carto::rc_ptr<soma::Transformation3d>( obj );
    }
    catch( std::exception & e )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, e.what() );
      sipRes = 0;
    }
%End

  // conversion to Object

  carto::Object toObject();
%MethodCode
  sipRes = new carto::Object(
    carto::Object::value(
      carto::rc_ptr<soma::Transformation3d>( sipCpp ) ) );
%End

  protected:
    Transformation3d();
    virtual Point3dd transformDouble( double x, double y, double z ) const = 0;
  };


  class AffineTransformationBase : soma::Transformation
  {
%TypeHeaderCode
#include <soma-io/transformation/affinetransformation3d_base.h>
%End

%TypeCode
#include <pyaims/vector/numpyarrayfunc.h>
%End

public:
    AffineTransformationBase( int order = 3 );
    AffineTransformationBase( const soma::AffineTransformationBase& other );
    AffineTransformationBase( const vector_FLOAT & mat );
    AffineTransformationBase( const carto::Object mat );
    virtual ~AffineTransformationBase();

    virtual bool operator == ( const soma::AffineTransformationBase & ) const;

    soma::AffineTransformationBase & operator *= (
      const soma::AffineTransformationBase & trans );
    // soma::AffineTransformationBase operator - () const;
    soma::AffineTransformationBase* __neg__() const /Factory/;
%MethodCode
  sipRes = new soma::AffineTransformationBase( -*sipCpp );
%End

    bool isIdentity() const;
    virtual void setToIdentity();
    virtual bool isDirect() const;

    virtual bool invertible() const;
    rc_ptr_Transformation getInverse() const;

    int order() const;
    virtual void extendOrder( unsigned n );
    virtual void squeezeOrder( unsigned n, bool check = true,
                               bool notify_fail = true );

    virtual vector_S32 transform( const vector_S32 & pos ) const;
    virtual vector_FLOAT transform( const vector_FLOAT & pos /Constrained/ ) const;
    virtual vector_DOUBLE transform( const vector_DOUBLE & pos ) const;

    vector_FLOAT toVector() const;
    vector_FLOAT toColumnVector() const;
    void fromColumnVector( const vector_FLOAT & vec );

    SIP_PYOBJECT __array__() /Factory/;
%MethodCode
  int n = sipCpp->matrix().ncols;
  vector_S32 dims( 2, n );

  PyArray_Descr *descr = PyArray_DescrFromType( NPY_FLOAT32 );
  sipRes = aims::initNumpyArray( sipSelf, descr, dims.size(),
                                 &dims[0],
                                 (char *) &(sipCpp->matrix()[0]),
                                 false );
%End

  void _arrayDestroyedCallback( SIP_PYOBJECT );
%MethodCode
  // remove weak reference in volume
  PyObject_DelAttrString( sipSelf, "_arrayref" );
  // dec reference to self that was manually incremented in __array__()
  // when building the numpy array
  Py_DECREF( sipSelf );
  // the aditional ref to self will be deleted when the callback
  // (method bound on self) is destroyed, just after now
  // so self can be destroyed safely
%End

  };


  class AffineTransformation3dBase
    : soma::Transformation3d, soma::AffineTransformationBase
  {
%TypeHeaderCode
#include <soma-io/transformation/affinetransformation3d_base.h>
%End

  public:
    AffineTransformation3dBase();
    AffineTransformation3dBase( const soma::AffineTransformation3dBase& other );
    AffineTransformation3dBase( const vector_FLOAT & mat );
    AffineTransformation3dBase( const carto::Object mat );
    virtual ~AffineTransformation3dBase();

    virtual bool operator == ( const soma::AffineTransformation3dBase & ) const;

    soma::AffineTransformation3dBase & operator *= (
      const soma::AffineTransformation3dBase & trans );
    // soma::AffineTransformation3dBase operator - () const;

    virtual bool isDirect() const;
    Point3d transform( const Point3d & p /Constrained/ ) const;
    Point3df transform( const Point3df & dir /Constrained/ ) const;
    Point3dd transform( const Point3dd & pos /Constrained/ ) const;
    Point3dd transform( double x, double y, double z ) const;
    virtual vector_S32 transform( const vector_S32 & pos ) const;
    virtual vector_FLOAT transform( const vector_FLOAT & pos /Constrained/ ) const;
    virtual vector_DOUBLE transform( const vector_DOUBLE & pos ) const;

    Point3df transformVector( const Point3df & dir /Constrained/ ) const;
    Point3dd transformVector( const Point3dd & vec /Constrained/ ) const;
    Point3dd transformVector( double x, double y, double z ) const;
    // Point3df transformVector( float x, float y, float z ) const;
    // Point3di transformVector( int x, int y, int z ) const;

    virtual vector_S32 transformVector( const vector_S32 & pos ) const;
    virtual vector_FLOAT transformVector( const vector_FLOAT & pos /Constrained/ ) const;
    virtual vector_DOUBLE transformVector( const vector_DOUBLE & pos ) const;

    Point3df transformNormal( const Point3df & dir /Constrained/ ) const;
    Point3dd transformNormal( const Point3dd & dir /Constrained/ ) const;
    Point3dd transformNormal( double x, double y, double z ) const;
    // Point3df transformNormal( float x, float y, float z ) const;
    Point3df transformUnitNormal( const Point3df & dir /Constrained/ ) const;
    Point3dd transformUnitNormal( const Point3dd & dir ) const;
    Point3dd transformUnitNormal( double x, double y, double z ) const;
    // Point3df transformUnitNormal( float x, float y, float z ) const;

    bool isIdentity() const;
    virtual void setToIdentity();

    virtual void extendOrder( unsigned n );

    virtual bool invertible() const;
    rc_ptr_Transformation getInverse() const;
    virtual void scale( const Point3df& sizeFrom, const Point3df& sizeTo );

    void setTranslation(Point3df trans);

  protected:
    virtual Point3dd transformDouble( double x, double y, double z ) const;
  };

};


namespace aims
{

class TransformationChain3d : soma::Transformation3d
{
%TypeHeaderCode
#include <aims/transformation/transformation_chain.h>
%End

%Docstring
      Container for a composition of multiple transformations.

      This container holds a list of transformations, and acts as the
      composition of all transformations. Transformations are composed from the
      front to the back of the list::

          chain = TransformationChain3d()
          chain.push_back(t1)
          chain.push_back(t2)
          # chain->transform(p) == t2.transform(t1.transform(p))

      **warning**: Do not modify the transformations once you have passed them to
      push_back() or push_front(): it is unspecified if the changes will be
      noticed by TransformationChain3d (a reference to the same object may be
      kept internally, or a copy could be made). This behaviour will allow
      optimizations to be implemented (e.g. composing adjacent affine
      transformations by multiplying their matrices).
%End

public:

  TransformationChain3d();

  void push_back(const rc_ptr_Transformation3d & transformation);
%Docstring
    chain.push_back(transformation)

    Add a transformation to the back of the list (applied last)
%End

  void pop_back();
%Docstring
    chain.pop_back()

    Remove the last transformation from the list
%End

  void push_front(const rc_ptr_Transformation3d & transformation);
%Docstring
    chain.push_front(transformation)

    Add a transformation to the front of the list (applied first)
%End

  void pop_front();
%Docstring
    chain.pop_front()

    Remove the first transformation from the list
%End

  size_t size() const;

  Py_ssize_t __len__() const;
%MethodCode
  sipRes = int( sipCpp->size() );
%End

  rc_ptr_Transformation3d __getitem__( int ) const;
%MethodCode
  if( a0 < 0 || a0 >= (long)sipCpp->size() )
  {
    sipRes = 0;
    sipIsErr = 1;
    PyErr_SetString( PyExc_IndexError, "std::list index out of range" );
  }
  else
  {
    aims::TransformationChain3d::const_iterator i, e = sipCpp->end();
    int n = 0;
    for( i=sipCpp->begin(); i!=e && n < a0; ++i, ++n )
    {}
    if( i != e )
    {
      sipRes = new carto::rc_ptr<soma::Transformation3d>(
        const_cast<soma::Transformation3d *>( i->pointer() ) );
    }
  }
%End

  bool isIdentity() const;
  virtual bool invertible() const;
  rc_ptr_Transformation getInverse() const;
  virtual bool isDirect() const;

  rc_ptr_Transformation3d simplify( bool allow_ffd = false ) const;
%Docstring
      Compute a simpler transformation that is equivalent to the chain.

      The transformation chain is simplified by applying the following rules,
      so that it should provide equivalent results to the original chain, up to
      numerical precision:

      - The simplification is applied recursively to any sub-chain.

      - Transformations of the simplified sub-chains are inserted at the top
        level (i.e. the simplified chain is flat, it contains no sub-chains).

      - Consecutive affine transformations are composed using matrix
        multiplication.

      - Identity transforms (i.e. transforms for which
        Transformation::isIdentity() returns true) are removed from the list.

      - If the simplified chain consists of only one transformation, no chain
        is returned, the contained transformation is returned directly.
        However, the transformation is not necessarily the same instance that
        was inserted in the chain (it will be a copy if it is an
        AffineTransformation3d).

      - If the simplified chain is empty, an empty chain is returned.

      No deep copy is made, so the result can contain pointers to the same
      transformations as the original chain.

      If the parameter allow_ffd is set to True, and if the chain contains at
      least a FFD deformation field, then the whole chain is reduced to a
      single FFD field, with the resolution of the first one found in the
      chain.
%End

%MethodCode
  carto::const_ref<soma::Transformation3d> simple = sipCpp->simplify( a0 );
  carto::rc_ptr<soma::Transformation3d> *res
    = new carto::rc_ptr<soma::Transformation3d>(
      const_cast<soma::Transformation3d *>( simple.pointer() ) );
  sipRes = res;
%End

  protected:
    virtual Point3dd transformDouble( double x, double y, double z ) const;
};


  class TransformationGraph3d : Graph
  {
%TypeHeaderCode
#include <aims/transformation/transformationgraph3d.h>
%End

%Docstring
    Manage transformations (affine and non-linear) transformations graph.

    It maintains a graph of directed transformations (vertices are
    referentials, edges are 3D transformations).

    It can be populated from a dictionary (Object) which can be read from a
    JSON or YAML file, and performs lazy-loading: transformations are actually
    loaded when they are used, more precisely in getTransformation() and when
    building a chain of composed transformations.

    The graph is oriented, and does not manage inverse transforms. We could
    thing this could be done automatically for affine or other inversible
    transformations, but the lazy loading feature does not allow to know if a
    not yet loaded transform will be inversible or not. So this has to be done
    manually for now.

    The graph populates with composed transforms when they are used: indirect
    paths from a referential to another are generated when needed, and stored
    in the graph as "deduced" transforms. Later access will thus use this cache
    instead of reccomputing it.
    A drawback, thus, is that the graph has to be used "statically": any
    modification in the graph structure, or in any of its transformations,
    should invalidate a number of deduced, cached ones, which we do not handle
    for now (it's a quite difficult problem to manage it efficiently). To date
    the only solution is to completely clear the cached transforms
    (clearCache()) after any modification.

    The TransformationGraph3d structure is a Graph, and its elements are Vertex
    or Edge objects. To manipulate transformations, the API generally allows to
    use identifiers (generally UUIDs) for elements, and the Transformation
    objects are accessed through the "transformation" property of edges, or
    using the transformation() convenience method:

        ref1 = tgraph.referentialById("23df7ce8-e405-bc31-3863-d543e3cc89e5")
        trans_edge = tgraph.getTransformation(
            "23df7ce8-e405-bc31-3863-d543e3cc89e5",
            "52929a27-6838-1d08-d6fc-7905ed9413a0")
        if trans_edge:
            trans = tgraph.transformation(trans_edge)

    see also:
    https://hbp-spatial-backend.apps.hbp.eu/swagger-ui#/api_v1/get_v1_transform_point
 %End

  public:
    TransformationGraph3d();
    virtual ~TransformationGraph3d();

    void updateIds();
%Docstring
    Update the internal UUIDs cache. Needs to be done after changes in
    elements UUIDs or insertion / deletions
%End

    Vertex* referentialById( const std::string & id ) const;
%Docstring
    Get the vertex (referential) with given ID.

    The id may be a referential ID or name, or a code identifying a
    particular referential, like:

    - "mni", or "mni152", or "NIFTI_XFORM_MNI_152" or
      "Talairach-MNI template-SPM"
      (``StandardReferentials.mniTemplateReferential()``), to designate the
      ICBM152 standard referential
    - "acpc", or "talairach", or "NIFTI_XFORM_TALAIRACH", or
      "Talairach-AC/PC-Anatomist"
      (``StandardReferentials.acPcReferential()``), to designate AIMS
      Talairach referential
%End

    Vertex* referentialByCode( const std::string & id,
                               carto::Object header,
                               const vector_STRING & refs ) const;
%Docstring
    Same as referentialById(id), but with the help of
    an object header, can look for additional codes:

    - "AIMS": the internal AIMS referential of the object
    - "first": first referential in the transformations destinations in the
      given header
    - "last": last referential in the header
    - a numpber or lexical number as string to designate the referential in
      referenced transformations destinations in the object header        -
      "auto": last referential in header, if one is present, or fallback to
      the internal AIMS referential of the object otherwise.
    - "aligned", or "NIFTI_XFORM_ALIGNED_ANAT", or
      "Coordinates aligned to another file or to anatomical truth", to
      designate a transformation (NIFTI-like) going to another object space
      as described this way
    - "NIFTI_XFORM_TEMPLATE_OTHER" to designate a different template as
      described in the object header (NIFTI-like)
    - "disk": the "disk space" orientation: voxels in the order they are
      stored on disk. Some software are working in this referential. Note
      that this referential is only available in the graph if the object
      header has been included using updateFromObjectHeader() with the
      parameter includeDiskSpace set to True.
    - other codes may be supported later, like "qform", "sform", "ITK",
      "ANTS", but are not implemented yet.

    The object header should have been included in the transformations
    graph using updateFromObjectHeader() before calling this method.

    The object header is still needed here because looking for the
    specified transformations are linked tot the order the specific header
    and the order they are given in it.

    The additional parameter, refs, should be the list returned by
    updateFromObjectHeader(). It is the list of destinaton referentials in
    the object header, but with possibly different codes or IDs because the
    original header may use ambiguous, non-specific identifiers.

    If not found for a known reason, the function will throw a
    runtime_error exception with a message. Otherwise 0 might be returned.
%End

    Edge* transformationById( const std::string & id ) const;
%Docstring
    Get the edge (transformation) with given ID
%End

    static rc_ptr_Transformation3d transformation( const Edge *edge );
%Docstring
    Get the Transformation3d object inside an edge. The returned reference
    counting pointer may contain a null pointer.
%End

    rc_ptr_Transformation3d transformation( const std::string & id ) const;
%Docstring
    Get the Transformation3d object for the given UUID.
    The returned reference counting pointer may contain a null pointer.
%End

    Edge* getTransformation( const std::string & src_ref,
                             const std::string & dst_ref ) const;
%Docstring
        edge = tgraph.getTransformation(src_ref, dst_ref, allow_compose=False)

        Get the transformation between source_ref and dst_ref. If it is not a
        registered transformation, return None.
        src_ref annd dst_ref may be string IDs or Vertex objects.

        If allow_compose is True and the transformation is not found, then a
        transformations chain path is looked for. If a matching one is found,
        it is then registered in the graph to allow fast access later. The
        drawback of this register operation is that it cannot react to changes
        in the transformation chain (if a new transformation is added and could
        make a shorter path, it will not be taken into account).
%End

    Edge* getTransformation( const Vertex *src_ref,
                             const Vertex *dst_ref ) const;
    Edge* getTransformation( const std::string & src_ref,
                             const std::string & dst_ref,
                             bool allow_compose );
    Edge* getTransformation( Vertex *src_ref,
                             Vertex *dst_ref,
                             bool allow_compose );

    Edge* getTransformation_raw( const Vertex *src_ref,
                                 const Vertex *dst_ref ) const;
%Docstring
    getTransformation_raw(src_ref, dst_ref)

    same as getTransformation(), but do not perform lazy loading
%End

    Edge* getTransformation_raw( const std::string & src_ref,
                                 const std::string & dst_ref ) const;

    rc_ptr_Transformation3d getTransformChain(
      const Vertex *src_ref, const Vertex *dst_ref ) const;
%Docstring
        Get a transformation chain between two vertices.

        You normally don't need to call this method directly, il is called
        through getTransformation( src_ref, dst_ref, true ). This method
        however does not modify the transformations graph and does not register
        the combined transformation.
        It will, however, load lazy-loading transformations.
%End

    rc_ptr_Transformation3d getTransformChain(
      const std::string & src_ref, const std::string & dst_ref ) const;

    Edge *registerTransformation(
      const std::string & src_ref, const std::string & dst_ref,
      rc_ptr_Transformation3d trans, bool deduced=false );
%Docstring
        Add (or register) the given transformation in the transformations
        graph.

        transform may be a filename, or a Transformation instance. a
        Transformation is supposed to be a non-linear FFD (Free Form
        Deformation), not a linear transformation. If transform is a string, it
        may be:
        - an empty string will be an identity transformation
        - a .trm file will be an affine transformation, and will be passed to
          the builtin Anatomist system
        - another file (.ima) will be a FFD transformation file and will be
          registered as its filename. Loading will happen only when the
          transformation is used, in get_transformation().
%End

    Edge *registerTransformation(
      Vertex *src_ref, Vertex *dst_ref, rc_ptr_Transformation3d trans,
      bool deduced=false );
    Edge *registerTransformation(
      const std::string & src_ref, const std::string & dst_ref,
      const std::string & filename );
    Edge *registerTransformation(
      Vertex *src_ref, Vertex *dst_ref, const std::string & filename );

    void loadTransformationsGraph( carto::Object desc,
                                   const std::string & dirname );
%Docstring
        Load a full transformations graph from a dict-like object.

        The description dict may typically be read from a .json or a .yaml
        file.

        The description is organized as a 2-level dictionary::

            {
                source_ref_id: {
                    dest_ref_id1: transformation_filename1,
                    dest_ref_id2: transformation_filename2,
                },
                ...
            }

        A referential ID is a string which may represent an UUID, or a name.

        A transformatioon filename may be an affine (.trm) or non-linear (.ima)
        transformation file, or an empty string to represent an identity
        transformation.

        The transformation files are found relatively to the directory given as
        the dirname parameter.

        Alternatively to file names, an affine transformation may be given
        directly as its 4x4 matrix, written as a line vector (all 16
        coefficients in a vecotr, describing the matrix horizontally). Ex::

            {
                source_ref_id: {
                    dest_ref_id1: [1, 0, 0, 0,
                                   0, 1, 0, 0,
                                   0, 0, 1, 0,
                                   0, 0, 0, 1],
                },
                ...
            }
%End

    void clearCache( bool chain_only=false );
%Docstring
    clearCache(chain_only=False)

    remove deduced transformations (built from composition).

    If chain_only is true, then inverses of direct transforms are not
    removed.
%End

    void registerInverseTransformations( bool loadAffines = false );
%Docstring
        registerInverseTransformations(loadAffines=False)

        register inverse transformations when they can be obtained.

        This can be done only for loaded transformations. So the lazy loading
        feature will prevent for it to work on not already loaded
        transformations, since we cannot know if they are invertible before
        they are loaded.

        For this reason, the loadAffines parameter allows to load affine
        transforms and register their inverses.
%End

    void loadAffineTransformations();
%Docstring
        Load all affine transformations.
%End

    carto::Object asDict( bool affine_only = false, bool allow_read = false,
                          bool embed_affines = false ) const;
%Docstring
        convert to a dict-like structure compatible with
        loadTransformationsGraph()

        Parameters
        ----------
        affine_only: bool
            if true, inclue only affine transformations
        allow_read: bool
            if true, and if affine_only or embed_affines is also
            true, all affine transformations will be read in order to determine
            which are affine.
            Otherwise only those already loaded in memory (via the lazy
            reading mechanism) will be known to be affine. This parameter has
            no effect on non-affine transformations.
        embed_affines: bool
            if true, affine transformations are not recorded as a
              file name (.trm format), but as an embedded 4x4 matrix.
%End

    vector_STRING updateFromObjectHeader( carto::Object header,
                                          bool includeDiskSpace = false );
%Docstring
    Insert all transformations / referentials found in an object header.

    Returns the translated referentials names in the order they are in the
    object header, beginning with the object referential itself.
    For instance for a header with the properties::

      {
        "referential": "A",
        "referentials": ["B", "Talairach-MNI template-SPM",
                          "Scanner-based anatomical coordinates"],
        "transformations": [...]  // list of 3 transformations
      }

    it will return::

      ["A", "B", "803552a6-ac4d-491d-99f5-b938392b674b",
        "Scanner-based anatomical coordinates_A"]

    If the header does not contain a "referential" property, then a new
    UUID will be generated for it.

    If the optional parameter includeDiskSpace is set to true, then
    transformations to disk orientation (in mm) will also be included.
%End
  };

};


// Compose Transformation3d using TransformationChain3d
rc_ptr_Transformation3d
    operator * ( const rc_ptr_Transformation3d & transformation3d1,
                 const rc_ptr_Transformation3d & transformation3d2 )
        /ReleaseGIL/;
%MethodCode
    sipRes = new carto::rc_ptr<soma::Transformation3d>(
      aims::operator * ( *a0, *a1 ) );
%End

soma::Transformation*
    operator * ( const soma::AffineTransformationBase & transformation3d1,
                 const soma::AffineTransformationBase & transformation3d2 )
        /Factory, ReleaseGIL/;
%MethodCode
    sipRes =
      new soma::AffineTransformationBase( soma::operator * ( *a0, *a1 ) );
%End


soma::Transformation*
    operator * ( const soma::AffineTransformation3dBase & transformation3d1,
                 const soma::AffineTransformation3dBase & transformation3d2 )
        /Factory, ReleaseGIL/;
%MethodCode
    sipRes =
      new soma::AffineTransformation3dBase( soma::operator * ( *a0, *a1 ) );
%End

soma::Transformation3d*
    operator * ( const soma::Transformation3d & transformation3d1,
                 const soma::Transformation3d & transformation3d2 )
        /Factory, ReleaseGIL/;
%MethodCode
    carto::rc_ptr<soma::Transformation3d>
      t1( const_cast<soma::Transformation3d *>( a0 ) ),
      t2( const_cast<soma::Transformation3d *>( a1 ) );
    carto::rc_ptr<soma::Transformation3d> t3 = aims::operator * ( t1, t2 );
    sipRes = t3.get();
    t1.release();
    t2.release();
    t3.release();
%End

