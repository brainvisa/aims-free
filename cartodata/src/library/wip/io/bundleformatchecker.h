#ifndef CARTODATA_IO_BUNDLEFORMATCHECKER_H
#define CARTODATA_IO_BUNDLEFORMATCHECKER_H

#ifdef USE_SOMA_IO
  ERROR_SHOULDNT_BE_INCLUDED;
#endif
#include <soma-io/checker/formatchecker.h>

namespace carto
{

  class PythonBundleFormatChecker : public FormatChecker
  {
  public:
    virtual ~PythonBundleFormatChecker();
    virtual Object check( DataSource & ds, DataSourceInfo & f ) const;
  };


  class XMLBundleFormatChecker : public FormatChecker
  {
  public:
    virtual ~XMLBundleFormatChecker();
    virtual Object check( DataSource & ds, DataSourceInfo & f ) const;
  };

}

#endif

