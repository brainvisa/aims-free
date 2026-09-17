//-------------------------------------------------------------------
#include <aims/io_soma/fsannotformatreader_d.h>
#include <soma-io/io/formatdictionary.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "FSANNOTFORMATREADER" )
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
template class FsAnnotFormatReader<float>;
// template class FsAnnotFormatReader<int16_t>;
}

namespace
{

  bool initfsannotformat()
  {
    {
      FsAnnotFormatReader<int16_t>  *r = new FsAnnotFormatReader<int16_t>;
      vector<string>  exts;
      exts.push_back( "annot" );
      FormatDictionary<TimeTexture<int16_t> >::registerFormat( "FSANNOT", r,
                                                               exts );
    }

    return true;
  }

  bool dummy __attribute__((unused)) = initfsannotformat();

}

#undef localMsg

