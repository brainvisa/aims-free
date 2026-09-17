/*
 *  Data reader class
 */
#ifndef AIMS_IO_COARSER_H
#define AIMS_IO_COARSER_H

#include <cartodata/volume/volume.h>
#include <string>

namespace aims
{

  ///	Reads a AimsData<int16_t>, with conversion if needed
  class CoarseReader
  {
  public:
    CoarseReader( const std::string & f );
    virtual ~CoarseReader();
    virtual void read( carto::VolumeRef<int16_t> & data, int border = 0,
                       const std::string* format = 0, int frame = -1 );

  private:
    std::string	_filename;
  };

}

#endif
