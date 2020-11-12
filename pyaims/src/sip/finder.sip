namespace aims
{

class Finder
{
%TypeHeaderCode
#include <aims/data/pheader.h>
#include <aims/io/finder.h>
%End

%TypeCode
#include <pyaims/object/numconv.h>
#include <aims/data/pheader.h>
%End

public:
  Finder();
  virtual ~Finder();
  bool check( const std::string & ) /ReleaseGIL/;
  std::string objectType() const;
  std::string dataType() const;
  vector_STRING possibleDataTypes() const;
  std::string format() const;
  carto::Object headerObject() const /PyName=header, ReleaseGIL/;
  static SIP_PYLIST extensions( const std::string & );
%MethodCode
  std::set<std::string> extensions = aims::Finder::extensions(*a0);
  std::set<std::string>::const_iterator	it, et;
  unsigned int i;

  if( (sipRes = PyList_New(extensions.size())) == NULL )
    return NULL;

  for( i=0, it = extensions.begin(), et = extensions.end();
        it != et; ++it, ++i)
  {
    PyObject *extension_py = carto::PyString_FromStdString( *it );
    if (PyList_SetItem(sipRes, i, extension_py) < 0)
    {
      Py_DECREF(sipRes);
      sipIsErr = 1;
      sipRes = 0;
      PyErr_SetString( PyExc_TypeError, "string insertion failed" );
      break;
    }
  }
%End

  void setHeader( carto::Object hdr );
  void setObjectType( const std::string & obj );
  void setDataType( const std::string & dat );
  void setPossibleDataTypes( const vector_STRING & dt );

  static void registerFormat( const std::string & fmtid,
                              aims::FinderFormat* format /Transfer/,
                              const vector_STRING & extensions,
                              const std::string & before = "" );
  static void unregisterFormat( const std::string & format );
  static aims::FinderFormat* finderFormat( const std::string & format );

};

  class FinderFormat
  {
%TypeHeaderCode
#include <aims/io/fileFormat.h>
%End

  public:
    virtual ~FinderFormat();
    virtual bool check( const std::string & filename, aims::Finder & f ) const = 0;
  };

};
