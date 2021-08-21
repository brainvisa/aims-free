
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
#include <aims/utility/converter_hsv.h>
typedef carto::Converter<carto::VolumeRef<%Template1%>,
                         carto::VolumeRef<%Template2%> >
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
  carto::VolumeRef<%Template1% > r(
    const_cast<carto::Volume<%Template1% > *>( a0 ) );
  carto::VolumeRef<%Template2% > *res = sipCpp->operator ()( r );
  r.release();
  sipRes = res->get();
  res->release();
  delete res;
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
  carto::VolumeRef<%Template1% > r(
    const_cast<Volume_%Template1typecode% *>( a0 ) );
  carto::VolumeRef<%Template2% > res( a1 );
  sipCpp->convert( r, res );
  r.release();
  res.release();
%End

%End

};


class ShallowConverter_Volume_%Template1typecode%_Volume_%Template2typecode%
{
%TypeHeaderCode
#include <aims/utility/converter_volume.h>
#include <pyaims/data/data.h>
#include <aims/utility/converter_rgb.h>
#include <aims/utility/converter_hsv.h>
typedef carto::ShallowConverter<carto::VolumeRef<%Template1%>, carto::VolumeRef<%Template2%> >
  ShallowConverter_Volume_%Template1typecode%_Volume_%Template2typecode%;
%End

public:
  ShallowConverter_Volume_%Template1typecode%_Volume_%Template2typecode%( bool = false );
  ~ShallowConverter_Volume_%Template1typecode%_Volume_%Template2typecode%();

%If ( CARTODATA )

  rc_ptr_Volume_%Template2typecode% operator () ( const Volume_%Template1typecode% & ) const;
%MethodCode
  carto::VolumeRef<%Template1% >	r(
    const_cast<carto::Volume<%Template1% > *>( a0 ) );
  carto::VolumeRef<%Template2% >	resvol;
  {
    carto::VolumeRef<%Template2% > *d = (*sipCpp)( r );
    /* we build a rc_ptr here to handle cases where the result is
       actually the input data */
    sipRes = new carto::rc_ptr<Volume_%Template2typecode%>( *d );
    delete d;
    // refcount should be 1 now.
  }
  r.release();
%End

  void convert( const Volume_%Template1typecode% &, Volume_%Template2typecode% & ) const;
%MethodCode
  carto::VolumeRef<%Template1% > r(
    const_cast<carto::Volume<%Template1% > *>( a0 ) );
  carto::VolumeRef<%Template2% > res( a1 );
  sipCpp->convert( r, res );
  r.release();
  res.release();
%End

%End

};

