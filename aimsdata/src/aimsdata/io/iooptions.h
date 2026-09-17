#include <cartobase/object/object.h>
#include <cartobase/object/property.h>

namespace aims
{

  class IOOptions : public carto::PropertySet
  {
  public:
    static carto::Object ioOptions();
    IOOptions();
    IOOptions & operator = ( const IOOptions & );

    bool writeMinf;
  };

}

namespace carto {

DECLARE_GENERIC_OBJECT_TYPE( aims::IOOptions )

} // namespace carto
