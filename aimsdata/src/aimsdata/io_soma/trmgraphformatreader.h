#ifndef AIMS_IO_SOMA_TRMGRAPH_FORMATREADER_H
#define AIMS_IO_SOMA_TRMGRAPH_FORMATREADER_H

#include <soma-io/reader/formatreader.h>

namespace aims
{
  class TransformationGraph3d;
}

namespace soma
{

  /** Read transformation graph from a file (yaml, json or other)

   */
  class TrmGraphFormatReader
    : public FormatReader<aims::TransformationGraph3d>
  {
  public:
    //========================================================================
    //   N E W   M E T H O D S
    //========================================================================
    virtual aims::TransformationGraph3d*
    createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                   const AllocatorContext & context,
                   carto::Object options );
    virtual void read( aims::TransformationGraph3d & obj,
                       carto::rc_ptr<DataSourceInfo> dsi,
                       const AllocatorContext & context,
                       carto::Object options );
    virtual FormatReader<aims::TransformationGraph3d>* clone() const;
    virtual std::string formatID() const { return "TRMGRAPH"; }
  };

}

#endif
