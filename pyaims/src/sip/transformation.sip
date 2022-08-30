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
#include <aims/transformation/transformation.h>
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
