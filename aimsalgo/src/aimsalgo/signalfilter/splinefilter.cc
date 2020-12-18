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

#include <aims/signalfilter/splinefilter_d.h>
#include <aims/signalfilter/convolutionfilter_d.h>
#include <vector>
#include <exception>
#include <string>

using namespace carto;

namespace aims {

  //==========================================================================
  //
  //                       INVERSE B-SPLINE FILTER
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  // Constructor / Copy
  //--------------------------------------------------------------------------

  InverseBSplineFilter::InverseBSplineFilter( unsigned n ):
    SymAllPoleIIRFilter(),
    _order( n )
  {
    setPoles();
  }

  InverseBSplineFilter::InverseBSplineFilter( const InverseBSplineFilter & other ):
    SymAllPoleIIRFilter( other ),
    _order( other._order )
  {}


  InverseBSplineFilter::~InverseBSplineFilter()
  {}

  InverseBSplineFilter & InverseBSplineFilter::operator=( const InverseBSplineFilter & other )
  {
    if( this != &other)
    {
      SymAllPoleIIRFilter::operator=( other );
      _order = other._order;
    }
    return *this;
  }

  //--------------------------------------------------------------------------
  // Parameters
  //--------------------------------------------------------------------------

  const unsigned & InverseBSplineFilter::order() const
  {
    return _order;
  }

  void InverseBSplineFilter::setOrder( const unsigned & n )
  {
    _order = n;
    setPoles();
  }

  //--------------------------------------------------------------------------
  // Helpers
  //--------------------------------------------------------------------------

  void InverseBSplineFilter::setPoles()
  {
    _poles.clear();

    // poles were computed from Unser & al polynomial equations with
    // numpy.roots, with a 15 digit precision.
    // (numpy float64 have a 52 bit mantissa precision)

    if( _order == 0 )
    {
      _gain = 1.;
      _precise = false;
    }
    else if( _order == 1 )
    {
      _gain = 1.;
      _precise = false;
    }
    else if( _order == 2 )
    {
      _poles.push_back(-1.7157287525380971E-1);
      _gain = 8.;
      _precise = false;
    }
    else if( _order == 3 )
    {
      _poles.push_back(-2.6794919243112281E-1);
      _gain = 6.;
      _precise = false;
    }
    else if( _order == 4 )
    {
      _poles.push_back(-3.613412259002202E-1);
      _poles.push_back(-1.372542929733912E-2);
      _gain = 384.;
      _precise = false;
    }
    else if( _order == 5 )
    {
      _poles.push_back(-4.30545347099974E-1);
      _poles.push_back(-4.3096288203265E-2);
      _gain = 120.;
      _precise = false;
    }
    else if( _order == 6 )
    {
      _poles.push_back(-0.488294589303044755130118038883789062112279161239377608394);
      _poles.push_back(-0.081679271076237512597937765737059080653379610398148178525368);
      _poles.push_back(-0.00141415180832581775108724397655859252786416905534669851652709);
      // _gain = 46080.;
      _gain = 2.598975999348577818390170516255374207847876853191217652822;
      _precise = true;
    }
    else if( _order == 7 )
    {
      _poles.push_back(-0.5352804307964381655424037816816460718339231523426924148812);
      _poles.push_back(-0.122554615192326690515272264359357343605486549427295558490763);
      _poles.push_back(-0.0091486948096082769285930216516478534156925639545994482648003);
      // _gain = 5040.;
      _gain = 3.0248282036441843886795463832305782146916878615537002580987;
      _precise = true;
    }
    else
      throw std::logic_error( std::string("InverseBSplineFilter with") +
        std::string(" n = ") + carto::toString(_order) +
        std::string(" not implemented yet.") );

    this->computeK0();
    this->computePreciseGain();
  }

  //==========================================================================
  //
  //                      CONVOLUTION FILTER
  //
  //==========================================================================

  template class ConvolutionFilter<DiscreteBSpline>;

#define conv_instantiate_methods_2( INP, OUTP ) \
  template VolumeRef<OUTP> ConvolutionFilter<DiscreteBSpline>::execute( const VolumeRef<INP> & ) const;

#define conv_instantiate_methods( T ) \
  conv_instantiate_methods_2( T, uint8_t )  \
  conv_instantiate_methods_2( T, int8_t )   \
  conv_instantiate_methods_2( T, uint16_t ) \
  conv_instantiate_methods_2( T, int16_t )  \
  conv_instantiate_methods_2( T, uint32_t ) \
  conv_instantiate_methods_2( T, int32_t )  \
  conv_instantiate_methods_2( T, uint64_t ) \
  conv_instantiate_methods_2( T, int64_t )  \
  conv_instantiate_methods_2( T, float )    \
  conv_instantiate_methods_2( T, double )

conv_instantiate_methods( uint8_t )
conv_instantiate_methods( int8_t )
conv_instantiate_methods( uint16_t )
conv_instantiate_methods( int16_t )
conv_instantiate_methods( uint32_t )
conv_instantiate_methods( int32_t )
conv_instantiate_methods( uint64_t )
conv_instantiate_methods( int64_t )
conv_instantiate_methods( float )
conv_instantiate_methods( double )

  //==========================================================================
  //
  //                      DIRECT B-SPLINE FILTER
  //
  //==========================================================================

  //--------------------------------------------------------------------------
  // Constructor / Copy
  //--------------------------------------------------------------------------

  DirectBSplineFilter::DirectBSplineFilter( float scale, bool normalize,
                                            unsigned n ):
    ConvolutionFilter<DiscreteBSpline>( std::vector<DiscreteBSpline>( 1,
                                  DiscreteBSpline( n, scale )
                              ) ),
    _normalize( normalize )
  {}

  DirectBSplineFilter::DirectBSplineFilter( const Point4df & scale, bool normalize,
                                            unsigned n ):
    ConvolutionFilter<DiscreteBSpline>(),
    _normalize( normalize )
  {
    this->_func.reserve(4);
    this->_func.push_back( DiscreteBSpline( n, scale[0] ) );
    this->_func.push_back( DiscreteBSpline( n, scale[1] ) );
    this->_func.push_back( DiscreteBSpline( n, scale[2] ) );
    this->_func.push_back( DiscreteBSpline( n, scale[3] ) );
  }

  DirectBSplineFilter::DirectBSplineFilter( const DirectBSplineFilter & other ):
    ConvolutionFilter<DiscreteBSpline>( other ),
    _normalize( other._normalize )
  {}

  DirectBSplineFilter::~DirectBSplineFilter()
  {}

  DirectBSplineFilter & DirectBSplineFilter::operator=( const DirectBSplineFilter & other )
  {
    if( this != &other )
    {
      ConvolutionFilter<DiscreteBSpline>::operator=( other );
      _normalize = other._normalize;
    }
    return *this;
  }


  //------------------------------------------------------------------------
  // Parameters
  //------------------------------------------------------------------------

  unsigned DirectBSplineFilter::order() const
  {
    return this->_func[0].order();
  }

  Point4df DirectBSplineFilter::scale() const
  {
    Point4df scale( this->_func[0].scale(), this->_func[0].scale(),
                    this->_func[0].scale(), this->_func[0].scale() );
    if( this->_func.size() > 1 )
      scale[1] = this->_func[1].scale();
    if( this->_func.size() > 2 )
      scale[2] = this->_func[2].scale();
    if( this->_func.size() > 3 )
      scale[3] = this->_func[3].scale();
    return scale;
  }

  bool DirectBSplineFilter::normalize() const
  {
    return _normalize;
  }

  void DirectBSplineFilter::setOrder( const unsigned & n )
  {
    for( size_t i = 0; i < this->_func.size(); ++i )
      this->_func[i].setOrder( n );
  }

  void DirectBSplineFilter::setScale( const float & scale )
  {
    this->_func.assign( 1, this->_func[0] );
    this->_func[0].setScale( scale );
  }

  void DirectBSplineFilter::setScale( const Point4df & scale )
  {
    this->_func.reserve(4);
    for( size_t i = 0; i < this->_func.size(); ++i )
      this->_func[i].setScale( scale[i] );
    for( int i = this->_func.size(); i < 4; ++i )
    {
      this->_func.push_back( this->_func[0] );
      this->_func[i].setScale( scale[i] );
    }
  }

  void DirectBSplineFilter::setNormalize( const bool & normalize )
  {
    _normalize = normalize;
  }

  //--------------------------------------------------------------------------
  // Instantiation
  //--------------------------------------------------------------------------

#define dir_instantiate_methods_2( INP, OUTP ) \
  template VolumeRef<OUTP> DirectBSplineFilter::execute( const VolumeRef<INP> &, VolumeRef<OUTP> & ) const;

#define dir_instantiate_methods( T ) \
  dir_instantiate_methods_2( T, uint8_t )  \
  dir_instantiate_methods_2( T, int8_t )   \
  dir_instantiate_methods_2( T, uint16_t ) \
  dir_instantiate_methods_2( T, int16_t )  \
  dir_instantiate_methods_2( T, uint32_t ) \
  dir_instantiate_methods_2( T, int32_t )  \
  dir_instantiate_methods_2( T, uint64_t ) \
  dir_instantiate_methods_2( T, int64_t )  \
  dir_instantiate_methods_2( T, float )    \
  dir_instantiate_methods_2( T, double )

dir_instantiate_methods( uint8_t )
dir_instantiate_methods( int8_t )
dir_instantiate_methods( uint16_t )
dir_instantiate_methods( int16_t )
dir_instantiate_methods( uint32_t )
dir_instantiate_methods( int32_t )
dir_instantiate_methods( uint64_t )
dir_instantiate_methods( int64_t )
dir_instantiate_methods( float )
dir_instantiate_methods( double )

}
