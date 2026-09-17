#ifndef AIMS_IO_SOMA_XFMFORMATWRITER_H
#define AIMS_IO_SOMA_XFMFORMATWRITER_H

#include <soma-io/writer/formatwriter.h>


namespace aims
{
  class AffineTransformation3d;
}

namespace soma
{

  class Transformation3d;


  class XfmFormatWriter : public FormatWriter<aims::AffineTransformation3d>
  {
  public:
    virtual bool filterProperties(carto::Object properties,
                                  carto::Object options = carto::none());

    virtual bool write( const aims::AffineTransformation3d & obj,
                        carto::rc_ptr<DataSourceInfo> dsi,
                        carto::Object options );
  };


  class Xfm3DFormatWriter : public FormatWriter<Transformation3d>
  {
  public:
    virtual bool filterProperties(carto::Object properties,
                                  carto::Object options = carto::none());

    virtual bool write( const Transformation3d & obj,
                        carto::rc_ptr<DataSourceInfo> dsi,
                        carto::Object options );
  };

}

#endif

