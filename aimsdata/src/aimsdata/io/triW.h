/*
 *  Tri writer class
 */
#ifndef AIMS_IO_TRIW_H
#define AIMS_IO_TRIW_H

#include <aims/config/aimsdata_config.h>
#include <string>
#include <aims/mesh/surface.h>


namespace aims
{

  class AIMSDATA_API TriWriter
  {
  public:

    TriWriter( const std::string& name ) : _name(name) { }
    virtual ~TriWriter() { }

    void write( const AimsSurfaceTriangle& thing );

  private:
    std::string   _name;
  };

  inline 
  TriWriter & operator << ( TriWriter & writer, AimsSurfaceTriangle & thing )
  {
    writer.write( thing );
    return( writer );
  }

}

#endif










