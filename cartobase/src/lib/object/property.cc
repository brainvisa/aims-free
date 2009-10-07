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

#include <cartobase/object/property.h>
#include <cartobase/object/object_d.h>

using namespace std;

namespace carto {

  //---------------//
 //  PropertySet  //
//---------------//

INSTANTIATE_GENERIC_OBJECT_TYPE( PropertySet )
INSTANTIATE_GENERIC_OBJECT_TYPE( PropertySet::iterator )


bool PropertySet::_mandatoryProperty( true );

//-----------------------------------------------------------------------------
PropertySet::PropertySet()
            : Observable()
{
}


//-----------------------------------------------------------------------------
PropertySet::~PropertySet()
{
}


//-----------------------------------------------------------------------------
PropertySet::PropertySet( const PropertySet &other ) :
  RCObject( other ), Interface(), SizeInterface(), IterableInterface(), 
  Observable(), DictionaryInterface()
{
  operator =( other );
}


//-----------------------------------------------------------------------------
PropertySet &PropertySet::operator = ( const PropertySet &other )
{
  /* important note:
    here builtin properties of this are kept builtin (not erased)
    If other has values for those builtins of this (either builtin or not in 
    other), their values are copied to this (this may result in an exception 
    if their type doesn't match).
    Other non-builtin properties of other are added (cloned) to the properties of 
    this.
    Builtin properties of other which are not builtin in this are NOT copied at 
    all. Use copyBuiltinProperties() to do it.
    
    NB: this copy operator doesn't emit signals - it this what we want ?
  */
  if ( this != & other ) {
    this->Observable::operator= ( other );

    PropertiesOrder::iterator io, io2, eo = _propertiesOrder.end();
    for( io=_propertiesOrder.begin(); io!=eo; )
      {
        if( (*io)->second.builtin )
          {
            if( (*io)->second.builtin != &_mandatoryProperty )
              *(*io)->second.builtin = false; // unset it
            ++io; // keep builtin properties
          }
        else
          {
            io2 = io;
            ++io;
            _properties.erase( *io2 );
            _propertiesOrder.erase( io2 );
          }
      }
    Properties::iterator ib, eb = _properties.end();
    for( Properties::const_iterator it = other._properties.begin();
	 it != other._properties.end(); ++it ) 
      if( !it->second.builtin || *it->second.builtin )
        {
          ib = _properties.find( it->first );
          if( ib != eb ) // builtin in this
            {
              if( ib->second.builtin 
                  && ib->second.builtin != &_mandatoryProperty )
                *ib->second.builtin = true;
              ib->second.object->setValue( it->second.object ); // (may throw)
            }
          else if ( !it->second.builtin )
            // don't copy builtins: it should be done 
            // at higher level because different behaviours are possible
            _insert( it->first,
                     Property( it->second.object->clone(), false ) );
        }
  }
  return *this;
}


//-----------------------------------------------------------------------------
void PropertySet::copyBuiltinProperties( const PropertySet &other )
{
  if ( this != & other )
    {
      Properties::iterator eb = _properties.end();
      for( Properties::const_iterator it = other._properties.begin();
           it != other._properties.end(); ++it )
        if ( it->second.builtin && *it->second.builtin 
             && _properties.find( it->first ) == eb )
          _insert( it->first, it->second );
    }
}


//-----------------------------------------------------------------------------
void PropertySet::setProperty( const std::string &key, Object value )
{
  Properties::iterator it = _properties.find( key );
  if ( it == _properties.end() ) {
    _insert( key, Property( value, false ) );
  } else {
    // TODO: the value should not be cloned if the signal is
    //       not connected to any slot.
    Object oldValue;
    if ( ! it->second.object.isNull() 
         && ( !it->second.builtin || *it->second.builtin ) ) {
      oldValue = it->second.object->clone();
    }
    if ( it->second.builtin ) {
      it->second.object->setValue( value );
      if( it->second.builtin != &_mandatoryProperty )
        *it->second.builtin = true;
    } else {
      it->second.object = value;
    }
    _signalPropertyChanged.sigcemit( Object::reference( *this ), 
                                     it->first, oldValue );
  }
}


//-----------------------------------------------------------------------------
bool PropertySet::removeProperty( const std::string &key )
{
  Properties::iterator it = _properties.find( key );
  if ( it != _properties.end() ) {
    if ( it->second.builtin )
      {
        if( it->second.builtin != &_mandatoryProperty && *it->second.builtin )
          {
            *it->second.builtin = false;
            return true;
          }
        return false;
      }
    for( PropertiesOrder::iterator it2 = _propertiesOrder.begin();
         it2 != _propertiesOrder.end(); ++it2 ) {
      if ( *it2 == it ) {
        _propertiesOrder.erase( it2 );
        break;
      }
    }
    _properties.erase( key );
  }
  return true;
}


//-----------------------------------------------------------------------------
bool PropertySet::hasProperty( const std::string &key ) const
{
  Properties::const_iterator it = _properties.find( key );
  return it != _properties.end() 
    && ( !it->second.builtin || *it->second.builtin );
}


//-----------------------------------------------------------------------------
bool PropertySet::getProperty( const std::string &key, Object & result ) const
{
  Properties::const_iterator it = _properties.find( key );
  if ( it != _properties.end() 
       && ( !it->second.builtin || *it->second.builtin ) )
    {
      result = it->second.object;
      return true;
    }
  return false;
}


//-----------------------------------------------------------------------------
PropertySet::Signal& 
PropertySet::getSignalPropertyChanged()
{

  return _signalPropertyChanged;

}


//-----------------------------------------------------------------------------
void PropertySet::clearProperties()
{
  _properties.clear();
  _propertiesOrder.clear();
}


//-----------------------------------------------------------------------------
size_t PropertySet::size() const
{
  // not optiaml at all...
  Properties::const_iterator	i, e = _properties.end();
  size_t			n = 0;
  for( i=_properties.begin(); i!=e; ++i )
    if( !i->second.builtin || *i->second.builtin )
      ++n;
  return n;
}


//-----------------------------------------------------------------------------
Object PropertySet::objectIterator() const
{
  return Object::value( getIterator() );
}


  //-------------------------//
 //  PropertySet::iterator  //
//-------------------------//


//-----------------------------------------------------------------------------
bool PropertySet::iterator::isValid() const 
{
  return _iterator != _end; 
}


//-----------------------------------------------------------------------------
std::string PropertySet::iterator::key() const
{
  return (*_iterator)->first;
}


//-----------------------------------------------------------------------------
Object PropertySet::iterator::currentValue() const
{
  return (*_iterator)->second.object;
}


//-----------------------------------------------------------------------------
void PropertySet::iterator::next()
{
  do
    {
      ++_iterator;
    }
  while( _iterator != _end 
         && (*_iterator)->second.builtin 
         && !*(*_iterator)->second.builtin );
}



} // namespace carto
