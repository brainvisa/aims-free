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
    /// computation from an AimsData
    void doit( const AimsData<T>& thing );
    /// computation from an already computed classical histogram
    void doit( const SimpleHistogram<T>& thing );
    //@}

    /// return the bin value for a given percentage
    int valueForPercentage( int );
};



template< class T > inline
void CumulatedHistogram<T>::doit( const AimsData<T>& thing )
{
  SimpleHistogram<T> histo;
  
  histo.doit( thing );
  doit( histo );
}


template< class T > inline
void CumulatedHistogram<T>::doit( const SimpleHistogram<T>& thing )
{
  AimsData<int32_t> res( thing.data().dimX() );

  this->_nPoints = thing.totalPoints();
  this->_minValid = thing.minValid();
  this->_maxValid = thing.maxValid();

  res( 0 ) = thing.data()( 0 );

  AimsData<int32_t>::const_iterator in;
  AimsData<int32_t>::iterator out = res.begin() + 1;
  for ( in = thing.data().begin() + 1; in != thing.data().end(); ++in, ++out )
    *out = *in + *( out - 1 );

  this->_data = res;
}


template< class T > inline
int CumulatedHistogram<T>::valueForPercentage( int percent )
{
  int index = 0;
  int realValue = this->_nPoints * percent / 100;

  AimsData<int32_t>::iterator it = this->_data.begin();
  while ( *it < realValue && it != this->_data.end() ) 
  {
    index++;
    it++;
  }

  return index + this->_minValid;
}

#endif
