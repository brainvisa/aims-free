#ifndef AIMS_IO_SOMA_FSANNOTFORMATREADER_H
#define AIMS_IO_SOMA_FSANNOTFORMATREADER_H

#include <soma-io/reader/formatreader.h>


template <typename T> class TimeTexture;

namespace soma
{

  /** Freesurfer texture (curvature) format
   */
  template <typename T>
  class FsAnnotFormatReader
    : public FormatReader<TimeTexture<T> >
  {
  public:
    //========================================================================
    //   N E W   M E T H O D S
    //========================================================================
    virtual TimeTexture<T>*
    createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                   const AllocatorContext & context,
                   carto::Object options );
    virtual void read( TimeTexture<T> & obj,
                       carto::rc_ptr<DataSourceInfo> dsi,
                       const AllocatorContext & context,
                       carto::Object options );
    virtual FormatReader<TimeTexture<T> >* clone() const;
    virtual std::string formatID() const { return "FSCURV"; }

    bool readTexture( carto::rc_ptr<DataSource> ds,
                      TimeTexture<T> & obj, int nv,
                      bool ascii, bool bswap ) const;
  };

}

#endif
