
#ifndef AIMS_TALAIRACH_TALVOLUME_H
#define AIMS_TALAIRACH_TALVOLUME_H

#include <cartodata/volume/volume.h>
#include <aims/talairach/talBoxBase.h>


template< class T >
class TalairachVolume
{
public:

  TalairachVolume( int dx=157, int dy=189, int dz=136, int cx=78, int cy=76, 
		   int dz=85 );
  virtual ~Talairach() { }

  carto::VolumeRef< T >& volume() { return m_d; }

  Point3df toVolume( const TalairachBoxBase&, const Point3df& );
  Point3df fromVolume( const TalairachBoxBase&, const Point3df& );

  carto::VolumeRef< T >& create( const TalairachBoxBase&,
                                 const carto::rc_ptr<carto::Volume< T > > & );

private:

  Point3df _scale;
  Point3df _translation;

  carto::VolumeRef< T > m_d;
};


template< class T > inline
void TalairachVolume< T >::TalairachVolume( int dx, int dy, int dz, 
					    int cx, int cy, int cz )
{
  m_d = carto::VolumeRef< T >( dx, dy, dz );
  m_d.setVoxelSize( 1.0f, 1.0f, 1.0f );

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
carto::VolumeRef< T >& TalairachVolume< T >::create(
  const TalairachBoxBase& tb, carto::rc_ptr<carto::Volume< T > > & d )
{
  int i, j, k;
  int dx = m_d.getSizeX();
  int dy = m_d.getSizeY();
  int dz = m_d.getSizeZ();

  Point3df pt, npt;
  Point3df r( 0.5f, 0.5f, 0.5f );

  for ( k=0; k<dz; k++ )
    for ( j=0; j<dy; j++ )
      for ( i=0; i<dx; i++ )
	{
	  pt = Point3df( (float)i, (float)j, (float)k );
	  npt = fromVolume( pt ) + r;
	  m_d( i, j, k ) = d->at( (int)npt[ 0 ], (int)npt[ 1 ], (int)npt[ 2 ] );
	}
}


#endif
