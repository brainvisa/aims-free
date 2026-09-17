// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/baseFormats_hierarchy.h>
#include <aims/roi/hie.h>
#include <aims/io/hieR.h>
#include <graph/tree/twriter.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace aims
{

  bool HieFormat::read( const string & filename, Hierarchy & obj, 
			const AllocatorContext & /*context*/, 
                        Object /*options*/ )
  {
    HierarchyReader	r( filename );
    r.read( obj );
    return true;
  }


  bool HieFormat::write( const std::string & filename,
                         const Hierarchy & obj, carto::Object )
  {
    TreeWriter     w( filename, HierarchyReader::syntaxSet() );
    w << obj;
    return true;
  }

}
