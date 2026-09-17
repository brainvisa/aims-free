#ifndef AIMS_IO_SELECTIONR_H
#define AIMS_IO_SELECTIONR_H

#include <string>
#include <iostream>

namespace aims
{
  class SelectionSet;

  class SelectionReader
  {
  public:
    SelectionReader( const std::string & filename );
    SelectionReader( std::istream & istr );
    ~SelectionReader();

    void open( const std::string & filename );
    void open( std::istream & istr );

    void read( SelectionSet & );

  private:
    std::string		_filename;
    std::istream	*_stream;
  };

}

#endif

