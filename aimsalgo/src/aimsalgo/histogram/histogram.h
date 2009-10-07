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


#ifndef AIMS_HISTOGRAM_HISTOGRAM_H
#define AIMS_HISTOGRAM_HISTOGRAM_H

#include <aims/data/data.h>
#include <fstream>


template < class T > class Histogram;

template < class T > 
std::ostream& operator << ( std::ostream& os, const Histogram<T>& thing );

/** Base class of histogram container class. */
template < class T>
class Histogram
{
  public:

    /** @name Constructors and destructor. */
    //@{
    /// constructor.
    Histogram();
    /// copy constructor.
    Histogram( const Histogram<T>& other );
    /// destructor.
    virtual ~Histogram() { }
    //@}

    /** @name Data of histogram */
    //@{
    /// return a reference to the data field of the histogram class.
    AimsData<int32_t>& data() { return _data; }
    /// return a constant reference to the data field of the histogram class.
    const AimsData<int32_t>& data() const { return _data; }
    //@}

    /** @name Iterators */
    //@{
    /// return the iterator to the first valid value of the histogram
    AimsData<int32_t>::iterator beginValid() 
    { return _data.begin(); }
    /// return the iterator to the last valid value of the histogram
    AimsData<int32_t>::iterator endValid()
    { return _data.begin() + _maxValid - _minValid + 1; }

    /// return the constant iterator to the first valid value of the histogram
    AimsData<int32_t>::const_iterator beginValid() const 
    { return _data.begin(); }
    /// return the constant iterator to the last valid value of the histogram
    AimsData<int32_t>::const_iterator endValid() const 
    { return _data.begin() + _maxValid - _minValid + 1; }
    //@}

    /// return the minimum valid (used) value of the histogram
    int minValid() const { return _minValid; }
    /// returnn the maximum valid (used) value of the histogram
    int maxValid() const { return _maxValid; }

    /// return the total number of points in the histogram
    int totalPoints() const { return _nPoints; }

    /** the histogram computation function. \\
        This virtual function must be defined in the child classes. */
    virtual void doit( const AimsData<T>& ) { }

    /** @name Output stream */
    //@{
    /// 
    friend std::ostream& operator << <> ( std::ostream& os, 
					  const Histogram<T>& thing );
    //@}

  protected:

    /// total number of points
    int _nPoints;
    /// minimum used value
    int _minValid;
    /// maximum used value
    int _maxValid;

    /// histogram datas
    AimsData<int32_t> _data;
};


template< class T > inline
Histogram<T>::Histogram() 
             : _nPoints( 0 ), _minValid( 0 ), _maxValid( 0 )
{
}


template< class T > inline
Histogram<T>::Histogram( const Histogram<T>& other )
             : _nPoints( other._nPoints ), _minValid( other._minValid ), 
               _maxValid( other._maxValid )
{
  _data = other.data().clone();
}


template < class T > inline
std::ostream& operator << ( std::ostream& os, const Histogram<T>& thing )
{
  os << "{nPoints=" << thing._nPoints << ",";
  os << "minValid=" << thing._minValid << ",";
  os << "maxValid=" << thing._maxValid << ",";
  os << "data=" << thing._data << "}";

  return os;
}

#endif
