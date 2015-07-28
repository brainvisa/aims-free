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


#ifndef AIMS_SIGNALFILTER_MINFILTER_H
#define AIMS_SIGNALFILTER_MINFILTER_H

#include <aims/signalfilter/filteringimagealgorithm.h>
#include <aims/signalfilter/filteringfunction_element.h>
#include <aims/connectivity/structuring_element.h>

namespace aims {

  template <typename T>
  class MinFilter: public ElementFilteringImageAlgorithm<T>
  {
  public:
    typedef typename carto::DataTypeTraits<T>::ChannelType ChannelType;
    MinFilter( const StructuringElement & se = strel::Cube(1.0),
                    const carto::Object & options = carto::none() ):
      ElementFilteringImageAlgorithm<T>( MinFilterFunc<ChannelType>( options ),
                                      se )
    {}
    MinFilter( const carto::Object & options ):
      ElementFilteringImageAlgorithm<T>( MinFilterFunc<ChannelType>( options ),
                                      strel::Cube(1.0) )
    {}
    MinFilter( const MinFilter<T> & other ):
      ElementFilteringImageAlgorithm<T>( other )
    {}
    ~MinFilter() {}
    MinFilter<T> & operator=( const MinFilter<T> & other )
    {
      ElementFilteringImageAlgorithm<T>::operator=( (ElementFilteringImageAlgorithm<T>&)other );
      return *this;
    }
    MinFilter<T> *clone() const { return new MinFilter<T>(*this); }
  };

} // namespace aims

//============================================================================
//   Backward compatibility bindings
//============================================================================

#include <aims/data/data_g.h>
#include <vector>

template <typename T>
class MinSmoothing
{
  public:
    MinSmoothing( int sx = 3, int sy = 3, int sz = 3 );
    virtual ~MinSmoothing();
    virtual AimsData<T> doit( const AimsData<T>& in );
  private:
    MinSmoothing<T> & operator = ( const MinSmoothing<T> & );
    int _sx;
    int _sy;
    int _sz;
};

//----------------------------------------------------------------------------
//   DEFINITIONS
//----------------------------------------------------------------------------

template <typename T>
MinSmoothing<T>::MinSmoothing( int sx, int sy, int sz ):
  _sx(sx), _sy(sy), _sz(sz)
{}

template <typename T>
MinSmoothing<T>::~MinSmoothing()
{}

template <typename T>
MinSmoothing<T> & MinSmoothing<T>::operator= (
  const MinSmoothing<T> & other
)
{
  _sx = other._sx;
  _sy = other._sy;
  _sz = other._sz;
  return (*this);
}

template <typename T>
AimsData<T> MinSmoothing<T>::doit( const AimsData<T>& in )
{
  std::vector<double> amplitude(3,0.);
  amplitude[0] = .5 * (double)_sx;
  amplitude[1] = .5 * (double)_sy;
  amplitude[2] = .5 * (double)_sz;
  aims::strel::Cube se( amplitude, true );
  aims::MinFilter<T> f( se );
  return f.execute( in );
}


#endif
