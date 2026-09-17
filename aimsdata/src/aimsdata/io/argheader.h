/*
 *  Bucket header class
 */
#ifndef AIMS_IO_ARGHEADER_H
#define AIMS_IO_ARGHEADER_H


#include <aims/config/aimsdata_config.h>
#include <aims/data/pheader.h>
#include <aims/def/general.h>


namespace aims
{

/** Descriptor class for the .arg GRAPH file format header.
*/
  class AIMSDATA_API ArgHeader : public PythonHeader
  {
  public:
    ArgHeader( const std::string& name ) :
      PythonHeader(), 
      _name( name )
    { }
    virtual ~ArgHeader() { }

    std::string name() const { return _name; }

    std::string filename() const;
    virtual std::string extension() const { return( ".arg" ); }
    virtual std::set<std::string> extensions() const;

    void read( size_t* offset = 0 );
    void write();

  private:
    std::string	_name;
// Yann // A vérifier et compléter
  };

}


#endif
