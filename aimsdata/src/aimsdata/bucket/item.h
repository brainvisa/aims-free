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










