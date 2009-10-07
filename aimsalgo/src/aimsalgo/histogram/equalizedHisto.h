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


#ifndef AIMS_HISTOGRAM_EQUALIZEDHISTO_H
#define AIMS_HISTOGRAM_EQUALIZEDHISTO_H

#include <aims/histogram/simpleHisto.h>

/** Classical histogram container class.
 */
template< class T>
class Equalizer
{ 
 public:
  
  /** @name Constructors and destructor */
  //@{
  /// constructor. Does nothing.
  Equalizer() {};
  /// destructor.
  virtual ~Equalizer() { }
  //@}
  
  //
  /**@name Methods */
  //
  // Histogram equalization from AimsData
  // return the equalized image
  AimsData<T> doit(const AimsData<T>& thing);
};


template <class T> inline
AimsData<T> Equalizer<T>::doit(const AimsData<T>& thing)
{

  // 
  // Data 
  // 
  int min = int(thing.minimum()), max = int(thing.maximum()), nb_tot = thing.dimX() * thing.dimY() * thing.dimZ(); 
  float fraction;
  int tmp;
  
  AimsData<T> res = thing.clone();
  typename AimsData<T>::const_iterator it;
  
  //
  // Histogram computation
  // 
  max = max - min;
  AimsData<int32_t> myHisto( max + 1 );      
  
  for(it = thing.begin(); it != thing.end(); ++it)
    {
      tmp = ( int32_t ) *it;
      ++myHisto( tmp - min );
    }  
  
  AimsData<float> cumul( max + 1 );
  cumul( 0 ) = myHisto( 0 ); 
  
  AimsData<int32_t>::iterator it1;
  AimsData<float>::iterator it2 = cumul.begin() + 1;
  
  for ( it1 = myHisto.begin() + 1; it1 != myHisto.end(); ++it1, ++it2 )
      *it2 = *it1 + *( it2 - 1 );
  
  for (it2 = cumul.begin(); it2 != cumul.end(); ++it2 )
      *it2 /= (float) nb_tot;

  //
  // Rounded cumulated histogram
  //
  AimsData<int32_t> roundcumul( max + 1 );

  it2  = cumul.begin();
  for (it1  = roundcumul.begin(); it1  != roundcumul.end(); ++it1 , ++it2)
    {
      fraction = *it2  * max;
      *it1 += int32_t(fraction + 0.5);            // round by implicit cast
    }

  //
  // Equalized data computation
  // 
  typename AimsData<T>::iterator it3;

  for ( it3 = res.begin(); it3 != res.end(); ++it3, ++it)
      *it3 = static_cast<T> ( roundcumul( int32_t(*it3) ) - roundcumul(0) );
  
  return (res);  
}

#endif
