namespace carto
{

class PropertySet
{
%TypeHeaderCode
#include <cartobase/object/property.h>
%End

public:
  PropertySet();
  PropertySet( const carto::PropertySet & );
  virtual ~PropertySet();

  carto::Object getProperty( const std::string & ) const;
  carto::Object getProperty( carto::Object ) const;
  virtual void setProperty( const std::string &, carto::Object );
  virtual bool removeProperty( const std::string & );
  virtual bool hasProperty( const std::string & ) const;
  virtual size_t size() const;
  virtual void clearProperties();
  virtual void copyBuiltinProperties( const carto::PropertySet & );

  // Iterable interface methods
  virtual carto::Object objectIterator() const;
};

}; // namespace carto
