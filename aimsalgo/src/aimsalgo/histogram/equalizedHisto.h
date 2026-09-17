
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
  // Histogram equalization from Volume
  // return the equalized image
  carto::VolumeRef<T> doit(const carto::rc_ptr<carto::Volume<T> > & thing);
};


template <class T> inline
carto::VolumeRef<T> Equalizer<T>::doit(
  const carto::rc_ptr<carto::Volume<T> > & thing)
{

  // 
  // Data 
  // 
  int min = int(thing->min()), max = int(thing->max()),
    nb_tot = thing->getSizeX() * thing->getSizeY() * thing->getSizeZ();
  float fraction;
  int tmp;
  
  carto::VolumeRef<T> res = carto::VolumeRef<T>( thing ).deepcopy();
  typename carto::Volume<T>::const_iterator it;
  
  //
  // Histogram computation
  // 
  max = max - min;
  carto::VolumeRef<int32_t> myHisto( max + 1, 1, 1, 1,
                                     carto::AllocatorContext::fast() );
  
  for(it = thing->begin(); it != thing->end(); ++it)
  {
    tmp = ( int32_t ) *it;
    ++myHisto( tmp - min );
  }
  
  carto::VolumeRef<float> cumul( max + 1, 1, 1, 1,
                                 carto::AllocatorContext::fast() );
  cumul( 0 ) = myHisto( 0 ); 
  
  carto::Volume<int32_t>::iterator it1;
  float * it2 = &*cumul.begin() + 1;
  
  for( it1 = myHisto.begin(), ++it1; it1 != myHisto.end(); ++it1, ++it2 )
      *it2 = *it1 + *( it2 - 1 );
  
  carto::Volume<float>::iterator it4;
  for (it4 = cumul.begin(); it4 != cumul.end(); ++it4 )
      *it4 /= (float) nb_tot;

  //
  // Rounded cumulated histogram
  //
  carto::VolumeRef<int32_t> roundcumul( max + 1, 1, 1, 1,
                                        carto::AllocatorContext::fast() );

  it2  = &*cumul.begin();
  for (it1  = roundcumul.begin(); it1  != roundcumul.end(); ++it1 , ++it2)
    {
      fraction = *it2  * max;
      *it1 += int32_t(fraction + 0.5);            // round by implicit cast
    }

  //
  // Equalized data computation
  // 
  typename carto::Volume<T>::iterator it3;

  for ( it3 = res.begin(); it3 != res.end(); ++it3, ++it)
      *it3 = static_cast<T> ( roundcumul( int32_t(*it3) ) - roundcumul(0) );
  
  return (res);  
}

#endif
