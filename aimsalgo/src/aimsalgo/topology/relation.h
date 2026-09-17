
#ifndef AIMS_TOPOLOGY_RELATION_H
#define AIMS_TOPOLOGY_RELATION_H

#include <aims/vector/vector.h>


class TopologicalRelation
{
public:

  TopologicalRelation();
  TopologicalRelation( const TopologicalRelation& );
  virtual ~TopologicalRelation() { }

  const Point3d& deplacement( int i ) const { return P[ i ]; }
  int nbNeighbors( int i ) const { return _nbNeighbors[ i ]; }
  int relation( int i, int j ) const { return _relation[ i ][ j ]; }

protected:

  Point3d P[ 27 ];

  int _nbNeighbors[ 27 ];
  int _relation[ 27 ][ 26 ];

private:

  void init();

  virtual void fillNeighbors() { }
};

#endif
