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
  for( int i = 0; i < this->_func.size(); ++i )
    length[i] = this->_func[i].length();
  return length;
}

FilterType::Direction MovingAverageSubSampler::type() const
{
  return this->_func[0].type();
}

void MovingAverageSubSampler::setLength( unsigned length )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setLength( length );
}

void MovingAverageSubSampler::setLength( const Point4du & length )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setLength( length[i] );
}

void MovingAverageSubSampler::setType( FilterType::Direction symtype )
{
  for( int i = 0; i < this->_func.size(); ++i )
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
  for( int i = 0; i < this->_func.size(); ++i )
    length[i] = this->_func[i].length();
  return length;
}

Point4du MovingAverageCascadeSubSampler::cascade() const
{
  Point4du cascade;
  for( int i = 0; i < this->_func.size(); ++i )
    cascade[i] = this->_func[i].cascade();
  return cascade;
}

FilterType::Direction MovingAverageCascadeSubSampler::type() const
{
  return this->_func[0].type();
}

void MovingAverageCascadeSubSampler::setLength( unsigned length )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setLength( length );
}

void MovingAverageCascadeSubSampler::setLength( const Point4du & length )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setLength( length[i] );
}

void MovingAverageCascadeSubSampler::setCascade( unsigned n )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setCascade( n );
}

void MovingAverageCascadeSubSampler::setCascade( const Point4du & n )
{
  for( int i = 0; i < this->_func.size(); ++i )
    this->_func[i].setCascade( n[i] );
}

void MovingAverageCascadeSubSampler::setType( FilterType::Direction symtype )
{
  for( int i = 0; i < this->_func.size(); ++i )
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
