#ifndef AIMS_IO_SOMA_FSSURFFORMATREADER_H
#define AIMS_IO_SOMA_FSSURFFORMATREADER_H

#include <soma-io/reader/formatreader.h>


template <int D, typename T> class AimsTimeSurface;

namespace soma
{

  /** Freesurfer surface format for a mesh
   */
  template <int D>
  class FsSurfFormatReader
    : public FormatReader<AimsTimeSurface<D, Void> >
  {
  public:
    //========================================================================
    //   N E W   M E T H O D S
    //========================================================================
    virtual AimsTimeSurface<D, Void>*
    createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                   const AllocatorContext & context,
                   carto::Object options );
    virtual void read( AimsTimeSurface<D, Void> & obj,
                       carto::rc_ptr<DataSourceInfo> dsi,
                       const AllocatorContext & context,
                       carto::Object options );
    virtual FormatReader<AimsTimeSurface<D, Void> >* clone() const;
    virtual std::string formatID() const { return "FSSURF"; }

    bool readVertices( carto::rc_ptr<DataSource> ds,
                       AimsTimeSurface<D, Void> & obj, int nv,
                       bool ascii, bool bswap ) const;
    bool readPolygons( carto::rc_ptr<DataSource> ds,
                       AimsTimeSurface<D, Void> & obj, int np,
                       bool ascii, bool bswap ) const;
    bool readAdditions( carto::rc_ptr<DataSource> ds,
                        AimsTimeSurface<D, Void> & obj, bool ascii,
                        bool bswap ) const;
  };

}

#endif
