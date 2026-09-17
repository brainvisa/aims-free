/*
 *  Bucket item class
 */
#ifndef AIMS_BUCKET_ITEM_H
#define AIMS_BUCKET_ITEM_H


#include <aims/config/aimsdata_config.h>
#include <iostream>
#include <aims/vector/vector.h>



template <class T> class AimsBucketItem;


template <class T> AIMSDATA_API 
std::ostream& operator << (std::ostream& out, const AimsBucketItem<T>& thing);

template <class T>
int operator == (const AimsBucketItem<T>& thing1,
                 const AimsBucketItem<T>& thing2);

/** The template base class for all types of bucket items.
    The template argument gives the type of the value associated
    to a location. It can be a scalar, but also a more complex data.
*/
template <class T>
class AIMSDATA_API AimsBucketItem
{ protected:
    /**@name Data*/
    //@{
    /// Location of the item in the 3D image
    AimsVector<short,3> _location;
    /// Value associated to the location
    T _value;
    //@}

  public:
    /**@name Constructors and destructor*/
    //@{
    /// Constructor sets location to origin (0,0,0)
    AimsBucketItem() : _location((short)0) { }
    /// Copy constructor
    AimsBucketItem(const AimsBucketItem<T>& other) : 
                       _location(other._location), _value(other._value) { }
    /// Destructor does nothing
    ~AimsBucketItem() { }
    //@}

    /**@name Location and Value manipulation methods, references*/
    //@{
    /// Get a const reference to the value of the bucket item
    const T& value() const { return _value; }
    /// Get a non-const reference to the value of the bucket item
    T& value() { return _value; }

    /// Get a const reference to the location of the bucket item
    const AimsVector<short,3>& location() const { return _location; }
    /// Get a non-const reference to the location of the bucket item
    AimsVector<short,3>& location() { return _location; }
    //@}

    /// 
    friend
    int operator == <> (const AimsBucketItem<T>& thing1,
                        const AimsBucketItem<T>& thing2);

    ///
    friend
    std::ostream& operator << <> (std::ostream& out, 
				  const AimsBucketItem<T>& thing);
};


template <class T> inline
int operator == (const AimsBucketItem<T>& thing1,
                 const AimsBucketItem<T>& thing2)
{ return thing1.location() == thing2.location() &&
         thing1.value()    == thing2.value();
}


template <class T> inline
std::ostream& operator << (std::ostream& out, const AimsBucketItem<T>& thing)
{ return out << "{" << thing.location() << "," << thing.value() << "}";
}


#endif










