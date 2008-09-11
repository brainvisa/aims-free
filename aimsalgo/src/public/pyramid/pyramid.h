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


#ifndef AIMS_PYRAMID_PYRAMID_H
#define AIMS_PYRAMID_PYRAMID_H

#include <aims/def/assert.h>
#include <aims/data/data_g.h>
#include <aims/io/writer.h>
#include <aims/pyramid/pyr-func.h>


//
// class Pyramid
//
template <class T>
class Pyramid
{
  public:

    Pyramid( const PyramidFunc<T>& func ) 
           : _func( func ), _ref( 0 ), _level( 0 ), _ppItem ( 0 ) { }
    virtual ~Pyramid() { _free_ppItem(); }

    void setRef( AimsData<T>& ref );
    void setLevel( int level );
    const AimsData<T>& item( int level ) const;
    void save( const std::string& name ) const;

  protected:

    const PyramidFunc<T>& _func;
    AimsData<T>* _ref;
    int _level;
    AimsData<T>** _ppItem;

    void _new_ppItem( int level );
    void _free_ppItem();
    void _build();
};


template <class T> inline
void Pyramid<T>::setRef( AimsData<T>& ref )
{
  if ( _ref )
  {
      setLevel( _level );
  }
  _ref = &ref;
}


template <class T> inline
void Pyramid<T>::_new_ppItem( int level )
{
  _ppItem = new AimsData<T>*[ level ];

  int dimX = _ref->dimX();
  int dimY = _ref->dimY();
  int dimZ = _ref->dimZ();
  int dimT = _ref->dimT();

  float sizeX = _ref->sizeX();
  float sizeY = _ref->sizeY();
  float sizeZ = _ref->sizeZ();
  float sizeT = _ref->sizeT();

  for ( int k = 0; k < level; k++ )
  {
    dimX /= 2;
    dimY /= 2;
    dimZ /= 2;
    if( dimX == 0 )
      dimX = 1 ;
    if( dimY == 0 )
      dimY = 1 ;
    if( dimZ == 0 )
      dimZ = 1 ;
    
    sizeX *= 2.0;
    sizeY *= 2.0;
    sizeZ *= 2.0;

    _ppItem[ k ] = new AimsData<T>( dimX, dimY, dimZ, dimT );
    _ppItem[ k ]->setSizeXYZT( sizeX, sizeY, sizeZ, sizeT );
  }
}

template <class T> inline
void Pyramid<T>::_free_ppItem()
{
  if ( _ppItem )
  {
    for ( int k = 0; k < _level; k++ )
      delete _ppItem[ k ];
    delete[] _ppItem;
    _ppItem = 0;
  }
}


template <class T> inline
void Pyramid<T>::setLevel( int level )
{
  ASSERT( _ref );

  _free_ppItem();
  _level = level;
  _new_ppItem( level );

  _build();
}

template <class T> inline
void Pyramid<T>::_build()
{
  int x, y, z, t, k, nx, ny, nz, n;

  int dimX = _ref->dimX();
  int dimY = _ref->dimY();
  int dimZ = _ref->dimZ();
  int dimT = _ref->dimT();

  AimsData<T>* pUp;
  AimsData<T>* pDown = _ref;

  AimsData<T> tab( 8 );

  for ( k = 0; k < _level; k++ )
    {
      pUp = _ppItem[ k ];

      int lowerLvlDimX = dimX ;
      int lowerLvlDimY = dimY ;
      int lowerLvlDimZ = dimZ ;
      
      dimX /= 2;
      dimY /= 2;
      dimZ /= 2;
      
      if( dimX == 0 )
	dimX = 1 ;
      if( dimY == 0 )
	dimY = 1 ;
      if( dimZ == 0 )
	dimZ = 1 ;
      for ( t = 0; t < dimT; t++ )
	for ( z = 0; z < dimZ; z++ )
	  for ( y = 0; y < dimY; y++ )
	    for ( x = 0; x < dimX; x++ )
	      {
		n = 0;
		for ( nz = 0; nz < 2; nz++ )
		  for ( ny = 0; ny < 2; ny++ )
		    for ( nx = 0; nx < 2; nx++ ){
		      tab(n++) = (*pDown)( std::min(2 * x + nx, lowerLvlDimX - 1 ), 
					   std::min(2 * y + ny, lowerLvlDimY - 1 ), 
					   std::min(2 * z + nz, lowerLvlDimZ - 1 ), t );
		    }
		(*pUp)( x, y, z, t ) = _func.doit( tab );
	      }
      pDown = pUp;
    }
}


template <class T> inline
const AimsData<T>& Pyramid<T>::item( int level ) const
{
  ASSERT( _ref );
  ASSERT( level <= _level );

  if ( level == 0 )
    return *_ref;
  else
    return *_ppItem[ level - 1 ];
}


template <class T> inline
void Pyramid<T>::save( const std::string& name ) const
{
  for ( int k = 0; k < _level; k++ )
  {
    char ext[5];
    sprintf( ext, "%d", k );
    std::string outName = name + std::string( ext );
    aims::Writer<AimsData<T> > dataW( outName.c_str() );
    dataW << item( k );
  }
}


#endif
