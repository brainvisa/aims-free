/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#ifndef AIMS_DISTANCEMAP_STLSORT_H
#define AIMS_DISTANCEMAP_STLSORT_H

#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <aims/data/data.h>
#include <aims/connectivity/connectivity.h>
#include <set>
#include <map>

namespace aims
{

  

    //Def of the operator < for the Point3df (required by the map and the set)
    struct Point3dfCompare 
      : public std::binary_function<Point3df,Point3df, bool>
    {
      bool operator () ( const Point3df & p1, const Point3df & p2 ) const
	{
	  return p1[2] < p2[2] || 
	    ( p1[2] == p2[2] && 
	      ( p1[1] < p2[1] || 
		( p1[1] == p2[1] && 
		  ( p1[0] < p2[0] ) ) ) );
	}
    };

    //Def of the operator < for the Point3df (required by the map and the set)
    struct Point3dCompare : public std::binary_function<Point3d,Point3d, bool>
    {
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
: public std::binary_function<std::pair<T,T>, std::pair<T,T>, bool>
{
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
  : public std::binary_function<std::set<T>,std::set<T>, bool>
{
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
