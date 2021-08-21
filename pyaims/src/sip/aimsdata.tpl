%#define PyAims_AimsData_%Template1typecode%_defined%

%MappedType AimsData<%Template1% >
{
%TypeHeaderCode
#include <aims/data/data.h>
%Template1typeinclude%
%Template1sipinclude%

typedef AimsData<%Template1% > AimsData_%Template1typecode%;

#ifndef PYAIMS_VOLUME_%Template1typecode%_CHECK_DEFINED
#define PYAIMS_VOLUME_%Template1typecode%_CHECK_DEFINED
inline int pyaimsVolume_%Template1typecode%_Check( PyObject* o )
{ return sipCanConvertToType( o, sipFindType( "Volume_%Template1typecode%" ), SIP_NOT_NONE | SIP_NO_CONVERTORS ); }
#endif

%End

%ConvertFromTypeCode

  if (!sipCpp)
    return Py_None;

  // Convert to a Python wrapped rc_ptr<Volume> of %Template1%

  carto::rc_ptr<carto::Volume<%Template1% > > *
    vol = new carto::rc_ptr<carto::Volume<%Template1% > >( sipCpp->volume() );

  const sipTypeDef *td = sipFindType( "rc_ptr_Volume_%Template1typecode%" );
  PyObject *l = sipConvertFromType( vol, td, Py_None );

  return l;
%End

%ConvertToTypeCode
  // Convert a Volume or rc_ptr<Volume> of %Template1%
  // to a AimsData<%Template1% > on the heap.

  if (sipIsErr == NULL)
  {
    return pyaimsVolume_%Template1typecode%_Check( sipPy )
      || sipCanConvertToType(
        sipPy, sipFindType( "rc_ptr_Volume_%Template1typecode%" ),
        SIP_NOT_NONE | SIP_NO_CONVERTORS );
  }

  if (sipPy == Py_None)
  {
    *sipCppPtr = NULL;

    return 0;
  }

  PyErr_Clear();

  int iserr = 0;
  const sipTypeDef *td = 0;
  carto::Volume<%Template1% >* vol = 0;

  if( pyaimsVolume_%Template1typecode%_Check( sipPy ) )
  {
    td = sipFindType( "Volume_%Template1typecode%" );
    vol = reinterpret_cast< carto::Volume<%Template1% >* >(
        sipConvertToType( sipPy, td, NULL, SIP_NOT_NONE, 0, &iserr ) );
  }
  else
  {
    td = sipFindType( "rc_ptr_Volume_%Template1typecode%" );
    carto::rc_ptr<carto::Volume<%Template1% > >* rvol
      = reinterpret_cast< carto::rc_ptr<carto::Volume<%Template1% > >* >(
        sipConvertToType( sipPy, td, NULL, SIP_NOT_NONE, 0, &iserr ) );
    vol = rvol->get();
  }

  if( iserr )
  {
    *sipIsErr = 1;
    std::ostringstream msg;
    msg << "could not obtain the Volume to convert to AimsData";
    PyErr_SetString( PyExc_TypeError, msg.str().c_str() );
    return 0;
  }

  AimsData<%Template1% > *qa = new AimsData<%Template1% >( vol );

  *sipCppPtr = qa;

  return 1;
%End

};


%MappedType AimsData_%Template1typecode%
{
%TypeHeaderCode
#include <aims/data/data.h>
%Template1typeinclude%
%Template1sipinclude%

typedef AimsData<%Template1% > AimsData_%Template1typecode%;

#ifndef PYAIMS_VOLUME_%Template1typecode%_CHECK_DEFINED
#define PYAIMS_VOLUME_%Template1typecode%_CHECK_DEFINED
inline int pyaimsVolume_%Template1typecode%_Check( PyObject* o )
{ return sipCanConvertToType( o, sipFindType( "Volume_%Template1typecode%" ), SIP_NOT_NONE | SIP_NO_CONVERTORS ); }
#endif

%End

%ConvertFromTypeCode

  if (!sipCpp)
    return Py_None;

  // Convert to a Python wrapped rc_ptr<Volume> of %Template1%

  carto::rc_ptr<carto::Volume<%Template1% > > *
    vol = new carto::rc_ptr<carto::Volume<%Template1% > >( sipCpp->volume() );

  const sipTypeDef *td = sipFindType( "rc_ptr_Volume_%Template1typecode%" );
  PyObject *l = sipConvertFromType( vol, td, Py_None );

  return l;
%End

%ConvertToTypeCode
  // Convert a Volume or rc_ptr<Volume> of %Template1%
  // to a AimsData<%Template1% > on the heap.

  if (sipIsErr == NULL)
  {
    return pyaimsVolume_%Template1typecode%_Check( sipPy )
      || sipCanConvertToType(
        sipPy, sipFindType( "rc_ptr_Volume_%Template1typecode%" ),
        SIP_NOT_NONE | SIP_NO_CONVERTORS );
  }

  if (sipPy == Py_None)
  {
    *sipCppPtr = NULL;

    return 0;
  }

  PyErr_Clear();

  int iserr = 0;
  const sipTypeDef *td = 0;
  carto::Volume<%Template1% >* vol = 0;

  if( pyaimsVolume_%Template1typecode%_Check( sipPy ) )
  {
    td = sipFindType( "Volume_%Template1typecode%" );
    vol = reinterpret_cast< carto::Volume<%Template1% >* >(
        sipConvertToType( sipPy, td, NULL, SIP_NOT_NONE, 0, &iserr ) );
  }
  else
  {
    td = sipFindType( "rc_ptr_Volume_%Template1typecode%" );
    carto::rc_ptr<carto::Volume<%Template1% > >* rvol
      = reinterpret_cast< carto::rc_ptr<carto::Volume<%Template1% > >* >(
        sipConvertToType( sipPy, td, NULL, SIP_NOT_NONE, 0, &iserr ) );
    vol = rvol->get();
  }

  if( iserr )
  {
    *sipIsErr = 1;
    std::ostringstream msg;
    msg << "could not obtain the Volume to convert to AimsData";
    PyErr_SetString( PyExc_TypeError, msg.str().c_str() );
    return 0;
  }

  AimsData<%Template1% > *qa = new AimsData<%Template1% >( vol );

  *sipCppPtr = qa;

  return 1;
%End

};


%#undef PyAims_AimsData_%Template1typecode%_defined%

