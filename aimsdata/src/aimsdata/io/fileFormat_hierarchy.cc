// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  IO classes
 */

/* this source is only here to force instanciation of some of the most useful 
   Reader templates */

#include <aims/io/fileFormat_d.h>
#include <aims/roi/hie.h>
#include <aims/io/baseFormats_hierarchy.h>

using namespace std;
using namespace aims;

namespace aims
{

  // The following line is required for compilation on gcc 3.3
  template<> void FileFormatDictionary<Hierarchy>::registerBaseFormats();

  template<> void
  FileFormatDictionary<Hierarchy>::registerBaseFormats()
  {
    vector<string>	ext;
    ext.push_back( "hie" );
    HieFormat	*fm = new HieFormat;
    registerFormat( "HIE", fm, ext );
  }

template class FileFormatDictionary<Hierarchy>;
template class FileFormat<Hierarchy>;

} // namespace aims


static bool _hiedic()
{
  FileFormatDictionary<Hierarchy>::init();
  return true;
}

static bool hiedic __attribute__((unused)) = _hiedic();

