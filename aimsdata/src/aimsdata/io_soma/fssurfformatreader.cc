//-------------------------------------------------------------------
#include <aims/io_soma/fssurfformatreader_d.h>
#include <soma-io/io/formatdictionary.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "FSSURFFORMATREADER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------

using namespace aims;
using namespace soma;
using namespace carto;
using namespace std;

//============================================================================
//   I N I T
//============================================================================

namespace carto {
template class FsSurfFormatReader<3>;
// template class FsSurfFormatReader<4>;
}

namespace
{

  bool initfssurfformat()
  {
    {
      FsSurfFormatReader<3>  *r = new FsSurfFormatReader<3>;
      vector<string>  exts;
      exts.push_back( "white" );
      exts.push_back( "pial" );
      FormatDictionary<AimsTimeSurface<3, Void> >::registerFormat( "FSSURF", r,
                                                                   exts );
    }

//     {
//       FsSurfFormatReader<4>  *r = new FsSurfFormatReader<4>;
//       vector<string>  exts;
//       exts.push_back( "white" );
//       exts.push_back( "pial" );
//       FormatDictionary<AimsTimeSurface<4, Void> >::registerFormat( "FSSURF", r,
//                                                                    exts );
//     }

    return true;
  }

  bool dummy __attribute__((unused)) = initfssurfformat();

}

#undef localMsg

