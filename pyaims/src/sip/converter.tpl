
class Converter_%Template1typecode%_%Template2typecode%
{
%TypeHeaderCode
#include <aims/utility/converter_g.h>
#include <pyaims/data/data.h>
typedef carto::Converter<%Template1%, %Template2% > 
  Converter_%Template1typecode%_%Template2typecode%;
%End

public:
  Converter_%Template1typecode%_%Template2typecode%( bool = false );
  ~Converter_%Template1typecode%_%Template2typecode%();

  %Template2typecode%* operator ()
    ( const %Template1typecode% & ) const /Factory/;
  void convert( const %Template1typecode% &, %Template2typecode% & ) const;
};


class ShallowConverter_%Template1typecode%_%Template2typecode%
{
%TypeHeaderCode
#include <aims/utility/converter_g.h>
#include <pyaims/data/data.h>
typedef carto::ShallowConverter<%Template1%, %Template2% > 
  ShallowConverter_%Template1typecode%_%Template2typecode%;
%End

public:
  ShallowConverter_%Template1typecode%_%Template2typecode%( bool = false );
  ~ShallowConverter_%Template1typecode%_%Template2typecode%();

  %Template2typecode%* operator ()
    ( const %Template1typecode% & ) const /Factory/;
  void convert( const %Template1typecode% &, %Template2typecode% & ) const;
};


