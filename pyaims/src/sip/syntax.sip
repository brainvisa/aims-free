
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


typedef std::map<std::string,carto::Semantic> Syntax;
typedef std::map<std::string,carto::Syntax> SyntaxSet;


class PySyntaxSet /PyName=PSyntaxSet/
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

  vector_STRING * keys() const /Factory/;
%MethodCode
  sipRes = new std::vector<std::string>;
  sipRes->reserve( sipCpp->size() );
  carto::SyntaxSet::const_iterator i, e = sipCpp->end();
  for( i=sipCpp->begin(); i!=e; ++i )
    sipRes->push_back( i->first );
%End

  vector_STRING * syntaxKeys( const std::string & ) const /Factory/;
%MethodCode
  sipRes = new std::vector<std::string>;
  sipRes->reserve( sipCpp->size() );
  carto::SyntaxSet::const_iterator i = sipCpp->find( *a0 );
  if( i != sipCpp->end() )
  {
    const carto::Syntax & stx = i->second;
    carto::Syntax::const_iterator is, es = stx.end();
    for( is=stx.begin(); is!=es; ++is )
      sipRes->push_back( is->first );
  }
%End

/*
//  std::map<std::string,carto::Syntax> * tomap() /Factory/;
  std::map<std::string,carto::Syntax> * tomap();
%MethodCode
  sipRes = new SyntaxSet( *sipCpp );
%End
*/
};


class SyntaxReader
{
%TypeHeaderCode
#include <cartobase/object/sreader.h>
#include <pyaims/object/object.h>
%End

%TypeCode
#include <cartobase/exception/file.h>
%End

public:
  SyntaxReader( const std::string & ) throw ();
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


  // carto::SyntaxSet* read() /Factory/;

private:
  SyntaxReader( const carto::SyntaxReader & );
};

};

