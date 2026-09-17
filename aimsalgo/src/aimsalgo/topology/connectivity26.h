
#ifndef AIMS_TOPOLOGY_CONNECTIVITY26_H
#define AIMS_TOPOLOGY_CONNECTIVITY26_H

#include <aims/topology/relation.h>


class Connectivity26 : public TopologicalRelation
{
public:

  Connectivity26();

private:

  void fillNeighbors();
};

#endif
