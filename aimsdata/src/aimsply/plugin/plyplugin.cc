#include <aims/plugin/plyplugin.h>
#include <aims/io/plyformat_d.h>
#include <aims/io/plyfinderformat.h>
#include <aims/io/finder.h>
#ifdef __APPLE__
#include <aims/io/macosxbugs.h>
#endif

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  bool initply()
  {
    new PlyPlugin;
    return true;
  }

  bool plyinit __attribute__((unused)) = initply();

}

PlyPlugin::PlyPlugin() : Plugin()
{
  vector<string>	ext;
  ext.push_back( "ply" );
  PlyFormat<2, Void>	*pf2 = new PlyFormat<2, Void>;
  FileFormatDictionary<AimsTimeSurface<2, Void> >::registerFormat( "PLY", pf2, 
                                                                   ext );

  PlyFormat<3, Void>	*pf3 = new PlyFormat<3, Void>;
#ifdef AIMS_APPLE_GCC33BUG
  macosxbugs::fileFormatDictionary_surfacetriangle_registerFormat( "PLY", pf3, 
                                                                   ext );
#else
  FileFormatDictionary<AimsTimeSurface<3, Void> >::registerFormat( "PLY", pf3, 
                                                                   ext );
#endif

  PlyFormat<4, Void>	*pf4 = new PlyFormat<4, Void>;
  FileFormatDictionary<AimsTimeSurface<4, Void> >::registerFormat( "PLY", pf4, 
                                                                   ext );

  Finder::registerFormat( "PLY", new FinderPlyFormat, ext );
}


PlyPlugin::~PlyPlugin()
{
}


string PlyPlugin::name() const
{
  return "PLY IO";
}


bool PlyPlugin::noop()
{
  return true;
}


