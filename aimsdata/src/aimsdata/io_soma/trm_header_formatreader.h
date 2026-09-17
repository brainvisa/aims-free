#ifndef AIMS_IO_SOMA_TRM_HEADER_FORMATREADER_H
#define AIMS_IO_SOMA_TRM_HEADER_FORMATREADER_H

#include <soma-io/reader/formatreader.h>

namespace aims
{
  class AffineTransformation3d;
}

namespace soma
{

  /** Read transformation in a file (nifti or other) file header

      Reader options:

      - index (int):
          index of the transformation in the list (several may be available)
      - target (string):
          name of the target referential (in the header "referentials" field),
          used to retreive the index
      - inv (bool 0/1):
          if true, invert the transformation
   */
  class TrmHeaderFormatReader
    : public FormatReader<aims::AffineTransformation3d>
  {
  public:
    //========================================================================
    //   N E W   M E T H O D S
    //========================================================================
    virtual aims::AffineTransformation3d*
    createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                   const AllocatorContext & context,
                   carto::Object options );
    virtual void read( aims::AffineTransformation3d & obj,
                       carto::rc_ptr<DataSourceInfo> dsi,
                       const AllocatorContext & context,
                       carto::Object options );
    virtual FormatReader<aims::AffineTransformation3d>* clone() const;
    virtual std::string formatID() const { return "TRMHEADER"; }
  };

}

#endif
