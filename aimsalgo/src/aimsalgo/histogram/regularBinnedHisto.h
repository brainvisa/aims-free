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


#ifndef AIMS_HISTOGRAM_REGULARBINNEDHISTO_H
#define AIMS_HISTOGRAM_REGULARBINNEDHISTO_H

#include <aims/histogram/histogram.h>
#include <vector>
#include <map>

namespace aims
{

  /** Histogram container class, with a specified number of regular bins.
  */
  template< typename T>
  class RegularBinnedHistogram : public Histogram<T>
  {
    public:

      RegularBinnedHistogram( unsigned bins = 0 );
      RegularBinnedHistogram( const RegularBinnedHistogram<T>& other );
      virtual ~RegularBinnedHistogram() { }

      /** classical histogram computation function.
      */
      void doit( const AimsData<T>& thing );
      // same but specify min/max to avoid seaching extrema
      void doit( const AimsData<T>& thing, T mindataval, T maxdataval );
      unsigned bins() const { return _bins; }
      void setBins( unsigned bins );
      T minDataValue() const { return _minvalue; }
      T maxDataValue() const { return _maxvalue; }
      std::vector<T> *unique( const AimsData<T>& thing ) const;

    private:
      unsigned _bins;
      T _minvalue;
      T _maxvalue;
  };


  template< typename T> inline
  RegularBinnedHistogram<T>::RegularBinnedHistogram( unsigned bins )
    : Histogram<T>(), _bins( bins ), _minvalue( 0 ), _maxvalue( 0 )
  {
  }


  template< typename T> inline
  RegularBinnedHistogram<T>::RegularBinnedHistogram(
    const RegularBinnedHistogram<T>& other )
    : Histogram< T >( other ), _bins( other._bins ),
    _minvalue( other._minvalue ), _maxvalue( other._maxvalue )
  {
  }


  template< typename T> inline
  void RegularBinnedHistogram<T>::setBins( unsigned bins )
  {
    _bins = bins;
    this->_data = AimsData<int32_t>();
  }


  template< typename T > inline
  void RegularBinnedHistogram<T>::doit( const AimsData<T>& thing )
  {
    doit( thing, thing.minimum(), thing.maximum() );
  }


  template< typename T > inline
  void RegularBinnedHistogram<T>::doit( const AimsData<T>& thing, T mini,
                                        T maxi )
  {
    _minvalue = mini;
    _maxvalue = maxi;

    if( _bins == 0 )
    {
      _bins = 256;
    }

    this->_data = AimsData<int32_t>( _bins );
    this->_data = 0;
    typename AimsData<T>::const_iterator  iv, fv=thing.end();
    double scl = (double) _bins / (double) ( maxi - mini );
    double x;
    int y;

    int iy, iz, it, nx = thing.dimX(), ny = thing.dimY(), nz = thing.dimZ(),
      nt = thing.dimT();
    for( it=0; it<nt; ++it )
      for( iz=0; iz<nz; ++iz )
        for( iy=0; iy<ny; ++iy )
          for( iv=&thing( 0, iy, iz, it ), fv=iv+nx; iv!=fv; ++iv )
          {
//             std::cout << "iv: " << iv << ", nx: " << nx << ", ny: " << ny << ", nz: " << nz << ", nt: " << nt << ", iv+1: " << &thing( 1, iy, iz, it ) << std::endl;
            x = (double) ( (double) (*iv) - mini ) * scl;
            y = (int) x;
            if( y < 0 )
            {}
            else if( y >= (int) _bins )
            {
              if( x == (int) _bins )
                ++this->_data( _bins-1 );
            }
            else
              ++this->_data( y );
          }
  }


  template< typename T > inline
  std::vector<T> *
  RegularBinnedHistogram<T>::unique( const AimsData<T>& thing ) const
  {
    std::map<T, unsigned> vals;
    typename AimsData<T>::const_iterator  iv, fv=thing.end();
    for( iv=thing.begin(); iv!=fv; ++iv )
    {
      ++vals[*iv];
    }
    std::vector<T> *res = new std::vector<T>( vals.size() );
    typename std::map<T, unsigned>::iterator im, e = vals.end();
    typename std::vector<T>::iterator i = res->begin();
    for( im=vals.begin(); im!=e; ++im, ++i )
      *i = im->first;
    return res;
  }

}

#endif
