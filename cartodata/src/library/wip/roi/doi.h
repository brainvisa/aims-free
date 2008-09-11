/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

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
