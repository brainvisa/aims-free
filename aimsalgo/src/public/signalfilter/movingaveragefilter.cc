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

#include <aims/signalfilter/movingaveragefilter.h>
#include <aims/signalfilter/convolutionfilter_d.h>
#include <exception>

using namespace aims;
using namespace carto;
using namespace std;

namespace aims {

//============================================================================
//
//                       MOVING AVERAGE KERNEL
//
//============================================================================

//----------------------------------------------------------------------------
// Constructor/Destructor/Copy
//----------------------------------------------------------------------------

MovingAverageKernel::MovingAverageKernel( unsigned length, FilterType::Direction symtype ):
  _values( length, 1./length ),
  _support()
{
  setSupport( symtype );
}

MovingAverageKernel::MovingAverageKernel( const MovingAverageKernel & other ):
  _values( other._values ),
  _support( other._support )
{}

MovingAverageKernel::~MovingAverageKernel()
{}

MovingAverageKernel & MovingAverageKernel::operator= ( const MovingAverageKernel & other )
{
  if( this != &other )
  {
    _values = other._values;
    _support = other._support;
  }
}

//----------------------------------------------------------------------------
// Get value
//----------------------------------------------------------------------------

double MovingAverageKernel::operator() ( int x ) const
{
  return _values[ x - _support[0] ];
}

double MovingAverageKernel::at( int x ) const
{
  return (*this)( x );
}

const Point2di & MovingAverageKernel::support() const
{
  return _support;
}

//----------------------------------------------------------------------------
// Set parameters
//----------------------------------------------------------------------------

void MovingAverageKernel::reset( unsigned length, FilterType::Direction symtype )
{
  _values.assign( length, 1./length );
  setSupport( symtype );
}

void MovingAverageKernel::setLength( unsigned length )
{
  _values.assign( length, 1./length );
  setSupport( type() );
}

void MovingAverageKernel::setType( FilterType::Direction symtype )
{
  setSupport( symtype );
}

unsigned MovingAverageKernel::length() const
{
  return _values.size();
}

FilterType::Direction MovingAverageKernel::type() const
{
  return _support[1] == 0 ? FilterType::Causal :
         _support[0] == 0 ? FilterType::AntiCausal :
         FilterType::Symmetrical;
}

//----------------------------------------------------------------------------
// Helper
//----------------------------------------------------------------------------

void MovingAverageKernel::setSupport( FilterType::Direction symtype )
{
  unsigned length = _values.size();
  if( length == 0 )
    throw invalid_argument( "MovingAverageKernel: length cannot be zero." );

  switch( symtype )
  {
    case FilterType::Symmetrical:
      if( length % 2 == 0 )
        throw invalid_argument( "MovingAverageKernel: A symmetrical kernel "
                                "should be of length odd." );
      _support[0] = -int(length)/2;
      _support[1] = length/2;
      break;
    case FilterType::Causal:
      _support[0] = -int(length)+1;
      _support[1] = 0;
      break;
    case FilterType::AntiCausal:
      _support[0] = 0;
      _support[1] = length - 1;
      break;
  }
}


//============================================================================
//
//                       MOVING AVERAGE CASCADE KERNEL
//
//============================================================================

#include <exception>

//----------------------------------------------------------------------------
// Constructor/Destructor/Copy
//----------------------------------------------------------------------------

MovingAverageCascadeKernel::MovingAverageCascadeKernel( unsigned length, unsigned n, FilterType::Direction symtype ):
  _values(length),
  _support(),
  _n(n)
{
  setValues( length, n, symtype );
}

MovingAverageCascadeKernel::MovingAverageCascadeKernel( const MovingAverageCascadeKernel & other ):
  _values( other._values ),
  _support( other._support ),
  _n( other._n )
{}

MovingAverageCascadeKernel::~MovingAverageCascadeKernel()
{}

MovingAverageCascadeKernel & MovingAverageCascadeKernel::operator= ( const MovingAverageCascadeKernel & other )
{
  if( this != &other )
  {
    _values = other._values;
    _support = other._support;
    _n = other._n;
  }
}

//----------------------------------------------------------------------------
// Get value
//----------------------------------------------------------------------------

double MovingAverageCascadeKernel::operator() ( int x ) const
{
  return _values[ x - _support[0] ];
}

double MovingAverageCascadeKernel::at( int x ) const
{
  return (*this)( x );
}

const Point2di & MovingAverageCascadeKernel::support() const
{
  return _support;
}

//----------------------------------------------------------------------------
// Set parameters
//----------------------------------------------------------------------------

void MovingAverageCascadeKernel::reset( unsigned length, unsigned n, FilterType::Direction symtype )
{
  setValues( length, n, symtype );
}

void MovingAverageCascadeKernel::setLength( unsigned length )
{
  setValues( length, cascade(), type() );
}

void MovingAverageCascadeKernel::setCascade( unsigned n )
{
  setValues( length(), n, type() );
}

void MovingAverageCascadeKernel::setType( FilterType::Direction symtype )
{
  setValues( length(), cascade(), symtype );
}

unsigned MovingAverageCascadeKernel::length() const
{
  return ( _values.size() - 1 ) / _n + 1;
}

unsigned MovingAverageCascadeKernel::cascade() const
{
  return _n;
}

FilterType::Direction MovingAverageCascadeKernel::type() const
{
  return _support[1] == 0 ? FilterType::Causal :
         _support[0] == 0 ? FilterType::AntiCausal :
         FilterType::Symmetrical;
}

//----------------------------------------------------------------------------
// Helper
//----------------------------------------------------------------------------

void MovingAverageCascadeKernel::setValues( unsigned length, unsigned n, FilterType::Direction symtype )
{
  if( length == 0 )
    throw invalid_argument( "MovingAverageCascadeKernel: length cannot be zero." );

  _n = n;
  unsigned size = n * (length - 1) + 1;

  _values.assign( size, 0. );
  _values.back() = 1.;
  std::vector<double> prev = _values;
  for( int i = 0; i < n; ++i )
  {
    _values.assign( size, 0. );
    for( int j = 0; j < size; ++j )
      for( int k = 0; k < length; ++k )
        _values[j] += ( j+k < size ? prev[j+k] / length : 0. );
  }

  switch( symtype )
  {
    case FilterType::Symmetrical:
      if( size % 2 == 0 )
        throw invalid_argument( "MovingAverageCascadeKernel: A symmetrical kernel "
                                "should be of length odd or of cascade even." );
      _support[0] = -int(size - 1) / 2;
      _support[1] = (size - 1) / 2;
      break;
    case FilterType::Causal:
      _support[0] = -int(size)+1;
      _support[1] = 0;
      break;
    case FilterType::AntiCausal:
      _support[0] = 0;
      _support[1] = size - 1;
      break;
  }
}


//============================================================================
//
//                       MOVING AVERAGE FILTER
//
//============================================================================

//----------------------------------------------------------------------------
// Constructor / Copy
//----------------------------------------------------------------------------

MovingAverageFilter::MovingAverageFilter( unsigned length, FilterType::Direction symtype ):
  ConvolutionFilter<MovingAverageKernel>( std::vector<MovingAverageKernel>( 4, MovingAverageKernel( length, symtype ) ) )
{}

MovingAverageFilter::MovingAverageFilter( const Point4du & length, FilterType::Direction symtype ):
  ConvolutionFilter<MovingAverageKernel>( std::vector<MovingAverageKernel>( 4, MovingAverageKernel( length[0], symtype ) ) )
{
  this->_func[1].setLength( length[1] );
  this->_func[2].setLength( length[2] );
  this->_func[3].setLength( length[3] );
}

MovingAverageFilter::MovingAverageFilter( const MovingAverageFilter & other ):
  ConvolutionFilter<MovingAverageKernel>( other )
{}

MovingAverageFilter::~MovingAverageFilter()
{}

MovingAverageFilter & MovingAverageFilter::operator=( const MovingAverageFilter & other )
{
  if( this != &other )
  {
    ConvolutionFilter<MovingAverageKernel>::operator=( other );
  }
  return *this;
}

//----------------------------------------------------------------------------
// Set parameters
//----------------------------------------------------------------------------

void MovingAverageFilter::setLength( unsigned length )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setLength( length );
}

void MovingAverageFilter::setLength( const Point4du & length )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setLength( length[i] );
}

void MovingAverageFilter::setType( FilterType::Direction symtype )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setType( symtype );
}

//----------------------------------------------------------------------------
// Get parameters
//----------------------------------------------------------------------------

Point4du MovingAverageFilter::length() const
{
  Point4du length;
  for( int i = 0; i < this->_func.size(); ++i )
    length[i] = this->_func[i].length();
  return length;
}

FilterType::Direction MovingAverageFilter::type() const
{
  return this->_func[0].type();
}

//============================================================================
//
//                       MOVING AVERAGE CASCADE FILTER
//
//============================================================================

//----------------------------------------------------------------------------
// Constructor / Copy
//----------------------------------------------------------------------------

MovingAverageCascadeFilter::MovingAverageCascadeFilter( unsigned length, unsigned n, FilterType::Direction symtype ):
  ConvolutionFilter<MovingAverageCascadeKernel>( std::vector<MovingAverageCascadeKernel>( 4, MovingAverageCascadeKernel( length, n, symtype ) ) )
{}

MovingAverageCascadeFilter::MovingAverageCascadeFilter( const Point4du & length, unsigned n, FilterType::Direction symtype ):
  ConvolutionFilter<MovingAverageCascadeKernel>( std::vector<MovingAverageCascadeKernel>( 4, MovingAverageCascadeKernel( length[0], n, symtype ) ) )
{
  this->_func[1].setLength( length[1] );
  this->_func[2].setLength( length[2] );
  this->_func[3].setLength( length[3] );
}


MovingAverageCascadeFilter::MovingAverageCascadeFilter( const Point4du & length, const Point4du & n, FilterType::Direction symtype ):
  ConvolutionFilter<MovingAverageCascadeKernel>( std::vector<MovingAverageCascadeKernel>( 4, MovingAverageCascadeKernel( length[0], n[0], symtype ) ) )
{
  this->_func[1].reset( length[1], n[1], symtype );
  this->_func[2].reset( length[2], n[2], symtype );
  this->_func[3].reset( length[3], n[3], symtype );
}

MovingAverageCascadeFilter::MovingAverageCascadeFilter( const MovingAverageCascadeFilter & other ):
  ConvolutionFilter<MovingAverageCascadeKernel>( other )
{}

MovingAverageCascadeFilter::~MovingAverageCascadeFilter()
{}

MovingAverageCascadeFilter & MovingAverageCascadeFilter::operator=( const MovingAverageCascadeFilter & other )
{
  if( this != &other )
  {
    ConvolutionFilter<MovingAverageCascadeKernel>::operator=( other );
  }
  return *this;
}

//----------------------------------------------------------------------------
// Set parameters
//----------------------------------------------------------------------------

void MovingAverageCascadeFilter::setLength( unsigned length )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setLength( length );
}

void MovingAverageCascadeFilter::setLength( const Point4du & length )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setLength( length[i] );
}

void MovingAverageCascadeFilter::setCascade( unsigned n )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setCascade( n );
}

void MovingAverageCascadeFilter::setCascade( const Point4du & n )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setCascade( n[i] );
}

void MovingAverageCascadeFilter::setType( FilterType::Direction symtype )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setType( symtype );
}

//----------------------------------------------------------------------------
// Get parameters
//----------------------------------------------------------------------------

Point4du MovingAverageCascadeFilter::length() const
{
  Point4du length;
  for( int i = 0; i < this->_func.size(); ++i )
    length[i] = this->_func[i].length();
  return length;
}

Point4du MovingAverageCascadeFilter::cascade() const
{
  Point4du cascade;
  for( int i = 0; i < this->_func.size(); ++i )
    cascade[i] = this->_func[i].cascade();
  return cascade;
}

FilterType::Direction MovingAverageCascadeFilter::type() const
{
  return this->_func[0].type();
}

//============================================================================
//
//                      CONVOLUTION FILTER
//
//============================================================================

template class ConvolutionFilter<MovingAverageKernel>;
template class ConvolutionFilter<MovingAverageCascadeKernel>;

#define conv_instantiate_methods_2( INP, OUTP ) \
  template VolumeRef<OUTP> ConvolutionFilter<MovingAverageKernel>::execute( const VolumeRef<INP> & ) const; \
  template VolumeRef<OUTP> ConvolutionFilter<MovingAverageKernel>::execute( const VolumeRef<INP> &, VolumeRef<OUTP> & ) const; \
  template VolumeRef<OUTP> ConvolutionFilter<MovingAverageCascadeKernel>::execute( const VolumeRef<INP> & ) const; \
  template VolumeRef<OUTP> ConvolutionFilter<MovingAverageCascadeKernel>::execute( const VolumeRef<INP> &, VolumeRef<OUTP> & ) const;

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

} // namespace aims
