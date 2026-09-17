

#ifndef INDIVIDUALS_H
#define INDIVIDUALS_H

#include <vector>
#include <aims/vector/vector.h>


namespace aims {
  template <class T> class Individuals{
  public:
    Individuals(): myPos(0.), myVal(0){}
    Individuals( const Point3df& pos, const std::vector<T>& val ) ;
    ~Individuals() ;
    
    const Point3df& position() const { return myPos ; }
    const std::vector<T>& value() const { return myVal ; }
    
  private:
    Point3df myPos ;
    std::vector<T> myVal ;
  } ;
}

#endif
