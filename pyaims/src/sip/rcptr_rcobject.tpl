
/*

WARNING
This file is currently not used...

*/

%MappedType carto::rc_ptr< %Template1% >
{
%TypeHeaderCode
#include <cartobase/smart/rcptrtrick.h>
%Template1typeinclude%
%Template1sipinclude%
%End

%ConvertFromTypeCode
  if( !sipCpp )
    Py_RETURN_NONE;

  return %Template1pyFromC%( sipCpp->get() );
%End

%ConvertToTypeCode
  if( !sipIsErr )
  {
    // check type
    //return %Template1testPyType%( sipPy );
    return sipCanConvertToInstance( sipPy, sipClass_%Template1sipClass%,
      SIP_NOT_NONE);
  }

  if( sipPy == Py_None )
  {
    *sipCppPtr = 0;
    return 0;
  }

  %Template1% *t = %Template1CFromPy%( sipPy );
  if( t )
  {
    *sipCppPtr = new carto::rc_ptr<%Template1sipClass% >( t );
    // if there is no rc_ptr on this object yet, make it inactive
    /* if( carto::rc_ptr_trick::refCount( **sipCppPtr ) == 1 )
      --carto::rc_ptr_trick::refCount( **sipCppPtr ); */
    return sipGetState(sipTransferObj);
  }
  *sipIsErr = 1;
  return 0;
%End
};


