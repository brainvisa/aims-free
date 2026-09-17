#ifndef AIMS_IO_SOMA_TRMGRAPH_FORMATWRITER_H
#define AIMS_IO_SOMA_TRMGRAPH_FORMATWRITER_H

#include <soma-io/writer/formatwriter.h>

namespace aims
{
  class TransformationGraph3d;
}

namespace soma
{

  /** Write transformation graph in files

      Writing options:

      - allow_read: true
      - affine_only: false
      - release_loaded: false
      - embed_affines: false

   */
  class TrmGraphFormatWriter
    : public FormatWriter<aims::TransformationGraph3d>
  {
  public:
    virtual bool filterProperties(carto::Object properties,
                                  carto::Object options = carto::none());

    virtual bool write( const aims::TransformationGraph3d & obj,
                        carto::rc_ptr<DataSourceInfo> dsi,
                        carto::Object options );
  };

}

#endif
