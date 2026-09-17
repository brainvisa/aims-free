
#ifndef AIMS_HISTOGRAM_CUMULATED_H
#define AIMS_HISTOGRAM_CUMULATED_H

#include <aims/histogram/simpleHisto.h>

/** @name Cumulated histogram container class. */
template< class T >
class CumulatedHistogram : public Histogram<T>
{
  public:

    /** Constructors and destructor */
    //@{
    /// constructor
    CumulatedHistogram() : Histogram<T>() {}
    /// copy constructor
    CumulatedHistogram( const CumulatedHistogram<T>& other )
      : Histogram< T >( other ) { } 
    /// destructor
    virtual ~CumulatedHistogram() { }
    //@}

    /** @name Cumulated histogram computation */
    //@{
    /// computation from a Volume
    void doit( const carto::rc_ptr<carto::Volume<T> > & thing );
    /// computation from an already computed classical histogram
    void doit( const SimpleHistogram<T>& thing );
    //@}

    /// return the bin value for a given percentage
    int valueForPercentage( float );
};



template< class T > inline
void CumulatedHistogram<T>::doit(
  const carto::rc_ptr<carto::Volume<T> > & thing )
{
  SimpleHistogram<T> histo;
  
  histo.doit( thing );
  doit( histo );
}


template< class T > inline
void CumulatedHistogram<T>::doit( const SimpleHistogram<T>& thing )
{
  carto::VolumeRef<int32_t> res( thing.data().getSizeX(), 1, 1, 1,
                                 carto::AllocatorContext::fast() );

  this->_nPoints = thing.totalPoints();
  this->_minValid = thing.minValid();
  this->_maxValid = thing.maxValid();

  res( 0 ) = thing.data()( 0 );

  carto::Volume<int32_t>::const_iterator in;
  int32_t * out = &*res.begin() + 1;
  for ( in = thing.data().begin(), ++in; in != thing.data().end();
        ++in, ++out )
    *out = *in + *( out - 1 );

  this->_data = res;
}


template< class T > inline
int CumulatedHistogram<T>::valueForPercentage( float percent )
{
  int index = 0;
  int realValue = (int) ( this->_nPoints * percent / 100. );

  carto::Volume<int32_t>::iterator it = this->_data.begin();
  while ( *it < realValue && it != this->_data.end() ) 
  {
    index++;
    it++;
  }

  return index + this->_minValid;
}

#endif
