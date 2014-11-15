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


#ifndef AIMS_DISTANCEMAP_MESHPARCELLATION_D_H
#define AIMS_DISTANCEMAP_MESHPARCELLATION_D_H

#include <aims/distancemap/meshparcellation.h>
#include <aims/distancemap/meshdistance.h>
#include <aims/distancemap/meshmorphomat.h>
#include <aims/distancemap/stlsort.h>
#include <aims/mesh/surfaceOperation.h>
#include <aims/mesh/surface.h>
#include <stack>
#include <set>
#include <float.h>

namespace aims
{
  namespace meshdistance
  {

// Select the gyrus seeds
template <class T>
Texture<std::set<T> > 
gyrusSeedDefinition( const AimsSurface<3,Void> & mesh,
                      const Texture<T> & inittex,
                      const std::set<T> & setBack, const std::set<T> & setFor,
                      const std::set<std::set<T> > & labelAllowed)
{

  const std::vector<Point3df>		& vert = mesh.vertex();
  const std::vector< AimsVector<uint,3> >	& poly = mesh.polygon();
  unsigned				i, n = vert.size();
  Texture<std::set<T> >                  tex(n);
  std::map <unsigned, std::set<T> >           labelNeigh;//vertex -> labels of neighbours 
  typename std::map <unsigned, std::set<T> >::iterator il,el;

  ASSERT( inittex.nItem() == n );

  // neighbours map
  std::map<unsigned, std::set<unsigned> >	neighbours;
  unsigned			v1, v2, v3;

  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
      labelNeigh[v1].insert( inittex.item(v2) );
      labelNeigh[v2].insert( inittex.item(v1) );
      labelNeigh[v1].insert( inittex.item(v3) );
      labelNeigh[v3].insert( inittex.item(v1) );
      labelNeigh[v3].insert( inittex.item(v2) );
      labelNeigh[v2].insert( inittex.item(v3) );
    }
  
  std::set<std::set<short> >::const_iterator ia,dl;
  std::set<short>   setLabel,setTemp;

  //  bool ok;
  //   for ( il = labelNeigh.begin(), el = labelNeigh.end(); il!= el; ++il) 
  //     {
  //       ok = false;
  //       if ( (il->second).size() >= 2 )
  //         { 
  // 	  setLabel.clear();
  // 	  setLabel = il->second;
  // 	  for (ia = labelAllowed.begin(),dl = labelAllowed.end(); ia != dl; ++ia)
  // 	    {
  // 	      setTemp.clear();
  // 	      std::insert_iterator<std::set<short> >	ii(setTemp, setTemp.begin() );
  // 	      set_intersection(setLabel.begin(),setLabel.end(),ia->begin(),ia->end(),ii );
  // 	      if (setTemp.size() >= 2)
  // 		{
  // 		  tex.item(il->first) = *ia;
  // 		  ok = true;
  // 		  break;
  // 		}
  // 	    }
  // 	  if (!ok)
  // 	    tex.item(il->first) = setBack;
  // 	} 
  //       else
  // 	{
  // 	  if ( il->second != setFor )
  // 	    tex.item(il->first) = setBack;
  // 	  else
  // 	    tex.item(il->first) = setFor;
  // 	}
  //     }

 Texture<std::set<T> >                  tex2(n);
 unsigned interSize,sizeNeigh;
 typename std::map<unsigned,std::set<T> >::reverse_iterator im,em;
 std::multimap<unsigned,std::set<T> > matchedLabel;

  bool ok;
  for ( il = labelNeigh.begin(), el = labelNeigh.end(); il!= el; ++il) 
    {
      ok = false;
      sizeNeigh = (il->second).size();
      //2 neighbours
      
      /*
      if ( sizeNeigh == 2)
        { 
          setLabel = il->second;
	  for (ia = labelAllowed.begin(),dl = labelAllowed.end(); ia != dl; ++ia)
	    if( (*ia).size() == 2 )
	    {
	      setTemp.clear();
	      std::insert_iterator<std::set<T> >	ii(setTemp, setTemp.begin() );
	      set_intersection(setLabel.begin(),setLabel.end(),ia->begin(),ia->end(),ii );
              if (setTemp.size() == 2 )
		{
		  tex.item(il->first) = *ia;
		  ok = true;
		  break;
		}
	    }
	  if (!ok)
            tex.item(il->first) = setBack;
	} 
      else
      */
        //3 neighbours
	//if ( sizeNeigh == 3)
	if ( sizeNeigh > 1 )
          {
            setLabel = il->second;
            matchedLabel.clear();

            typename std::set<T>::iterator i1,i2;
            
            for (ia = labelAllowed.begin(),dl = labelAllowed.end(); ia != dl; ++ia)
              {
                setTemp.clear();
                std::insert_iterator<std::set<T> >	ii(setTemp, setTemp.begin() );
                set_intersection(setLabel.begin(),setLabel.end(),ia->begin(),ia->end(),ii );
                interSize = setTemp.size(); 
                if ( interSize >= 2 )
                  matchedLabel.insert(std::pair<unsigned, std::set<T> >(interSize,*ia) ) ;
              }

            if (matchedLabel.size() == 1) 
              {
                im = matchedLabel.rbegin(); //best matched label
                tex.item(il->first) = im->second;
              }
            else
              {
                tex.item(il->first) = setBack;
                //if (matchedLabel.size() > 1)
                //std::cerr << "Cannot define the gyrus label" << std::endl;
              }
          }
        else
          {
            if ( il->second != setFor )
              tex.item(il->first) = setBack;
            else
              tex.item(il->first) = setFor;
          }
    }


  return (tex);
} 

// Dilate the gyrus s
// in the SKIZ
template <class T>
Texture<T> 
gyrusSeedDilationInSKIZ( const AimsSurface<3,Void> & mesh,
                          const Texture<T> & seed,
                          const Texture<T> & skiz,
                          const T & Back,
                          const T & For)
{

  const std::vector<Point3df>		& vert = mesh.vertex();
  unsigned				i, n = vert.size();
  Texture<T>                  tex(n);
  
  ASSERT( seed.nItem() == n || skiz.nItem() == n);

  for(i=0; i<n; ++i)
    if( skiz.item( i ) == Back )
      tex.item( i ) = seed.item(i);
    else
      tex.item( i ) = For;
      
  tex = MeshDilation( mesh,tex, Back, For,FLT_MAX,false);

  for(i=0; i<n; ++i)
    if( skiz.item( i ) != Back )
      tex.item ( i ) = seed.item( i );


  return (tex);
} 


// Extract the point of the border of a parcellisation
template <class T>
Texture<std::set<T> > 
MeshBorderVoronoi( const AimsSurface<3,Void> & mesh,
                   const Texture<T> & inittex,
                   const std::set<T> & setBack, const std::set<T> & setFor )
{

  const std::vector<Point3df>		& vert = mesh.vertex();
  const std::vector< AimsVector<uint,3> >	& poly = mesh.polygon();
  unsigned				i, n = vert.size();
  Texture<std::set<T> >                  tex(n);
  std::map <unsigned, std::set<T> >           labelNeigh;//vertex -> labels of neighbours 
  typename std::map <unsigned, std::set<T> >::iterator il,el;

  ASSERT( inittex.nItem() == n );

  // neighbours map
  std::map<unsigned, std::set<unsigned> >	neighbours;
  unsigned			v1, v2, v3;

  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
      labelNeigh[v1].insert( inittex.item(v2) );
      labelNeigh[v2].insert( inittex.item(v1) );
      labelNeigh[v1].insert( inittex.item(v3) );
      labelNeigh[v3].insert( inittex.item(v1) );
      labelNeigh[v3].insert( inittex.item(v2) );
      labelNeigh[v2].insert( inittex.item(v3) );
    }

   unsigned sizeNeigh;

  for ( il = labelNeigh.begin(), el = labelNeigh.end(); il!= el; ++il) 
    {
      sizeNeigh = (il->second).size();
      if ( sizeNeigh >= 2)
        tex.item(il->first) = setBack;
      else
        tex.item(il->first) = setFor;
    }


  return (tex);
} 






 
// Convert the texture of set to a texture of short labels
template <class T>
Texture<short> 
border2Texture( const Texture<std::set<T> > &tex,
                const AimsSurface<3,Void> & mesh,
                const std::set<T> & setBack, const std::set<T> & setFor)
{
  const std::vector<Point3df>		& vert = mesh.vertex();
  unsigned 				i, n = vert.size();;
  Texture<short> 			outtex(n);
  std::set<std::set<T>,SetCompare<T> >    labSet;
  typename std::set<std::set<T>,SetCompare<T> >::iterator il,el;
  std::map<std::set<T>,short,SetCompare<T> > newLab;
  unsigned 	inc = 0;

  for (i=0;i<n;++i)
      labSet.insert( tex.item(i) );
    
  //typename std::set<T>::iterator is,es;
  /*
  cout << "Label of the frontier \n";
  for (il=labSet.begin(), el = labSet.end(); il!=el;++il)
    { 
      for (is=il->begin(),es=il->end();is!=es;++is)
	cout << *is << " ";
      cout << std::endl;
    }
  */
  il=labSet.begin();
  if (*il == setFor)
    {
      newLab[*il] = -1;
      ++il;
    }
  if (*il == setBack)
    {
      newLab[*il] = 0 ;
      ++il;
    } 
  el=labSet.end();
  
  while(il!=el)
    {
      ++inc;
      newLab[*il]=inc;
      ++il;
    }
  
  for (i=0;i<n;++i)
    if ( tex.item(i).size() > 1 )
      outtex.item(i) = newLab[tex.item(i)];
    
  return(outtex);
}



template<class T, class U>
Texture<short> 
Voronoi2toTexture( const Texture<std::set<T> > & vor,
                   const AimsSurface<3,Void> & mesh,
                   const std::set<T> & setBack, const std::set<T> & setFor )
{
  std::set<std::set<U>,SetCompare<U> >	lab_front_sulci;
  typename std::set<std::set<U>,SetCompare<U> >::iterator il,el;
  std::set<U>				setTemp;
  typename std::set<T>::iterator	is,es;
  const std::vector<Point3df>		& vert = mesh.vertex();
  unsigned				i, n = vert.size();
  Texture<std::set<U> >			sTex(n);
  Texture<short>                        Tex(n);
  std::map<std::set<U>, short>			lab_set_short;
  //map<T,U>	  			cc = cc_sulci;
  ASSERT( n == vor.nItem() );

  for (i=0;i<n;++i)
    {
      setTemp.clear();
      for (is = vor.item(i).begin(), es = vor.item(i).end();is != es; ++is)
	setTemp.insert( *is );
      //setTemp.insert( cc[*is] );
      sTex.item(i) = setTemp; 
      lab_front_sulci.insert(setTemp); 
    }
  
  unsigned inc = 1;

  lab_set_short[setFor] = MESHDISTANCE_FORBIDDEN;
  lab_set_short[setBack] = 0; 
   
  for ( il=lab_front_sulci.begin(), el= lab_front_sulci.end(); il!=el;++il )
    if (*il !=  setFor && *il !=  setBack)
      {
	lab_set_short[*il] = inc;
	++inc;
      }
  /*
  cout << "Correspondance label frontier -> sulci\n";
  for ( il=lab_front_sulci.begin(), el= lab_front_sulci.end(); il!=el;++il )
    { 
      for (is = (*il).begin(), es = (*il).end(); is !=es; ++is)
	cout << *is << " " ;
      cout << " -> " << lab_set_short[*il] << std::endl;
    } 
  */
  for (i=0;i<n;++i)
    Tex.item(i) = lab_set_short[sTex.item(i)];

  return(Tex);

}



template<class T>
std::map<T,float> 
SurfaceParcel( const Texture<T> & tex, const AimsSurface<3,Void> & mesh )
{
  
  std::map<T,float>				stat;
  const std::vector<Point3df>			& vertex = mesh.vertex();
  unsigned 					i,n = vertex.size();
  const std::vector< AimsVector<uint,3> >	& poly = mesh.polygon();
  ASSERT( n == tex.nItem() );
  std::map<T,std::set<unsigned> >		labels;
  typename std::map<T,std::set<unsigned> >::iterator	il,el=labels.end();
  T						lab;
  for (i=0;i<n;++i)
      labels[tex.item(i)].insert(i);
 
  // blob surface 
  
  for (il=labels.begin();il != el; ++il)
    {
      std::set<unsigned> 	vset = il->second;
      lab = il->first;
      std::set<unsigned>::iterator 	evset = vset.end();
      float 			surface=0;
      Point3df       		zero( 0.0, 0.0, 0.0 ),AB,AC,H,CH;

      for( unsigned ii=0, n=poly.size(); ii!=n; ++ii )
	if( vset.find( poly[ii][0] ) != evset 
	    && vset.find( poly[ii][1] ) != evset 
	    && vset.find( poly[ii][2] ) != evset )
	  {
	    const AimsVector<uint,3>	& pol = poly[ii];
	    AB = vertex[ pol[ 1 ] ] - vertex[ pol[ 0 ] ];
	    AC = vertex[ pol[ 2 ] ] - vertex[ pol[ 0 ] ];
	    H  = vertex[ pol[ 0 ] ] + ( AB.dot( AC ) / norm2( AB ) ) * AB;
	    CH = H - vertex[ pol[ 2 ] ];
	    if ( AB != zero )
	      surface += norm( CH ) * norm( AB ) / 2;
	  } 
      stat[lab] = surface;
      //cout << lab << " -> " << surface << " mm2" << std::endl;
    }

  return(stat);
}





template<class T>
std::map<T,float> 
VolumeParcel( const AimsData<T> & vol )
{
  
  std::map<T,float>			stat;
  std::set<T> 				labels;
  typename std::set<T>::iterator	il,el;
  int    				x,y,z;
  //T					lab;
  float		voxelVol = vol.sizeX() * vol.sizeY() * vol.sizeZ();
  ForEach3d(vol,x,y,z)
      labels.insert(vol(x,y,z));

  for ( il = labels.begin(), el = labels.end(); il != el; ++il )
    stat[*il] = 0;
 
  ForEach3d(vol,x,y,z)
    stat[vol(x,y,z)] += voxelVol;



  return(stat);
}

  }
}

#endif
