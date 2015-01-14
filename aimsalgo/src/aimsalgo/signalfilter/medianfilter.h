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


#ifndef AIMS_SIGNALFILTER_MEDIANFILTER_H
#define AIMS_SIGNALFILTER_MEDIANFILTER_H

#include <aims/signalfilter/filteringimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/connectivity/structuring_element.h>

namespace aims {

  template <typename T>
  class MedianFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    MedianFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( MedianFilterFunc<ChannelType>( options ),
                                      se )
    {}
    MedianFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( MedianFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    MedianFilter( const MedianFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~MedianFilter() {}
    MedianFilter<T> & operator=( const MedianFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    MedianFilter<T> *clone() { return new MedianFilter<T>(*this); }
  };

  template <typename T>
  class NotNullMedianFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    NotNullMedianFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( NotNullMedianFilterFunc<ChannelType>( options ),
                                      se )
    {}
    NotNullMedianFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( NotNullMedianFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    NotNullMedianFilter<T>( const NotNullMedianFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~NotNullMedianFilter() {}
    NotNullMedianFilter<T> & operator=( const NotNullMedianFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    NotNullMedianFilter<T> *clone() { return new NotNullMedianFilter<T>(*this); }
  };

} // namespace aims

//============================================================================
//   Backward compatibility bindings
//============================================================================

#include <aims/data/data_g.h>
#include <vector>

template <typename T>
class MedianSmoothing
{
  public:
    MedianSmoothing( int sx = 3, int sy = 3, int sz = 3 );
    virtual ~MedianSmoothing();
    virtual AimsData<T> doit( const AimsData<T>& in ) const;
  private:
    MedianSmoothing<T> & operator = ( const MedianSmoothing<T> & );
    int _sx;
    int _sy;
    int _sz;
};

//----------------------------------------------------------------------------
//   DEFINITIONS
//----------------------------------------------------------------------------

template <typename T>
MedianSmoothing<T>::MedianSmoothing( int sx, int sy, int sz ):
  _sx(sx), _sy(sy), _sz(sz)
{}

template <typename T>
MedianSmoothing<T>::~MedianSmoothing()
{}

template <typename T>
MedianSmoothing<T> & MedianSmoothing<T>::operator= (
  const MedianSmoothing<T> & other
)
{
  _sx = other._sx;
  _sy = other._sy;
  _sz = other._sz;
  return (*this);
}

template <typename T>
AimsData<T> MedianSmoothing<T>::doit( const AimsData<T>& in ) const
{
  std::vector<double> amplitude(3,0.);
  amplitude[0] = .5 * (double)_sx;
  amplitude[1] = .5 * (double)_sy;
  amplitude[2] = .5 * (double)_sz;
  aims::strel::Cube se( amplitude, true );
  aims::MedianFilter<T> f( se );
  return f.execute( in );
}


#endif
