
#ifndef AIMS_DISTANCEMAP_STLSORT_H
#define AIMS_DISTANCEMAP_STLSORT_H

#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <aims/connectivity/connectivity.h>
#include <set>
#include <map>

namespace aims
{

  

    //Def of the operator < for the Point3df (required by the map and the set)
    struct Point3dfCompare 
    {
      typedef Point3df first_argument_type;
      typedef Point3df second_argument_type;
      typedef bool result_type;

      bool operator () ( const Point3df & p1, const Point3df & p2 ) const
	{
	  return p1[2] < p2[2] || 
	    ( p1[2] == p2[2] && 
	      ( p1[1] < p2[1] || 
		( p1[1] == p2[1] && 
		  ( p1[0] < p2[0] ) ) ) );
	}
    };

    //Def of the operator < for the Point3d (required by the map and the set)
    struct Point3dCompare 
    {
      typedef Point3d first_argument_type;
      typedef Point3d second_argument_type;
      typedef bool result_type;

      bool operator () ( const Point3d & p1, const Point3d & p2 ) const
	{
	  return p1[2] < p2[2] || 
	    ( p1[2] == p2[2] && 
	      ( p1[1] < p2[1] || 
		( p1[1] == p2[1] && 
		  ( p1[0] < p2[0] ) ) ) );
	}
    };

  template <class T>
struct PairCompare 
{
  typedef std::pair<T,T> first_argument_type;
  typedef std::pair<T,T> second_argument_type;
  typedef bool result_type;

  bool operator () ( const std::pair<T,T> & p, 
		     const std::pair<T,T> & r ) const
  {
    return p.first < r.first 
      || (p.first == r.first && p.second < r.second) ;
    
  }
    };

// Def of the operator "<" for the set<T> (required by the map)
template <class T>
struct SetCompare 

{
  typedef std::set<T> first_argument_type;
  typedef std::set<T> second_argument_type;
  typedef bool result_type;

  bool operator () ( const std::set<T> & s1, const std::set<T> & s2 ) const
      {
	typename std::set<T>::iterator 
          is1=s1.begin(),es1=s1.end(),is2=s2.begin();
	unsigned 		       	n1 = s1.size(), n2 = s2.size();
	if (n1 != n2)
	  return (n1 < n2);
	else
	  {
	    while ( ( is1 != es1) &&  ( (*is1) == (*is2) )  )
	      {
		++is1;
		++is2;
	      }
	    if (is1 != es1)
	      return( (*is1) < (*is2) );
	    else
	      return false;
	  }
      }
};


//typedefs
typedef std::set<Point3df,Point3dfCompare> Point3dfSet;
typedef std::map<Point3df,Point3dfSet,Point3dfCompare> Point3dfNeigh;
}

#endif
