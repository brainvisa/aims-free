#ifndef AIMS_IO_SOMA_TRMINGRAPH_FORMATREADER_H
#define AIMS_IO_SOMA_TRMINGRAPH_FORMATREADER_H

#include <soma-io/reader/formatreader.h>

namespace soma
{
  class Transformation3d;

  /** Read transformation in a transformation graph (TransformationGraph3d)

      The reader filename is the transformations graph, options specify which transform in the graph is to be used.

      Reader options:

      - source (string, mandatory):
          source referential in graph
      - destination (string, mandatory):
          destination referential in graph
   */
class TrmInGraphFormatReader
    : public FormatReader<soma::Transformation3d>
  {
  public:
    //========================================================================
    //   N E W   M E T H O D S
    //========================================================================
    virtual soma::Transformation3d*
    createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                   const AllocatorContext & context,
                   carto::Object options );
//     virtual void read( soma::Transformation3d & obj,
//                        carto::rc_ptr<DataSourceInfo> dsi,
//                        const AllocatorContext & context,
//                        carto::Object options );
    virtual FormatReader<soma::Transformation3d>* clone() const;
    virtual std::string formatID() const { return "TRMINGRAPH"; }
  };

}

#endif
