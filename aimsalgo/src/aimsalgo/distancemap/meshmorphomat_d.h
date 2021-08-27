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


#ifndef AIMS_DISTANCEMAP_MESHMORPHOMAT_D_H
#define AIMS_DISTANCEMAP_MESHMORPHOMAT_D_H

#include <aims/distancemap/meshmorphomat.h>
#include <aims/distancemap/stlsort.h>
#include <aims/distancemap/meshdistance.h>
#include <aims/connectivity/meshcc_d.h>
#include <stack>
#include <float.h>

namespace aims
{
  namespace meshdistance
  {

template<class T>
Texture<T> LineariseSulci( const AimsSurface<3,Void> & mesh,
                            const Texture<T> & inittex,
                            const T & label,
                            const T & Back, const T & For,
                            unsigned & min, unsigned & max, unsigned nbDil)
{

  Texture<float>                          dist;
  Texture<T>                              tex = inittex;
  bool                                    allowUnreached = false;
  const std::vector<Point3df> &           vert = mesh.vertex();
  unsigned                                n = vert.size(),imax,i,j,jmax,nbpt=0;
  TimeTexture<float>                      distance(11,n);
  float                                   distMax = 0;
  std::vector<unsigned>                   nodeMax;
  const std::vector< AimsVector<uint,3> > & poly = mesh.polygon();
  ASSERT (n == tex.nItem() );


  //init object map
  for (i=0;i<n;++i)
    {
      if (inittex.item(i) == label)
        {
        tex.item(i) = Back;
        ++nbpt;
        }
      else
        tex.item(i) = For;
    }

  if (nbpt == 0 || nbpt == 1)
    {
      std::cout << "The label " << label << " has only one node ! \n";
      return(inittex);
    }
  //ncc = AimsMeshLabelNbConnectedComponent(mesh,tex,Back);
  //ASSERT ( ncc == 1 );

  // neighbours map

  std::map<unsigned, std::set<unsigned> >        neighbours;
  unsigned                        v1, v2, v3;

  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
      if(tex.item(v1)!=For
         && tex.item(v2)!=For)
        {
          neighbours[v1].insert( v2 );
          neighbours[v2].insert( v1 );
        }
      if(tex.item(v1)!=For
         && tex.item(v3)!=For)
        {
          neighbours[v1].insert( v3 );
          neighbours[v3].insert( v1 );
        }
      if(tex.item(v2)!=For
         && tex.item(v3)!=For)
        {
          neighbours[v2].insert( v3 );
          neighbours[v3].insert( v2 );
        }
    }

  //init distance map
  i=0;
  while (tex.item(i) == For)
    ++ i;
  tex.item(i) = label;

  dist = MeshDistance<T>(mesh, tex, allowUnreached);
  distance[0] = dist;

  for (i=0;i<n;++i)
    if (distance[0].item(i) >= distMax)
      {
        distMax = distance[0].item(i);
        imax = i;
      }

  nodeMax.push_back(imax);

  //Distance map
  for (j=1;j<5;++j)
    {
      distMax = 0;
      for (i=0;i<n;++i)
        {
          if (inittex.item(i) > 0)
            tex.item(i) = Back;
          else
            tex.item(i) = For;
        }
      tex.item(imax) = 1;
      dist = MeshDistance<T>(mesh, tex, allowUnreached);
      distance[j] = dist;
      for (i=0;i<n;++i)
        if (dist.item(i) > distMax )
          {
            distMax = dist.item(i);
            imax = i;
          }
      nodeMax.push_back(imax);
    }

  Texture<T> sulci(n);
  std::set<unsigned>::iterator in,en;
  float dmin;
  min = imax = nodeMax[4];
  max = jmax = nodeMax[3];
  std::stack<unsigned> line;
  i=imax;
  while (i != jmax)
    {
      line.push(i);
      dmin = FLT_MAX;
      for (in = neighbours[i].begin(), en = neighbours[i].end(); in !=en ; ++in)
        if (dist.item(*in) < dmin)
          {
          i = *in;
          dmin = dist.item(*in);
          }
      sulci.item(i) = label;
    }

  unsigned inc = 0, l = line.size();

  std::cout << "Size of the line: " << l << " nbDil: " << nbDil << std::endl;
  if (nbDil != 0)
    while (!line.empty())
      {
        ++inc;
        i = line.top();
        line.pop();
        if (inc <= nbDil || inc >= l - nbDil)
          sulci.item(i) = Back;
      }

  for (i=0;i<n;++i)
    {
      if (inittex.item(i) != label)
      sulci.item(i) = inittex.item(i);
    }

  return (sulci);
}








template<class T>
Texture<T> SplitSulci( const AimsSurface<3,Void> & mesh,
                        const Texture<T> & inittex,
                        const T & Back, const T & For ,
                        const T label,
                        const unsigned min, const unsigned max)
{


  const std::vector<Point3df>                & vert = mesh.vertex();
  unsigned                                 ncc, n = vert.size(),i;
  Texture<T>                                dilat(n),line(n),split(n);
  const std::vector< AimsVector<uint,3> >        & poly = mesh.polygon();


  ASSERT (n = inittex.nItem() );

  // Forbidden neighbours map

  std::set<unsigned>                        neighbours;
  unsigned                        v1, v2, v3;
  std::set<unsigned>::iterator       is,es;

  i = min;
  v1 = poly[i][0];
  v2 = poly[i][1];
  v3 = poly[i][2];
  neighbours.insert(v1);
  neighbours.insert(v2);
  neighbours.insert(v3);
  i = max;
  v1 = poly[i][0];
  v2 = poly[i][1];
  v3 = poly[i][2];
  neighbours.insert(v1);
  neighbours.insert(v2);
  neighbours.insert(v3);
  neighbours.insert(min);
  neighbours.insert(max);

  for (i=0;i<n;++i)
    {
      if (inittex.item(i) == label)
        dilat.item(i) = label;
      else
        dilat.item(i) = Back;
    }



  for (is=neighbours.begin(), es=neighbours.end();is != es; ++is)
    //if (dilat.item(*is) == Back)
      dilat.item(*is) = For;


  dilat = MeshDilation<T>(mesh,dilat,Back,For,1,true);

  for (i=0;i<n;++i)
    split.item(i) = dilat.item(i);


  for (i=0;i<n;++i)
    if (inittex.item(i) == label || dilat.item(i) == For )
      split.item(i) = Back;


  ncc = AimsMeshLabelNbConnectedComponent(mesh,split,label);
  std::cout << "ncc: " << ncc << std::endl;


  return(split);

}



//closing and thinning of the sulci
template<class T>
Texture<T> CloseSulci( const AimsSurface<3,Void> & mesh,
                        const Texture<T> & sulctex,
                        float nbDilMax,bool connexity,
                        const T & Back, const T & For,
                        const std::vector<std::list<unsigned> >  & neigho)
{

  const std::vector<Point3df>                & vert = mesh.vertex();
  unsigned                                 inc=0,nbLabel,i,/*nbDil,*/n = vert.size();
  Texture<T>                             sulctemp(n),ccsulcus(n);
  Texture<T>                                tex(n);
  short                                  ncc;
  T                                         label;
  //std::set<short>                          labsulci;
  std::set<T>                                cc_sulci_labels;//labels of the connex components
  typename std::set<T>::iterator        ilab,elab;
  bool hole=true;

  // init
  for (i=0; i<n; ++i)
    {
      cc_sulci_labels.insert(sulctex.item(i));
      tex.item(i) = sulctex.item(i);
    }

  cc_sulci_labels.erase(Back);
  cc_sulci_labels.erase(For);
  nbLabel = cc_sulci_labels.size();
  std::cout << "\nNb of labels: " << nbLabel << std::endl;


  for ( ilab = cc_sulci_labels.begin(), elab=cc_sulci_labels.end(); ilab!=elab; ++ilab)
      {
        ++inc;
        //nbDil=0;
        label = *ilab;
        std::cout << inc << "/" << nbLabel << std::endl;
        for (i=0; i<n; ++i)
          {
            if (sulctex.item(i) == label)
              ccsulcus.item(i) = label;
            else
              ccsulcus.item(i) = Back;
          }
        ncc = AimsMeshLabelNbConnectedComponent(mesh,ccsulcus,label);
        std::cout << "Nb initial of cc: " << ncc << std::endl;

        if (ncc == 1)
          {
            hole = HasHole(ccsulcus,mesh,label);

            if (hole)
              std::cout << "With hole(s) \n";
            else
              std::cout << "Without hole \n";
          }
        float nbd=1;
        unsigned NB_PT_MAX = 10,nbpt;

        //homotopic closing
        while ( (ncc != 1 || hole) && nbd <= nbDilMax)
          {
            sulctemp = MeshDilation<T>( mesh, ccsulcus,Back,For,nbd,connexity);
            ncc = AimsMeshLabelNbConnectedComponent(mesh,sulctemp,label);
            std::cout << "After Dilation (" << nbd << ") : cc: " << ncc << std::endl ;
            if (ncc == 1)
              {
                hole = HasHole(sulctemp,mesh,label);
                if (hole)
                  std::cout << "With hole(s) \n";
                else
                  std::cout << "Without hole \n";
              }
            nbd += 1;
          }

        if (ncc != 1)
          std::cerr <<"Not able to close the projected connnected component, but I continue.\n";
        if (nbd == 1)
          sulctemp = ccsulcus;
        ccsulcus = MeshDilation<T>( mesh, ccsulcus,Back,For,1,connexity);
        std::cout << "Skeletonization" << std::endl;
        ccsulcus = MeshSkeletization( mesh,sulctemp,label,Back,neigho);
        nbpt = NbOfPoint(sulctemp,label);
        if ( nbpt > NB_PT_MAX)
          ccsulcus = MeshSkeletization( mesh,ccsulcus,label,Back,neigho);//2 steps for the ebarbulage
        else
          std::cout << "Not enough point for the ebarbulage ( " << nbpt << " < " << NB_PT_MAX << ")" << std::endl;
        ncc = AimsMeshLabelNbConnectedComponent(mesh,ccsulcus,label);
        std::cout << "ncc: " << ncc   << std::endl ;
        hole = HasHole(ccsulcus,mesh,label);
        if (hole)
          std::cout << "With hole(s) \n";
        else
          std::cout << "Without hole \n";

        //old thinning
        /*
          if (ncc != 1 && nbDil > 0)
          {
          std::cout << "Impossible to close the sulci of label " << label  << std::endl;
          ccsulcus = AimsMeshFilterConnectedComponent( mesh, ccsulcus,label );
          }
          if (AimsMeshLabelNbConnectedComponent(mesh,ccsulcus,label) == 1)
          {
          std::cout << "Boundaries thinning \n";
          ccsulcus = LineariseSulci<short>( mesh, ccsulcus ,label,Back,For,min,max,nbDil);
          //std::cout << "Split sulci \n";
          //ccsulcus = SplitSulci(mesh,ccsulcus,Back,For,label,min,max);

          }
        */

        //check if the node is free
        for (i=0;i<n;++i)
          if (ccsulcus.item(i) == label)
          {
            if (sulctex.item(i)==label || sulctex.item(i) == Back )
              tex.item(i)=label;
            else
              std::cout << "The point " << i << " cannot change its label \n";
          }


        for (i=0;i<n;++i)
          if (tex.item(i) == label)
          {
            if (ccsulcus.item(i) == label)
              tex.item(i) = label;
            else
              tex.item(i) = Back ;
          }
      }

  return(tex);
}



//id but no thinning for label in the set label_forbidden
template<class T>
Texture<T> CloseSulci( const AimsSurface<3,Void> & mesh,
                        const Texture<T> & sulctex,
                        float nbDilMax,bool connexity,
                        const T & Back, const T & For,
                        const std::vector<std::list<unsigned> >  & neigho,
                        const std::set<T> & label_forbidden)
{

  const std::vector<Point3df>                & vert = mesh.vertex();
  unsigned                                 inc=0,nbLabel,i,/*nbDil,*/n = vert.size();
  Texture<T>                             sulctemp(n),ccsulcus(n);
  Texture<T>                                tex(n);
  short                                  ncc;
  T                                         label;
  //std::set<short>                          labsulci;
  std::set<T>                                cc_sulci_labels;//labels of the connex components
  typename std::set<T>::iterator        ilab,elab;
  bool hole=true;

  // init
  for (i=0; i<n; ++i)
    {
      cc_sulci_labels.insert(sulctex.item(i));
      tex.item(i) = sulctex.item(i);
    }

  cc_sulci_labels.erase(Back);
  cc_sulci_labels.erase(For);
  nbLabel = cc_sulci_labels.size();
  std::cout << "\nNb of labels: " << nbLabel << std::endl;

  typename std::set<T>::iterator lfe = label_forbidden.end();

  for ( ilab = cc_sulci_labels.begin(), elab=cc_sulci_labels.end(); ilab!=elab; ++ilab)
      {
        ++inc;
        //nbDil=0;
        label = *ilab;
        std::cout << inc << "/" << nbLabel << std::endl;
        for (i=0; i<n; ++i)
          {
            if (sulctex.item(i) == label)
              ccsulcus.item(i) = label;
            else
              ccsulcus.item(i) = Back;
          }
        ncc = AimsMeshLabelNbConnectedComponent(mesh,ccsulcus,label);
        std::cout << "Nb initial of cc: " << ncc << std::endl;

        if (ncc == 1)
          {
            hole = HasHole(ccsulcus,mesh,label);

            if (hole)
              std::cout << "With hole(s) \n";
            else
              std::cout << "Without hole \n";
          }
        float nbd=1;
        unsigned NB_PT_MAX = 10,nbpt;

        //homotopic closing
        while ( (ncc != 1 || hole) && nbd <= nbDilMax)
          {
            sulctemp = MeshDilation<T>( mesh, ccsulcus,Back,For,nbd,connexity);
            ncc = AimsMeshLabelNbConnectedComponent(mesh,sulctemp,label);
            std::cout << "After Dilation (" << nbd << ") : cc: " << ncc << std::endl ;
            if (ncc == 1)
              {
                hole = HasHole(sulctemp,mesh,label);
                if (hole)
                  std::cout << "With hole(s) \n";
                else
                  std::cout << "Without hole \n";
              }
            nbd += 1;
          }

        if (ncc != 1)
          std::cerr <<"Not able to close the projected connnected component, but I continue.\n";
        if (nbd == 1)
          sulctemp = ccsulcus;
        ccsulcus = MeshDilation<T>( mesh, ccsulcus,Back,For,1,connexity);
        std::cout << "Skeletonization" << std::endl;

        if (label_forbidden.find(*ilab) == lfe)
          {
            ccsulcus = MeshSkeletization( mesh,sulctemp,label,Back,neigho);
            nbpt = NbOfPoint(sulctemp,label);
            if ( nbpt > NB_PT_MAX)
              ccsulcus = MeshSkeletization( mesh,ccsulcus,label,Back,neigho);//2 steps for the ebarbulage
            else
              std::cout << "Not enough point for the ebarbulage ( " << nbpt << " < " << NB_PT_MAX << ")" << std::endl;
            ncc = AimsMeshLabelNbConnectedComponent(mesh,ccsulcus,label);
            std::cout << "ncc: " << ncc   << std::endl ;
            hole = HasHole(ccsulcus,mesh,label);
            if (hole)
              std::cout << "With hole(s) \n";
            else
              std::cout << "Without hole \n";
          }
            else
              std::cout << "No thinning for the INSULA\n";

        //old thinning
        /*
          if (ncc != 1 && nbDil > 0)
          {
          std::cout << "Impossible to close the sulci of label " << label  << std::endl;
          ccsulcus = AimsMeshFilterConnectedComponent( mesh, ccsulcus,label );
          }
          if (AimsMeshLabelNbConnectedComponent(mesh,ccsulcus,label) == 1)
          {
          std::cout << "Boundaries thinning \n";
          ccsulcus = LineariseSulci<short>( mesh, ccsulcus ,label,Back,For,min,max,nbDil);
          //std::cout << "Split sulci \n";
          //ccsulcus = SplitSulci(mesh,ccsulcus,Back,For,label,min,max);

          }
        */

        //check if the node is free
        for (i=0;i<n;++i)
          if (ccsulcus.item(i) == label)
          {
            if (sulctex.item(i)==label || sulctex.item(i) == Back )
              tex.item(i)=label;
            else
              std::cout << "The point " << i << " cannot change its label \n";
          }


        for (i=0;i<n;++i)
          if (tex.item(i) == label)
          {
            if (ccsulcus.item(i) == label)
              tex.item(i) = label;
            else
              tex.item(i) = Back ;
          }
      }


  return(tex);
}


namespace
{

  template<class T> inline
  bool SimplePoint( const Texture<T> & tex,
                    const std::list<unsigned>  & neigho)
  {

    std::list<unsigned>::const_iterator              il=neigho.begin(), el=neigho.end();
    short                                       l=tex.item(*il);
    int                                         nb=0;//nb chgmt of value

    while(il != el)
      {
        if (tex.item(*il) != l)
          {
            ++nb;
            l = tex.item(*il);
          }
        ++il;
      }
    return(nb == 1 || nb == 2);
  }

}

template<class T> inline
bool HasHole( const Texture<T> &tex,
              const AimsSurface<3,Void> & mesh, const T label )
{

  const std::vector<Point3df>                & vert = mesh.vertex();
  const std::vector< AimsVector<uint,3> >        & poly = mesh.polygon();
  unsigned                                i, n = vert.size();
  unsigned                                 S=0,A=0,F=0;

  ASSERT( tex.nItem() == n );

  // neighbours map

  std::set<std::set<unsigned> >             neighbours;
  std::set<unsigned>                temp;
  unsigned                             v1, v2, v3;

  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];

      if(tex.item(v1)==label && tex.item(v2)==label)
        {
          temp.clear();
          temp.insert(v1);
          temp.insert(v2);
          neighbours.insert(temp);
        }

      if(tex.item(v1)==label && tex.item(v3)==label)
        {
         temp.clear();
         temp.insert(v1);
         temp.insert(v3);
         neighbours.insert(temp);
        }

      if(tex.item(v3)==label && tex.item(v2)==label)
        {
          temp.clear();
          temp.insert(v2);
          temp.insert(v3);
          neighbours.insert(temp);
        }

      if(tex.item(v3)==label && tex.item(v2)==label && tex.item(v1)==label)
        ++F;
    }

  //Nb de sommets S
  for (i=0;i<n;++i)
    if (tex.item(i) == label)
      ++S;

  //Nb d'aretes A
  A = neighbours.size();


  return( (S - A + F != 1) );

}


namespace
{

  template<class T> inline
  bool ImmortalPoint( const Texture<T> & tex,
                      const std::list<unsigned>  & neigho )
  {

    std::list<unsigned>::const_iterator              il=neigho.begin(), el=neigho.end();
    short                                       l=tex.item(*il);
    int                                         nb=0;//nb chgmt of value

    while(il != el)
      {
        if (tex.item(*il) != l)
          {
            ++nb;
            l = tex.item(*il);
          }
        ++il;
      }
    il = neigho.begin();
    if (tex.item(*il) != l)
            ++nb;

    return(nb > 2 || nb ==1 );
  }

}



template<class T> inline
unsigned NbOfPoint( const Texture<T> tex, const T label )
{
  unsigned nb = 0;
  for (unsigned i=0; i < tex.nItem(); ++i)
    if (tex.item(i) == label)
      ++nb;

  return(nb);

}


template<class T>
Texture<T> MeshSkeletization( const AimsSurface<3,Void> & mesh,
                              const Texture<T> & inittex,
                              T label, T Back,
                              const std::vector<std::list<unsigned> >  & neigho )
{

  unsigned        i,n = mesh.vertex().size();
  Texture<short>  skelet(n),temp(n);
  Texture<short>  immortal(n); //0 : still alive , 1: immortal
  Texture<T>      tex(n);
  std::list<unsigned>::const_iterator il,el;

  for (i=0; i<n; ++i)
    if (inittex.item(i) == label)
        skelet.item(i) = 1;

  //init


  //Def initial immortal point map
  for (i=0; i<n; ++i)
    if ( skelet.item(i) == 1 && ImmortalPoint(skelet,neigho[i]) )
      immortal.item(i) = 1;

  for (i=0; i<n; ++i)
    if (skelet.item(i) == 1)
      temp.item(i) = 0;
    else
      temp.item(i) = 1;

  Texture<float>                        dist;
  std::multimap<float,unsigned>              dfront;
  std::multimap<float,unsigned>::iterator    id,ed;
  dist = MeshDistance(mesh,temp,true);


  // node of the front are ordered according to their distance from the background
  for (i=0; i<n; ++i)
    if (skelet.item(i) == 1 && immortal.item(i) == 0)
      for (il=neigho[i].begin(), el=neigho[i].end();il!=el;++il)
        if (skelet.item(*il) == 0 )
          dfront.insert( std::pair<float,unsigned>(dist.item(i),i) );


  while (dfront.size()>0)
    {

      for (id=dfront.begin(), ed=dfront.end();id != ed; ++id)
        if ( immortal.item(id->second) == 0 )
          {
            skelet.item(id->second) = 0;
            //upgrade immortal map
            for (il=neigho[id->second].begin(), el=neigho[id->second].end();il!=el;++il)
              if ( skelet.item(*il) == 1 && ImmortalPoint(skelet,neigho[*il]) )
                immortal.item(*il) = 1;
          }

      //upgrade front

      dfront.clear();
      for (i=0; i<n; ++i)
        if (skelet.item(i) == 1 && immortal.item(i) == 0)
          for (il=neigho[i].begin(), el=neigho[i].end();il!=el;++il)
            if (skelet.item(*il) == 0 )
               dfront.insert( std::pair<float,unsigned>(dist.item(i),i) );
    }

    for (i=0; i<n; ++i)
      if ( skelet.item(i) == 1)
        tex.item(i) =label;
      else
        tex.item(i) = Back;

  return(tex);

}

template <class T>
Texture<T> MeshDilation( const AimsSurface<3,Void> & mesh,
                         const Texture<T> & inittex,
                         const T & Back, const T & For,
                         const float dist,bool connexity )
{

  Texture<T>        tex;
  tex = MeshVoronoiT<T>( mesh, inittex, Back, For, dist, connexity,true );
  return(tex);

}

template <class T>
Texture<T> MeshErosion( const AimsSurface<3,Void> & mesh,
                        const Texture<T> & inittex,
                        const T & Back, const T & For,
                        const float dist,bool connexity )
{

  Texture<T>        tex;
  tex = MeshVoronoiT<T>( mesh, inittex, Back,For,dist, connexity,false );
  return(tex);
}

  }
}

#endif

