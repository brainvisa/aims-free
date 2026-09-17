
#ifndef AIMS_TOPOLOGY_COMPONENTS18_H
#define AIMS_TOPOLOGY_COMPONENTS18_H

#include <aims/topology/components.h>


class Components18Neighborhood : public Components
{
public:

  Components18Neighborhood( TopologicalRelation, Adjacency );

  int getScalar( int * );
};

#endif
