/* Copyright (c) 2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef CARTO_PYAIMS_OBJECT_RCPTRCONV_H
#define CARTO_PYAIMS_OBJECT_RCPTRCONV_H

#include <cartobase/smart/sharedptr.h>

namespace pyaims
{

  template <typename T> inline 
  T* fromT( PyObject * sipPy, sipWrapperType* tclass, PyObject *transferObj )
  {
    if( sipCanConvertToInstance( sipPy, tclass, SIP_NO_CONVERTORS ) )
    {
      int state = 0;
      int sipIsErr = 0;
      void *obj = sipConvertToInstance( sipPy, tclass, transferObj,
                                        SIP_NO_CONVERTORS, &state,
                                        &sipIsErr );
      //sipReleaseInstance( obj, tclass, state );
      return reinterpret_cast<T *>( obj );
    }
    return 0;
  }

  template <typename T, typename RcT> inline 
  T* fromRcptr( PyObject * sipPy, sipWrapperType* rcclass,
                PyObject *transferObj )
  {
    if( sipCanConvertToInstance( sipPy, rcclass, 0 ) )
    {
      int state = 0;
      int sipIsErr = 0;
      void *obj = sipConvertToInstance( sipPy, rcclass, transferObj,
                                        0, &state, &sipIsErr );
      RcT *robj = reinterpret_cast<RcT *>( obj );
      if( robj->get() && carto::rc_ptr_trick::refCount( *robj ) > 0 )
        ++carto::rc_ptr_trick::refCount( *robj );
      sipReleaseInstance( obj, rcclass, state );
      return robj->get();
    }
    return 0;
  }


  template <typename T> inline
  T* fromRcptrs( PyObject * sipPy, PyObject* tclass, PyObject* rcclass,
                 PyObject* wsclass, PyObject* wkclass, PyObject *transferObj )
  {
    return fromT<T>( sipPy, tclass, transferObj )
        || fromRcptr<T, carto::rc_ptr<T> >( sipPy, rcclass, transferObj )
        || fromRcptr<T, carto::weak_shared_ptr<T> >( sipPy, wsclass,
            transferObj )
        || fromRcptr<T, carto::weak_ptr<T> >( sipPy, wkclass, transferObj );
  }

}

#endif

