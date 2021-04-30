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


#include <aims/distancemap/thickness.h>
#include <aims/distancemap/distancemap_g.h>
#include <aims/distancemap/projection.h>
#include <aims/distancemap/meshvoronoi_d.h>
#include <aims/io/writer.h>
#include <aims/connectivity/meshcc.h>
#include <aims/distancemap/meshdistance.h>
#include <aims/morphology/morphology_g.h>
#include <graph/graph/graph.h>
#include <iomanip>
#include <float.h>

 
using namespace aims::meshdistance;
using namespace aims;
using namespace std;



void aims::meshdistance::ClosestMeshTrianglesFromSulcusVoxel(const Point3df &pt,
							     const pair<Point3df,Point3df> & bipoint,
							     Point3df & n_sulci, float & dmn, float & dmp, 
							     float demin, float /* dpmin*/, 
							     const AimsSurface<3,Void> & mesh)
{
  const vector<Point3df>		& vert= mesh.vertex();
  const vector< AimsVector<uint,3> >     & poly = mesh.polygon();

  unsigned i, n = poly.size();
  float de ;
  dmn = FLT_MAX;
  dmp = FLT_MAX;
  Point3df dir;
  unsigned			v1, v2, v3;
  Point3df         pt_intersection, normal_tri;
  

  for ( i = 0; i < n; i++)
    {

      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];

      pt_intersection = IntersectionPointOfPlanAndLine( vert[v1],vert[v2],
                                                        vert[v3],
                                                        bipoint.second, pt );
      dir = pt - pt_intersection;
      de = dir.norm();

      if( cross(vert[v1] - vert[v2], vert[v1] - pt_intersection)
            .dot(cross(vert[v1] - pt_intersection,vert[v1] - vert[v3]) ) >= 0
          && cross(vert[v2] - vert[v3], vert[v2] - pt_intersection)
            .dot(cross(vert[v2] - pt_intersection,vert[v2] - vert[v1]) ) >= 0
          && cross(vert[v3] - vert[v1], vert[v3] - pt_intersection)
            .dot(cross(vert[v3] - pt_intersection,vert[v3] - vert[v2]) ) >= 0
            && de < demin )
      {
        if ( dir.dot(n_sulci) < 0  )//sulcus wall side according to n_sulci
        {
          if ( de  < dmn )
            dmn = de;

        }
        else
          if ( de  < dmp )
            dmp = de;
      }
    }

   if (dmp == FLT_MAX ||  dmn == FLT_MAX)
    {
      dmp = demin;
      dmn = demin;
    }

}



void aims::meshdistance::ClosestMeshNodesFromSulcusVoxel(const Point3df &pt,
							 const pair<Point3df,Point3df> & bipoint,
							 Point3df & n0, float demin, float dpmin, 
							 float & dmn, float & dmp,
							 const AimsSurface<3,Void> & mesh)
{
  const vector<Point3df>		& normal = mesh.normal();
  const vector<Point3df>		& vert= mesh.vertex();
  unsigned i, n = vert.size();
  float de , dp;
  dmn = FLT_MAX;
  dmp = FLT_MAX;
  Point3df pos,dir;
  
  for ( i = 0; i < n; i++)
    {
      pos = vert[i];
      dir = pos - pt;
      de = dir.norm();
      dp = DistancePointToLine(pos,bipoint); 	
      if (de < demin 
	  && dp < dpmin 
	  && dir.dot(normal[i]) < 0)//only for close pt and on the sulci walls
	{
	  if ( dir.dot(n0) < 0  )//sulcus wall side according to n0 
	    {
	      if ( de  < dmn ) 
		  dmn = de;
		  
	    }
	  else
	    if ( de  < dmp ) 
		dmp = de;
	}
    }
	     
  if (dmp == FLT_MAX ||  dmn == FLT_MAX)
    {
      dmp = demin;
      dmn = demin;
    } 

}


void aims::meshdistance::CCOperture(const vector<Point3df>    &cc,
					      const AimsSurface<3,Void> & mesh,
					      const  Point3dfNeigh &neigh,
					      std::map<Point3df, pair<float,float> , Point3dfCompare >& sulci2mesh,
					      const float demin, const float dpmin)
{

  
  Point3df                              PtR, pos,dir,normal_plan,n_sulci = Point3df(0,0,0);
  pair<Point3df,Point3df>               bipoint;
  Point3dfSet                         setpoint;
   Point3dfNeigh::const_iterator  is,es = neigh.end();
  unsigned nb = 0, MIN_NEIGH = 6;//min nub of neighbours in the sulcus to estime the normal
  vector<Point3df>::const_iterator icc, ecc = cc.end();
  float dmn,dmp;
    
  n_sulci = meanNormalofCC(cc,neigh,MIN_NEIGH);
  
  for(icc = cc.begin(); icc != ecc; ++icc)
    {
	
      PtR = *icc;
      is = neigh.find(PtR);
      if (is != es)
	{
	  if (is->second.size() > MIN_NEIGH)
	    normal_plan = NormalFromPoints(is->first,is->second); //normal to the pt
	  else
	    cerr << "Not enough point to estime the normal at the point " << PtR << " \n";
	}
      else
	cerr << "Pb3: cannot find the neighbours of the point " << PtR <<"\n";
	
      normal_plan *= normal_plan.dot(n_sulci); //put the normal in the same orientation as n_sulci
      normal_plan.normalize();
	
      if (is->second.size() > MIN_NEIGH) 
	{
	  //def of the line normal to the sulci passing through the current pt
	  bipoint = pair<Point3df,Point3df>(PtR,normal_plan); 

	  //distance to the mesh
	  ClosestMeshTrianglesFromSulcusVoxel(PtR,bipoint,n_sulci,dmn,dmp,demin,dpmin,mesh);
	  //ClosestMeshNodesFromSulcusVoxel(PtR,bipoint,n_sulci,demin,dpmin,dmn,dmp,mesh);
	  sulci2mesh[PtR] = pair<float,float>(dmn,dmp) ;
	  ++ nb;
	}
    }
  cout << nb << " / " << cc.size() << " pt processed \n";
}



void  aims::meshdistance::SulcusOperture( const AimsSurface<3,Void> & mesh, 
					  map<Point3df, pair<float,float> , Point3dfCompare > &sulci2mesh,
					  const AimsData <short> & surface_vol,
					  const float demin, const float dpmin, const unsigned MINCC,
					  const map <short,string> & trans,
					  const set<string>	& labels)
{

  long    				x,y,z;
  map<unsigned,Point3dfSet > 	cc_sulci_coord,sulci_coord; //label->sulci(voxel or tri)
  map<Point3df, Point3df, Point3dfCompare >		initend;//voxel->tri
  map<short,vector<Point3df> >				cc_labels;//short labels of the sulci/connex components
  map<short,vector<Point3df> >::iterator			ilab,elab;
  map <short,string>::const_iterator	flab,glab = trans.end();
  AimsData<short>			svol = surface_vol.clone() ;
  set<string>::const_iterator		iclab,eclab;
  float sx = surface_vol.sizeX(),sy = surface_vol.sizeY(),sz = surface_vol.sizeZ() ;

  // Label filtering
  cout << "Selecting the sulci to be studied\n";
  eclab = labels.end();

  ForEach3d(svol,x,y,z)
    if (svol(x,y,z) != 0)
      {  
	flab = trans.find( svol(x,y,z) );
	if (flab != glab)
	  {
	    iclab = labels.find( flab->second );
	    if ( iclab == eclab || *iclab == "unknown"  )
	      svol( x , y , z ) = 0;
	  }	  
	else
	  cout << "the label " << svol(x,y,z) << "cannot be translated\n";
      }

  
  // Extract Connected Components
  AimsData<short>     ccvol = svol.clone() ;
  cout << "Extracting connex components (with at least " << MINCC << " points in each components)" << endl;
  AimsConnectedComponent<short>( ccvol,
                                 aims::Connectivity::CONNECTIVITY_26_XYZ,
                                 short(0), short(0), false, MINCC );
  cc_labels.clear();
  ForEach3d(ccvol,x,y,z)
      if (ccvol(x,y,z) != 0)
	cc_labels[ccvol(x,y,z)].push_back(Point3df(x*sx,y*sy,z*sz));
 int ncc = cc_labels.size(),nc = 1;

  //Neighbourhood
  cout << "Find the neighbourhood of each point in the cc sulci"<< endl;
   Point3dfNeigh neigh;
  for (ilab=cc_labels.begin(),elab=cc_labels.end();ilab != elab; ++ilab)
      NeighbourInCC(neigh ,ccvol,ccvol,aims::Connectivity::CONNECTIVITY_26_XYZ,ilab->first,3);


  //Operture for the cc 
  nc = 1;
  for (ilab=cc_labels.begin(),elab=cc_labels.end();ilab != elab; ++ilab)
    {
      cout << nc << " / " << ncc << endl;
      ++nc;
      CCOperture(ilab->second,mesh, neigh, sulci2mesh,demin,dpmin);
    }
}



AimsData<float> aims::meshdistance::OpertureParameters(const AimsSurface<3,Void> & /* mesh */, 
					    const map<Point3df, pair<float,float> , Point3dfCompare > &sulci2mesh,
					    const AimsData<short> & vol)
{

  Point3d Pt;
  Point3df PtR;
  map<Point3df, pair<float,float> , Point3dfCompare >::const_iterator ig,eg =sulci2mesh.end() ;
  float sx = vol.sizeX(),   sy = vol.sizeY(), sz = vol.sizeZ();  
  float de;
  map<short,set<float> >::iterator io,eo;
  AimsData<float> ovol(vol.dimX(),vol.dimY(),vol.dimZ(),1);
  map<short,set<float> > operture;

  for (ig = sulci2mesh.begin(); ig != eg; ++ig)
    {
      PtR = ig->first; //sulci pt
      Pt = Point3d((short)(PtR[0]/sx),(short)(PtR[1]/sy),(short)(PtR[2]/sz));
      de = (ig->second).first + (ig->second).second; //white operture
      operture[vol(Pt)].insert(de);
      ovol(Pt) = de ;
    }
  
  set<float>::iterator iv,ev;

  for (io = operture.begin(), eo = operture.end(); io != eo; ++io)
    {
      cout << "Mean operture of the label " << io->first << endl;
      for (iv = (io->second).begin(), ev = (io->second).end(); iv != ev; ++iv )
	cout << *iv  << " " ;
      cout << endl;
    }

  return (ovol);
}








std::set<unsigned> aims::meshdistance::SubMesh(const AimsSurface<3,Void> & mesh,
							const Point3df &bbmin, const Point3df & bbmax)
{

  set<unsigned> points;
  const vector<Point3df>		& vert= mesh.vertex();
  unsigned i,n = vert.size();
  Point3df pt;
  
  for ( i = 0; i < n; ++i )
    {
      pt = vert[i];
      if ( pt[0] < bbmax[0] &&
	   pt[1] < bbmax[1] &&
	   pt[2] < bbmax[2] &&
	   pt[0] > bbmin[0] &&
	   pt[1] > bbmin[1] &&
	   pt[2] > bbmin[2] )
	points.insert(i);
    }  
  return (points);

}
