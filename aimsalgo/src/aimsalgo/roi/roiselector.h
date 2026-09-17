
#ifndef AIMS_ROI_ROISELECTOR_H
#define AIMS_ROI_ROISELECTOR_H

#include <graph/tree/tree.h>
#include <cartobase/object/syntax.h>


namespace aims {

class RoiSelector : public Tree
{
public:

  RoiSelector();
  virtual ~RoiSelector();

  carto::SyntaxSet& syntax();

private:

  carto::SyntaxSet  syntaxRoiSelector;
};

}

#endif
