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


#ifndef AIMS_DISTANCEMAP_PROJECTION_H
#define AIMS_DISTANCEMAP_PROJECTION_H

#include <cstdlib>
#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <aims/data/data.h>
#include <aims/connectivity/connectivity.h>
#include <aims/distancemap/stlsort.h>
#include <aims/math/gausslu.h>
#include <aims/math/eigen.h>
#include <set>
#include <list>
#include <map>
#include <float.h>


class Graph;
namespace aims
{
  namespace meshdistance
  {
    

    /** Projection of the voxels relative to the bottom of the sulci to the 
	edges of a triangulation.
	The curvature of the triangulation is taken into account.
	The label of the projected sulci is the same as the initial label.
	Use a curvature or a depth map to project the sulcal line */
    TimeTexture<short> 
    SulcusVolume2Texture( const AimsSurface<3,Void> & mesh, 
			  const Texture<float> & curvtex,
			  const AimsData <short> & bottom_vol,
			  const AimsData <short> & surface_vol,
			  float alpha, float dmin, 
			  int MINCC,
			  const std::map <short,std::string> & trans,
			  const std::set<std::string>	& labels,
			  float maxdil,float maxdil_mesh,float alpha_reg,
                          bool connexity,
			  const std::vector<std::list<unsigned> >  & neigho);
    /// Use interpolation plane to project the sulcal line
    TimeTexture<short> 
    SulcusVolume2Texture( const AimsSurface<3,Void> & mesh, 
			  const AimsData <short> & bottom_vol,
			  const AimsData <short> & surface_vol,
			  const Point3df & CA,
			  float demin,float dpmin, 
			  int MINCC,
			  const std::map <short,std::string> & trans,
			  const std::set<std::string>	& labels,
			  float maxdil,float maxdil_mesh,float alpha_reg,
                          bool connexity,
			  const std::vector<std::list<unsigned> >  & neigho);


    Texture<short> 
    FirstSulciProjectionWithCurvatureMap(const AimsData<short> &ccvol,
					 const AimsData<short>    &sulcvol,
					 const AimsSurface<3,Void> & mesh,
					 std::set<short>  &cc_sulci_labels,
					 std::map<short,short > &tri_sulci,
					 std::map<unsigned, Point3dfSet > &cc_sulci_coord,
					 std::map<Point3df, Point3df, 
					 Point3dfCompare > &initend,
					 const Texture<float> &curvtex,
					 float alpha,float dmin);

    Texture<short> 
    FirstSulciProjectionWithInterpolationPlane
    ( const AimsData<short> &ccvol, const AimsData<short> &sulcvol,
      const AimsSurface<3,Void> & mesh,
      std::set<short> &cc_sulci_labels,
      std::map<short,short > &tri_sulci,
      const std::map<Point3df, Point3dfSet, 
      Point3dfCompare > &neigh,
      std::map<unsigned, Point3dfSet > &cc_sulci_coord,
      std::map<Point3df, Point3df, Point3dfCompare > &initend,
      float demin, float dpmin,
      short label_insula_left,short label_insula_right,
      const Point3df & CA );

    void 
    AffinePointProjection( Texture<short> &tex,
			   const AimsSurface<3,Void> & mesh,
			   const std::map<Point3df, Point3df, Point3dfCompare >
			   &initend,
			   const Point3dfSet &setpointi,
			   short label,
			   float dmin,float alpha_reg );

    AimsData<short> MeshParcellation2Volume(const AimsData<short>    &initVol,
					    const Texture<short> &tex,
					    const AimsSurface<3,Void> & mesh,
					    short val_domain, 
					    short back);

    Texture<short> 
    VolumeParcellation2MeshParcellation(const AimsData<short>    &initVol,
                                        const AimsSurface<3,Void> & mesh,
                                        short back);

    /** Give the neighbours of a point
	The neighbours have the same label as the point */
    void 
    NeighbourInCC(std::map<Point3df, Point3dfSet, Point3dfCompare > &neigh,
		  const AimsData <short> & bvol,const AimsData <short> & svol, 
		  Connectivity::Type connectivity,
		  short label, const unsigned size_neigh, 
                  unsigned max_points = 50);

    
    
    //inline functions
    
    //first : barycenter of the input pt
    //second : normal vector

    inline std::pair<Point3df,Point3df> NormalFromPoints( const Point3dfSet  & init_point )
    {
      std::pair<Point3df,Point3df>  output;
      unsigned inc = 0, i=0,j=0,n = init_point.size();
      Point3df mean = Point3df(0,0,0),temp;
      Point3dfSet norm_point;
      AimsData<float> cov(3,3),w(3) ;
      Point3dfSet::const_iterator is,es;
  
      for (is = init_point.begin(), es = init_point.end(); is != es; ++is)
	mean += *is;

      mean = mean / (float)n;

      for (is = init_point.begin(), es = init_point.end(); is != es; ++is, 
             ++inc)
	norm_point.insert(*is - mean);
  
      cov = 0;
      for (is = norm_point.begin(), es = norm_point.end(); is != es; ++is, 
             ++inc)
	{
	  temp = *is;
	  for (i=0; i<3; ++i)
	    for (j=0; j<3; ++j)
	      cov(i,j) += temp[i] * temp[j];  
	} 
      AimsEigen< float > eigen;
      AimsData< float > value = eigen.doit( cov );

      eigen.sort(cov,value);
  
      temp[0] = cov(0,2);
      temp[1] = cov(1,2);
      temp[2] = cov(2,2);

      temp.normalize();
      output.second = temp;
      output.first = mean;
  
      return(output);  
    }

    //Point3df : normal vector to the plane passing through pt
    inline Point3df NormalFromPoints( const Point3df & pt,
				      const Point3dfSet  & init_point )
    {
      unsigned                 inc = 0, i=0,j=0;
      Point3df                 temp;
      Point3dfSet norm_point;
      AimsData<float> cov(3,3),w(3) ;
      Point3dfSet::const_iterator is,es;
  
  

      for (is = init_point.begin(), es = init_point.end(); is != es; ++is, ++inc)
	norm_point.insert(*is - pt);
  
      cov = 0;
      for (is = norm_point.begin(), es = norm_point.end(); is != es; ++is, ++inc)
	{
	  temp = *is;
	  for (i=0; i<3; ++i)
	    for (j=0; j<3; ++j)
	      cov(i,j) += temp[i] * temp[j];  
	} 
      AimsEigen< float > eigen;
      AimsData< float > value = eigen.doit( cov );

      eigen.sort(cov,value);
  
      temp[0] = cov(0,2);
      temp[1] = cov(1,2);
      temp[2] = cov(2,2);

      temp.normalize();
  
  
      return(temp);  
    }

    inline  Point3df cross(const Point3df &A, const Point3df &B)
    {
      Point3df C;
      C[0] = A[1] * B[2] - A[2] * B[1];
      C[1] = A[2] * B[0] - A[0] * B[2]; 
      C[2] = A[0] * B[1] - A[1] * B[0];
      return (C);
      
    }
    

    //Give the point of intersection 
    //between a line 
    //(defined by a point 'pt' 
    //and a direction  'direction_line' )
    // and a plane
    //(defined by 3 points s1, s2, s3)
    inline Point3df IntersectionPointOfPlanAndLine(const Point3df &s1,const Point3df &s2,const Point3df &s3,
						   const Point3df & direction_line, const Point3df & pt)
    {
      float a0, b0, c0;
      float xb,yb,zb;
      float a,b,c;
      float xm,ym,zm;
      Point3df         pt_intersection, normal_tri;

      
      ASSERT(direction_line != Point3df(0,0,0));
      normal_tri = cross(s1 - s2,s1 - s3);
      normal_tri.normalize();

      //mesh triangle normal 
      a0 = normal_tri[0];
      b0 = normal_tri[1];
      c0 = normal_tri[2];

      //one node of the triangle
      xb = s1[0];
      yb = s1[1];
      zb = s1[2];
      
      //direction of the line 
      a = direction_line[0];
      b = direction_line[1];
      c = direction_line[2];

      //coord of the pt defining the line 
      xm = pt[0];
      ym = pt[1];
      zm = pt[2];

      pt_intersection[0] = (c *  c0 * xm + 
			    a * c0 * zb - 
			    a * b0 * ym + 
			    a * a0 * xb + 
			    a * b0 * yb + 
			    b0 * b * xm - 
			    c0 * a * zm) /
	( c * c0 + a * a0 + b0 * b);
		    

      pt_intersection[1] = ( b * c0 * zb + 
                             b * a0 * xb + 
                             b * b0 * yb - 
                             b * xm * a0 + 
                             c * ym * c0 + 
                             ym * a * a0 - 
                             b * zm * c0 ) / 
	(c * c0 + a * a0 + b0 * b);
	
      pt_intersection[2] = ( c * c0 * zb 
			     - b0 * c * ym 
			     + c * a0 * xb 
			     + c * b0 * yb 
			     + a * zm * a0 
			     + b0 * b * zm 
			     - c * xm * a0 ) /
	(c * c0 + a * a0 + b0 * b);

      return (pt_intersection);
    }
    

    /* Distance from a point (A) to the plane defined by 
       one of its point : bipoint.first
       its normal (norm = 1):  bipoint.second */
    inline float DistancePointToPlane(const Point3df &A, const std::pair<Point3df,Point3df> &plane )
    {

      float d;
      Point3df temp = plane.first - A;
      d = fabs( temp.dot(plane.second) );

      return(d);
    }

    
    /* Distance from a point (A) to a line defined by 
       one of its point : bipoint.first
       its direction vector (norm = 1):  bipoint.second */
    inline float DistancePointToLine(const Point3df &A, const std::pair<Point3df,Point3df> &line )
    {

      float d;
      Point3df temp = A - line.first;
      d = cross(temp,line.second).norm();

      return(d);
    }

    //Compute the mean normal n0 of the cc
    // n1 fix the sens of each normal
    // Only pt with a least MIN_NEIGH neighbours are taken into account
    // It avoid considering extremal points whose normal estimation is bad
    // The normal of these pt can be seen as outliers
    inline Point3df meanNormalofCC(const std::vector<Point3df> &cc,
				   const Point3dfNeigh & neigh,
				   unsigned MIN_NEIGH)
    {
      Point3df                              PtR,n0 = Point3df(0,0,0),n1= Point3df(0,0,0),normal_plan;
      Point3dfNeigh::const_iterator  is,es = neigh.end();
      std::vector<Point3df>::const_iterator icc,ecc = cc.end();
      bool ok1 = false;
      //Fix an arbitrary orientation (n1) for the normals of each pt of the cc
      icc = cc.begin();
      while (!ok1 && icc != ecc)
	{
	  PtR = *icc;
	  is = neigh.find(PtR);
	  if (is != es)
	    {
	      if (is->second.size() > MIN_NEIGH)
		{
		  ok1 = false;	   
		  normal_plan = NormalFromPoints(is->first,is->second);
		}
	    }
	  else
	    std::cerr << "Pb1: cannot find the neighbours of the point " 
		      << PtR <<"\n";
	  ++icc;
	}
      n1 = normal_plan;

      for(icc = cc.begin(); icc != ecc; ++icc)
	{
	  PtR = *icc;
	  is = neigh.find(PtR);
	  if (is != es)
	    {
	      if (is->second.size() > MIN_NEIGH)
		normal_plan = NormalFromPoints(is->first,is->second);
	    }
	  else
	    std::cerr << "Pb2: cannot find the neighbours of the point " 
		      << PtR <<"\n";
	  normal_plan *= normal_plan.dot(n1);
	  normal_plan.normalize();
	  n0 += normal_plan ;
	}	
	
      n0.normalize();
      std::cout << "CC mean normal : "<< n0 << std::endl;
  
      return (n0);
    }
    
   

    /// global variable...
    extern const short MESHDISTANCE_FORBIDDEN;
    extern const short MESHDISTANCE_UNREACHED;

  }
}


#endif
