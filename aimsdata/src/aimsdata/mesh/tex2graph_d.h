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

/*
 *  Texture class
 */
#ifndef AIMS_MESH_TEX2GRAPH_D_H
#define AIMS_MESH_TEX2GRAPH_D_H

#include <aims/mesh/tex2graph.h>
#include <graph/graph/graph.h>
#include <cartobase/smart/rcptr.h>
#include <aims/graph/graphmanip.h>
#include <stdio.h>

namespace aims
{


  template <class T>
  inline void Tex2Graph<T>::fillLabel2index(const Texture<T> & )
  {
  }

  template <class T>
  inline int Tex2Graph<T>::getIndex(const T & label)
  {
    return (int) label;

  }

  template <>
  inline int Tex2Graph<std::set<short> >::getIndex(const std::set<short> & label)
  {
    return label2index[label];
  }



  template <>
  inline void Tex2Graph<std::set<short> >::fillLabel2index(const Texture<std::set<short> > & tex)
  {
    std::set<std::set<short> >            all_lab;
    std::set<std::set<short> >::iterator       bl,fl;
    int                     inc = 0, n = tex.nItem();

    for( int i=0; i<n; ++i )
     all_lab.insert( tex.item(i) );

     for (bl=all_lab.begin(), fl=all_lab.end(); bl != fl; ++bl,++inc)
      label2index[*bl] = inc;
  }

  template <class T>
  inline void Tex2Graph<T>::makeGraph( Graph & g, const AimsSurfaceTriangle & initmesh,
            const Texture<T> & tex,
            const std::map<T,std::string> & lab2name, float epsilon )
  {

    T             lab;
    std::map<T,Vertex*>     nodeLabels;
    typename std::map<T,std::set<unsigned> >::iterator il,el;
    std::map<T,std::set<unsigned> >  triLabels;
    std::set<unsigned>::iterator       it,et;
    Vertex      *v;
    const std::vector< AimsVector<uint, 3 > >  & initpoly = initmesh.polygon();
    unsigned      n = tex.nItem(), i, m = initpoly.size();
    typename std::map <T,std::string>::const_iterator iname,ename = lab2name.end();

    if ( n != initmesh.vertex().size() )
      throw std::logic_error("Mesh and texture must have the same size");


    AimsSurfaceTriangle  mesh = initmesh;
    std::vector<Point3df>     &vert = mesh.vertex();
    std::vector<Point3df>     &normal = mesh.normal();
    std::vector< AimsVector<uint, 3 > >   &poly = mesh.polygon();

    for (i=0; i<n; ++i)
    {
      vert[i][0] = vert[i][0] + epsilon * normal[i][0];
      vert[i][1] = vert[i][1] + epsilon * normal[i][1];
      vert[i][2] = vert[i][2] + epsilon * normal[i][2];
    }

    std::vector<float>  vs;
    vs.push_back( 1 );
    vs.push_back( 1 );
    vs.push_back( 1 );
    g.setProperty( "voxel_size", vs );
    g.setProperty( "filename_base", "*" );

    std::vector<int>  bbmin(3), bbmax(3);
    bool    first = true;
    std::string        name;
    typename std::set<T>::iterator bl,fl;

    fillLabel2index(tex);

    for( i=0; i<n; ++i )
      {
  lab = tex.item(i);

  if ( nodeLabels.find( lab ) == nodeLabels.end() )
    {
      v = g.addVertex( "roi" );
      //v->setProperty( "roi_label", (int) lab );
      v->setProperty( "roi_label",  getIndex(lab) );
      iname = lab2name.find(lab);
      if (iname != ename)
        {
    name =  iname->second ;
    v->setProperty( "name",  name );
        }
      else
        {
    std::cerr << "Cannot translate a label " << std::endl ;
    v->setProperty( "name",  (std::string)"unknown" );
        }
      nodeLabels[lab] = v;


    }
  const Point3df  & pt = mesh.vertex()[i];
  if( first )
    {
      bbmin[0] = bbmax[0] = (int) rint( pt[0] );
      bbmin[1] = bbmax[1] = (int) rint( pt[1] );
      bbmin[2] = bbmax[2] = (int) rint( pt[2] );
      first = false;
    }
  else
    {
      if( pt[0] < bbmin[0] )
        bbmin[0] = (int) rint( pt[0] );
      if( pt[1] < bbmin[1] )
        bbmin[1] = (int) rint( pt[1] );
      if( pt[2] < bbmin[2] )
        bbmin[2] = (int) rint( pt[2] );
      if( pt[0] > bbmax[0] )
        bbmax[0] = (int) rint( pt[0] );
      if( pt[1] > bbmax[1] )
        bbmax[1] = (int) rint( pt[1] );
      if( pt[2] > bbmax[2] )
        bbmax[2] = (int) rint( pt[2] );
    }
      }

    g.setProperty( "boundingbox_min", bbmin );
    g.setProperty( "boundingbox_max", bbmax );


    for ( i=0; i<m; ++i )
      {
  if ( tex.item(poly[i][0]) == tex.item(poly[i][1]) &&
       tex.item(poly[i][2]) == tex.item(poly[i][1]) )
    triLabels[tex.item(poly[i][0])].insert(i);
      }

    for ( il = triLabels.begin(), el = triLabels.end(); il != el; ++il )
      {
  uint        ind=0,a,b,c;
  std::map<uint,uint>      conversion;
  std::map<uint,uint>::iterator        ic;
  AimsVector<uint,3>    tri;

  carto::rc_ptr<AimsSurfaceTriangle>       surface(new AimsSurfaceTriangle);

  for (it = il->second.begin(), et = il->second.end(); it != et; ++it)
    {
      tri = poly[*it];
      if ( (ic=conversion.find( tri[0] )) == conversion.end() )
        {
    conversion[tri[0] ] = ind;
    a=ind;
    ++ind;
    surface->vertex().push_back(mesh.vertex()[tri[0] ]  );
        }
      else
        a =ic->second;

      if ( (ic=conversion.find(tri[1] )) == conversion.end() )
        {
    conversion[tri[1] ] = ind;
    b=ind;
    ++ind;
    surface->vertex().push_back(mesh.vertex()[tri[1] ] );
        }
      else
        b = ic->second;

      if ( (ic=conversion.find( tri[2] )) == conversion.end() )
        {
    conversion[ tri[2] ] = ind;
    c=ind;
    ++ind;
    surface->vertex().push_back(mesh.vertex()[tri[2] ] );
        }
      else
        c = ic->second;
      surface->polygon().push_back(AimsVector<uint,3>(a,b,c) );
    }

          v = nodeLabels[il->first];
          surface->updateNormals();
          GraphManip::storeAims( g, v, "aims_Tmtktri", surface );
          v->setProperty("aims_Tmtktri",surface);
      }

  }


  template <class T>
  inline void Tex2Graph<T>::makeGraph( Graph & g, const AimsSurfaceTriangle & initmesh,
                                    const Texture<T> & tex, float epsilon )
  {
    T             lab;
    std::map<T,Vertex*>     nodeLabels;
    typename std::map<T,std::set<unsigned> >::iterator il,el;
    std::map<T,std::set<unsigned> >  triLabels;
    std::set<unsigned>::iterator       it,et;
    Vertex      *v;
    const std::vector< AimsVector<uint, 3 > >  & initpoly = initmesh.polygon();
    unsigned      n = tex.nItem(), i, m = initpoly.size();

    if ( n != initmesh.vertex().size() )
      throw std::logic_error("Mesh and texture must have the same size");

    AimsSurfaceTriangle  mesh = initmesh;
    std::vector<Point3df>    &vert =  mesh.vertex() ;
    std::vector<Point3df>     &normal = mesh.normal();
    std::vector< AimsVector<uint, 3 > >  &poly = mesh.polygon();

    for (i=0; i<n; ++i)
    {
      vert[i][0] = vert[i][0] + epsilon * normal[i][0];
      vert[i][1] = vert[i][1] + epsilon * normal[i][1];
      vert[i][2] = vert[i][2] + epsilon * normal[i][2];
    }

    std::vector<float>  vs;
    vs.push_back( 1 );
    vs.push_back( 1 );
    vs.push_back( 1 );
    g.setProperty( "voxel_size", vs );
    g.setProperty( "filename_base", "*" );

    std::vector<int>  bbmin(3), bbmax(3);
    bool    first = true;
    char    name[10];

    fillLabel2index(tex);


    for( i=0; i<n; ++i )
      {
        lab = tex.item(i);
        if ( nodeLabels.find( lab ) == nodeLabels.end() )
          {
            v = g.addVertex( "roi" );
            v->setProperty( "roi_label",  getIndex(lab) );
            sprintf( name, "%d",  getIndex(lab) );
            v->setProperty( "name", "label_"+std::string( name ) );
            nodeLabels[lab] = v;
          }
        const Point3df  & pt = mesh.vertex()[i];
        if( first )
          {
            bbmin[0] = bbmax[0] = (int) rint( pt[0] );
            bbmin[1] = bbmax[1] = (int) rint( pt[1] );
            bbmin[2] = bbmax[2] = (int) rint( pt[2] );
            first = false;
          }
        else
          {
            if( pt[0] < bbmin[0] )
              bbmin[0] = (int) rint( pt[0] );
            if( pt[1] < bbmin[1] )
              bbmin[1] = (int) rint( pt[1] );
            if( pt[2] < bbmin[2] )
              bbmin[2] = (int) rint( pt[2] );
            if( pt[0] > bbmax[0] )
              bbmax[0] = (int) rint( pt[0] );
            if( pt[1] > bbmax[1] )
              bbmax[1] = (int) rint( pt[1] );
            if( pt[2] > bbmax[2] )
              bbmax[2] = (int) rint( pt[2] );
          }
      }

    g.setProperty( "boundingbox_min", bbmin );
    g.setProperty( "boundingbox_max", bbmax );

    for ( i=0; i<m; ++i )
      {
        if ( tex.item(poly[i][0]) == tex.item(poly[i][1]) &&
             tex.item(poly[i][2]) == tex.item(poly[i][1]) )
          triLabels[tex.item(poly[i][0])].insert(i);
      }

    for ( il = triLabels.begin(), el = triLabels.end(); il != el; ++il )
      {
        uint        ind=0,a,b,c;
        std::map<uint,uint>      conversion;
        std::map<uint,uint>::iterator    ic;
        AimsVector<uint,3>    tri;
        carto::rc_ptr<AimsSurfaceTriangle>       surface(new AimsSurfaceTriangle);
        for (it = il->second.begin(), et = il->second.end(); it != et; ++it)
          {
            tri = poly[*it];
            if ( (ic=conversion.find( tri[0] )) == conversion.end() )
              {
                conversion[tri[0] ] = ind;
                a=ind;
                ++ind;
                surface->vertex().push_back(mesh.vertex()[tri[0] ] );
                surface->normal().push_back(mesh.normal()[tri[0] ] );
              }
            else
              a =ic->second;

            if ( (ic=conversion.find(tri[1] )) == conversion.end() )
              {
                conversion[tri[1] ] = ind;
                b=ind;
                ++ind;
                surface->vertex().push_back(mesh.vertex()[tri[1] ] );
                surface->normal().push_back(mesh.normal()[tri[1] ] );
              }
            else
              b = ic->second;

            if ( (ic=conversion.find( tri[2] )) == conversion.end() )
              {
                conversion[ tri[2] ] = ind;
                c=ind;
                ++ind;
                surface->vertex().push_back(mesh.vertex()[tri[2] ] );
                surface->normal().push_back(mesh.normal()[tri[2] ] );
              }
            else
              c = ic->second;
            surface->polygon().push_back(AimsVector<uint,3>(a,b,c) );
            v = nodeLabels[il->first];
            v->setProperty("aims_Tmtktri",surface);
          }
      }

    carto::rc_ptr<std::map<std::string,std::map<std::string,GraphElementCode> > >
      objmap( new std::map<std::string,std::map<std::string,GraphElementCode> > );
    GraphElementCode   &gec = (*objmap)["roi"]["Tmtktri"];
    gec.id = "Tmtktri";
    gec.attribute = "aims_Tmtktri";
    gec.objectType = "Mesh";
    gec.dataType = "VOID";
    gec.syntax = "roi";
    gec.local_file_attribute = "Tmtktri_filename";
    gec.global_index_attribute = "Tmtktri_label";
    gec.global_filename = "patch.mesh";
    //gec.global = false;
    g.setProperty("aims_objects_table",objmap);
  }


}



#endif
