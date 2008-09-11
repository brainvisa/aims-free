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


#ifndef AIMS_TOPOLOGY_TOPOLOGY_H
#define AIMS_TOPOLOGY_TOPOLOGY_H

#include <aims/data/data.h>
#include <aims/topology/topoBase.h>


template< class T >
class Topology : public TopologyBase
{
public:

  Topology( const AimsData< T >& );

  void fillNeighborhood( const Point3d&, int );
  void fillNeighborhood( const Point3d&, int, int );
  void fillNeighborhoodComplement( const Point3d&, int );

private:

  void init( const AimsData< T >& );

  AimsData< T > _data;
};


template< class T > inline
Topology< T >::Topology( const AimsData< T >& d ) : TopologyBase()
{
  init( d );
}


template< class T > inline
void Topology< T >::init( const AimsData< T >& d )
{
  if ( d.borderWidth() == 0 )
    {
      int dx = d.dimX(), dy = d.dimY(), dz = d.dimZ();
      int x, y, z;
      
      _data = AimsData< T >( dx, dy, dz, 1, 1 );
      _data.setSizeXYZT( d.sizeX(), d.sizeY(), d.sizeZ() );
      
      for ( z=0; z<dz; z++ )
        for ( y=0; y<dy; y++ )
	  for ( x=0; x<dx; x++ )
	    _data( x, y, z ) = d( x, y, z );
    }
  else  _data = d;
  
  _data.fillBorder( (T)0 );
}


template< class T > inline
void Topology< T >::fillNeighborhood( const Point3d& pt, int label )
{
  if ( pt[0] > 0 && pt[1] > 0 && pt[2] > 0 && pt[0] < _data.dimX()-1 &&
       pt[1] < _data.dimY()-1 && pt[2] < _data.dimZ()-1 )
    {
      Point3d dep;

      myX[ 0 ] = 0;

      int *xptr = myX + 1;
      for ( int i=1; i<27; i++ )
	{
	  dep = pt + connex26.deplacement( i );
	  *xptr++ = ( _data( dep ) == (T)label );
	}
    }
  else  _cstar = _cbar = 0;
}


template< class T > inline
void Topology< T >::fillNeighborhood( const Point3d& pt, int lb1, int lb2 )
{
  if ( pt[0] > 0 && pt[1] > 0 && pt[2] > 0 && pt[0] < _data.dimX()-1 &&
       pt[1] < _data.dimY()-1 && pt[2] < _data.dimZ()-1 )
    {
      Point3d dep;
      T temp;

      myX[ 0 ] = 0;

      int *xptr = myX + 1;
      for ( int i=1; i<27; i++ )
	{
	  dep = pt + connex26.deplacement( i );
	  temp = _data( dep );
	  *xptr++ = ( temp == (T)lb1 ) || ( temp == (T)lb2 );
	}
    }
  else  _cstar = _cbar = 0;
}


template< class T > inline
void Topology< T >::fillNeighborhoodComplement( const Point3d& pt, int label )
{
  if ( pt[0] > 0 && pt[1] > 0 && pt[2] > 0 && pt[0] < _data.dimX()-1 &&
       pt[1] < _data.dimY()-1 && pt[2] < _data.dimZ()-1 )
    {
      Point3d dep;

      myX[ 0 ] = 0;

      int *xptr = myX + 1;
      for ( int i=1; i<27; i++ )
	{
	  dep = pt + connex26.deplacement( i );
	  *xptr++ = ( _data( dep ) != (T)label );
	}
    }
  else  _cstar = _cbar = 0;
}

#endif
