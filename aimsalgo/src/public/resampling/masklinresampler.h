/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#ifndef AIMS_RESAMPLING_MASKLINRESAMPLER_H
#define AIMS_RESAMPLING_MASKLINRESAMPLER_H

#include <aims/resampling/motion.h>
#include <aims/resampling/resampler.h>
#include <aims/data/data.h>
#include <aims/vector/vector.h>


template <class T>
class MaskLinearResampler : public Resampler< T >
{
  public:

    MaskLinearResampler() : Resampler< T >() { };
    ~MaskLinearResampler() { }

    void doit( const Motion& motion, AimsData<T>& thing );
    AimsData<T> doit( const Motion& motion,int dimX, int dimY, int dimZ,
                      const Point3df& resolution );

  protected:

    void _sliceResamp( AimsData<T>& resamp, T* out,
		       const Point3df& start, 
                       int t, const AimsData<float>& Rinv );

    void 
    doResample( const AimsData< T > &, const Motion &, const T &, 
                const Point3df &, T &, int ) {}
};


template <class T> inline
AimsData<T> 
MaskLinearResampler<T>::doit( const Motion& motion,
			  int dimX, int dimY, int dimZ,
			  const Point3df& resolution )
{
  ASSERT( this->_ref );

  AimsData<T> thing( dimX, dimY, dimZ, this->_ref->dimT() );
  thing.setSizeXYZT( resolution[ 0 ], resolution[ 1 ], resolution[ 2 ],
                     this->_ref->sizeT() );

  doit( motion, thing );

  return thing;
}


template <class T> inline
void MaskLinearResampler<T>::doit( const Motion& motion,
				       AimsData<T>& thing )
{
  ASSERT( this->_ref );
  ASSERT( thing.dimT() == this->_ref->dimT() && thing.borderWidth() == 0 );

  Motion dirMotion = motion;
  Motion invMotion = motion.inverse();
  
  // scale motion
  Point3df sizeFrom( this->_ref->sizeX(), this->_ref->sizeY(), this->_ref->sizeZ() );
  Point3df sizeTo( thing.sizeX(), thing.sizeY(), thing.sizeZ() );
  dirMotion.scale( sizeFrom, sizeTo );
  invMotion.scale( sizeTo, sizeFrom );

  //
  Point3df start;
  typename AimsData<T>::iterator it = thing.begin() + thing.oFirstPoint();

#ifdef DEBUG
  std::cout << "Resampling volume [  1] / slice [  1]" << std::flush;
#endif
  for (int t = 1; t <= thing.dimT(); t++ )
  {
    start = invMotion.translation();
    for ( int s = 1; s <= thing.dimZ(); s++ )
    {
#ifdef DEBUG
      std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" 
           << std::setw( 3 ) << t << "] / slice ["
           << std::setw( 3 ) << s << "]" << std::flush;
#endif
      _sliceResamp( thing, it, start, t - 1, invMotion.rotation() );
      it += thing.oSlice();
      start[ 0 ] += invMotion.rotation()( 0, 2 );
      start[ 1 ] += invMotion.rotation()( 1, 2 );
      start[ 2 ] += invMotion.rotation()( 2, 2 );
    }
  }
  std::cout << std::endl;
}


template <class T> inline
void MaskLinearResampler<T>::_sliceResamp( AimsData<T>& resamp,
                                           T* out,
                                           const Point3df& start, int t,
                                           const AimsData<float>& Rinv )
{
  typename AimsData<T>::const_iterator 
    pOrig = this->_ref->begin() + this->_ref->oFirstPoint() +
            t * this->_ref->oVolume();

  int dimX1 = this->_ref->dimX();
  int dimX2 = resamp.dimX();
  int dimY2 = resamp.dimY();

  const int SIXTEEN = 16;
  const int TWO_THEN_SIXTEEN = 65536;

  int maxX = TWO_THEN_SIXTEEN * ( dimX1 - 1 );
  int maxY = TWO_THEN_SIXTEEN * ( this->_ref->dimY() - 1 );
  int maxZ = TWO_THEN_SIXTEEN * ( this->_ref->dimZ() - 1 );

  int xLinCurrent = ( int )( 65536.0 * start[ 0 ] );
  int yLinCurrent = ( int )( 65536.0 * start[ 1 ] );
  int zLinCurrent = ( int )( 65536.0 * start[ 2 ] );

  int xu = ( int )( 65536.0 * Rinv( 0, 0 ) );
  int yu = ( int )( 65536.0 * Rinv( 1, 0 ) );
  int zu = ( int )( 65536.0 * Rinv( 2, 0 ) );
  int xv = ( int )( 65536.0 * Rinv( 0, 1 ) );
  int yv = ( int )( 65536.0 * Rinv( 1, 1 ) );
  int zv = ( int )( 65536.0 * Rinv( 2, 1 ) );

  int xCurrent, yCurrent, zCurrent;
  int dx, dy, dz;
  typename AimsData<T>::const_iterator it;
  int   incr_vois[8] = {0, 1,
			dimX1+1,dimX1, 
			this->_ref->oSlice(),this->_ref->oSlice()+1,
			this->_ref->oSlice()+ dimX1+1,this->_ref->oSlice() + dimX1};
  float stock1, stock2, stock3;
  for ( int v = dimY2; v--; )
  {
    xCurrent = xLinCurrent;
    yCurrent = yLinCurrent;
    zCurrent = zLinCurrent;

    for ( int u = dimX2; u--; )
    {
      if ( xCurrent >= 0 && xCurrent < maxX &&
           yCurrent >= 0 && yCurrent < maxY &&
           zCurrent >= 0 && zCurrent < maxZ   )
      {
        dx = xCurrent & 0xffff;
        dy = yCurrent & 0xffff;
        dz = zCurrent & 0xffff;

        it = pOrig + ( zCurrent >> SIXTEEN ) * this->_ref->oSlice()
                   + ( yCurrent >> SIXTEEN ) * dimX1
                   + ( xCurrent >> SIXTEEN );

	if (*(it + incr_vois[0]) == -32768 ||
	    *(it + incr_vois[1]) == -32768 ||
	    *(it + incr_vois[2]) == -32768 ||
	    *(it + incr_vois[3]) == -32768 ||
	    *(it + incr_vois[4]) == -32768 ||
	    *(it + incr_vois[5]) == -32768 ||
	    *(it + incr_vois[6]) == -32768 ||
	    *(it + incr_vois[7]) == -32768 )
	  {
	    *out++ = -32768;
	  }
	else
	  {

	  
	    stock1 = *it * ( TWO_THEN_SIXTEEN - dx );
	    stock1 += *(++it) * dx;
	    stock1 /= TWO_THEN_SIXTEEN;

	    it += dimX1;
	    stock2 = *it * dx;
	    stock2 += *(--it) * ( TWO_THEN_SIXTEEN - dx );
	    stock2 /= TWO_THEN_SIXTEEN;

	    stock1 *= ( TWO_THEN_SIXTEEN - dy );
	    stock1 += stock2 * dy;
	    stock1 /= TWO_THEN_SIXTEEN;

	    it += this->_ref->oSlice() - dimX1;
	    stock2 = *it * ( TWO_THEN_SIXTEEN - dx );
	    stock2 += *(++it) * dx;
	    stock2 /= TWO_THEN_SIXTEEN;

	    it += dimX1;
	    stock3 = *it * dx;
	    stock3 += *(--it) * ( TWO_THEN_SIXTEEN - dx );
	    stock3 /= TWO_THEN_SIXTEEN;

	    stock2 *= TWO_THEN_SIXTEEN - dy;
	    stock2 += stock3 * dy;
	    stock2 /= TWO_THEN_SIXTEEN;

	    stock1 *= ( TWO_THEN_SIXTEEN - dz );
	    stock1 += stock2 * dz;

	    *out++ = ( T )( stock1 / TWO_THEN_SIXTEEN );
	  }
      }
      else
        *out++ = ( T ) -32768;      

      xCurrent += xu;
      yCurrent += yu;
      zCurrent += zu;
    }
    xLinCurrent += xv;
    yLinCurrent += yv;
    zLinCurrent += zv;
  }
}

#endif
