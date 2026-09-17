#ifndef AIMS_IO_SOMA_TRM_COMPOSE_FORMATREADER_H
#define AIMS_IO_SOMA_TRM_COMPOSE_FORMATREADER_H

#include <soma-io/reader/formatreader.h>

namespace aims
{
  class AffineTransformation3d;
}

namespace soma
{

  class Transformation3d;

  /** Read a transformation build from compising several transformation files

      An artificial ".trmc" extension is used to recognize this format.

      Files are separated with a "*" character. If some of them have options
      ("?option=value"), then the final composition should also have an option
      separator ("toto.trm?inv=1*tutu.nii.trmhdr.trmc?") in order to avoid the
      URL parser to parse everything after the last "?" character as options.
   */
  class TrmComposeFormatReader
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
    virtual std::string formatID() const { return "TRMCOMPOSE"; }
  };


  class TrmChainFormatReader
    : public FormatReader<Transformation3d>
  {
  public:
    virtual Transformation3d*
    createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                   const AllocatorContext & context,
                   carto::Object options );
    Transformation3d* create( carto::Object header,
                              const AllocatorContext & context,
                              carto::Object options );
    virtual void read( Transformation3d & obj,
                       carto::rc_ptr<DataSourceInfo> dsi,
                       const AllocatorContext & context,
                       carto::Object options );
    virtual FormatReader<Transformation3d>* clone() const;
    virtual std::string formatID() const { return "TRMCHAIN"; }
  };


}

#endif
