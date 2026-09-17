/*
 *  Tri reader class
 */
#ifndef AIMS_IO_TRIR_H
#define AIMS_IO_TRIR_H

#include <aims/config/aimsdata_config.h>
#include <aims/mesh/surface.h>


namespace aims
{

  /**	Tri format readers for mesh objects.
	This reader can be used directly, or via the Reader / TriFormat 
	interface.
	End users should always use Reader which hides the format-specific 
	operations, TriReader is only a part of the mechanism

	\see MeshReader
  */
  class AIMSDATA_API TriReader
  {
  public:
    TriReader( const std::string& name ) : _name( name ) { }
    ~TriReader() { }

    void read( AimsSurfaceTriangle& thing, int frame = -1 );

  private:
    std::string   _name;
  };


  inline 
  TriReader & operator >> ( TriReader & reader, AimsSurfaceTriangle & thing )
  {
    reader.read( thing );
    return( reader );
  }

}


#endif
