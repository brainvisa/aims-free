
#ifndef AIMS_HISTOGRAM_HISTOGRAM_H
#define AIMS_HISTOGRAM_HISTOGRAM_H

#include <cartodata/volume/volume.h>
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
    carto::VolumeRef<int32_t>& data() { return _data; }
    /// return a constant reference to the data field of the histogram class.
    const carto::VolumeRef<int32_t>& data() const { return _data; }
    //@}

    /** @name Iterators */
    //@{
    /// return the iterator to the first valid value of the histogram
    int32_t * beginValid()
    { return &*_data.begin(); }
    /// return the iterator to the last valid value of the histogram
    int32_t * endValid()
    { return &_data.at( _maxValid - _minValid + 1 ); }

    /// return the constant iterator to the first valid value of the histogram
    const int32_t * beginValid() const
    { return &*_data.begin(); }
    /// return the constant iterator to the last valid value of the histogram
    const int32_t * endValid() const
    { return &_data.at( _maxValid - _minValid + 1 ); }
    //@}

    /// return the minimum valid (used) value of the histogram
    int minValid() const { return _minValid; }
    /// returnn the maximum valid (used) value of the histogram
    int maxValid() const { return _maxValid; }

    /// return the total number of points in the histogram
    int totalPoints() const { return _nPoints; }

    /** the histogram computation function. \\
        This virtual function must be defined in the child classes. */
    virtual void doit( const carto::rc_ptr<carto::Volume<T> > & ) { }

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
    carto::VolumeRef<int32_t> _data;
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
  _data = other.data().deepcopy();
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
