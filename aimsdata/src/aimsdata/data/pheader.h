#ifndef AIMS_DATA_PHEADER_H
#define AIMS_DATA_PHEADER_H

#include <aims/data/header.h>
#include <cartobase/object/object.h>
#include <cartobase/object/syntax.h>
#include <cartobase/object/property.h>
#include <cartobase/object/headered.h>

namespace aims
{
  namespace internal
  {
    /// wrapper to Headered/PropertySet
    typedef carto::PointerObject<carto::PropertySet> PythonHeaderType;
  }

  class PythonHeader : public internal::PythonHeaderType, public Header
  {
  public:
    PythonHeader();
    PythonHeader( const internal::PythonHeaderType & );
    PythonHeader( const PythonHeader & );
    /// initialization by reference
    PythonHeader( carto::Headered & );
    virtual ~PythonHeader();

    virtual PythonHeader & operator = ( const PythonHeader & );

    virtual const char* id() const;
    virtual bool readMinf( const std::string & filename );
    virtual bool writeMinf( const std::string & filename );
    virtual bool writeMinf( const std::string & filename ) const;
    virtual Header* cloneHeader( bool keepUuid = false ) const;
    virtual void copy( const PythonHeader &, bool keepUuid = false );
    virtual void copy( const carto::Object &, bool keepUuid = false );
    virtual bool isScalar() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isArray() const { return false; }
    virtual bool isDynArray() const { return false; }
    virtual bool isIterator() const { return false; }
    virtual bool isDictionaryIterator() const { return false; }

    static carto::SyntaxSet* syntax();
    static bool writeMinf( const carto::Object & header,
                           const std::string & filename );
    static bool writeMinf( carto::Object & header,
                           const std::string & filename );
    static void copy( const carto::Object & src, carto::Object & dst,
                      bool keepUuid = false );
    static carto::Object cloneHeader( const carto::Object & src,
                                      bool keepUuid = false );
  };

} // namespace aims


namespace carto
{

  template <>
  inline Object Object::value<aims::PythonHeader>()
  {
    return Object( static_cast<GenericObject *>( new aims::PythonHeader ) );
  }

  template <>
  inline Object Object::value( const aims::PythonHeader &v )
  {
    return Object( static_cast<GenericObject *>(
        new aims::PythonHeader( v ) ) );
  }
}

namespace carto {
DECLARE_GENERIC_OBJECT_TYPE( aims::PythonHeader )
}

#endif

