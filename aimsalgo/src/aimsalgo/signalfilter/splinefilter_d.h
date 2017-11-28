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

#ifndef AIMS_SIGNALFILTER_SPLINEFILTER_D_H
#define AIMS_SIGNALFILTER_SPLINEFILTER_D_H

#include <aims/signalfilter/splinefilter.h>

//============================================================================
//
//                       DIRECT B-SPLINE FILTER
//
//============================================================================

namespace aims {

  //--------------------------------------------------------------------------
  // Execution
  //--------------------------------------------------------------------------

  template <typename OUTP, typename INP>
  carto::VolumeRef<OUTP> DirectBSplineFilter::execute(
    const carto::VolumeRef<INP> & in, carto::VolumeRef<OUTP> & out ) const
  {
    ConvolutionFilter<DiscreteBSpline>::execute( in, out );
    std::vector<int> size = in.getSize();
    double div = 1;
    for( int i = 0; i < 4; ++i )
      if( this->_dir[i] && size[i] > 1 )
        div *= ( this->_func.size() > i ? this->_func[i].scale()
                                        : this->_func[0].scale() );
    out /= div;
    return out;
  }

} // namespace aims

// Commenting this class out
// See splinefilter.h for the explanation
#if 0
//============================================================================
//
//                    INVERSE SMOOTHING B-SPLINE FILTER
//
//============================================================================

#include <exception>
#include <string>

namespace aims {

  //--------------------------------------------------------------------------
  // Constructor / Copy
  //--------------------------------------------------------------------------

  InverseSmoothingSplineFilter::InverseSmoothingSplineFilter( float lambda, unsigned n ):
    IIRFilterBase(),
    _order( n ),
    _lambda( lambda ),
    _causal(),
    _anticausal(),
    _symmetric(),
    _interpolation(n)
  {
    setFilters();
  }

  InverseSmoothingSplineFilter::InverseSmoothingSplineFilter( const InverseSmoothingSplineFilter & other ):
    IIRFilterBase( other ),
    _order( other._order ),
    _lambda( other._lambda ),
    _causal( other._causal ),
    _anticausal( other._anticausal ),
    _symmetric( other._symmetric ),
    _interpolation( other._interpolation )
  {}


  InverseSmoothingSplineFilter::~InverseSmoothingSplineFilter()
  {}

  InverseSmoothingSplineFilter & InverseSmoothingSplineFilter::operator=( const InverseSmoothingSplineFilter & other )
  {
    if( this != &other )
    {
      IIRFilterBase::operator=( other );
      _order = other._order;
      _lambda = other._lambda;
      _causal = other._causal;
      _anticausal = other._anticausal;
      _symmetric = other._symmetric;
      _interpolation = other._interpolation;
    }
    return *this;
  }

  //--------------------------------------------------------------------------
  // Parameters
  //--------------------------------------------------------------------------

  const unsigned & InverseSmoothingSplineFilter::order() const
  {
    return _order;
  }

  const float & InverseSmoothingSplineFilter::lambda() const
  {
    return _lambda;
  }

  void InverseSmoothingSplineFilter::setOrder( unsigned n )
  {
    _order = n;
    setFilters();
  }

  void InverseSmoothingSplineFilter::setLambda( float l )
  {
    _lambda = l;
    setFilters();
  }

  //--------------------------------------------------------------------------
  // Helpers
  //--------------------------------------------------------------------------

  // For details, see:
  // Unser, Aldroubi & Eden, "B-Spline Signal Processing: Part II"
  // in IEEE Transactions on Signal Processing (1993)

  void InverseSmoothingSplineFilter::setFilters()
  {
    if( _order == 3 )
    {
      if( _lambda == 0. )
      {
        _ftype = Interpolation;
      }
      else
      {

        double tmp1 = 24. * _lambda;
        double tmp2 = tmp1 * std::sqrt( 3. + 144. * _lambda );
        double ksi = 1. - 96. * _lambda + tmp2;
        double rho = ( tmp1 - 1. - std::sqrt(ksi) ) / tmp1
                     * std::sqrt( ( 48. * _lambda + tmp2 ) / ksi );
        double tan_omega = std::sqrt( ( 144. * _lambda - 1. ) / ksi );
        double cos_omega = std::sqrt( 1. / ( 1 + tan_omega * tan_omega ) );
        double b1 = 2. * rho * cos_omega;
        double b2 = - rho * rho;
        double gain = 1 - b1 - b2;
        double delta = 4. * b2 + b1 * b1;

        std::cout << "tmp1      : " << tmp1 << std::endl;
        std::cout << "tmp2      : " << tmp2 << std::endl;
        std::cout << "ksi       : " << ksi << std::endl;
        std::cout << "rho       : " << rho << std::endl;
        std::cout << "tan_omega : " << tan_omega << std::endl;
        std::cout << "cos_omega : " << cos_omega << std::endl;
        std::cout << "b1        : " << b1 << std::endl;
        std::cout << "b2        : " << b2 << std::endl;
        std::cout << "delta     : " << delta << std::endl;

        if( delta > 0 )
        {
          std::vector<double> poles;
          poles.push_back( - ( b1 - std::sqrt( delta ) ) / ( 2. * b2 ) );
          poles.push_back( - ( b1 + std::sqrt( delta ) ) / ( 2. * b2 ) );
          std::cout << "z1        : " << poles[0] << std::endl;
          std::cout << "z2        : " << poles[1] << std::endl;
          _symmetric.setPoles( poles );
          _symmetric.setGain( gain * gain );
          _ftype = SymRealPole;
        }
        else
        {
          std::vector<double> coeffs;
          coeffs.push_back( b1 );
          coeffs.push_back( b2 );
          _causal.setCoeffs( coeffs );
          _causal.setGain( gain );
          _anticausal.setCoeffs( coeffs );
          _anticausal.setGain( gain );
          _ftype = SymComplexPole;
        }
      }
    }
    else
      throw std::logic_error( std::string("InverseSmoothingSplineFilter with") +
        std::string(" n = ") + carto::toString(_order) +
        std::string(" not implemented yet.") );
  }

  void InverseSmoothingSplineFilter::filter1d( const carto::VolumeRef<double> in,
                                                  carto::VolumeRef<double> out, int dir ) const
  {
    switch( _ftype )
    {
      case Interpolation:
        _interpolation.filter1d( in, out, dir );
        break;
      case SymComplexPole:
        _symmetric.filter1d( in, out, dir );
        break;
      case SymRealPole:
        _causal.filter1d( in, out, dir );
        _anticausal.filter1d( out, out, dir );
        break;
    }
  }

} // namespace aims
#endif

#endif // AIMS_SIGNALFILTER_SPLINEFILTER_D_H
