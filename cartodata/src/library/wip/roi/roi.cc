#include <cartodata/wip/roi/roi.h>

namespace carto {

  Roi::~Roi() {}


  DiscreteRoi::~DiscreteRoi() {}

  bool DiscreteRoi::holds(const Site &site)
  {
    rc_ptr<SiteIterator> siteIt = getSiteIterator();

    while ( siteIt->isValid() ){
      if (  ( *(siteIt->getSite()) ) == site ) return true;
      else siteIt->next();    
    }
    return false;
  }


  ContinuousRoi::~ContinuousRoi() {}


  RoiIterator::~RoiIterator() {}


  ContinuousRoiIterator::~ContinuousRoiIterator() {}


  DiscreteRoiIterator::~DiscreteRoiIterator() {}

} // namespace carto
