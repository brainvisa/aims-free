// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/fileFormat_d.h>
#include <aims/io/baseFormats_genericobject.h>


using namespace aims;
using namespace std;

namespace aims
{

  template<> void 
  FileFormatDictionary<carto::GenericObject>::registerBaseFormats()
  {
    std::vector<std::string>      ext;
    ext.push_back( "minf" );
    MinfFormat<carto::GenericObject> *fm
        = new MinfFormat<carto::GenericObject>;
    registerFormat( "PYTHON", fm, ext );
    fm = new MinfFormat<carto::GenericObject>;
    registerFormat( "XML", fm, ext );
  }


  template<> void 
  FileFormatDictionary<carto::Object>::registerBaseFormats()
  {
    std::vector<std::string>      ext;
    ext.push_back( "minf" );
    MinfFormat<carto::Object> *fm = new MinfFormat<carto::Object>;
    registerFormat( "PYTHON", fm, ext );
    fm = new MinfFormat<carto::Object>;
    registerFormat( "XML", fm, ext );
  }


  template class FileFormatDictionary<carto::GenericObject>;
  template class FileFormatDictionary<carto::Object>;
  template class FileFormat<carto::GenericObject>;
  template class FileFormat<carto::Object>;


  static bool _minfdic()
  {
    FileFormatDictionary<carto::GenericObject>::init();
    FileFormatDictionary<carto::Object>::init();
    return true;
  }

  static bool minfdic __attribute__((unused)) = _minfdic();

}

