#ifndef AIMS_IO_SOMA_TRMFORMATREADER_H
#define AIMS_IO_SOMA_TRMFORMATREADER_H

#include <soma-io/reader/formatreader.h>


namespace aims
{
  class AffineTransformation3d;
}

namespace soma
{
  class Transformation3d;

  /** .trm format for a transformation file

      Reader options:

      - inv (bool 0/1):
          if true, invert the transformation
   */
  class TrmFormatReader : public FormatReader<aims::AffineTransformation3d>
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
    virtual std::string formatID() const { return "TRM"; }
  };


  class TrmT3DFormatReader : public FormatReader<Transformation3d>
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
    virtual std::string formatID() const { return "AFFINETRANS"; }
  };

}

#endif
