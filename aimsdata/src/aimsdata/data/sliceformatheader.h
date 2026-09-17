#ifndef AIMS_DATA_SLICEFORMATHEADER_H
#define AIMS_DATA_SLICEFORMATHEADER_H

#include <aims/data/pheader.h>
#include <vector>

namespace aims
{

  /**	Header used for 2D image formats. It contains a mechanism to 
     retreive a list of slices / frames to build a 3D or 4D volume from 
     a files list from the .minf attributes
  */
  class SliceFormatHeader : public PythonHeader
  {
  public:
    SliceFormatHeader( const std::string & filename, int dimx = 1, 
                       int dimy = 1, int dimz = 1, int dimt = 1, float vsx = 1,
                       float vsy = 1, float vsz = 1, float vst = 1 );
    virtual ~SliceFormatHeader();

    virtual std::vector<std::string> inputFilenames();
    virtual std::vector<std::string> 
    outputFilenames() const;
    std::string inputNamepattern( unsigned & smin, unsigned & smax, 
                                  unsigned & tmin, unsigned & tmax ) const;
    std::string filename() const;

  protected:
    std::string	_name;
  };

}

#endif

