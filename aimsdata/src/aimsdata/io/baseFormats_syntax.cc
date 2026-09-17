// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/baseFormats_syntax.h>
#include <cartobase/object/sreader.h>
#include <cartobase/object/swriter.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace aims
{

  bool StxFormat::read( const string & filename, SyntaxSet & obj,
                        const AllocatorContext & /*context*/,
                        Object /*options*/ )
  {
    SyntaxReader     r( filename );
    r.read( obj );
    return true;
  }


  bool StxFormat::write( const std::string & filename,
                         const SyntaxSet & obj, carto::Object )
  {
    SyntaxWriter     w( filename );
    w.write( obj );
    return true;
  }

}
