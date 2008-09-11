
namespace carto
{

  class SharedObject : /* carto::WeakObject,*/ carto::RCObject
  {
%TypeHeaderCode
#include <cartobase/smart/sharedptr.h>
%End

%TypeCode
#include <cartobase/smart/rcptrtrick.h>
%End

  public:
    SharedObject();
    SharedObject( const carto::SharedObject & x );
    virtual ~SharedObject();

    int __weakcount__();
%MethodCode
  sipRes = carto::rc_ptr_trick::weakCount( *sipCpp );
%End

};

};

