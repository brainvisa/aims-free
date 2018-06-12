namespace aims
{

  class StandardReferentials
  {
%TypeHeaderCode
#include <aims/resampling/standardreferentials.h>
%End

  public:
    static std::string mniTemplateReferential();
    static std::string mniTemplateReferentialID();
    static std::string acPcReferential();
    static std::string acPcReferentialID();
    static std::string talairachReferential();
    static std::string commonScannerBasedReferential();
    static std::string commonScannerBasedReferentialID();
  };

};

