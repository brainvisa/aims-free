
#ifndef AIMS_TOPOLOGY_CONNECTIVITY6_H
#define AIMS_TOPOLOGY_CONNECTIVITY6_H

#include <aims/topology/relation.h>


class Connectivity6 : public TopologicalRelation
{
public:

  Connectivity6();

private:

  void fillNeighbors();
};

#endif
