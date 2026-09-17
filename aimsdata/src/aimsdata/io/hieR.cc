
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/roi/hie.h>
#include <aims/io/hieR.h>
#include <aims/def/path.h>
#include <cartobase/object/sreader.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;

SyntaxSet & HierarchyReader::syntaxSet()
{
  static SyntaxSet  synt;
  if( synt.empty() )
  {
    const Path  & pth = Path::singleton();
    SyntaxReader  sr( pth.syntax() + FileUtil::separator() + "hierarchy.stx" );
    sr.read( synt );
  }
  return synt;
}


HierarchyReader::HierarchyReader( const string& name, const SyntaxSet& stx )
  : TreeReader( name, stx.empty() ? syntaxSet() : stx ), _name( name )
{
}
 

HierarchyReader::~HierarchyReader()
{
}


void  HierarchyReader::read(Hierarchy& thing)
{
  TreeReader::read( thing );
}
