

#ifndef AIMS_TOPOLOGY_TOPOCLASSIF_H
#define AIMS_TOPOLOGY_TOPOCLASSIF_H

#include <aims/topology/topoClassifBase.h>
#include <aims/topology/topology.h>


template< class T >
class TopologicalClassification : public TopologicalClassificationBase
{
public:

  TopologicalClassification( const carto::rc_ptr<carto::Volume< T > > & );
  virtual ~TopologicalClassification();
};


template< class T > inline
TopologicalClassification< T >::TopologicalClassification( 
  const carto::rc_ptr<carto::Volume< T > > & d )
  : TopologicalClassificationBase()
{
  _topo = new Topology< T >( d );
}


template< class T > inline
TopologicalClassification< T >::~TopologicalClassification()
{
  delete _topo;
}

#endif
