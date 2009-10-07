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

#ifndef CARTOBASE_OBJECT_SYNTOBJECT_H
#define CARTOBASE_OBJECT_SYNTOBJECT_H

#include <cartobase/object/object.h>
#include <cartobase/object/syntax.h>
#include <cartobase/object/property.h>


namespace carto
{

  namespace internal
  {

    template<typename T> class SyntaxedNonInterfaceObject 
      : public ValueObject<T>, public SyntaxedInterface
    {
    public:
      SyntaxedNonInterfaceObject( const std::string& s );
      SyntaxedNonInterfaceObject( const SyntaxedNonInterfaceObject<T> & );
      virtual ~SyntaxedNonInterfaceObject();

      virtual bool hasSyntax() const;
      virtual std::string getSyntax() const;
      virtual void setSyntax( const std::string& syntactic );

    private:
      std::string _syntactic;
    };


    template <typename T>
    class SyntaxedInterfaceType
      : public T, public SyntaxedInterface
    {
    public:
      SyntaxedInterfaceType( const std::string & s = "" );
      SyntaxedInterfaceType( const SyntaxedInterfaceType & );
      virtual ~SyntaxedInterfaceType();

      virtual bool hasSyntax() const;
      virtual std::string getSyntax() const;
      virtual void setSyntax( const std::string& syntactic );

    private:
      std::string _syntactic;
    };


    template <typename T>
    class SyntaxedInterfaceObject
      : public ValueObject<SyntaxedInterfaceType<T> >
    {
    public:
      SyntaxedInterfaceObject( const std::string & s ) 
        : ValueObject<SyntaxedInterfaceType<T> >()
      { this->getValue().setSyntax( s ); }
      virtual bool hasSyntax() const { return true; }
      std::string getSyntax() const
      { return const_cast<SyntaxedInterfaceObject *>( this )->
          getValue().getSyntax(); }
      void setSyntax( const std::string & s ) 
      { this->getValue().setSyntax( s ); }
    };


    template <typename T, bool x> 
    class SyntaxedObjectSwitch
    {
    };


    template <typename T> 
    class SyntaxedObjectSwitch<T, true>
    {
    public:
      typedef SyntaxedInterfaceObject<T> ObjectType;
      typedef SyntaxedInterfaceType<T> ContentType;
    };


    template <typename T> 
    class SyntaxedObjectSwitch<T, false>
    {
    public:
      typedef SyntaxedNonInterfaceObject<T> ObjectType;
      typedef T ContentType;
    };

  } // namespace internal


  template <typename T> 
  class SyntaxedObject 
    : public internal::SyntaxedObjectSwitch<T, SUPERSUBCLASS(Interface, T)>::
    ObjectType
  {
  public:
    SyntaxedObject( const std::string & s ) 
      : internal::SyntaxedObjectSwitch<T, SUPERSUBCLASS(Interface, T)>::
      ObjectType( s )
      {}
    SyntaxedObject( const SyntaxedObject & x ) 
      : internal::SyntaxedObjectSwitch<T, SUPERSUBCLASS(Interface, T)>::
      ObjectType( x )
      {}
    typedef typename internal::SyntaxedObjectSwitch<T, 
      SUPERSUBCLASS(Interface, T)>::ContentType ContentType;
  };


  // definitions

  namespace internal
  {

    template<typename T> inline 
    SyntaxedNonInterfaceObject<T>::SyntaxedNonInterfaceObject
    ( const std::string & s )
      : ValueObject<T>(), _syntactic( s )
    {
    }

    template<typename T> inline 
    SyntaxedNonInterfaceObject<T>::SyntaxedNonInterfaceObject
    ( const SyntaxedNonInterfaceObject & x )
      : RCObject(), 
        Interface(), 
        StringInterface(), 
        ScalarInterface(), 
        SizeInterface(), 
        IterableInterface(), 
        ArrayInterface(), 
        DynArrayInterface(), 
        DictionaryInterface(), 
        IteratorInterface(), 
        DictionaryIteratorInterface(), 
        ValueObject<T>( x ), 
        SyntaxedInterface(), 
        _syntactic( x._syntactic )
    {
    }

    template<typename T> inline 
    bool SyntaxedNonInterfaceObject<T>::hasSyntax() const
    {
      return true;
    }

    template<typename T> inline 
    std::string SyntaxedNonInterfaceObject<T>::getSyntax() const
    {
      return _syntactic;
    }

    template<typename T> inline 
    void SyntaxedNonInterfaceObject<T>::setSyntax
    ( const std::string& syntactic )
    {
      _syntactic = syntactic;
    }

    //

    template<typename T> inline 
    SyntaxedInterfaceType<T>::SyntaxedInterfaceType
    ( const SyntaxedInterfaceType & x )
      : Interface(),
        T( x ),
        SyntaxedInterface(),
        _syntactic( x._syntactic )
    {
    }

    template<typename T> inline 
    SyntaxedInterfaceType<T>::SyntaxedInterfaceType( const std::string & s )
      : T(), SyntaxedInterface(), _syntactic( s )
    {
    }

    template<typename T> inline 
    bool SyntaxedInterfaceType<T>::hasSyntax() const
    {
      return true;
    }

    template<typename T> inline 
    std::string SyntaxedInterfaceType<T>::getSyntax() const
    {
      return _syntactic;
    }

    template<typename T> inline 
    void SyntaxedInterfaceType<T>::setSyntax( const std::string& syntactic )
    {
      _syntactic = syntactic;
    }


    extern template class SyntaxedNonInterfaceObject<Dictionary>;
    extern template class SyntaxedInterfaceType<PropertySet>;
    extern template class SyntaxedInterfaceObject<PropertySet>;
  }

  DECLARE_GENERIC_OBJECT_TYPE( internal::SyntaxedInterfaceType<PropertySet> )
}

#endif
