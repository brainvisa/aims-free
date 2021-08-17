
class Converter_%Template1typecode%_%Template2typecode%
{
%Docstring
Converter classes can be used using the convert() method, or as a callable object:

result = converter(input_data)

Parameters
----------
input_data: %Template1typecode%
    data to be converted

Returns
-------
output data: %Template2typecode%
    converted data
%End

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
%Docstring
converter.convert(input_data, output_data)

In-place converson inside existing allocated data

Parameters
----------
input_data: %Template1typecode%
    data to be converted
output_data: %Template2typecode%
    conversion output will be done into this data

Returns
-------
None
%End
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

%%Template2defIsRcptr%%
%#ifdef IS_RCPTR%

  %Template2typecode% *operator ()
    ( const %Template1typecode% & ) const /Factory/;

%#else%

  rc_ptr_%Template2typecode% operator ()
    ( const %Template1typecode% & ) const;
%MethodCode
  sipRes = new carto::rc_ptr< %Template2% >( sipCpp->operator()
    ( %Template1deref%( a0 ) ) );
%End

%#endif%

  void convert( const %Template1typecode% &, %Template2typecode% & ) const;
};


