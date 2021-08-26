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
