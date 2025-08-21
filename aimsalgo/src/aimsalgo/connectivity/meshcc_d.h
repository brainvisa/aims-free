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
                             const Texture<T> & inittex, T threshold, int mode, int verbose )
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
  if (verbose) {
    std::cout << "Thresholding: " << threshold << std::endl;
  }
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
  if (verbose) {
    std::cout << nnode << "/" << n << std::endl;
  }

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

  if (verbose) {
    printf("Computing connected component\n");
    fflush(stdout);
  }
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
  if (verbose) {
    std::cout << "nb cc: " << label << std::endl;
  }

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
  
  Texture<T>                  tex_ord;
  std::map<T,std::set<unsigned> >  labels;
  std::multimap<unsigned, T>       nbLabels;

  ASSERT( inittex.nItem() == n );
  tex.reserve( n );
  tex_ord.reserve( n );

  // neighbours map

  std::map<unsigned, std::set<unsigned> >    neighbours;
  unsigned               v1, v2, v3;


  // init texture
  for( i=0; i<n; ++i )
  {
    if(inittex.item(i) < threshold)
    {
      tex.push_back( FORBIDDEN );
      tex_ord.push_back( FORBIDDEN );
    }
    else
    {
      tex.push_back( 0 );
      tex_ord.push_back( 0 );
    }
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
        labels[label].insert(point);
        for( in=neighbours[point].begin(), fn=neighbours[point].end(); in!=fn; ++in )
        {
          if(tex.item(*in)==0) current.push(*in);
        }
      }
      nbLabels.insert( std::pair<unsigned, T> ( labels[label].size(), label ) );
    }
  
  std::cout << "Nb of cc : "<< label << std::endl;

  typename std::multimap<unsigned,T>::reverse_iterator rit;
  std::map<T, T> nbLabels_ord;
  T lab = 0;
  for (rit=nbLabels.rbegin(); rit!=nbLabels.rend(); ++rit)
  {
    nbLabels_ord[rit->second] = ++lab;
    std::cout << "The " << lab << "th cc has " << rit->first << " points" << std::endl;
  }
  
  typename std::map<T, T>::iterator it; 
  for (i=0;i<n;++i)
    if (tex_ord.item(i) == FORBIDDEN)
       tex_ord.item(i) = 0;
    else
    {
      for (it=nbLabels_ord.begin(); it!=nbLabels_ord.end(); ++it)
      {
        if (tex.item(i) == it->first)
        {
          tex_ord.item(i) = it->second;
          break;
        }  
      }
    }

  return tex_ord;
}

template<class T>
Texture<T> AimsMeshFilterConnectedComponent(
  const AimsSurface<3,Void> & mesh, const Texture<T> & inittex, T cc_label,
  const T & background, unsigned long ncomp, unsigned long min_npts,
  float min_surf )
{
  Texture<long>                   tex;
  Texture<T>                      out_tex;
  const std::vector<Point3df>     & vert = mesh.vertex();
  const std::vector< AimsVector<uint,3> >    & poly = mesh.polygon();
  unsigned                    i, n = vert.size();

  std::map<long, std::set<unsigned> >  labels;
  std::multimap<unsigned, long>        nbLabels;
  std::map<long, float>                areaLabels;

  ASSERT( inittex.nItem() == n );
  tex.reserve( n );
  out_tex.reserve( n );

  // neighbours map

  std::map<unsigned, std::set<unsigned> >    neighbours;
  unsigned               v1, v2, v3;
  std::vector<float> areas( inittex.nItem(), 0. );
  float surf;


  // init texture
  for( i=0; i<n; ++i )
  {
    if( inittex.item(i) != cc_label )
      tex.push_back( FORBIDDEN );
    else
      tex.push_back( 0 );
  }

  // Detect connectivity
  for( i=0; i<poly.size(); ++i )
  {
    v1 = poly[i][0];
    v2 = poly[i][1];
    v3 = poly[i][2];

    // triangle area / 3 (1/3 assigned to each vertex)
    surf = ( vert[v2] - vert[v1] ).dot( vert[v3] - vert[v1] ) / 6;
    areas[v1] += surf;
    areas[v2] += surf;
    areas[v3] += surf;

    if( tex.item(v1)!=FORBIDDEN
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
        areaLabels[label] += areas[point];
        for( in=neighbours[point].begin(), fn=neighbours[point].end(); in!=fn;
             ++in )
        {
          if(tex.item(*in)==0)
            current.push(*in);
        }
      }
      nbLabels.insert( std::make_pair( labels[label].size(), label) );
      std::cout << "The " << label << " th cc (" << cc_label << ") has "
        << labels[label].size() << " points, area: " << areaLabels[label]
        << " mm2" << std::endl;
    }

  std::cout << "Nb of cc : "<< label << std::endl;

  std::multimap<unsigned, long>::reverse_iterator il, el=nbLabels.rend();
  std::set<long> allowed_labels;
  unsigned long c;

  // filter out labels
  for( il=nbLabels.rbegin(), c=0; il!=el; ++il, ++c )
  {
    if( ncomp != 0 && c >= ncomp )
      break;
    if( min_npts != 0 && il->first < min_npts )
      continue;
    if( min_surf != 0. && areaLabels[il->second] < min_surf )
      continue;
    allowed_labels.insert( il->second );
  }
  std::cout << "keeping " << allowed_labels.size() << " components"
    << std::endl;

  for( i=0; i<n; ++i )
  {
    if( tex.item(i) == FORBIDDEN )
      out_tex.push_back( inittex.item(i) );
    else
      if( allowed_labels.find( tex.item(i) ) != allowed_labels.end() )
        out_tex.push_back( cc_label );
      else
        out_tex.push_back( background );
  }

  return out_tex;
}


#endif
