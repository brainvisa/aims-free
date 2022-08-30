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

%ConvertToSubClassCode
  // std::cout << "ConvertToSubClassCode Transformation " << sipCpp << " / " << typeid( *sipCpp ).name() << std::endl;

  sipType = 0;
  if( dynamic_cast<carto::Transformation3d *>( sipCpp ) )
  {
    sipType = sipType_soma_Transformation3d;
    *sipCppRet = dynamic_cast<soma::Transformation3d *>( sipCpp );
    // std::cout << "I'm a Transformation3d: " << sipCppRet << std::endl;
    if( dynamic_cast<aims::AffineTransformation3d *>( sipCpp ) )
    {
      sipType = sipType_aims_AffineTransformation3d;
      *sipCppRet = dynamic_cast<aims::AffineTransformation3d *>( sipCpp );
      // std::cout << "I'm an AffineTransformation3d: " << sipCppRet << std::endl;
    }
    else if( dynamic_cast<aims::TransformationChain3d *>( sipCpp ) )
    {
      sipType = sipType_aims_TransformationChain3d;
      *sipCppRet = dynamic_cast<aims::TransformationChain3d *>( sipCpp );
      // std::cout << "I'm an TransformationChain3d: " << sipCppRet << std::endl;
    }
  }

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

  protected:
    Transformation();
  };


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

    Point3dd transform( double x, double y, double z ) const;
    Point3dd transform( const Point3dd & pos ) const;
    Point3df transform( const Point3df & dir ) const;
    Point3d transform( const Point3d & p ) const;

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

  protected:
    Transformation3d();
    virtual Point3dd transformDouble( double x, double y, double z ) const = 0;
    virtual Point3dd transformPoint3dd( const Point3dd & pos ) const;
    virtual Point3df transformPoint3df( const Point3df & dir ) const;
    virtual Point3d transformPoint3d( const Point3d & p ) const;
    virtual Point3df transformFloat( float x, float y, float z ) const;
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

  int __len__() const;
%MethodCode
  sipRes = int( sipCpp->size() );
%End

  bool isIdentity() const;
  bool invertible() const;
  rc_ptr_Transformation3d getInverse() const;

  rc_ptr_Transformation3d simplify() const;
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
%End

%MethodCode
  carto::const_ref<soma::Transformation3d> simple = sipCpp->simplify();
  carto::rc_ptr<soma::Transformation3d> *res
    = new carto::rc_ptr<soma::Transformation3d>(
      const_cast<soma::Transformation3d *>( simple.pointer() ) );
  sipRes = res;
%End

  protected:
    virtual Point3dd transformDouble( double x, double y, double z ) const;
};

};
