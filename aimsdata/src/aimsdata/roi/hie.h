#ifndef AIMS_ROI_HIE_H
#define AIMS_ROI_HIE_H

#include <aims/config/aimsdata_config.h>
#include <graph/tree/tree.h>
#include <cartobase/object/syntax.h>


namespace aims
{

  class AIMSDATA_API Hierarchy : public Tree
  {
  public:

    Hierarchy();
    virtual ~Hierarchy();

    carto::SyntaxSet& syntax();

  private:

    carto::SyntaxSet  sHie;
  };

}

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template<> inline std::string DataTypeCode<aims::Hierarchy>::objectType()
  {
    return "Hierarchy";
  }

  template<> inline std::string DataTypeCode<aims::Hierarchy>::dataType()
  {
    return "VOID";
  }

  template<> inline std::string DataTypeCode<aims::Hierarchy>::name()
  {
    return "Hierarchy";
  }

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

#endif
