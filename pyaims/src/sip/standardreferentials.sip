namespace aims
{

  class StandardReferentials
  {
%TypeHeaderCode
#include <aims/resampling/standardreferentials.h>
%End

  public:
    static std::string mniTemplateReferential();
    static std::string acPcReferential();
    static std::string mniTemplateReferentialID();
    static std::string acPcReferentialID();
  };

};

