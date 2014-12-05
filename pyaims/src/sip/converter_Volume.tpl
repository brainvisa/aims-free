
class Converter_Volume_%Template1typecode%_Volume_%Template2typecode%
{
%Docstring
Converter classes can be used using the convert() method, or as a callable object:

result = converter(input_data)

Parameters
----------
input_data: Volume_%Template1typecode%
    data to be converted

Returns
-------
output data: Volume_%Template2typecode%
    converted data
%End

%TypeHeaderCode
#include <aims/utility/converter_volume.h>
#include <pyaims/data/data.h>
#include <aims/utility/converter_rgb.h>
typedef carto::Converter<AimsData<%Template1%>, AimsData<%Template2%> > 
  Converter_Volume_%Template1typecode%_Volume_%Template2typecode%;
%End

public:
  Converter_Volume_%Template1typecode%_Volume_%Template2typecode%( bool = false );
  Converter_Volume_%Template1typecode%_Volume_%Template2typecode%( bool, SIP_PYOBJECT, SIP_PYOBJECT, SIP_PYOBJECT, SIP_PYOBJECT, bool = false );
%MethodCode
  // Allocate rescaler info object
  carto::RescalerInfo info;

  bool  a1isnone = (a1 == Py_None);
  bool  a2isnone = (a2 == Py_None);
  bool  a3isnone = (a3 == Py_None);
  bool  a4isnone = (a4 == Py_None);

  if ((!a1isnone and !PyFloat_Check(a1)) or
     (!a2isnone and !PyFloat_Check(a2)) or
     (!a3isnone and !PyFloat_Check(a3)) or
     (!a4isnone and !PyFloat_Check(a4))  )
  {
    sipIsErr = 1;
    PyErr_SetString(PyExc_TypeError, "wrong argument type");
  }
  else {

    if (!a1isnone) {
      info.vmin = PyFloat_AsDouble(a1);
    }

    if (!a2isnone) {
      info.vmax = PyFloat_AsDouble(a2);
    }

    if (!a3isnone) {
      info.omin =  PyFloat_AsDouble(a3);
    }

    if (!a4isnone) {
      info.omax =  PyFloat_AsDouble(a4);
    }

    info.usevtypelimits = a5;

  }

  if( !sipIsErr )  {
    sipCpp = new Converter_Volume_%Template1typecode%_Volume_%Template2typecode%( a0, info );
  }
  else {
    sipCpp = NULL;
  }

%End

  ~Converter_Volume_%Template1typecode%_Volume_%Template2typecode%();

%If ( CARTODATA )

  Volume_%Template2typecode% * operator () ( const Volume_%Template1typecode% & )  const /Factory/;
%MethodCode
  carto::rc_ptr<Volume_%Template1typecode%>	r( const_cast<Volume_%Template1typecode% *>( a0 ) );
  carto::rc_ptr<Volume_%Template2typecode%>	resvol;
  {
    AimsData_%Template1typecode% dat( r );
    AimsData_%Template2typecode% *d = (*sipCpp)( r );
    resvol = d->volume(); // refcount should be 2 now.
    delete d; // refcount should be 1 now
  }
  r.release();
  sipRes = resvol.get();
  resvol.release(); // refcount back to 0
%End

  void convert( const Volume_%Template1typecode% &, Volume_%Template2typecode% & ) const;
%Docstring
converter.convert(input_data, output_data)

In-place converson inside existing allocated data

Parameters
----------
input_data: Volume_%Template1typecode%
    data to be converted
output_data: Volume_%Template2typecode%
    conversion output will be done into this data

Returns
-------
None
%End
%MethodCode
  carto::rc_ptr<Volume_%Template1typecode%> r1( const_cast<Volume_%Template1typecode% *>( a0 ) );
  carto::rc_ptr<Volume_%Template2typecode%> r2( a1 );
  AimsData_%Template1typecode%  dat1( r1 );
  AimsData_%Template2typecode%  dat2( r2 );
  sipCpp->convert( dat1, dat2 );
  r2.release();
  r1.release();
%End

%End

  AimsData_%Template2typecode%* operator ()
    ( const AimsData_%Template1typecode% & ) const /Factory/;
  void convert( const AimsData_%Template1typecode% &,
    AimsData_%Template2typecode% & ) const;
};


class ShallowConverter_Volume_%Template1typecode%_Volume_%Template2typecode%
{
%TypeHeaderCode
#include <aims/utility/converter_volume.h>
#include <pyaims/data/data.h>
#include <aims/utility/converter_rgb.h>
typedef carto::ShallowConverter<AimsData<%Template1%>, AimsData<%Template2%> > 
  ShallowConverter_Volume_%Template1typecode%_Volume_%Template2typecode%;
%End

public:
  ShallowConverter_Volume_%Template1typecode%_Volume_%Template2typecode%( bool = false );
  ~ShallowConverter_Volume_%Template1typecode%_Volume_%Template2typecode%();

%If ( CARTODATA )

  rc_ptr_Volume_%Template2typecode% operator () ( const Volume_%Template1typecode% & ) const;
%MethodCode
  carto::rc_ptr<Volume_%Template1typecode%>	r( const_cast<Volume_%Template1typecode% *>( a0 ) );
  carto::rc_ptr<Volume_%Template2typecode%>	resvol;
  {
    AimsData_%Template1typecode% dat( r );
    AimsData_%Template2typecode% *d = (*sipCpp)( dat );
    /* we build a rc_ptr here to handle cases where the result is
       actually the input data */
    sipRes = new carto::rc_ptr<Volume_%Template2typecode%>( d->volume() );
    // refcount should be 2 now.
    delete d; // refcount should be 1 now
  }
  r.release();
%End

  void convert( const Volume_%Template1typecode% &, Volume_%Template2typecode% & ) const;
%MethodCode
  carto::rc_ptr<Volume_%Template1typecode%> r1( const_cast<Volume_%Template1typecode% *>( a0 ) );
  carto::rc_ptr<Volume_%Template2typecode%> r2( a1 );
  AimsData_%Template1typecode%  dat1( r1 );
  AimsData_%Template2typecode%  dat2( r2 );
  sipCpp->convert( dat1, dat2 );
  r2.release();
  r1.release();
%End

%End

  AimsData_%Template2typecode%* operator ()
    ( const AimsData_%Template1typecode% & ) const /Factory/;
  void convert( const AimsData_%Template1typecode% &,
    AimsData_%Template2typecode% & ) const;
};

