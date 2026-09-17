/*
 *  ColorMap writer class
 */
#ifndef AIMS_IO_CMAPW_H
#define AIMS_IO_CMAPW_H

#include <aims/config/aimsdata_config.h>
#include <aims/color/colormap.h>
#include <fstream>
#include <string>

class AimsColorMapWriter;


AIMSDATA_API AimsColorMapWriter&
operator << ( AimsColorMapWriter& writer, const aims::ColorMap& thing );


class AIMSDATA_API AimsColorMapWriter
{
  public:

    AimsColorMapWriter( const std::string& name ) : _name( name ) { }
    virtual ~AimsColorMapWriter() { }

    void write( const aims::ColorMap& thing );

    // Return a name without .rgb extension
    std::string removeExtension( const std::string& name );

  friend
  AimsColorMapWriter& 
  operator << ( AimsColorMapWriter& writer, const aims::ColorMap& thing );

  private:

    std::ofstream _os;
    std::string   _name;
};


#endif










