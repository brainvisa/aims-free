/* Copyright (c) 1995-2005 CEA
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

/*
 *  Flip data.
 */
#ifndef AIMS_UTILITY_FLIP_H
#define AIMS_UTILITY_FLIP_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/data.h>


/** The template object function for flipping.
    This object function returns the flipped data.
    The template argument is the basic data type.
*/
template <class T>
class AIMSDATA_API AimsFlip
{ public:
    /**@name Constructor and Destructor*/
    //@{
    /// Constructor does nothing
    AimsFlip() { }
    /// Destructor does nothing
    virtual ~AimsFlip() { }
    //@}
    
    /**@name Methods*/
    //@{
    /// function which returns the XX flipped data
    inline AimsData<T> doXX( const AimsData<T> &thing );
    /// function which returns the YY flipped data
    inline AimsData<T> doYY( const AimsData<T> &thing );
    /// function which returns the ZZ flipped data
    inline AimsData<T> doZZ( const AimsData<T> &thing );
    /// function which returns the XXZZ flipped data
    inline AimsData<T> doXXZZ( const AimsData<T> &thing );
    /// function which returns the XXYY flipped data
    inline AimsData<T> doXXYY( const AimsData<T> &thing );
    /// function which returns the YYZZ flipped data
    inline AimsData<T> doYYZZ( const AimsData<T> &thing );
    /// function which returns the XXYYZZ flipped data
    inline AimsData<T> doXXYYZZ( const AimsData<T> &thing );
    /// function which returns the XY flipped data
    inline AimsData<T> doXY( const AimsData<T> &thing );
    /// function which returns the XZ flipped data
    inline AimsData<T> doXZ( const AimsData<T> &thing );
    /// function which returns the YZ flipped data
    inline AimsData<T> doYZ( const AimsData<T> &thing );
    //@}
};


template <class T> inline
AimsData<T> AimsFlip<T>::doXX( const AimsData<T> &thing )
{
  AimsData<T> res( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(), 
		   thing.borderWidth() );
  if( thing.header() )
      res.setHeader( thing.header()->cloneHeader() );
  res.setSizeXYZT( thing );

  int x, y, z, t;
  ForEach4d( thing, x, y, z, t )
    res( x, y, z, t ) = thing( thing.dimX() - x - 1, y, z, t );

  return res;
}


template <class T> inline
AimsData<T> AimsFlip<T>::doYY( const AimsData<T> &thing )
{
  AimsData<T> res( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(), 
		   thing.borderWidth() );
  if( thing.header() )
      res.setHeader( thing.header()->cloneHeader() );
  res.setSizeXYZT( thing );

  int x, y, z, t;
  ForEach4d( thing, x, y, z, t )
    res( x, y, z, t ) = thing( x, thing.dimY() - y - 1, z, t );

  return res;
}


template <class T> inline
AimsData<T> AimsFlip<T>::doZZ( const AimsData<T> &thing )
{
  AimsData<T> res( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(), 
		   thing.borderWidth() );
  if( thing.header() )
      res.setHeader( thing.header()->cloneHeader() );
  res.setSizeXYZT( thing );

  int x, y, z, t;
  ForEach4d( thing, x, y, z, t )
    res( x, y, z, t ) = thing( x, y, thing.dimZ() - z - 1, t );

  return res;
}


template <class T> inline
AimsData<T> AimsFlip<T>::doXXZZ( const AimsData<T> &thing )
{
  AimsData<T> res( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(), 
		   thing.borderWidth() );
  if( thing.header() )
      res.setHeader( thing.header()->cloneHeader() );
  res.setSizeXYZT( thing );

  int x, y, z, t;
  ForEach4d( thing, x, y, z, t )
    res( x, y, z, t ) = thing(thing.dimX() - x - 1, y, 
			      thing.dimZ() - z - 1, t);

  return res;
}

template <class T> inline
AimsData<T> AimsFlip<T>::doXXYY( const AimsData<T> &thing )
{
  AimsData<T> res( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(), 
		   thing.borderWidth() );
  if( thing.header() )
      res.setHeader( thing.header()->cloneHeader() );
  res.setSizeXYZT( thing );

  int x, y, z, t;
  ForEach4d( thing, x, y, z, t )
    res( x, y, z, t ) = thing(thing.dimX() - x - 1, thing.dimY() - y - 1, 
			      z, t);

  return res;
}

template <class T> inline
AimsData<T> AimsFlip<T>::doYYZZ( const AimsData<T> &thing )
{
  AimsData<T> res( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(), 
		   thing.borderWidth() );
  if( thing.header() )
      res.setHeader( thing.header()->cloneHeader() );
  res.setSizeXYZT( thing );

  int x, y, z, t;
  ForEach4d( thing, x, y, z, t )
    res( x, y, z, t ) = thing(x, thing.dimY() - y - 1, 
			      thing.dimZ() - z - 1, t);

  return res;
}


template <class T> inline
AimsData<T> AimsFlip<T>::doXY( const AimsData<T> &thing )
{
  AimsData<T> res( thing.dimY(), thing.dimX(), thing.dimZ(), thing.dimT(), 
		   thing.borderWidth() );
  if( thing.header() )
      res.setHeader( thing.header()->cloneHeader() );
  res.setSizeX( thing.sizeY() );
  res.setSizeY( thing.sizeX() );
  res.setSizeZ( thing.sizeZ() );
  res.setSizeT( thing.sizeT() );

  int x, y, z, t;
  ForEach4d( thing, x, y, z, t )
    res( x, y, z, t ) = thing( y, x, z, t );

  return res;
}


template <class T> inline
AimsData<T> AimsFlip<T>::doXZ( const AimsData<T> &thing )
{
  AimsData<T> res( thing.dimZ(), thing.dimY(), thing.dimX(), thing.dimT(), 
		   thing.borderWidth() );
  if( thing.header() )
      res.setHeader( thing.header()->cloneHeader() );
  res.setSizeX( thing.sizeZ() );
  res.setSizeY( thing.sizeY() );
  res.setSizeZ( thing.sizeX() );
  res.setSizeT( thing.sizeT() );

  int x, y, z, t;
  ForEach4d( thing, x, y, z, t )
    res( x, y, z, t ) = thing( z, y, x, t );

  return res;
}


template <class T> inline
AimsData<T> AimsFlip<T>::doYZ( const AimsData<T> &thing )
{
  AimsData<T> res( thing.dimX(), thing.dimZ(), thing.dimY(), thing.dimT(), 
		   thing.borderWidth() );
  if( thing.header() )
      res.setHeader( thing.header()->cloneHeader() );
  res.setSizeX( thing.sizeX() );
  res.setSizeY( thing.sizeZ() );
  res.setSizeZ( thing.sizeY() );
  res.setSizeT( thing.sizeT() );

  int x, y, z, t;
  ForEach4d( thing, x, y, z, t )
    res( x, y, z, t ) = thing( x, z, y, t );

  return res;
}


template <class T> inline
AimsData<T> AimsFlip<T>::doXXYYZZ( const AimsData<T> &thing )
{
  AimsData<T> res( thing.dimX(), thing.dimY(), thing.dimZ(), thing.dimT(), 
		   thing.borderWidth() );
  if( thing.header() )
      res.setHeader( thing.header()->cloneHeader() );
  res.setSizeXYZT( thing );

  int x, y, z, t;
  ForEach4d( thing, x, y, z, t )
    res( x, y, z, t ) = thing( thing.dimX() - x - 1,
                               thing.dimY() - y - 1,
                               thing.dimZ() - z - 1, t );

  return res;
}


#endif
