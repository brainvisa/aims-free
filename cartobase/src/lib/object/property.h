/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#ifndef CARTOBASE_OBJECT_PROPERTY_H
#define CARTOBASE_OBJECT_PROPERTY_H

#include <cartobase/config/cartobase_config.h>
#include <cartobase/object/object.h>
#include <cartobase/pattern/observable.h>
#include <cartobase/sigc++/signal.h>
#include <map>
#include <stdexcept>


namespace carto
{


  //---------------//
 //  PropertySet  //
//---------------//

/** A dictionary generic Object

    PropertySet can replace a std::map<std::string,Object> in 
    GenericObject implementations, and offers additional interesting features:

    - builtin properties support: a builtin property is a reference to an 
      external existing variable (generally a member of a structure or class 
      tied to the property set). They are used transparently just like any 
      other dynamic properties, but as they use lower-level underlying 
      variables, this allows optimizations in special cases when a program 
      knows it operates on the exact intentded structure without breaking 
      compatibility with the generic API. \\
      A builtin property can change value, but cannot change type because it 
      references a typed value at a fixed memory location.

    - signal / slot support: signals are emitted whenever a property changes. 
      The "raw" signal support is a low-level mechanism which fires every time 
      any property changes. For a more convenient behaviour, a filter 
      mechanism can be used in conjunction with Propertyset: PropertyFilter
      
 */
class PropertySet : virtual public RCObject, public Observable, 
    public DictionaryInterface
{
public:
  /** This signal type informs the slots about the sender (this), the 
      property name and its old value */
  typedef ::sigc::signal3< void, const Object &, const std::string&, 
                           const Object& > Signal;

  PropertySet();
  PropertySet( const PropertySet & );
  PropertySet &operator =( const PropertySet & );
  virtual ~PropertySet();

  Signal& getSignalPropertyChanged();

  // DictionaryInterface methods
  template <typename T> void setProperty( const std::string &, const T & );
  /** Reimplementation needed because of a bizarre "feature" of the C++ 
      language */
  void setProperty( const std::string & key, const char * value )
  { DictionaryInterface::setProperty( key, value ); }
  template <typename T> void addBuiltinProperty( const std::string &, T & );
  /// add an optional builtin property
  template <typename T> void addBuiltinProperty( const std::string &, T &, 
                                                 bool & provided );
  template <typename T> bool getProperty( const std::string &, T & ) const;

  virtual bool getProperty( const std::string &, Object & ) const;
  /** Reimplementation needed because of a bizarre "feature" of the C++ 
      language */
  Object getProperty( const std::string & key ) const
  { return DictionaryInterface::getProperty( key ); }
  /** Reimplementation needed because of a bizarre "feature" of the C++ 
      language */
  Object getProperty( Object key ) const
  { return DictionaryInterface::getProperty( key ); }
  virtual void setProperty( const std::string &, Object );
  virtual bool removeProperty( const std::string & );
  virtual bool hasProperty( const std::string & ) const;
  virtual size_t size() const;
  virtual void clearProperties();
  virtual void copyBuiltinProperties( const PropertySet & source );

  // Iterable interface methods
  virtual Object objectIterator() const;

  class iterator;

  inline iterator getIterator() const;

private:

  struct Property
  {
    inline Property( const Object &o, bool b ) 
      : object( o ), builtin( b ? &PropertySet::_mandatoryProperty : 0 ) {}
    inline Property( const Object &o, bool *b ) : object( o ), builtin( b ) {}
    inline Property() : builtin( 0 ) {}
    Object object;
    bool *builtin;
  };
  typedef std::map< std::string, Property > Properties;
  typedef std::list< Properties::iterator > PropertiesOrder;

public:

  class iterator : public DictionaryIteratorInterface
  {
  public:
    inline iterator();

    // DictionaryIteratorInterface methods
    bool isValid() const;
    std::string key() const;
    Object currentValue() const;
    void next();

  private:

    friend  PropertySet::iterator PropertySet::getIterator() const;
    
    inline iterator( const PropertiesOrder::const_iterator &begin,
                     const PropertiesOrder::const_iterator &end );

    PropertiesOrder::const_iterator _iterator; 
    PropertiesOrder::const_iterator _end;
  };


private:

  inline void _insert( const std::string &key, const Property &value );

  Properties _properties;
  PropertiesOrder _propertiesOrder;
  Signal _signalPropertyChanged;

  static bool _mandatoryProperty;
};



  //-------------------------------//
 //  DataTypeCode< PropertySet >  //
//-------------------------------//

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

template <> class DataTypeCode< PropertySet >
{
public:
  std::string objectType() const
  { return( "PropertySet" ); }
  std::string dataType() const
  { return std::string( "Object" ); }
  static std::string name()
  {
    return std::string( "PropertySet" );
  }
};

#endif


DECLARE_GENERIC_OBJECT_TYPE( PropertySet )
DECLARE_GENERIC_OBJECT_TYPE( PropertySet::iterator )


  //---------------//
 //  PropertySet  //
//---------------//


//-----------------------------------------------------------------------------
template <typename T> 
void PropertySet::setProperty( const std::string &key, const T &value )
{
  Properties::iterator it = _properties.find( key );
  if ( it == _properties.end() ) {
    _insert( key, Property( Object::value( value ), false ) );
  } else {
    Object oldValue;
    bool   willemit = false;
    if( !_signalPropertyChanged.empty() )
      {
        willemit = true;
        if( !it->second.object.isNull() )
          oldValue = it->second.object->clone();
      }
    if ( it->second.builtin ) {
      it->second.object->setValue( value );
      if( it->second.builtin != &_mandatoryProperty )
        *it->second.builtin = true;
    } else {
      it->second.object = Object::value( value );
    }
    if( willemit )
      _signalPropertyChanged.sigcemit( Object::reference( *this ), 
                                       it->first, oldValue );
  }
}


//-----------------------------------------------------------------------------
template <typename T> 
void PropertySet::addBuiltinProperty( const std::string &key, T &reference )
{
  Properties::iterator it = _properties.find( key );
  if ( it == _properties.end() ) {
    _insert( key, Property( Object::reference( reference ), true ) );
  } else {
    throw std::runtime_error( std::string( "Built-in property " ) + key + 
                              " already exists" );
  }
}


//-----------------------------------------------------------------------------
template <typename T> 
void PropertySet::addBuiltinProperty( const std::string &key, T &reference, 
                                      bool & provided )
{
  Properties::iterator it = _properties.find( key );
  if ( it == _properties.end() ) {
    _insert( key, Property( Object::reference( reference ), &provided ) );
  } else {
    throw std::runtime_error( std::string( "Built-in property " ) + key + 
                              " already exists" );
  }
}


//-----------------------------------------------------------------------------
template <typename T>
bool PropertySet::getProperty( const std::string &key, T &value ) const
{
  Properties::const_iterator it = _properties.find( key );
  if ( it != _properties.end() && 
       ( !it->second.builtin || it->second.builtin == &_mandatoryProperty ) )
    {
      return it->second.object->value( value );
    }
  return false;
}


//-----------------------------------------------------------------------------
inline PropertySet::iterator PropertySet::getIterator() const 
{
  return iterator( _propertiesOrder.begin(),  _propertiesOrder.end() );
}


//-----------------------------------------------------------------------------
inline void PropertySet::_insert( const std::string &key, 
                                  const Property &value ) {
  _propertiesOrder.push_back( 
    _properties.insert( std::pair<std::string, Property>( key, value ) ).first
  );
}


  //-------------------------//
 //  PropertySet::iterator  //
//-------------------------//

//-----------------------------------------------------------------------------
inline PropertySet::iterator::iterator()
{
}


//-----------------------------------------------------------------------------
inline PropertySet::iterator::
iterator(  const PropertiesOrder::const_iterator &begin,
		 const PropertiesOrder::const_iterator &end ) :
 _iterator( begin ), _end( end ) 
{
  while( _iterator != end && (*_iterator)->second.builtin 
         && !*(*_iterator)->second.builtin )
    ++_iterator;
}



} // namespace carto


#endif
