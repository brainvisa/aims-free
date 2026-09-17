// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/iooptions.h>
#include <cartobase/object/object_d.h>

using namespace aims;
using namespace carto;

Object IOOptions::ioOptions()
{
  static Object	opt;
  if( opt.isNull() )
    opt = Object::value( IOOptions() );
  return opt;
}


IOOptions::IOOptions()
  : PropertySet(), writeMinf( true )
{
  addBuiltinProperty( "writeMinf", writeMinf );
}


IOOptions & IOOptions::operator = ( const IOOptions & other )
{
  if( this == &other )
    return *this;
  PropertySet::operator = ( other );
  writeMinf = other.writeMinf;
  return *this;
}


namespace carto {

INSTANTIATE_GENERIC_OBJECT_TYPE( aims::IOOptions )

} // namespace carto
