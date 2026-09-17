#ifndef CARTODATA_ROI_DOI_H
#define CARTODATA_ROI_DOI_H

#include <cartobase/object/object_d.h>
#include <cartodata/wip/roi/roi.h>

#define DECLARE_VIRTUAL_GENERIC_OBJECT_TYPE( T ) \
template <> \
struct GenericObjectTypeDeclared< T > \
{ \
    static inline void check() {}; \
}; \
template <> \
void DictionaryInterface::setProperty( const std::string &key, \
                                       T const &value ); \
template <> \
bool DictionaryInterface::getProperty( const std::string &key, T &value ) const; \
extern template class TypedObject< T >; \
extern template class ReferenceObject< T >; \
extern template class PointerObject< T >; \
extern template \
T const &GenericObject::value< T >() const; \
extern template \
T &GenericObject::value< T >(); \
template <> \
bool GenericObject::value( T &dest ) const; \
template <> \
void GenericObject::setValue( T const & x ); \
extern template bool DictionaryInterface:: \
getProperty( const std::string &, T &  ) const; \
extern template void DictionaryInterface:: \
setProperty( const std::string &, T const & ); \


namespace carto {


  //------------------//
 //  RcptrObject<T>  //
//------------------//

// This class has nthing to do here, it should go in object.h.
// It is just for testing without compiling everything.
template <typename T>
class RcptrObject : public TypedObject<T>
{
public:
  inline RcptrObject( T &x );
  virtual ~RcptrObject();
  virtual T &getValue();
  
  virtual Object clone() const;
  
private:

  mutable rc_ptr<T> _pvalue;
};


  //--------------------//
 //  RcptrObject<T>  //
//--------------------//

//-----------------------------------------------------------------------------
template <typename T>
inline RcptrObject<T>::RcptrObject( T &x ) : _pvalue( &x )
{
}


  //----------------//
 //  DiscreteDOI   //
//----------------//

class DiscreteDOI : public virtual RCObject, virtual public IterableInterface
{
public:
  virtual ~DiscreteDOI();

  virtual Object objectIterator() const = 0;
  virtual Object valueAt( const Site &site ) const;

};

// DECLARE_VIRTUAL_GENERIC_OBJECT_TYPE( DiscreteDOI );

#if 0
#define T DiscreteDOI
template <>
struct GenericObjectTypeDeclared< T >
{
    static inline void check() {};
};
template <>
void DictionaryInterface::setProperty( const std::string &key,
                                       T const &value );
template <>
bool DictionaryInterface::getProperty( const std::string &key, T &value ) const;
template <>
bool GenericObject::value( T &dest ) const;
template <>
void GenericObject::setValue( T const & x );
extern template class TypedObject< T >;
extern template class ReferenceObject< T >;
extern template class PointerObject< T >;
extern template
T const &GenericObject::value< T >() const;
extern template
T &GenericObject::value< T >();
extern template
bool GenericObject::value( T &dest ) const;
extern template
void GenericObject::setValue( T const & x );
extern template bool DictionaryInterface::
getProperty( const std::string &, T &  ) const;
extern template void DictionaryInterface::
setProperty( const std::string &, T const & );
#undef T
#endif


  //-------------------//
 //  NumberIterator   //
//-------------------//

class NumberIterator : public virtual RCObject, virtual public IteratorInterface
{
  Object currentValue() const;
  virtual double currentNumber() const = 0;
};

// DECLARE_VIRTUAL_GENERIC_OBJECT_TYPE( NumberIterator );


  //----------------------//
 //  DiscreteNumberDOI   //
//----------------------//

class DiscreteNumberDOI : public DiscreteDOI
{
  virtual Object objectIterator() const;
  virtual Object valueAt( const Site &site ) const;
  virtual NumberIterator *numberIterator() const = 0;
  virtual double numberAt( const Site &site ) const;
};

// DECLARE_VIRTUAL_GENERIC_OBJECT_TYPE( DiscreteNumberDOI );



} // namespace carto

#endif // ifndef CARTODATA_ROI_DOI_H
