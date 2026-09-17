#include <aims/pyramid/movingaveragesubsampler.h>
#include <aims/pyramid/convolutionsubsampler_d.h>
#include <cartodata/volume/volume.h>

using namespace carto;
using namespace std;

namespace aims {

//============================================================================
//
//                       MOVING AVERAGE SUBSAMPLER
//
//============================================================================

//----------------------------------------------------------------------------
/// Constructor / Copy
//----------------------------------------------------------------------------

MovingAverageSubSampler::MovingAverageSubSampler( unsigned r, unsigned length, FilterType::Direction symtype ):
  ConvolutionSubSampler<MovingAverageKernel>( std::vector<MovingAverageKernel>( 4, MovingAverageKernel( length, symtype ) ), r )
{}

MovingAverageSubSampler::MovingAverageSubSampler( const Point4du & r, unsigned length, FilterType::Direction symtype ):
  ConvolutionSubSampler<MovingAverageKernel>( std::vector<MovingAverageKernel>( 4, MovingAverageKernel( length, symtype ) ), r )
{}

MovingAverageSubSampler::MovingAverageSubSampler( const Point4du & r, const Point4du & length, FilterType::Direction symtype ):
  ConvolutionSubSampler<MovingAverageKernel>( std::vector<MovingAverageKernel>( 4, MovingAverageKernel( length[0], symtype ) ), r )
{
  this->_func[1].setLength( length[1] );
  this->_func[2].setLength( length[2] );
  this->_func[3].setLength( length[3] );
}

MovingAverageSubSampler::MovingAverageSubSampler( const MovingAverageSubSampler & other ):
  ConvolutionSubSampler<MovingAverageKernel>( other )
{}

MovingAverageSubSampler::~MovingAverageSubSampler()
{}

MovingAverageSubSampler & MovingAverageSubSampler::operator=( const MovingAverageSubSampler & other )
{
  if( this != &other )
  {
    ConvolutionSubSampler<MovingAverageKernel>::operator=( other );
  }
  return *this;
}


//----------------------------------------------------------------------------
/// Parameters
//----------------------------------------------------------------------------

Point4du MovingAverageSubSampler::length() const
{
  Point4du length;
  for( size_t i = 0; i < this->_func.size(); ++i )
    length[i] = this->_func[i].length();
  return length;
}

FilterType::Direction MovingAverageSubSampler::type() const
{
  return this->_func[0].type();
}

void MovingAverageSubSampler::setLength( unsigned length )
{
  for( size_t i = 0; i < this->_func.size(); ++i )
    this->_func[i].setLength( length );
}

void MovingAverageSubSampler::setLength( const Point4du & length )
{
  for( size_t i = 0; i < this->_func.size(); ++i )
    this->_func[i].setLength( length[i] );
}

void MovingAverageSubSampler::setType( FilterType::Direction symtype )
{
  for( size_t i = 0; i < this->_func.size(); ++i )
    this->_func[i].setType( symtype );
}

//============================================================================
//
//                   MOVING AVERAGE CASCADE SUBSAMPLER
//
//============================================================================

//----------------------------------------------------------------------------
/// Constructor / Copy
//----------------------------------------------------------------------------

MovingAverageCascadeSubSampler::MovingAverageCascadeSubSampler( unsigned r, unsigned length, unsigned n, FilterType::Direction symtype ):
  ConvolutionSubSampler<MovingAverageCascadeKernel>( std::vector<MovingAverageCascadeKernel>( 4, MovingAverageCascadeKernel( length, n, symtype ) ), r )
{}

MovingAverageCascadeSubSampler::MovingAverageCascadeSubSampler( const Point4du & r, unsigned length, unsigned n, FilterType::Direction symtype ):
  ConvolutionSubSampler<MovingAverageCascadeKernel>( std::vector<MovingAverageCascadeKernel>( 4, MovingAverageCascadeKernel( length, n, symtype ) ), r )
{}

MovingAverageCascadeSubSampler::MovingAverageCascadeSubSampler( const Point4du & r, const Point4du & length, unsigned n, FilterType::Direction symtype ):
  ConvolutionSubSampler<MovingAverageCascadeKernel>( std::vector<MovingAverageCascadeKernel>( 4, MovingAverageCascadeKernel( length[0], n, symtype ) ), r )
{
  this->_func[1].setLength( length[1] );
  this->_func[2].setLength( length[2] );
  this->_func[3].setLength( length[3] );
}

MovingAverageCascadeSubSampler::MovingAverageCascadeSubSampler( const Point4du & r, const Point4du & length, const Point4du & n, FilterType::Direction symtype ):
  ConvolutionSubSampler<MovingAverageCascadeKernel>( std::vector<MovingAverageCascadeKernel>( 4, MovingAverageCascadeKernel( length[0], n[0], symtype ) ), r )
{
  this->_func[1].reset( length[1], n[1], symtype );
  this->_func[2].reset( length[2], n[2], symtype );
  this->_func[3].reset( length[3], n[3], symtype );
}

MovingAverageCascadeSubSampler::MovingAverageCascadeSubSampler( const MovingAverageCascadeSubSampler & other ):
  ConvolutionSubSampler<MovingAverageCascadeKernel>( other )
{}

MovingAverageCascadeSubSampler::~MovingAverageCascadeSubSampler()
{}

MovingAverageCascadeSubSampler & MovingAverageCascadeSubSampler::operator=( const MovingAverageCascadeSubSampler & other )
{
  if( this != &other )
  {
    ConvolutionSubSampler<MovingAverageCascadeKernel>::operator=( other );
  }
  return *this;
}


//----------------------------------------------------------------------------
/// Parameters
//----------------------------------------------------------------------------

Point4du MovingAverageCascadeSubSampler::length() const
{
  Point4du length;
  for( size_t i = 0; i < this->_func.size(); ++i )
    length[i] = this->_func[i].length();
  return length;
}

Point4du MovingAverageCascadeSubSampler::cascade() const
{
  Point4du cascade;
  for( size_t i = 0; i < this->_func.size(); ++i )
    cascade[i] = this->_func[i].cascade();
  return cascade;
}

FilterType::Direction MovingAverageCascadeSubSampler::type() const
{
  return this->_func[0].type();
}

void MovingAverageCascadeSubSampler::setLength( unsigned length )
{
  for( size_t i = 0; i < this->_func.size(); ++i )
    this->_func[i].setLength( length );
}

void MovingAverageCascadeSubSampler::setLength( const Point4du & length )
{
  for( size_t i = 0; i < this->_func.size(); ++i )
    this->_func[i].setLength( length[i] );
}

void MovingAverageCascadeSubSampler::setCascade( unsigned n )
{
  for( size_t i = 0; i < this->_func.size(); ++i )
    this->_func[i].setCascade( n );
}

void MovingAverageCascadeSubSampler::setCascade( const Point4du & n )
{
  for( size_t i = 0; i < this->_func.size(); ++i )
    this->_func[i].setCascade( n[i] );
}

void MovingAverageCascadeSubSampler::setType( FilterType::Direction symtype )
{
  for( size_t i = 0; i < this->_func.size(); ++i )
    this->_func[i].setType( symtype );
}

//============================================================================
//
//                      CONVOLUTION FILTER
//
//============================================================================

template class ConvolutionSubSampler<MovingAverageKernel>;
template class ConvolutionSubSampler<MovingAverageCascadeKernel>;

#define conv_instantiate_methods_2( INP, OUTP ) \
  template VolumeRef<OUTP> ConvolutionSubSampler<MovingAverageKernel>::execute( const VolumeRef<INP> & ) const; \
  template VolumeRef<OUTP> ConvolutionSubSampler<MovingAverageKernel>::execute( const VolumeRef<INP> &, VolumeRef<OUTP> & ) const; \
  template VolumeRef<OUTP> ConvolutionSubSampler<MovingAverageCascadeKernel>::execute( const VolumeRef<INP> & ) const; \
  template VolumeRef<OUTP> ConvolutionSubSampler<MovingAverageCascadeKernel>::execute( const VolumeRef<INP> &, VolumeRef<OUTP> & ) const;

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

}
