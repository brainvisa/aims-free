namespace carto {

class Headered : carto::RCObject
{
%TypeHeaderCode
#include <cartobase/object/headered.h>
%End

public:
  Headered();
  Headered( const carto::Headered & );
  virtual ~Headered();

  carto::Object *header() /Factory/;
%MethodCode
    carto::PropertySet & ps = sipCpp->header();
    sipRes = new carto::Object( carto::Object::reference( ps ) );
%End

};

}; // namespace carto

