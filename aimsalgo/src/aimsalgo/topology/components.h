
#ifndef AIMS_TOPOLOGY_COMPONENTS_H
#define AIMS_TOPOLOGY_COMPONENTS_H

#include <aims/topology/relation.h>
#include <aims/topology/adjacency.h>


class Components
{
public:

  enum PointType
  {
    InList=101,
    Exist,
    TooFar
  };

  Components( TopologicalRelation, Adjacency );
  virtual ~Components() { }

  virtual int getScalar( int * );

private:

  TopologicalRelation _topoRel;
  Adjacency _adj;
};

#endif
