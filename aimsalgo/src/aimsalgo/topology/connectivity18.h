
#ifndef AIMS_TOPOLOGY_CONNECTIVITY18_H
#define AIMS_TOPOLOGY_CONNECTIVITY18_H

#include <aims/topology/relation.h>


class Connectivity18 : public TopologicalRelation
{
public:

  Connectivity18();

private:

  void fillNeighbors();
};

#endif
