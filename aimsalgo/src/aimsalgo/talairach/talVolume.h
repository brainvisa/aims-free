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


#ifndef AIMS_TALAIRACH_TALVOLUME_H
#define AIMS_TALAIRACH_TALVOLUME_H

#include <aims/data/data.h>
#include <aims/talairach/talBoxBase.h>


template< class T >
class TalairachVolume
{
public:

  TalairachVolume( int dx=157, int dy=189, int dz=136, int cx=78, int cy=76, 
		   int dz=85 );
  virtual ~Talairach() { }

  AimsData< T >& volume() { return m_d; }

  Point3df toVolume( const TalairachBoxBase&, const Point3df& );
  Point3df fromVolume( const TalairachBoxBase&, const Point3df& );

  AimsData< T >& create( const TalairachBoxBase&, const AimsData< T >& );

private:

  Point3df _scale;
  Point3df _translation;

  AimsData< T > m_d;
};


template< class T > inline
void TalairachVolume< T >::TalairachVolume( int dx, int dy, int dz, 
					    int cx, int cy, int cz )
{
  m_d = AimsData< T >( dx, dy, dz );
  m_d.setXYZT( 1.0f, 1.0f, 1.0f );

  _translation = Point3df( (float)cx, -(float)cy, -(float)cz );

  int cdx = ( dx - cx - 1 > cx ) ? dx - cx - 1 : cx;
  _scale = Point3df( (float)cdx, (float)( dy - cy - 1 ), (float)cz );
}


template< class T > inline
Point3df TalairachVolume< T >::toVolume( const TalairachBoxBase& tb,
					 const Point3df& pt )
{
  Point3df res = tb.toTalairach( pt );

  for ( int i=0; i<3; i++ )
    res[ i ] = res[ i ] * _scale[ i ] + _translation[ i ];

  return res;
}


template< class T > inline
Point3df TalairachVolume< T >::fromVolume( const TalairachBoxBase& tb,
					   const Point3df& pt )
{
  Point3df res;

  for ( int i=0; i<3; i++ )
    res[ i ] = ( pt[ i ] - _translation[ i ] ) / _scale[ i ];

  return tb.fromTalairach( res );
}


template< class T > inline
AimsData< T >& TalairachVolume< T >::create( const TalairachBoxBase& tb,
					     AimsData< T >& d )
{
  int i, j, k;
  int dx = m_d.dimX();
  int dy = m_d.dimY();
  int dz = m_d.dimZ();

  Point3df pt, npt;
  Point3df r( 0.5f, 0.5f, 0.5f );

  for ( k=0; k<dz; k++ )
    for ( j=0; j<dy; j++ )
      for ( i=0; i<dx; i++ )
	{
	  pt = Point3df( (float)i, (float)j, (float)k );
	  npt = fromVolume( pt ) + r;
	  m_d( i, j, k ) = d( (int)npt[ 0 ], (int)npt[ 1 ], (int)npt[ 2 ] );
	}
}


#endif
