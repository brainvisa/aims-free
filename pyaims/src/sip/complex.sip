
class cfloat
{
%TypeHeaderCode
#include <cartobase/type/types.h>
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
          sipCpp->real( (float) PyFloat_AsDouble( pyitem ) );
        else
          sipCpp->imag( (float) PyFloat_AsDouble( pyitem ) );
      }
    }
  }
%End

  float real() const;
  float imag() const;
  void real( float val );
  void imag( float val );
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
    sipCpp->real( a1 );
  else if( a0 == 1 )
    sipCpp->imag( a1 );
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
          sipCpp->real( PyFloat_AsDouble( pyitem ) );
        else
          sipCpp->imag( PyFloat_AsDouble( pyitem ) );
      }
    }
  }
%End

  double real() const;
  double imag() const;
  void real( double val );
  void imag( double val );
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
    sipCpp->real( a1 );
  else if( a0 == 1 )
    sipCpp->imag( a1 );
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

