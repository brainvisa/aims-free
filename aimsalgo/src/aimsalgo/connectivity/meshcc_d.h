/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */



#ifndef AIMS_MESH_CONNECTIVITY_MESHCC_D_H
#define AIMS_MESH_CONNECTIVITY_MESHCC_D_H

#include <aims/connectivity/meshcc.h>
#include <aims/distancemap/meshvoronoi.h>
#include <cstdlib>
#include <map>
#include <set>
#include <stack>
#include <cstdlib>
//#include <stdio.h>


template<class T>
Texture<T> AimsMeshLabelConnectedComponent( const AimsSurface<3,Void> & mesh,
                             const Texture<T> & inittex, T threshold, int mode )
{
  Texture<T>                  tex;
  const std::vector<Point3df>      & vert = mesh.vertex();
  const std::vector< AimsVector<uint,3> >    & poly = mesh.polygon();
  unsigned                    i, n = vert.size();

  ASSERT( inittex.nItem() == n );
  tex.reserve( n );

  // neighbours map

  std::map<unsigned, std::set<unsigned> >    neighbours;
  unsigned               v1, v2, v3;
  int nnode = 0;

  // init texture
  std::cout << "Thresholding: " << threshold << std::endl;
  if (mode==1)
  {
     for( i=0; i<n; ++i )
     {
          if(inittex.item(i)>=threshold) tex.push_back( FORBIDDEN );
          else
          {
          tex.push_back( 0 );
          nnode++;
          }
     }
  }
  else if (mode==0)
  {
    for( i=0; i<n; ++i )
    {
      if(inittex.item(i)<=threshold) tex.push_back( FORBIDDEN );
      else
     {
       tex.push_back( 0 );
       nnode++;
     }
    }
  }
  else
  {
     std::cerr << "AimsMeshLabelConnectedComponent : thresholding mode unknown" << std::endl;
     exit(1);
   }
  std::cout << nnode << "/" << n << std::flush;

  //Detect connectivity
  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
      if(tex.item(v1)!=FORBIDDEN
      && tex.item(v2)!=FORBIDDEN)
     {
       neighbours[v1].insert( v2 );
       neighbours[v2].insert( v1 );
     }
      if(tex.item(v1)!=FORBIDDEN
      && tex.item(v3)!=FORBIDDEN)
     {
       neighbours[v1].insert( v3 );
       neighbours[v3].insert( v1 );
     }
      if(tex.item(v2)!=FORBIDDEN
      && tex.item(v3)!=FORBIDDEN)
     {
       neighbours[v2].insert( v3 );
       neighbours[v3].insert( v2 );
     }
    }

  T label = 0;
  unsigned point;
  std::stack<unsigned> current;
  std::set<unsigned>::iterator          in, fn;


  printf("Computing connected component\n");
  fflush(stdout);
  for( i=0; i<n; ++i )
    {
      if(tex.item(i)==0)
     {
       label++;
       current.push(i);
       fflush(stdout);
       while(!current.empty())
         {
           point = current.top();
           current.pop();
           tex.item(point)=label;
           for( in=neighbours[point].begin(), fn=neighbours[point].end(); in!=fn; ++in )
          {
            if(tex.item(*in)==0) current.push(*in);
          }
         }
     }
    }
  std::cout << "nb cc: " << label << std::endl;

  return tex;
}


template<class T>
unsigned AimsMeshLabelNbConnectedComponent( const AimsSurface<3,Void> & mesh,
                           const Texture<T> & inittex, const T lab )
{
  Texture<T>                  tex;
  const std::vector<Point3df>      & vert = mesh.vertex();
  const std::vector< AimsVector<uint,3> >    & poly = mesh.polygon();
  unsigned                    i, n = vert.size();

  ASSERT( inittex.nItem() == n );
  tex.reserve( n );

  // init texture
  for (i=0; i<n; ++i)
    if (inittex.item(i) == lab)
      tex.push_back(0);
    else
      tex.push_back(FORBIDDEN);

  //Detect connectivity
  std::map<unsigned, std::set<unsigned> >    neighbours;
  unsigned               v1, v2, v3;
  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
      if(tex.item(v1)!=FORBIDDEN
      && tex.item(v2)!=FORBIDDEN
      && inittex.item(v1) == inittex.item(v2) )

     {
       neighbours[v1].insert( v2 );
       neighbours[v2].insert( v1 );
     }
      if(tex.item(v1)!=FORBIDDEN
      && tex.item(v3)!=FORBIDDEN
      && inittex.item(v1) == inittex.item(v3) )
     {
       neighbours[v1].insert( v3 );
       neighbours[v3].insert( v1 );
     }
      if(tex.item(v2)!=FORBIDDEN
      && tex.item(v3)!=FORBIDDEN
      && inittex.item(v2) == inittex.item(v3) )
     {
       neighbours[v2].insert( v3 );
       neighbours[v3].insert( v2 );
     }
    }

  T label = 0;
  unsigned inc =0;
  unsigned point;
  std::stack<unsigned> current;
  std::set<unsigned>::iterator          in, fn;


  for( i=0; i<n; ++i )
    {
      if(tex.item(i)==0)
     {
       label++;
       ++inc;
       current.push(i);
       while(!current.empty())
         {
           point = current.top();
           current.pop();
           tex.item(point)=label;
           for( in=neighbours[point].begin(), fn=neighbours[point].end(); in!=fn; ++in )
          {
            if(tex.item(*in)==0) current.push(*in);
          }
         }
     }
    }

  return inc;
}


// Give a connected map of inittex.
// An initial connex area composed of different labels
// is splitted in sub-area of same labels
template<class T>
Texture<T> AimsMeshLabelConnectedComponent2Texture( const AimsSurface<3,Void> & mesh,
                                  const Texture<T> & inittex,  T threshold )
{
  Texture<T>                  tex;
  const std::vector<Point3df>      & vert = mesh.vertex();
  const std::vector< AimsVector<uint,3> >    & poly = mesh.polygon();
  unsigned                    i, n = vert.size();

  ASSERT( inittex.nItem() == n );
  tex.reserve( n );

  // neighbours map

  std::map<unsigned, std::set<unsigned> >    neighbours;
  unsigned               v1, v2, v3;


  // init texture
  for( i=0; i<n; ++i )
    {
      if(inittex.item(i) < threshold)
     tex.push_back( FORBIDDEN );
      else
     tex.push_back( 0 );
    }

  //Detect connectivity
  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
      if(tex.item(v1)!=FORBIDDEN
      && tex.item(v2)!=FORBIDDEN
     && inittex.item(v1) == inittex.item(v2) )

     {
       neighbours[v1].insert( v2 );
       neighbours[v2].insert( v1 );
     }
      if(tex.item(v1)!=FORBIDDEN
      && tex.item(v3)!=FORBIDDEN
      && inittex.item(v1) == inittex.item(v3) )
     {
       neighbours[v1].insert( v3 );
       neighbours[v3].insert( v1 );
     }
      if(tex.item(v2)!=FORBIDDEN
      && tex.item(v3)!=FORBIDDEN
      && inittex.item(v2) == inittex.item(v3) )
     {
       neighbours[v2].insert( v3 );
       neighbours[v3].insert( v2 );
     }
    }

  T label = 0;
  unsigned point;
  std::stack<unsigned> current;
  std::set<unsigned>::iterator          in, fn;


  for( i=0; i<n; ++i )
      if(tex.item(i) == 0)
      {
       label++;
       current.push(i);
       while(!current.empty())
         {
           point = current.top();
           current.pop();
           tex.item(point)=label;
           for( in=neighbours[point].begin(), fn=neighbours[point].end(); in!=fn; ++in )
          {
            if(tex.item(*in)==0)
              {
                current.push(*in);
              }
          }
         }
     }

  std::cout << "Nb of cc : "<< label << std::endl;

  for (i=0;i<n;++i)
    if (tex.item(i) == FORBIDDEN)
      tex.item(i) = 0;

  return tex;
}

template<class T>
Texture<T> AimsMeshFilterConnectedComponent( const AimsSurface<3,Void> & mesh,
                              const Texture<T> & inittex,  T threshold )
{
  Texture<T>                  tex;
  const std::vector<Point3df>      & vert = mesh.vertex();
  const std::vector< AimsVector<uint,3> >    & poly = mesh.polygon();
  unsigned                    i, n = vert.size();

  std::map<T,std::set<unsigned> >  labels;
  std::map<unsigned,T>        nbLabels;

  ASSERT( inittex.nItem() == n );
  tex.reserve( n );

  // neighbours map

  std::map<unsigned, std::set<unsigned> >    neighbours;
  unsigned               v1, v2, v3;


  // init texture
  for( i=0; i<n; ++i )
    {
      if(inittex.item(i) != threshold)
     tex.push_back( FORBIDDEN );
      else
     tex.push_back( 0 );
    }

  //Detect connectivity
  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
      if(tex.item(v1)!=FORBIDDEN
      && tex.item(v2)!=FORBIDDEN )

     {
       neighbours[v1].insert( v2 );
       neighbours[v2].insert( v1 );
     }
      if(tex.item(v1)!=FORBIDDEN
      && tex.item(v3)!=FORBIDDEN )
     {
       neighbours[v1].insert( v3 );
       neighbours[v3].insert( v1 );
     }
      if(tex.item(v2)!=FORBIDDEN
      && tex.item(v3)!=FORBIDDEN )
     {
       neighbours[v2].insert( v3 );
       neighbours[v3].insert( v2 );
     }
    }

  T label = 0;
  unsigned point;
  std::stack<unsigned> current;
  std::set<unsigned>::iterator          in, fn;


  for( i=0; i<n; ++i )
      if(tex.item(i) == 0)
      {
       label++;
       current.push(i);
       while(!current.empty())
         {
           point = current.top();
           current.pop();
           tex.item(point)=label;
           labels[label].insert(point);
           for( in=neighbours[point].begin(), fn=neighbours[point].end(); in!=fn; ++in )
          {
            if(tex.item(*in)==0)
                current.push(*in);
          }
         }
       nbLabels[ labels[label].size() ] = label;
       std::cout << "The " << label << " th cc (" << threshold << ")has " << labels[label].size() << " points " << std::endl;
     }

  std::cout << "Nb of cc : "<< label << std::endl;


  T maxLab = nbLabels.rbegin()->first;
  std::cout << "maxlab= " << maxLab << "-> " <<  nbLabels[maxLab] << std::endl;


  for (i=0;i<n;++i)
      {
     if (tex.item(i) == FORBIDDEN )
       tex.item(i) = inittex.item(i);
     else
       if (tex.item(i) == nbLabels[maxLab])
         tex.item(i) = threshold;
         else
       tex.item(i) = 0;
      }

  return tex;
}


#endif
