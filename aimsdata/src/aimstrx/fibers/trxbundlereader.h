
#ifndef AIMS_FIBERS_TRXBUNDLEREADER_H
#define AIMS_FIBERS_TRXBUNDLEREADER_H

#include <aims/fibers/bundles.h>

namespace aims
{

  /** .trx format reader

      This is a low-level reader class, normally only used through BundleReader. You should not need to use it directly.
  */
  class TrxBundleReader : public BundleFormatReader
  {
  public:

    TrxBundleReader();
    virtual ~TrxBundleReader();

    virtual void read();
    virtual carto::Object readHeader();
    static BundleFormatReader* create();

  private:
    template <typename T> void read_trx();
  };

}

#endif
