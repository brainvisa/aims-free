// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  IO classes
 */

#include <aims/io/fileFormat_d.h>
#include <aims/io/baseFormats_graph.h>
#include <aims/io/graphBundlesFormat.h>
#include <aims/io/baseFormats_syntax.h>
#include <aims/io/argR.h>
#include <cartodata/volume/volume.h>
#include <graph/graph/graph.h>


using namespace aims;
using namespace std;
using namespace carto;

namespace aims
{

  // The following line is required for compilation on gcc 3.3
  template<> void FileFormatDictionary<Graph>::registerBaseFormats();
  template<> void FileFormatDictionary<SyntaxSet>::registerBaseFormats();
  
  template<> const map<string, list<string> >
  & FileFormatDictionary<Graph>::extensions()
  {
    init();
    map<string, list<string> > & ext = _extensions();
    const map<string, list<string> >
      & volext = FileFormatDictionary<Volume<short> >::extensions();
    map<string, list<string> >::const_iterator	ive, eve = volext.end();
    list<string>::const_iterator                ie, ee;

    mutex().lock();
    for( ive=volext.begin(); ive!=eve; ++ive )
    {
      list<string> & gext = ext[ ive->first ];
      for( ie=gext.begin(), ee=gext.end(); ie!=ee; ++ie )
        if( *ie == "GraphVolume" )
          break;
      if( ie == ee )
        gext.push_back( "GraphVolume" );
    }
    mutex().unlock();

    return ext;
  }


  template<> void FileFormatDictionary<Graph>::registerBaseFormats()
  {
    vector<string>	ext;
    ext.push_back( "arg" );
    ArgFormat		*af = new ArgFormat;
    registerFormat( "ARG", af, ext );
    _formats()[ "GraphVolume" ] = new GraphVolumeFormat;

    ext.clear();
    ext.push_back( "bundles" );
    ext.push_back( "trk" );
    ext.push_back( "tck" );
    registerFormat( "BUNDLES", new GraphBundlesFormat, ext );
  }


  template<> void FileFormatDictionary<SyntaxSet>::registerBaseFormats()
  {
    vector<string>      ext;
    ext.push_back( "stx" );
    StxFormat           *af = new StxFormat;
    registerFormat( "STX", af, ext );
  }


template class FileFormatDictionary<Graph>;
template class FileFormat<Graph>;
template class FileFormatDictionary<SyntaxSet>;
template class FileFormat<SyntaxSet>;

} // namespace aims


static bool _graphdic()
{
  FileFormatDictionary<Graph>::init();
  FileFormatDictionary<SyntaxSet>::init();
  ArgReader::initLowLevelReaders();
  return true;
}

static bool graphdic __attribute__((unused)) = _graphdic();

