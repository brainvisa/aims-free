#ifndef AIMS_IO_SELECTIONW_H
#define AIMS_IO_SELECTIONW_H

#include <string>
#include <iostream>

namespace aims
{
  class SelectionSet;

  class SelectionWriter
  {
  public:
    SelectionWriter( const std::string & filename );
    SelectionWriter( std::ostream & str );
    ~SelectionWriter();

    void open( const std::string & filename );
    void open( std::ostream & ostr );

    void write( const SelectionSet & );

  private:
    std::string		_filename;
    std::ostream	*_stream;
  };

}

#endif

