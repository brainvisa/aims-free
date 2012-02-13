
class cfloat
{
%TypeHeaderCode
#include <cartobase/type/types.h>
%End

%ConvertToTypeCode
  if (sipIsErr == NULL)
    return ( PySequence_Check( sipPy ) && PySequence_Size( sipPy ) <= 2 )
           || sipCanConvertToInstance(sipPy, sipClass_cfloat,
                SIP_NOT_NONE | SIP_NO_CONVERTORS );

  if( sipCanConvertToInstance( sipPy, sipClass_cfloat, SIP_NO_CONVERTORS ) )
  {
    int state = 0;

    cfloat * dat
      = (cfloat *)
        sipConvertToInstance( sipPy, sipClass_cfloat,
          sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
    if( *sipIsErr && dat )
    {
      sipReleaseInstance( dat, sipClass_cfloat, state );
      dat = 0;
    }
    else if( dat )
    {
      *sipCppPtr = dat;
      return 0;
    }
  }

  if( PySequence_Check( sipPy ) )
  {
    *sipCppPtr = new cfloat;
    unsigned    i, n = PySequence_Size( sipPy );
    PyObject    *pyitem;
    for( i=0; i<n; ++i )
    {
      pyitem = PySequence_GetItem( sipPy, i );
      if( !pyitem || !PyFloat_Check( pyitem ) )
      {
        *sipIsErr = 1;
        delete *sipCppPtr;
        sipCppPtr = 0;
        std::ostringstream s;
        s << "wrong list item type, item " << i;
        PyErr_SetString( PyExc_TypeError, s.str().c_str() );
        return 0;
      }

      if( i == 0 )
        (*sipCppPtr)->real() = (float) PyFloat_AsDouble( pyitem );
      else
        (*sipCppPtr)->imag() = (float) PyFloat_AsDouble( pyitem );
      Py_DECREF( pyitem );
    }
    return sipGetState( sipTransferObj );
  }
  *sipCppPtr =
     (cfloat *) sipConvertToInstance( sipPy, sipClass_cfloat,
                           0, SIP_NO_CONVERTORS, 0, sipIsErr );
  return 0;
%End

public:
  cfloat( float r = float(), float i = float() );
  cfloat( const cfloat & );
  cfloat( SIP_PYOBJECT );
%MethodCode
  sipCpp = 0;
  if( !PySequence_Check( a0 ) )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "wrong argument type" );
  }
  else
  {
    unsigned    i, n = PySequence_Size( a0 );
    if( n > 2 )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_ValueError, "too many items" );
    }
    else
    {
      sipCpp = new cfloat;
      PyObject    *pyitem;
      for( i=0; i<n; ++i )
      {
        pyitem = PySequence_GetItem(a0,i);
        if( !pyitem || !PyFloat_Check( pyitem ) )
        {
          sipIsErr = 1;
          delete sipCpp;
          sipCpp = 0;
          std::ostringstream s;
          s << "wrong list item type, item " << i;
          PyErr_SetString( PyExc_TypeError, s.str().c_str() );
          break;
        }

        if( i == 0 )
          sipCpp->real() = (float) PyFloat_AsDouble( pyitem );
        else
          sipCpp->imag() = (float) PyFloat_AsDouble( pyitem );
      }
    }
  }
%End

  float real() const;
  float imag() const;
  void real( float val );
%MethodCode
  sipCpp->real() = a0; // older compilers do not support complex::real( value )
%End
  void imag( float val );
%MethodCode
  sipCpp->imag() = a0;
%End
  cfloat & operator+=(const float & t);
  cfloat & operator-=(const float & t);
  cfloat & operator*=(const float &);
  cfloat & operator/=(const float &);
  cfloat & operator+=(const cfloat &);
  cfloat & operator+=(const cdouble &);
  cfloat & operator-=(const cfloat &);
  cfloat & operator-=(const cdouble &);
  cfloat & operator*=(const cfloat &);
  cfloat & operator*=(const cdouble &);
  cfloat & operator/=(const cfloat &);
  cfloat & operator/=(const cdouble &);
  int __len__() const;
%MethodCode
  sipRes = 2;
%End

  float __getitem__( int ) const;
%MethodCode
  if( a0 < 0 )
    a0 = 2 + a0;
  if( a0 == 0 )
    sipRes = sipCpp->real();
  else if( a0 == 1 )
    sipRes = sipCpp->imag();
  if( a0 >= 2 )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_ValueError, "Index out of bounds" );
  }
%End

  void __setitem__( int, float ) const;
%MethodCode
  if( a0 < 0 )
    a0 = 2 + a0;
  if( a0 == 0 )
    sipCpp->real() = a1;
  else if( a0 == 1 )
    sipCpp->imag() = a1;
  if( a0 >= 2 )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_ValueError, "Index out of bounds" );
  }
%End

  std::string __str__() const;
%MethodCode
  std::ostringstream s;
  s << "( " << sipCpp->real() << ", " << sipCpp->imag() << " )";
  sipRes = new std::string( s.str() );
%End
};


class cdouble
{
%TypeHeaderCode
#include <cartobase/type/types.h>
%End

%ConvertToTypeCode
  if (sipIsErr == NULL)
    return ( PySequence_Check( sipPy ) && PySequence_Size( sipPy ) <= 2 )
           || sipCanConvertToInstance(sipPy, sipClass_cdouble,
                SIP_NOT_NONE | SIP_NO_CONVERTORS );

  if( sipCanConvertToInstance( sipPy, sipClass_cdouble, SIP_NO_CONVERTORS ) )
  {
    int state = 0;

    cdouble * dat
      = (cdouble *)
        sipConvertToInstance( sipPy, sipClass_cdouble,
          sipTransferObj, SIP_NO_CONVERTORS, &state, sipIsErr );
    if( *sipIsErr && dat )
    {
      sipReleaseInstance( dat, sipClass_cdouble, state );
      dat = 0;
    }
    else if( dat )
    {
      *sipCppPtr = dat;
      return 0;
    }
  }

  if( PySequence_Check( sipPy ) )
  {
    *sipCppPtr = new cdouble;
    unsigned    i, n = PySequence_Size( sipPy );
    PyObject    *pyitem;
    for( i=0; i<n; ++i )
    {
      pyitem = PySequence_GetItem( sipPy, i );
      if( !pyitem || !PyFloat_Check( pyitem ) )
      {
        *sipIsErr = 1;
        delete *sipCppPtr;
        sipCppPtr = 0;
        std::ostringstream s;
        s << "wrong list item type, item " << i;
        PyErr_SetString( PyExc_TypeError, s.str().c_str() );
        return 0;
      }

      if( i == 0 )
        (*sipCppPtr)->real() = PyFloat_AsDouble( pyitem );
      else
        (*sipCppPtr)->imag() = PyFloat_AsDouble( pyitem );
      Py_DECREF( pyitem );
    }
    return sipGetState( sipTransferObj );
  }
  *sipCppPtr =
     (cdouble *) sipConvertToInstance( sipPy, sipClass_cdouble,
                           0, SIP_NO_CONVERTORS, 0, sipIsErr );
  return 0;
%End

public:
  cdouble( double r = double(), double i = double() );
  cdouble( const cdouble & );
  cdouble( SIP_PYOBJECT );
%MethodCode
  if( !PySequence_Check( a0 ) )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "wrong argument type" );
  }
  else
  {
    unsigned    i, n = PySequence_Size( a0 );
    if( n > 2 )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_ValueError, "too many items" );
    }
    else
    {
      sipCpp = new cdouble;
      PyObject    *pyitem;
      for( i=0; i<n; ++i )
      {
        pyitem = PySequence_GetItem(a0,i);
        if( !pyitem || !PyFloat_Check( pyitem ) )
        {
          sipIsErr = 1;
          delete sipCpp;
          sipCpp = 0;
          std::ostringstream s;
          s << "wrong list item type, item " << i;
          PyErr_SetString( PyExc_TypeError, s.str().c_str() );
          break;
        }

        if( i == 0 )
          sipCpp->real() = PyFloat_AsDouble( pyitem );
        else
          sipCpp->imag() = PyFloat_AsDouble( pyitem );
      }
    }
  }
%End

  double real() const;
  double imag() const;
  void real( double val );
%MethodCode
  sipCpp->real() = a0; // older compilers do not support complex::real( value )
%End
  void imag( double val );
%MethodCode
  sipCpp->imag() = a0;
%End
  cdouble & operator+=(const double & t);
  cdouble & operator-=(const double & t);
  cdouble & operator*=(const double &);
  cdouble & operator/=(const double &);
  cdouble & operator+=(const cfloat &);
  cdouble & operator+=(const cdouble &);
  cdouble & operator-=(const cfloat &);
  cdouble & operator-=(const cdouble &);
  cdouble & operator*=(const cfloat &);
  cdouble & operator*=(const cdouble &);
  cdouble & operator/=(const cfloat &);
  cdouble & operator/=(const cdouble &);
  int __len__() const;
%MethodCode
  sipRes = 2;
%End

  double __getitem__( int ) const;
%MethodCode
  if( a0 < 0 )
    a0 = 2 + a0;
  if( a0 == 0 )
    sipRes = sipCpp->real();
  else if( a0 == 1 )
    sipRes = sipCpp->imag();
  if( a0 >= 2 )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_ValueError, "Index out of bounds" );
  }
%End

  void __setitem__( int, double ) const;
%MethodCode
  if( a0 < 0 )
    a0 = 2 + a0;
  if( a0 == 0 )
    sipCpp->real() = a1;
  else if( a0 == 1 )
    sipCpp->imag() = a1;
  if( a0 >= 2 )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_ValueError, "Index out of bounds" );
  }
%End

  std::string __str__() const;
%MethodCode
  std::ostringstream s;
  s << "( " << sipCpp->real() << ", " << sipCpp->imag() << " )";
  sipRes = new std::string( s.str() );
%End
};

