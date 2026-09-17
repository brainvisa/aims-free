#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  ColorMap class
 */
#include <aims/color/colormap.h>
using aims::ColorMap;

using namespace std;


ostream& operator << ( ostream& out, const ColorMap& thing )
{
  out << "{"
      << "name=" << thing.name() << ", "
      << "nItem=" << thing.nItem() << ", "
      << "item=(";
  for ( size_t n = 0; n < thing.nItem(); n++ )
    out << thing.item( n ) << ", ";
  out << "NULL)}";

  return out;
}
