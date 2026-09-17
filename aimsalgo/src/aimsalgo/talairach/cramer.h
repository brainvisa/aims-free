
#ifndef AIMS_TALAIRACH_CRAMER_H
#define AIMS_TALAIRACH_CRAMER_H

#include <aims/vector/vector.h>


class CramerSolver
{
public:

  CramerSolver() { }
  virtual ~CramerSolver() { }
  
  Point3df doit( const Point3df&, const Point3df&, const Point3df&, 
                 const Point3df& );
  
private:

  float determinant( const Point3df&, const Point3df&, const Point3df&);
};

#endif
