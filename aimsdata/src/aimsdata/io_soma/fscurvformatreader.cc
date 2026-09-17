//-------------------------------------------------------------------
#include <aims/io_soma/fscurvformatreader_d.h>
#include <soma-io/io/formatdictionary.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "FSCURVFORMATREADER" )
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
template class FsCurvFormatReader<float>;
// template class FsCurvFormatReader<int16_t>;
}

namespace
{

  bool initfscurvformat()
  {
    {
      FsCurvFormatReader<float>  *r = new FsCurvFormatReader<float>;
      vector<string>  exts;
      exts.push_back( "curv" );
      FormatDictionary<TimeTexture<float> >::registerFormat( "FSCURV", r,
                                                             exts );
    }

    return true;
  }

  bool dummy __attribute__((unused)) = initfscurvformat();

}

#undef localMsg

