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


#ifndef AIMS_HISTOGRAM_SIMPLEHISTO_H
#define AIMS_HISTOGRAM_SIMPLEHISTO_H

#include <aims/histogram/histogram.h>
#include <cartobase/containers/nditerator.h>

/** Classical histogram container class.
 */
template< class T>
class SimpleHistogram : public Histogram<T>
{
  public:

    /** @name Constructors and destructor */
    //@{
    /// constructor. Does nothing.
    SimpleHistogram() : Histogram<T>() { }
    /// copy constructor.
    SimpleHistogram( const SimpleHistogram<T>& other )
      : Histogram< T >( other ) { }
    /// destructor.
    virtual ~SimpleHistogram() { }
    //@}

    /** classical histogram computation function.\\
        Be careful that for float and double data types the histogram is
        not set in a float or double 1D Volume but in a 16 bits short
        1D Volume, i.e. a rebinning is performed.
    */
    void doit( const carto::rc_ptr<carto::Volume<T> > & thing );

    /** @name Rebinning functions. */
    //@{
    /** rebinning to a specific result vector size.*/
    void rebin( int size );
    /** rebinning from the beginning iterator to the ending one.
        The result vector size is then defined by the number of bins
        included between the two iterators. */
    void rebin( carto::Volume<int32_t>::iterator beg, carto::Volume<int32_t>::iterator end );
    /** rebinning from iterator 'beg' to iterator 'end' to a result
        vector of size 'size' */
    void rebin( int size, carto::Volume<int32_t>::iterator beg,
                carto::Volume<int32_t>::iterator end );
    //@}
};


template< class T > inline
void SimpleHistogram<T>::doit( const carto::rc_ptr<carto::Volume<T> > & thing )
{
  carto::VolumeRef<int32_t> res( 1 << 8 * sizeof( T ) );

  this->_minValid = (int)thing->min();
  this->_maxValid = (int)thing->max();

  this->_nPoints = thing->getSizeX() * thing->getSizeY() * thing->getSizeZ()
    * thing->getSizeT();

  const T *p, *pp;
  carto::const_line_NDIterator<T> it( &thing->at( 0 ), thing->getSize(),
                                      thing->getStrides(), true );
  for( ; !it.ended(); ++it )
  {
    p = &*it;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ) )
      res->at( (int)*p - this->_minValid )++;
  }

  this->_data = res;
}


template< class T > inline
void SimpleHistogram<T>::rebin( int size )
{
  rebin( size, this->_data.begin() , this->_data.end() );
}


template< class T > inline
void SimpleHistogram<T>::rebin( typename carto::Volume<int32_t>::iterator beg,
                                typename carto::Volume<int32_t>::iterator end )
{
  rebin( &*end - &*beg, beg, end );
}


template< class T > inline
void SimpleHistogram<T>::rebin( int size, carto::Volume<int32_t>::iterator beg,
                                carto::Volume<int32_t>::iterator end )
{
  int i, sum, length = &*end - &*beg;

  ASSERT( size <= length );

  int coef = (int)ceil( (double)length / (double)size );
  carto::VolumeRef<int32_t> res( size );

  this->_nPoints = 0;

  carto::Volume<int32_t>::iterator it2, it = beg;
  for ( it2=res.begin(); it2!=res.end(); )
  {
    sum = 0;

    for ( i = coef; i-- && it != end; )
      sum += *it++;

    *it2++ = sum;
    this->_nPoints += sum;
  }

  int tmpMin = this->_minValid;

  this->_minValid = this->_maxValid = tmpMin /coef;

  for ( it2 = res.begin(), i=0; *it2 == 0 && it2 != res.end(); it2++, ++i )
    this->_minValid = i + tmpMin / coef;

  for ( ; it2 != res.end(); it2++, ++i )
    if (*it2)
      this->_maxValid = i + tmpMin / coef;

  this->_data = res;
}


template <>
inline
void SimpleHistogram<float>::doit(
  const carto::rc_ptr<carto::Volume<float> > & thing )
{
  int size = 65536;
  carto::VolumeRef<int32_t> res( size );

  float minh = thing->min();
  float maxh = thing->max();
  float div = maxh - minh;

  if ( ( div < (float)size ) && ( div > 1.0f ) )  
  {
    div = (float)size;
    this->_minValid = (int)minh;
    this->_maxValid = (int)maxh;
  }
  else
  {
    this->_minValid = 0;
    this->_maxValid = size - 1;
  }

  this->_nPoints = thing->getSizeX() * thing->getSizeY() * thing->getSizeZ()
    * thing->getSizeT();

  const float *p, *pp;
  carto::const_line_NDIterator<float> it( &thing->at( 0 ), thing->getSize(),
                                          thing->getStrides(), true );
  for( ; !it.ended(); ++it )
  {
    p = &*it;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ) )
    {
      int x = (int)( (float) size * ( *p - minh ) / div );
      if( x < 0 )
        x = 0;
      else if( x >= size )
        x = size - 1;
      res.at( x )++;
    }
  }

  this->_data = res;
}


template <>
inline
void SimpleHistogram<double>::doit(
  const carto::rc_ptr<carto::Volume<double> > & thing )
{
  int size = 65536;
  carto::VolumeRef<int32_t> res( size );

  double minh = thing->min();
  double maxh = thing->max();
  double div = maxh - minh;

  if ( ( div < (float)size )  && ( div > 1.0f ) )
  {
    div = (double)size;
    this->_minValid = (int)minh;
    this->_maxValid = (int)maxh;
  }
  else
  {
    this->_minValid = 0;
    this->_maxValid = size - 1;
  }

  this->_nPoints = thing->getSizeX() * thing->getSizeY() * thing->getSizeZ()
    * thing->getSizeT();

  const double *p, *pp;
  carto::const_line_NDIterator<double> it( &thing->at( 0 ), thing->getSize(),
                                           thing->getStrides(), true );
  for( ; !it.ended(); ++it )
  {
    p = &*it;
    for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ) )
      res.at( (int)( (double)size * ( *p - minh ) / div ) )++;
  }

  this->_data = res;
}

#endif
