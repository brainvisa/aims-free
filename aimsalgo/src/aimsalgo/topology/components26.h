
#ifndef AIMS_TOPOLOGY_COMPONENTS26_H
#define AIMS_TOPOLOGY_COMPONENTS26_H

#include <aims/topology/components.h>


class Components26Neighborhood : public Components
{
public:

  Components26Neighborhood( TopologicalRelation, Adjacency );

  int getScalar( int * );
};

#endif
