
#ifndef AIMS_TOPOLOGY_TOPOLOGY_H
#define AIMS_TOPOLOGY_TOPOLOGY_H

#include <cartodata/volume/volume.h>
#include <aims/topology/topoBase.h>


template< class T >
class Topology : public TopologyBase
{
public:

  Topology( const carto::rc_ptr<carto::Volume< T > >& );

  void fillNeighborhood( const Point3d&, int );
  void fillNeighborhood( const Point3d&, int, int );
  void fillNeighborhoodComplement( const Point3d&, int );

private:

  void init( const carto::rc_ptr<carto::Volume< T > >& );

  carto::VolumeRef< T > _data;
};


template< class T > inline
Topology< T >::Topology( const carto::rc_ptr<carto::Volume< T > > & d )
  : TopologyBase()
{
  init( d );
}


template< class T > inline
void Topology< T >::init( const carto::rc_ptr<carto::Volume< T > > & d )
{
  if ( d->getBorders()[0] == 0 || d->getBorders()[1] == 0
       || d->getBorders()[2] == 0 )
  {
    int dx = d->getSizeX(), dy = d->getSizeY(), dz = d->getSizeZ();
    int x, y, z;

    _data = carto::VolumeRef< T >( dx, dy, dz, 1, 1 );
    _data.setVoxelSize( d->getVoxelSize() );

    for ( z=0; z<dz; z++ )
      for ( y=0; y<dy; y++ )
        for ( x=0; x<dx; x++ )
          _data( x, y, z ) = d->at( x, y, z );
  }
  else  _data = d;
  
  _data.fillBorder( (T)0 );
}


template< class T > inline
void Topology< T >::fillNeighborhood( const Point3d& pt, int label )
{
  if ( pt[0] > 0 && pt[1] > 0 && pt[2] > 0 && pt[0] < _data.getSizeX()-1 &&
       pt[1] < _data.getSizeY()-1 && pt[2] < _data.getSizeZ()-1 )
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
  if ( pt[0] > 0 && pt[1] > 0 && pt[2] > 0 && pt[0] < _data.getSizeX()-1 &&
       pt[1] < _data.getSizeY()-1 && pt[2] < _data.getSizeZ()-1 )
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
  if ( pt[0] > 0 && pt[1] > 0 && pt[2] > 0 && pt[0] < _data.getSizeX()-1 &&
       pt[1] < _data.getSizeY()-1 && pt[2] < _data.getSizeZ()-1 )
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
