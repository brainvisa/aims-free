namespace aims
{

  class Transformation : carto::RCObject
  {
%TypeHeaderCode
#include <aims/transformation/transformation.h>
%End

  public:
    virtual ~Transformation();

  protected:
    Transformation();
  };


  class Transformation3d : aims::Transformation
  {
%TypeHeaderCode
#include <aims/transformation/transformation.h>
%End

%TypeCode
#include <pyaims/vector/numpyarrayfunc.h>
%End

  public:
    virtual ~Transformation3d();

    virtual Point3dd transform( const Point3dd & pos ) const = 0;
    virtual Point3dd transformVector( const Point3dd & vec ) const;
    virtual Point3dd transformNormal( const Point3dd & dir ) const;
    Point3dd transformUnitNormal( const Point3dd & dir ) const;

    SIP_PYOBJECT transformPoints( SIP_PYOBJECT ) const /Factory/;
%MethodCode
    if( PyArray_Check(a0) )
    {
      PyArrayObject *arr = (PyArrayObject *) a0;
      if( arr->nd != 2 )
      {
        sipIsErr = 1;
        PyErr_SetString(PyExc_TypeError, "Array dimensions != 2");
      }
      else if( arr->descr->type_num != PyArray_DOUBLE )
      {
        sipIsErr = 1;
        PyErr_SetString( PyExc_TypeError, "wrong array data type: should be double" );
      }
      else if( arr->dimensions[1] != 3 )
      {
        sipIsErr = 1;
        PyErr_SetString(PyExc_TypeError, "Array width != 3");
      }
      else
      {
        npy_int stride0 = PyArray_STRIDE( arr, 0 );
        npy_int stride1 = PyArray_STRIDE( arr, 1 );
        npy_int npoints = arr->dimensions[0], i;
        char *point = (char *) arr->data;
        sipRes = PyArray_SimpleNew( 2, arr->dimensions, PyArray_DOUBLE );
        double *opoint = (double *) ((PyArrayObject *) sipRes)->data;
        for( i=0; i<npoints; ++i )
        {
          Point3dd vec( *(double *)point, *(double *) (point+stride1),
            *(double *) (point+stride1*2) );
          Point3dd tvec = sipCpp->transform( vec );
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
  };

};

