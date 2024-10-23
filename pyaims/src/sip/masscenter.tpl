
class MassCenters_%Template1typecode%
{
%TypeHeaderCode
#include <aims/utility/masscenter.h>
#ifndef PYAIMS_MASSCENTERS_%Template1typecode%_DEFINED
#define PYAIMS_MASSCENTERS_%Template1typecode%_DEFINED
typedef MassCenters<%Template1% > MassCenters_%Template1typecode%;
#endif
%End

  public:

    MassCenters_%Template1typecode%(
        const rc_ptr_Volume_%Template1typecode% & data,
        bool bin = false);
    MassCenters_%Template1typecode%(
        const rc_ptr_Volume_%Template1typecode% & data,
        const aims::RoiIterator & roiIterator,
        bool bin = false);
%MethodCode
    carto::rc_ptr<aims::RoiIterator> roi( const_cast<aims::RoiIterator *>( a1 ) );
    sipCpp = new MassCenters( carto::VolumeRef<%Template1% >( *a0 ), roi, a2 );
    roi.release();
%End

    void doit(bool force = false);

    SIP_PYOBJECT infos();
%MethodCode
    MassCenters<%Template1% >::RoiMassCenterInfo info = sipCpp->infos();
    PyObject *dict = PyDict_New();

    MassCenters<%Template1% >::RoiMassCenterInfo::const_iterator
      im, em = info.end();
    for( im=info.begin(); im!=em; ++im )
    {
      PyObject *d = PyDict_New();
      MassCenters<%Template1% >::TimedMassCenterInfo::const_iterator
        it, et = im->second.end();
      for( it=im->second.begin(); it!=et; ++it )
      {
        PyObject *l1 = PyList_New( 2 );
        PyObject *l2 = PyList_New( 3 );
        PyList_SetItem( l2, 0, PyFloat_FromDouble( it->second.first[0] ) );
        PyList_SetItem( l2, 1, PyFloat_FromDouble( it->second.first[1] ) );
        PyList_SetItem( l2, 2, PyFloat_FromDouble( it->second.first[2] ) );
        PyList_SetItem( l1, 0, l2 );
        PyList_SetItem( l1, 1, PyFloat_FromDouble( it->second.second ) );
        PyDict_SetItem( d, PyLong_FromLong( it->first ), l1 );
      }
      PyDict_SetItemString( dict, im->first.c_str(), d );
    }

    sipRes = dict;
%End

    SIP_PYOBJECT maskedmasscenter( aims::MaskIterator & maskIterator );
%MethodCode
    MassCenters<%Template1% >::TimedMassCenterInfo info
      = sipCpp->maskedmasscenter( *a0 );
    PyObject *dict = PyDict_New();
    MassCenters<%Template1% >::TimedMassCenterInfo::const_iterator
        it, et = info.end();
    for( it=info.begin(); it!=et; ++it )
    {
      PyObject *l1 = PyList_New( 2 );
      PyObject *l2 = PyList_New( 3 );
      PyList_SetItem( l2, 0, PyFloat_FromDouble( it->second.first[0] ) );
      PyList_SetItem( l2, 1, PyFloat_FromDouble( it->second.first[1] ) );
      PyList_SetItem( l2, 2, PyFloat_FromDouble( it->second.first[2] ) );
      PyList_SetItem( l1, 0, l2 );
      PyList_SetItem( l1, 1, PyFloat_FromDouble( it->second.second ) );
      PyDict_SetItem( dict, PyLong_FromLong( it->first ), l1 );
    }

    sipRes = dict;
%End
};

