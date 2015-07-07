
class AimsTimeSurface_%Template1typecode% : carto::RCObject
{
%TypeHeaderCode
#include <aims/mesh/surface.h>
#ifndef PYAIMS_AIMSTIMESURFACE_%Template1typecode%_VOID_DEFINED
#define PYAIMS_AIMSTIMESURFACE_%Template1typecode%_VOID_DEFINED
typedef AimsTimeSurface<%Template1%,Void> AimsTimeSurface_%Template1typecode%;
#endif

#if SIP_VERSION < 0x040700
#include "sipaimssipvector_AimsVector_U32_%Template1typecode%.h"
#include "sipaimssiprc_ptr_AimsTimeSurface_%Template1typecode%.h"
#endif
%End

%TypeCode
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy,
    sipClass_AimsTimeSurface_%Template1typecode%,
    sipTransferObj, sipIsErr, sipCppPtr );
%End

public:
  AimsTimeSurface_%Template1typecode%();
  virtual ~AimsTimeSurface_%Template1typecode%();

  void updateNormals();
  // const carto::GenericObject &  header() const;
  carto::GenericObject &  header() /Transfer/;

  void erase();
  unsigned size() const;

  // conversion from Object
  static  rc_ptr_AimsTimeSurface_%Template1typecode%
  fromObject( carto::GenericObject ) /Factory/;
%MethodCode
  try
    {
      carto::rc_ptr<AimsTimeSurface_%Template1typecode%> & mesh 
        = a0->value<carto::rc_ptr<AimsTimeSurface_%Template1typecode%> >();
      sipRes = new carto::rc_ptr<AimsTimeSurface_%Template1typecode%>( mesh );
    }
  catch( std::exception & e )
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_RuntimeError, e.what() );
      sipRes = 0;
    }
%End

// Thought by Alan (Idea regarding vertex() or normal())
// We could bind the meshs the same way we did in Volume/Texture's arraydata()
// since vertex is an std::vector<Point3df>, ie. std::vector<AimsVector<T,4>>
// where AimsVector has a T[4] field member, where T = basic type (eg. short)
// Thus, a correctly 2D strided PyArray should work, where first stride is
// sizeof(T), and second stride is sizeof(Point3df), and forcing second
// length to the correct size (ie. 4 here), thus the rest-of-the-class padding
// bytes will rightly be skipped/unaffected

  vector_POINT3DF* vertex( int = 0 );
%MethodCode
  sipRes = &(*sipCpp)[a0].vertex();
%End

  vector_POINT3DF* normal( int = 0 );
%MethodCode
  sipRes = &(*sipCpp)[a0].normal();
%End

  vector_AimsVector_U32_%Template1typecode%* polygon( int = 0 );
%MethodCode
  sipRes = &(*sipCpp)[a0].polygon();
%End


  SIP_SSIZE_T __len__() const;
%MethodCode
  sipRes = sipCpp->size();
%End

  SIP_PYLIST keys() const;
%MethodCode
  if( (sipRes = PyList_New(sipCpp->size())) == NULL )
    return NULL;

  unsigned n = sipCpp->size();
  AimsTimeSurface_%Template1typecode%::const_iterator it, et = sipCpp->end();
  unsigned i = 0;

  for( it=sipCpp->begin(); it!=et; ++it, ++i )
    if( PyList_SetItem( sipRes, i, PyInt_FromLong( it->first ) ) < 0 )
      {
        Py_DECREF(sipRes);
        sipIsErr = 1;
        sipRes = 0;
        PyErr_SetString( PyExc_TypeError, "item conversion failed" );
        break;
      }
%End

};



