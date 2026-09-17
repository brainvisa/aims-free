/*
 *  ColorMap reader class
 */
#ifndef AIMS_IO_CMAPR_H
#define AIMS_IO_CMAPR_H

#include <aims/config/aimsdata_config.h>
#include <aims/color/colormap.h>
#include <fstream>
#include <string>

class  AimsColorMapReader;


AIMSDATA_API AimsColorMapReader&
operator >> ( AimsColorMapReader& reader, aims::ColorMap& thing );


class AIMSDATA_API AimsColorMapReader
{
  public:

    AimsColorMapReader( const std::string& name ) : _name( name ) { }
    virtual ~ AimsColorMapReader() { }

    void read( aims::ColorMap& thing );

    /// Return a name without .rgb extension
    std::string removeExtension( const std::string& name );

  friend
  AimsColorMapReader& 
  operator >> ( AimsColorMapReader& reader, aims::ColorMap& thing );

  private:

    std::ifstream _is;
    std::string _name;
};


#endif










