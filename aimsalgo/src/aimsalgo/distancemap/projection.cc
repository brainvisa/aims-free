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


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/distancemap/projection.h>
#include <aims/distancemap/meshmorphomat.h>
#include <aims/distancemap/voronoi.h>
#include <aims/distancemap/meshvoronoi.h>
#include <aims/morphology/operatormorpho.h>


using namespace aims::meshdistance;
using namespace aims;
using namespace carto;
using namespace std;


//Projection with depth/curvature info
TimeTexture<short>  aims::meshdistance::SulcusVolume2Texture(
  const AimsSurface<3,Void> & mesh,
  const Texture<float> & curvtex,
  const rc_ptr<Volume<short> > & bottom_vol,
  const rc_ptr<Volume<short> > & surface_vol,
  float alpha, float dmin,
  int MINCC,
  const map <short,string> & trans,
  const set<string>	& labels,
  float maxdil,float maxdil_mesh,float alpha_reg,bool connexity,
  const vector<list<unsigned> >  & neigho)
{

  int    				x,y,z;
  const vector<Point3df>		& vert = mesh.vertex();
  unsigned				i, n = vert.size();
  TimeTexture<short>			sulctex(3,n);
  Point3df				pos,dir;
  map<unsigned, Point3dfSet > 	cc_sulci_coord,sulci_coord; //label->sulci(voxel or tri)
  map<Point3df, Point3df, Point3dfCompare >		initend;//voxel->tri
  set<short>				sulci_labels,cc_sulci_labels;//short labels of the sulci/connex components
  set<short>::iterator			ilab,elab;
  map <short,string>::const_iterator	flab,glab = trans.end();
  VolumeRef<short> sulcvol = VolumeRef<short>( bottom_vol ).deepcopy(),
    svol = VolumeRef<short>( surface_vol ).deepcopy();
  map<short,short>			tri_sulci;//tri_labels ->sulci_label
  set<string>::const_iterator		iclab,eclab;
  vector<int> bvdim = sulcvol.getSize();
  vector<int> svdim = svol.getSize();

  ASSERT( curvtex.nItem() == n );
 
  //-----------------------------------------------------------------------------------------------------
  //                                         Label filtering
  //-----------------------------------------------------------------------------------------------------
  cout << "Selecting the sulci to be projected\n";
  eclab = labels.end();
  for( z=0; z<bvdim[2]; ++z )
    for( y=0; y<bvdim[1]; ++y )
      for( x=0; x<bvdim[0]; ++x )
        if (sulcvol(x,y,z) != 0)
        {
          flab = trans.find( sulcvol(x,y,z) );
          if (flab != glab)
          {
            iclab = labels.find( flab->second );
            if ( iclab == eclab || *iclab == "unknown"  )
              sulcvol( x , y , z ) = 0;
            else
              sulci_labels.insert( sulcvol(x,y,z) );
          }
          else
            cerr << "the label " << sulcvol(x,y,z) << "cannot be translated\n";
        }

  for( z=0; z<svdim[2]; ++z )
    for( y=0; y<svdim[1]; ++y )
      for( x=0; x<svdim[0]; ++x )
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
            cerr << "the label " << svol(x,y,z) << "cannot be translated\n";
        }

  //-----------------------------------------------------------------------------------------------------
  //                                   Extract Connex components	   
  //-----------------------------------------------------------------------------------------------------
  VolumeRef<short>    ccvol = sulcvol.deepcopy();
  VolumeRef<short>    tempvol = sulcvol.deepcopy();
  float radius_close ;
  radius_close = maxdil;
  cout << "Voxeled Sulcal lines closing \n";
  for (ilab=sulci_labels.begin(),elab=sulci_labels.end();ilab != elab; ++ilab)
  {
    cout << "Label: " << trans.find(*ilab)->second << endl;
    for( z=0; z<bvdim[2]; ++z )
      for( y=0; y<bvdim[1]; ++y )
        for( x=0; x<bvdim[0]; ++x )
          if (sulcvol(x,y,z) == *ilab)
            tempvol(x,y,z) = 1;
          else
            tempvol(x,y,z) = 0;

    tempvol = AimsMorphoClosing<short>(tempvol,radius_close);
    for( z=0; z<bvdim[2]; ++z )
      for( y=0; y<bvdim[1]; ++y )
        for( x=0; x<bvdim[0]; ++x )
          if (tempvol(x,y,z) != 0 )
            ccvol(x,y,z) = *ilab;
  }

  sulcvol = ccvol;

  ccvol = svol.deepcopy();
  cout << "Voxeled Sulcal surface closing \n";
  for (ilab=sulci_labels.begin(),elab=sulci_labels.end();ilab != elab; ++ilab)
  {
    cout << "Label: " << trans.find(*ilab)->second << endl;
    for( z=0; z<svdim[2]; ++z )
      for( y=0; y<svdim[1]; ++y )
        for( x=0; x<svdim[0]; ++x )
          if (svol(x,y,z) == *ilab)
            tempvol(x,y,z) = 1;
          else
            tempvol(x,y,z) = 0;

    tempvol = AimsMorphoClosing<short>(tempvol,radius_close);
    for( z=0; z<svdim[2]; ++z )
      for( y=0; y<svdim[1]; ++y )
        for( x=0; x<svdim[0]; ++x )
          if (tempvol(x,y,z) != 0 )
            ccvol(x,y,z) = *ilab;
  }

  *svol = *ccvol;

  //-----------------------------------------------------------------------------------------------------
  //                             Compute neighbourhood of each sulcal line
  //-----------------------------------------------------------------------------------------------------

  cout << "Find the neighbourhood of each bottom point in the sulci"<< endl;
  Point3dfNeigh neigh;
  Point3dfSet                              cc;
  for (ilab=sulci_labels.begin(),elab=sulci_labels.end();ilab != elab; ++ilab)
    NeighbourInCC(
      neigh ,sulcvol,svol,aims::Connectivity::CONNECTIVITY_26_XYZ,*ilab,1);

  cout << "Extracting connex components (with at least " << MINCC << " points in each components)" << endl;
  *ccvol = *sulcvol;
  AimsConnectedComponent<short>( ccvol,
                                 aims::Connectivity::CONNECTIVITY_26_XYZ,
                                 (short)0, false, (size_t) MINCC );
  
  //-----------------------------------------------------------------------------------------------------
  //           First projection - Closest points according to euclidean and curvature/depth
  //-----------------------------------------------------------------------------------------------------
  sulctex[2] = FirstSulciProjectionWithCurvatureMap(ccvol, sulcvol, mesh, cc_sulci_labels,tri_sulci,cc_sulci_coord,initend,curvtex,alpha,dmin);
 
  //-----------------------------------------------------------------------------------------------------
  //                                      Estimation of affine projection
  //-----------------------------------------------------------------------------------------------------
  cc_sulci_labels.erase(0);
  unsigned		inc2=1;
  cout << "Estimation of affine transformation...\n";
  for (ilab = cc_sulci_labels.begin(), elab = cc_sulci_labels.end() ; ilab != elab; ++ilab)
  {
    Point3dfSet		&setpointi= cc_sulci_coord[*ilab],setpointf;
    flab = trans.find(tri_sulci[*ilab]);
    cout << endl << inc2 << "th connected component(" << tri_sulci[*ilab] << ": " << flab->second << ")" <<  endl;
    ++inc2;

    AffinePointProjection(
      sulctex[1],mesh,initend,setpointi,*ilab,dmin,alpha_reg);
  }
  cout << endl;
  

  //-----------------------------------------------------------------------------------------------------
  //                                        Closing of each projected cc
  //-----------------------------------------------------------------------------------------------------
  cout << "Closing the sulci\n";
  sulctex[0] = CloseSulci(mesh,sulctex[1],maxdil_mesh,connexity,(short)0,(short)-1,neigho); //linearize cc projected 2*

  Texture<short> & tex0 = sulctex[0];

  for (i=0;i<n;++i)
    if( tex0.item(i) != 0 )
      tex0.item(i) = tri_sulci[tex0.item(i)];

  return sulctex;
}




//projection with interpolate plane
TimeTexture<short>
aims::meshdistance::SulcusVolume2Texture(
  const AimsSurface<3,Void> & mesh,
  const rc_ptr<Volume<short> > & bottom_vol,
  const rc_ptr<Volume<short> > & surface_vol,
  const Point3df &CA,
  float demin,float dpmin,
  int MINCC,
  const map <short,string> & trans,
  const set<string>	& labels,
  float /*maxdil*/,float maxdil_mesh,float alpha_reg,bool connexity,
  const vector<list<unsigned> >  & neigho)
{

  int    				x,y,z;
  const vector<Point3df>		& vert = mesh.vertex();
  unsigned				i, n = vert.size();
  TimeTexture<short>			sulctex(3,n);
  Point3df				pos,dir;
  map<unsigned, Point3dfSet > 	cc_sulci_coord,sulci_coord; //label->sulci(voxel or tri)
  map<Point3df, Point3df, Point3dfCompare >		initend;//voxel->tri
  set<short>				sulci_labels,cc_sulci_labels;//short labels of the sulci/connex components
  set<short>::iterator			ilab,elab;
  map <short,string>::const_iterator	flab,glab = trans.end();
  VolumeRef<short> sulcvol = VolumeRef<short>( bottom_vol ).deepcopy(),
    svol = VolumeRef<short>( surface_vol ).deepcopy();
  map<short,short >			tri_sulci;//cc_labels ->sulci_label
  set<string>::const_iterator		iclab,eclab;

  vector<int> bvdim = sulcvol.getSize();
  vector<int> svdim = svol.getSize();

  //-----------------------------------------------------------------------------------------------------
  //                                         Label filtering
  //-----------------------------------------------------------------------------------------------------
  cout << "Selecting the sulci to be projected\n";
  eclab = labels.end();

  for( z=0; z<bvdim[2]; ++z )
    for( y=0; y<bvdim[1]; ++y )
      for( x=0; x<bvdim[0]; ++x )
        if (sulcvol(x,y,z) != 0)
        {
          flab = trans.find( sulcvol(x,y,z) );
          if (flab != glab)
          {
            iclab = labels.find( flab->second );
            if ( iclab == eclab ) //|| *iclab == "unknown"  )
              sulcvol( x , y , z ) = 0;
            else
              sulci_labels.insert( sulcvol(x,y,z) );
          }
          else
            cerr << "the label " << sulcvol(x,y,z) << "cannot be translated\n";
        }

  for( z=0; z<svdim[2]; ++z )
    for( y=0; y<svdim[1]; ++y )
      for( x=0; x<svdim[0]; ++x )
        if (svol(x,y,z) != 0)
        {
          flab = trans.find( svol(x,y,z) );
          if (flab != glab)
          {
            iclab = labels.find( flab->second );
            if ( iclab == eclab ) //|| *iclab == "unknown"  )
              svol( x , y , z ) = 0;

          }
          else
            cerr << "the label " << svol(x,y,z) << "cannot be translated\n";
        }

  //-----------------------------------------------------------------------------------------------------
  //                                   	  Closing  
  //-----------------------------------------------------------------------------------------------------
  VolumeRef<short>    ccvol = sulcvol.deepcopy();
  VolumeRef<short>    tempvol= sulcvol.deepcopy();
  //float radius_close ;
  //radius_close = maxdil;

  set<string>::iterator is,es = labels.end();
 
  cout << "Closing Sulcal lines  \n";
  for( ilab=sulci_labels.begin(),elab=sulci_labels.end();ilab != elab;
        ++ilab )
    if( trans.find(*ilab)->second.substr(0, 6) != "INSULA" )
    {
      cout << "Label: " << trans.find(*ilab)->second << endl;
      for( z=0; z<bvdim[2]; ++z )
        for( y=0; y<bvdim[1]; ++y )
          for( x=0; x<bvdim[0]; ++x )
            if (sulcvol(x,y,z) == *ilab)
              tempvol(x,y,z) = 1;
            else
              tempvol(x,y,z) = 0;

      //tempvol = AimsMorphoConnectivityChamferDilation(tempvol,radius_close,aims::Connectivity::CONNECTIVITY_26_XYZ);
      //tempvol = AimsMorphoConnectivityChamferHomotopicErosion(tempvol,radius_close,aims::Connectivity::CONNECTIVITY_26_XYZ);

      for( z=0; z<bvdim[2]; ++z )
        for( y=0; y<bvdim[1]; ++y )
          for( x=0; x<bvdim[0]; ++x )
            if (tempvol(x,y,z) != 0 )
              ccvol(x,y,z) = *ilab;

    }

  sulcvol = ccvol;

  ccvol = svol.deepcopy();

  cout << "Closing Sulcal surface \n";
  for( ilab=sulci_labels.begin(),elab=sulci_labels.end();
        ilab != elab; ++ilab )
  {
    cout << "Label: " << trans.find(*ilab)->second << endl;
    for( z=0; z<svdim[2]; ++z )
      for( y=0; y<svdim[1]; ++y )
        for( x=0; x<svdim[0]; ++x )
          if (svol(x,y,z) == *ilab)
            tempvol(x,y,z) = 1;
          else
            tempvol(x,y,z) = 0;

    //tempvol = AimsMorphoChamferDilation(tempvol,radius_close,aims::Connectivity::CONNECTIVITY_26_XYZ);
    //tempvol = AimsMorphoConnectivityChamferHomotopicErosion(tempvol,radius_close,aims::Connectivity::CONNECTIVITY_26_XYZ);


    for( z=0; z<svdim[2]; ++z )
      for( y=0; y<svdim[1]; ++y )
        for( x=0; x<svdim[0]; ++x )
          if (tempvol(x,y,z) != 0 )
            ccvol(x,y,z) = *ilab;
  }


  //Sulcal surface and bottom point in svol
  for( z=0; z<bvdim[2]; ++z )
    for( y=0; y<bvdim[1]; ++y )
      for( x=0; x<bvdim[0]; ++x )
      {
        svol(x,y,z) = ccvol(x,y,z);
        if(sulcvol(x,y,z)!=0)
          svol(x,y,z)=sulcvol(x,y,z);
      }

  //bottom volume for INSULA :
  //bottom pt projected in the direction of the tangent plane
  //+ pt of the surface projected on the direction of the normal of the tgt plane 
  short label_insula_left=0,label_insula_right=0;
  is = labels.find("INSULA_left");
  if (is != es)
  {
    cout << "Some points have the label 'INSULA'" << endl;
    for (flab = trans.begin();flab != glab; ++flab)
    {
      if (flab->second == "INSULA_left")
        label_insula_left = flab->first;
      if (flab->second == "INSULA_right")
        label_insula_right = flab->first;
    }
    for( z=0; z<svdim[2]; ++z )
      for( y=0; y<svdim[1]; ++y )
        for( x=0; x<svdim[0]; ++x )
          if (svol(x,y,z) == label_insula_left)
            sulcvol(x,y,z) = label_insula_left;
          else
            if (svol(x,y,z) == label_insula_right)
              sulcvol(x,y,z) = label_insula_right;
  }
  
  //-----------------------------------------------------------------------------------------------------
  //                             Compute neighbourhood of each sulcal line
  //-----------------------------------------------------------------------------------------------------

  cout << "Find the neighbourhood of each bottom point in the sulci"<< endl;
  Point3dfNeigh neigh;

  Point3dfSet                              cc;
  unsigned NEIGH_SIZE = 1;
  for (ilab=sulci_labels.begin(),elab=sulci_labels.end();ilab != elab; ++ilab)
    NeighbourInCC( neigh , sulcvol, svol,
                   aims::Connectivity::CONNECTIVITY_26_XYZ, *ilab,
                   NEIGH_SIZE );

  //-----------------------------------------------------------------------------------------------------
  //                             Extract connected components
  //-----------------------------------------------------------------------------------------------------

 
  cout << "Extracting connex components (with at least " << MINCC << " points in each components)" << endl;
  ccvol = sulcvol.deepcopy();
  AimsConnectedComponent<short>( ccvol,
                                 aims::Connectivity::CONNECTIVITY_26_XYZ,
                                 short(0), false, MINCC );
  


  //-----------------------------------------------------------------------------------------------------
  //           First projection - Closest points according to euclidean and curvature/depth
  //-----------------------------------------------------------------------------------------------------
 
  sulctex[2] = FirstSulciProjectionWithInterpolationPlane(
    ccvol, sulcvol, mesh, cc_sulci_labels,
    tri_sulci,neigh,cc_sulci_coord,initend,demin,dpmin,label_insula_left,
    label_insula_right,CA );


  //-----------------------------------------------------------------------------------------------------
  //                                      Estimation of affine projection
  //-----------------------------------------------------------------------------------------------------
  
  
  cc_sulci_labels.erase(0);
  unsigned		inc2=1;
  cout << "Estimation of affine transformation...\n";
  for (ilab = cc_sulci_labels.begin(), elab = cc_sulci_labels.end() ; ilab != elab; ++ilab)
  {
    Point3dfSet		&setpointi= cc_sulci_coord[*ilab],setpointf;
    flab = trans.find(tri_sulci[*ilab]);
    cout << endl << inc2 << "th connected component(" << tri_sulci[*ilab] << ": " << flab->second << ")" <<  endl;
    ++inc2;

    AffinePointProjection( sulctex[1],mesh,initend,setpointi,*ilab,demin,alpha_reg);
  }
  cout << endl;
  

  //-----------------------------------------------------------------------------------------------------
  //                                        Closing of each projected cc
  //-----------------------------------------------------------------------------------------------------
  cout << "Closing the sulci\n";
  set<short> label_insula;
  map<short,short>::const_iterator icc,ecc;
  for (icc = tri_sulci.begin(), ecc = tri_sulci.end(); icc != ecc; ++icc)
  {
    flab = trans.find(icc->second);
    if ( flab != glab && (flab->second == "INSULA_left" || flab->second == "INSULA_right") )
    {
      label_insula.insert( icc->first );
      cout << icc->first << " ";
    }
  }
  sulctex[0] = CloseSulci(mesh,sulctex[1],maxdil_mesh,connexity,( short )0,
                          ( short )-1,neigho,label_insula); //linearize cc projected 2*
    
  for (i=0;i<n;++i)
    if (sulctex[0].item(i) != 0)
      sulctex[0].item(i) = tri_sulci[sulctex[0].item(i)];

  return ( sulctex ); 
}


//sulcal line projection using pondered distance (depth and euclidean distance)
Texture<short> aims::meshdistance::FirstSulciProjectionWithCurvatureMap(
  const rc_ptr<Volume<short> > &ccvol,
  const rc_ptr<Volume<short> > &sulcvol,
  const AimsSurface<3,Void> & mesh,
  set<short> &cc_sulci_labels,
  map<short,short > &tri_sulci,
  map<unsigned, Point3dfSet > &cc_sulci_coord,
  map<Point3df, Point3df, Point3dfCompare > &initend,
  const Texture<float> &curvtex,
  float alpha,float dmin)
{

  int    				x,y,z;
  float                        xr,yr,zr;
  const vector<Point3df>		& vert = mesh.vertex();
  const vector<Point3df>		& normal=mesh.normal();
  short                                 label;
  unsigned                              i,im,n = vert.size();
  float                                 dm,d,d1;
  Point3df                              pos,dir;
  Texture<short>                        tex(n); 
  vector<int> cdim = ccvol->getSize();
  vector<float> vs = ccvol->getVoxelSize();

  cout << "First projection..." << endl;
  for ( z = 0; z < cdim[2]; z++ )
    for ( y = 0; y < cdim[1]; y++ )
      for ( x = 0; x < cdim[0]; x++ )
      {
        if ( ccvol->at( x , y , z ) > 0 ) // positive labels correspond to the diffferent cc ; 0 is the background
        {
          label = ccvol->at( x , y , z );//label of the cc
          tri_sulci[label] = sulcvol->at(x , y ,z );
          cc_sulci_labels.insert(label);
          im = 0;
          dm = FLT_MAX;
          xr = x * vs[0];
          yr = y * vs[1];
          zr = z * vs[2];
          cc_sulci_coord[label].insert( Point3df(xr,yr,zr) );

          for ( i = 0; i < n; i++)
          {
            pos = vert[i];
            dir = Point3df(pos[0] - xr,pos[1] - yr,pos[2] - zr);
            if ( dir.dot(normal[i]) < 0 ) // avoid projection through the surface
            {
              d = sqrt ( ( pos[0] - xr ) * ( pos[0] - xr )
                + ( pos[1] - yr ) * ( pos[1] - yr )
                + ( pos[2] - zr ) * ( pos[2] - zr ) ) ; // euclidean distance
              d1 = curvtex.item(i);
              if ( ( d - alpha * d1 < dm ) && ( d < dmin ) ) // only for close points (d<dmin)
              {
                dm = d - alpha * d1;
                im = i;
              }
            }
          }
          initend[Point3df(xr,yr,zr) ] =  vert[im] ;
          tex.item(im) = sulcvol->at(x,y,z);
        }
      }

  return tex;
}


//Sulcal line projection using an affine plan interpolation
Texture<short> aims::meshdistance::FirstSulciProjectionWithInterpolationPlane(
  const rc_ptr<Volume<short> > & ccvol,
  const rc_ptr<Volume<short> > & sulcvol,
  const AimsSurface<3,Void> & mesh,
  set<short> &cc_sulci_labels,
  map<short,short > &tri_sulci,
  const  Point3dfNeigh &neigh,
  map<unsigned, Point3dfSet > &cc_sulci_coord,
  map<Point3df, Point3df, Point3dfCompare > &initend,
  float demin, float dpmin,
  short label_insula_left, short label_insula_right,
  const Point3df & CA )
{

  int    				x,y,z;
  float                                 xr,yr,zr;
  const vector<Point3df>		& vert = mesh.vertex();
  //const vector<Point3df>		& normal=mesh.normal();
  short                                 label;
  unsigned                              i,im,n = vert.size();
  float                                 dm,dp,de;
  Point3df                              pos,dir,normal_plan,CA_dir;
  pair<Point3df,Point3df>               bipoint;
  Point3dfSet                         setpoint;
  Texture<short>                        tex(n);
  Point3dfNeigh::const_iterator  is,es = neigh.end();
  unsigned inc=0;
  vector<int> cdim = ccvol->getSize();
  vector<float> vs = sulcvol->getVoxelSize();

  cout << "First projection..." << endl;
  for ( z = 0; z < cdim[2]; z++ )
    for ( y = 0; y < cdim[1]; y++ )
      for ( x = 0; x < cdim[0]; x++ )
      {
        if ( ccvol->at( x , y , z ) > 0 ) // positif labels correspond to the diffferent cc ; 0 is the background
        {
          label = ccvol->at( x , y , z );//label of the cc
          tri_sulci[label] = sulcvol->at(x , y ,z );
          cc_sulci_labels.insert(label);
          im = 0;
          dm = FLT_MAX;
          xr = x * vs[0];
          yr = y * vs[1];
          zr = z * vs[2];


          is = neigh.find(Point3df(xr,yr,zr));
          if (is != es)
            normal_plan = NormalFromPoints(is->first,is->second);
          else
            cerr << "Pb: cannot find the neighbours of the point " << Point3df(xr,yr,zr) <<"\n";

          if (is->second.size() > 3)//do not estime the normal with less than 4 pt
          {
            bipoint = pair<Point3df,Point3df>(is->first,normal_plan);
            for ( i = 0; i < n; i++)
            {
              pos = vert[i];
              dir = pos - Point3df(xr,yr,zr);
              //if ( dir.dot(normal[i]) < 0 ) // avoid projection through the surface
              {
                if (tri_sulci[label] == label_insula_left || tri_sulci[label] == label_insula_right)
                {
                  dp = DistancePointToLine(pos,bipoint); //distance to the line passing through the pt in the direction of the normal
                  de = dir.norm(); // distance from the initial vertex to the node
                  CA_dir = CA - pos;
                  if ( de  < dm && dp < 5 && de < demin && CA_dir.dot(dir) > 0  )
                  {
                    dm = de;
                    im = i;
                  }
                }
                else
                {
                  dp = DistancePointToPlane(pos,bipoint); //distance from the node to the sulci plane
                  de = dir.norm(); // distance from the initial vertex to the node
                  if ( de  < dm && dp < dpmin && de < demin)
                  {
                    dm = de;
                    im = i;
                  }
                }
              }
            }

            if (dm != FLT_MAX )
            {
              initend[Point3df(xr,yr,zr) ] =  vert[im] ;
              tex.item(im) = sulcvol->at(x,y,z);
              cc_sulci_coord[label].insert( Point3df(xr,yr,zr) );
            }

          }
          else
            ++inc;
        }
      }
  
  cout << inc << " points could not have been projected because the tangent plane could not be estimated \n";
  return(tex);
}


void aims::meshdistance::AffinePointProjection (
  Texture<short> &tex,
  const AimsSurface<3,Void> & mesh,
  const map<Point3df, Point3df, Point3dfCompare > &initend,
  const Point3dfSet  &setpointi,
  short label,
  float dmin,float alpha_reg )
{

  Point3dfSet::const_iterator isb,ise;
 
  float 		error=0; 
  Point3dfSet  setpointi_reg;
  float                                 xr,yr,zr;
  const vector<Point3df>		& vert = mesh.vertex(); //,& normal = mesh.normal();
  unsigned                              inc,i,j,im,n = vert.size();
  float                                 dm,d;
  Point3df                              pos,dir;
  map<Point3df, Point3df, Point3dfCompare >::const_iterator is,es = initend.end();

  for (i=0;i<n;++i)
    if (tex.item(i) == label)
      tex.item(i) = 0;
  
  unsigned  size = setpointi.size();

  VolumeRef<float> Bx, By, Bz,
    X( size, 4 , 1, 1, carto::AllocatorContext::fast() ),
    Yx( size, 1, 1, 1, carto::AllocatorContext::fast() ),
    Yy( size, 1, 1, 1, carto::AllocatorContext::fast() ),
    Yz( size, 1, 1, 1, carto::AllocatorContext::fast() ),
    Xinv, pYx, pYy, pYz;
  
  //-----Estimation of affine transformation
  cout << "Estimation of the affine transformation A \n";
 
  //Def of initial matrix coordinates X=[Xx|Xy|Xz] Y=[Yx|Yy|Yz] 
  for( isb=setpointi.begin(), inc = 0, ise = setpointi.end(); isb!=ise;
       ++isb, ++inc  )
  {
    X(inc,0) = (*isb)[0];
    X(inc,1) = (*isb)[1];
    X(inc,2) = (*isb)[2];
    X(inc,3) = 1;
    is = initend.find(*isb);
    if (is != es)
      pos = is->second;
    else
      cerr << "Pb to fill the matrix for affine projection\n";
    Yx(inc) = pos[0];
    Yy(inc) = pos[1];
    Yz(inc) = pos[2];
  }

  if( size > 4 )
  {
    // Inversion of X
    Xinv = matrix_product(
      AimsInversionLU( matrix_product( transpose( X ), X ) ),
      transpose( X ) );
    Xinv = matrix_product(
      AimsInversionLU( matrix_product( transpose( X ), X ) ),
      transpose( X ) );

    // Definition of affine matrix A =[Bx|By|Bz]T=Xinv.[Yx|Yy|Yz] <=> Bi=Xinv.Yi
    Bx = matrix_product( Xinv, Yx );
    By = matrix_product( Xinv, Yy );
    Bz = matrix_product( Xinv, Yz );

    cout << "Affine transformation : \n";
    cout.precision(4);
    for (j=0;j<3;++j)
      cout << Bx(j) << " " << By(j) << " " << Bz(j) << endl;
    cout << "Translation : " << endl;
    cout << Bx(3) << endl << By(3) << endl << Bz(3) << endl;

    //Affine projection
    pYx = matrix_product( X, Bx );
    pYy = matrix_product( X, By );
    pYz = matrix_product( X, Bz );

    //Robust estimation
    multimap<float,Point3df> set_error;
    multimap<float,Point3df>::iterator iste,este;

    //Outlier selection
    cout << "Remove outlier for robust estimation of the affine transformation \n";
    for (j=0; j<size; j++)
    {
      error = 0;
      error += (pYx(j) - Yx(j))*(pYx(j) - Yx(j)) ;
      error += (pYy(j) - Yy(j))*(pYy(j) - Yy(j)) ;
      error += (pYz(j) - Yz(j))*(pYz(j) - Yz(j)) ;
      error = sqrt(error);
      set_error.insert(pair<const float,Point3df>(error , Point3df( X(j,0),X(j,1),X(j,2) ) ) ) ;
    }

    unsigned nb;
#if defined(__GNUC__) && defined (__sun__)
    nb = (unsigned) ::floor(set_error.size() * alpha_reg);
#else
    nb = (unsigned) floor(set_error.size() * alpha_reg);
#endif
    if (nb > 4)
    {
      cout << nb << " points from " << set_error.size() << "  are used to estime the regularized transformation\n";

      for (iste = set_error.begin(), este = set_error.end(), inc = 0;
           (iste != este && inc < nb) ; ++iste, ++inc)
        setpointi_reg.insert(iste->second);

      unsigned size_reg = setpointi_reg.size();


      VolumeRef<float> Bx_reg, By_reg, Bz_reg,
        X_reg( size_reg, 4, 1, 1, carto::AllocatorContext::fast() ),
        Yx_reg( size_reg, 1, 1, 1, carto::AllocatorContext::fast() ),
        Yy_reg( size_reg, 1, 1, 1, carto::AllocatorContext::fast() ),
        Yz_reg( size_reg, 1, 1, 1, carto::AllocatorContext::fast() ),
        Xinv_reg;

      for( isb=setpointi_reg.begin(), inc = 0, ise = setpointi_reg.end(); isb!=ise; ++isb, ++inc  )
      {
        X_reg(inc,0) = (*isb)[0];
        X_reg(inc,1) = (*isb)[1];
        X_reg(inc,2) = (*isb)[2];
        X_reg(inc,3) = 1;
        is = initend.find(*isb);
        if (is != es)
          pos = is->second;
        else
          cerr << "Pb to fill the matrix for regularized affine projection\n";
        //pos = initend.find(*isb)->second;
        Yx_reg(inc) = pos[0];
        Yy_reg(inc) = pos[1];
        Yz_reg(inc) = pos[2];
      }

      // Inversion of X
      Xinv_reg = matrix_product(
        AimsInversionLU( matrix_product( transpose( X_reg ), X_reg ) ),
        transpose( X_reg ) );

      // Definition of affine matrix A =[Bx|By|Bz]T=Xinv.[Yx|Yy|Yz] <=> Bi=Xinv.Yi
      Bx_reg = matrix_product( Xinv_reg, Yx_reg );
      By_reg = matrix_product( Xinv_reg, Yy_reg );
      Bz_reg = matrix_product( Xinv_reg, Yz_reg );

      cout << "Affine regularized transformation : \n";
      for (j=0;j<3;++j)
        cout << Bx_reg(j) << " " << By_reg(j) << " " << Bz_reg(j) << "    "  << endl;
      cout << "Regularized Translation : " << endl;
      cout << Bx_reg(3) << endl
            << By_reg(3) << endl
            << Bz_reg(3) << endl;

      Yx = matrix_product( X, Bx_reg );
      Yy = matrix_product( X, By_reg );
      Yz = matrix_product( X, Bz_reg );

      for (i=0; i<size; ++i)
      {
        xr = Yx(i);
        yr = Yy(i);
        zr = Yz(i);
        im = 0;
        dm = FLT_MAX;
        for ( j = 0; j < n; j++)
        {
          pos = vert[j];
          dir = pos - Point3df(xr,yr,zr);
          //if ( dir.dot(normal[j]) <= 0 ) // avoid projection through the surface
          {
            d = sqrt ( ( pos[0] - xr ) * ( pos[0] - xr ) +
                ( pos[1] - yr ) * ( pos[1] - yr ) +
                ( pos[2] - zr ) * ( pos[2] - zr ) )  ; // euclidean distance

            if (  d  < dm && dir.norm() < dmin)
            {
              dm = d ;
              im = j;
            }
          }
        }


        if (dm != FLT_MAX )
        {
          if (tex.item(im) != label && tex.item(im) != 0)
            cout << "Projection onto a point already labelled \n";
          tex.item(im) = label;
        }
        else
          cerr << "The point can't be projected (the projected node is too far from the the initial voxel\n";
      }

    }
    else
      cout  << "Not enough points (" << nb << " < 5) to estime the regularized affine transformation \n";
  }
  else
    cout  << "Not enough points (" << size << " < 5) to estime the affine transformation \n";
}


//Give the neigbours of each 3df point of bvol in svol (only for points whose label is label)
void aims::meshdistance::NeighbourInCC(
  Point3dfNeigh & neigh,
  const rc_ptr<Volume<short> > & bvol,
  const rc_ptr<Volume<short> > & svol,
  Connectivity::Type connectivity,
  short label, unsigned size_neigh, unsigned max_points)
{
  
  Connectivity cd( bvol->getStrides()[1], bvol->getStrides()[2],
                   connectivity );
  vector<float> vs = bvol->getVoxelSize();
  float sX = vs[0], sY = vs[1], sZ = vs[2];
  int x,y,z;
  unsigned                          n; 
  Point3d		             pos,newpos;
  Point3df                           pt;
  set<Point3d,Point3dCompare>        cc;
  set<Point3d,Point3dCompare>::iterator ic,ec;
  Point3dfSet::iterator icf,ecf,icf2,ecf2;
   Point3dfNeigh::iterator in,en;
  Point3dfNeigh temp;
  int dimX = svol->getSizeX(), dimY = svol->getSizeY(),
    dimZ = svol->getSizeZ();

  for ( z = 0; z < dimZ; z++ )
    for ( y = 0; y < dimY; y++ )
      for ( x = 0; x < dimX; x++ )
        if( bvol->at(x,y,z) == label )
          cc.insert(Point3d(x,y,z));

  cout <<"Size of the CC : " << cc.size() << endl;
   
  cout << "Neighbouring level: " << 0 << " " << flush;
  for (ic = cc.begin(), ec=cc.end(); ic != ec; ++ic)
  {
    pos = *ic;
    for ( n = 0; n < (unsigned) cd.nbNeighbors(); n++ )
    {
      newpos = pos + cd.xyzOffset( n );
      //if ( cc.find(newpos) != ec )
      if ( newpos[0] >= 0   &&
           newpos[0] < dimX &&
           newpos[1] >= 0   &&
           newpos[1] < dimY &&
           newpos[2] >= 0   &&
           newpos[2] < dimZ &&
           svol->at(newpos) == label )
        neigh[ Point3df(pos[0] * sX ,pos[1] * sY , pos[2] * sZ)  ].insert
          ( Point3df(newpos[0] * sX ,newpos[1] * sY , newpos[2] * sZ) );
    }
  }

  if (size_neigh > 1)
    for (n=1; n < size_neigh; ++n)
    {
      cout << n << " " << flush;
      temp = neigh;
      for (in = neigh.begin(), en = neigh.end(); in != en; ++in)
        if ( (in->second).size() < max_points )
          for( icf = in->second.begin(), ecf = in->second.end(); icf != ecf;
               ++icf )
            for( icf2 = neigh[*icf].begin(), ecf2 = neigh[*icf].end();
                 icf2 != ecf2; ++icf2 )
              temp[in->first].insert(*icf2);
      neigh = temp;
    }

  cout << endl;
}


VolumeRef<short> aims::meshdistance::MeshParcellation2Volume(
  const rc_ptr<Volume<short> >    &initVol,
  const Texture<short> &tex,
  const AimsSurface<3,Void> & mesh,
  short val_domain, short back )
{

  int                      x,y,z;
  const vector<Point3df> & vert = mesh.vertex();
  unsigned                 i ;
  vector<float>            vs = initVol->getVoxelSize();
  float                    coef_deplt, sx = vs[0], sy = vs[1], sz = vs[2];
  Point3d                  ptVol;
  Point3df                 posMesh, posVol, n;
  VolumeRef<short>         vol = VolumeRef<short>( initVol ).deepcopy();
  const vector<Point3df> & normal = mesh.normal();
  short                    out = 0;
  vector<int>              dim = initVol->getSize();
  int                      dx = dim[0], dy = dim[1], dz = dim[2];

  for( z=0; z<dz; ++z )
    for( y=0; y<dy; ++y )
      for( x=0; x<dx; ++x )
        if ( vol(x,y,z) != val_domain)
          vol(x,y,z) = back;

  cout << "Spatial resolution: " << Point3df(sx,sy,sz) << endl;

  for (i = 0; i< vert.size()  ; ++i)
    if (tex.item(i) != back)
    {
      posMesh = vert[i];
      n =  normal[i];
      coef_deplt = Point3df( sx * n[0] + sy * n[1] + sz * n[2] ).norm() * 0.5; //for sulcal wall
      posVol = posMesh - coef_deplt * n ;
      //ptVol[0] = (short)rint(posVol[0] / sx );
      //ptVol[1] = (short)rint(posVol[1] / sy );
      //ptVol[2] = (short)rint(posVol[2] / sz );
      ptVol[0] = (short)rint(posMesh[0] / sx );
      ptVol[1] = (short)rint(posMesh[1] / sy );
      ptVol[2] = (short)rint(posMesh[2] / sz );
      if (initVol->at(ptVol) == val_domain)
        vol(ptVol) = tex.item(i);
      else
        ++out;
    }

  cout << out << "/" << vert.size() <<" points cannot be projected onto the volume \n";
  int xmask = 3, ymask = 3, zmask = 3;
  float factor = 50;
  vol = AimsVoronoiFrontPropagation<short>( vol , val_domain, back ,
                                            xmask, ymask, zmask, factor );
  
  //ForEach3d(vol,x,y,z)
  //if (vol(x,y,z) == val_domain || initVol(x,y,z) != val_domain )
  //vol(x,y,z) = back;

  return(vol);
}


Texture<short> aims::meshdistance::VolumeParcellation2MeshParcellation(
  const rc_ptr<Volume<short> > &initVol,
  const AimsSurface<3,Void> & mesh, const short back)
{

  const vector<Point3df> & vert = mesh.vertex();
  unsigned                 i;
  float                    coef_deplt, sx = initVol->getVoxelSize()[0],
    sy = initVol->getVoxelSize()[1], sz = initVol->getVoxelSize()[2];
  Point3d                  ptVol;
  Point3df                 posMesh,posVol,n;
  const vector<Point3df> & normal = mesh.normal();
  Texture<short>           outTex(vert.size());

  for (i = 0; i< vert.size()  ; ++i)
  {
    posMesh = vert[i];
    n =  normal[i];
    coef_deplt = Point3df( sx * n[0] + sy * n[1] + sz * n[2] ).norm() * 0.5; //for sulcal wall
    posVol = posMesh - coef_deplt * n ;
    ptVol[0] = (short)rint(posVol[0] / sx );
    ptVol[1] = (short)rint(posVol[1] / sy );
    ptVol[2] = (short)rint(posVol[2] / sz );
    outTex.item(i) = initVol->at(ptVol);
  }
  outTex = MeshVoronoiT(mesh,outTex,back,(short)-1,FLT_MAX,false,true);

  return(outTex);
}




/*
VolumeRef<short> aims::meshdistance::Graph2LabelledVolume( const graph &fg,
							  const vector<float> & vsz,
							  const string & sulciName,
							  const string & bucketName,
							  const string &level, 
							  map<string,short> &trans )
{
  
  //string					bname = "aims_bottom",ssname = "aims_ss",othername= "aims_other";
  string					name,synt;
  FGraph					fg;
  VolumeRef<short>				vol( bb[0],bb[1],bb[2],1,1 );
  Graph::iterator				iv, fv=fg.end();
  FoldLabelsTranslator				levelTrans( level );
  short						i;
  rc_ptr<BucketMap<Void> >			bck;
  BucketMap<Void>::Bucket::const_iterator	ib, fb;
  AimsVector<short, 3>				pos;

  
  vol.setVoxelSize( vsz );
  vol = 0;
  vol.fillBorder(0);
   
  levelTrans.translate( fg, sulciName, "name" ); 
       
  Vertex::const_iterator ie, fe;


  for( iv=fg.begin(); iv!=fv; ++iv )
    {
      assert( (*iv)->getProperty( "name", name ) );
      i = trans[name];
      if( i != 0 )
	{
	  if( (*iv)->getProperty( bucketName, bck ) )
	    {
	      if( ( synt.empty() || (*iv)->getSyntax() == synt ) )
		{
		  BucketMap<Void>::Bucket	& bl = (*bck)[0];
		  for( ib=bl.begin(), fb=bl.end(); ib!=fb; ++ib )
		    {
		      pos = ib->first;
		      vol( pos[0], pos[1], pos[2] ) = i;
		    }
		}
	    }
	  else
	    {
	      for( ie=(*iv)->begin(), fe=(*iv)->end(); ie!=fe; ie++)
		{
		  if( ( synt.empty() || (*ie)->getSyntax() == synt )
		      && (*ie)->getProperty( bucketName, bck ) )
		    {
		      BucketMap<Void>::Bucket	& bl = (*bck)[0];
		      for( ib=bl.begin(), fb=bl.end(); ib!=fb; ++ib )
			{
			  pos = ib->first;
			  vol( pos[0], pos[1], pos[2] ) = i;
			}
		    }
		}
	    }
	}
    }
      
   
  return(vol);  

}
*/
