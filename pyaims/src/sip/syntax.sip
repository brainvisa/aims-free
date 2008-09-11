
namespace carto
{


struct Semantic
{
%TypeHeaderCode
#include <cartobase/object/syntax.h>
%End

public:
  Semantic();
  Semantic( const std::string &, bool = false, bool = false );

  std::string type;
  bool needed;
  bool internal;
};


//typedef std::map<std::string,carto::Semantic> Syntax;


class PySyntaxSet /PyName=SyntaxSet/
{
%TypeHeaderCode
#include <pyaims/object/object.h>
%End

public:
  PySyntaxSet();
  virtual ~PySyntaxSet();

  carto::Semantic & semantic( const std::string &, const std::string & );
%MethodCode
  sipRes = &(*sipCpp)[ *a0 ][ *a1 ];
%End
};


class SyntaxReader
{
%TypeHeaderCode
#include <cartobase/object/sreader.h>
#include <pyaims/object/object.h>
%End

public:
  SyntaxReader( const std::string & );
  virtual ~SyntaxReader();
  carto::PySyntaxSet *_read() /Factory, PyName=read/;
%MethodCode
  sipRes = new carto::PySyntaxSet;
  try
  {
    sipCpp->read( *sipRes );
  }
  catch( std::exception & e )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_IOError, e.what() );
    delete sipRes;
    sipRes = 0;
  }
%End

private:
  SyntaxReader( const carto::SyntaxReader & );
};

};

