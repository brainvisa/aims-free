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

#include <cstdlib>
#include <iostream>
#include <aims/getopt/getopt2.h>
#include <aims/data/data.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/texdata/texdata.h>
#include <aims/primalsketch/greyLevelBlob.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/primalsketch/scalespace.h>
#include <aims/primalsketch/finiteElementSmoother_d.h>
#include <aims/primalsketch/primalSketch.h>
#include <aims/primalsketch/primalSketch2graph.h>
#include <aims/primalsketch/primalSketchUtil.h>
#include <aims/io/motionR.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;

void graphePS(PrimalSketch<AimsSurface<3, Void>, Texture<float> > &sketch){
  std::list< ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type>* > blobList(sketch.BlobSet());
  std::list< ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type>* >::iterator itSSBlobs=blobList.begin();

  vector<uint> votes;
  for (uint i=0;i<100;i++)
    votes.push_back(0);
  float tmin=1000000000000.0,tmax=-1000000000000.0;
  for (; itSSBlobs!=blobList.end(); ++itSSBlobs)  {
    float valeur = (*itSSBlobs)->GetMeasurements().tValue;
    if (valeur<tmin) tmin=valeur;
    if (valeur>tmax) tmax=valeur;
  }
  printf("min: %f - max: %f\n", tmin, tmax);
  for (itSSBlobs=blobList.begin(); itSSBlobs!=blobList.end(); ++itSSBlobs)  {
    float valeur = (*itSSBlobs)->GetMeasurements().tValue;
//     printf("%d ", (uint) (((valeur - tmin) / (tmax-tmin))*votes.size()));
    votes[(uint) (((valeur - tmin) / (tmax-tmin))*votes.size())]++;
  }
  printf("\n");
  for (uint i=0;i<100;i++){
    int aux=0;
    for (uint j=i;j<100;j++){
      aux += votes[j];
      
    }
    printf("%.3f %d\n",tmin+i*(tmax-tmin)/100.0, aux);
  }
  printf("\n");
  
  
}


void bloquerAscendance(ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type>*ssb, vector<bool> &blobtag){
  blobtag[ssb->Label()] = false;
  std::list<ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type>*> topblobs= ssb->TopBifurcation()->TopBlobs();
  std::list<ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type>*>::iterator blobit=topblobs.begin();
  for (;blobit!=topblobs.end();blobit++){
    bloquerAscendance(*blobit,blobtag);
  }
  return;
}
    
    
PrimalSketch<AimsSurface<3,Void>, Texture<float> > filterPS2(PrimalSketch<AimsSurface<3,Void>, Texture<float> > &sketch, float threshold=1.0){
  printf("FILTERPS\n");
  std::list< ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type>* > blobList(sketch.BlobSet());
  std::list< ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type>* >::iterator itSSBlobs=blobList.begin();

//   std::multimap<float, ScaleSpaceBlob<SiteType<AimsSurface<3,Void> >::type> *> blobmap;
//   std::multimap<float, ScaleSpaceBlob<SiteType<AimsSurface<3,Void> >::type> * >::iterator mapit;
  
  printf("\n\n%d", sketch.BlobSet().size());
  PrimalSketch<AimsSurface<3, Void>, Texture<float> > outsketch(sketch.Subject(), sketch.scaleSpace(), SURFACE);

  printf("\n\n%d", outsketch.BlobSet().size());
  for (itSSBlobs=blobList.begin();itSSBlobs!=blobList.end();itSSBlobs++){
    cout << (*itSSBlobs)->GetMeasurements().tValue << " ";
    if ((*itSSBlobs)->GetMeasurements().tValue > threshold)
      outsketch.AddBlob(*itSSBlobs);
  }
  printf("\n\n%d", outsketch.BlobSet().size());
  return outsketch;
}
    
PrimalSketch<AimsSurface<3,Void>, Texture<float> > filterPS(PrimalSketch<AimsSurface<3,Void>, Texture<float> > &sketch, float threshold=1.0){
  printf("FILTERPS\n");
  std::list< ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type>* > blobList(sketch.BlobSet());
  std::list< ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type>* >::iterator itSSBlobs=blobList.begin();

  std::multimap<float, ScaleSpaceBlob<SiteType<AimsSurface<3,Void> >::type> *> blobmap;
  std::multimap<float, ScaleSpaceBlob<SiteType<AimsSurface<3,Void> >::type> * >::iterator mapit;
  
  for (; itSSBlobs!=blobList.end(); ++itSSBlobs)  {
    float scalemin = (*itSSBlobs)->ScaleMin();
    pair<float, ScaleSpaceBlob<SiteType<AimsSurface<3,Void> >::type> * > aux;
    ScaleSpaceBlob<SiteType<AimsSurface<3,Void> >::type> * aux2(*itSSBlobs);
    aux.first = scalemin;
    aux.second = aux2;
    blobmap.insert(aux);
  }
  std::vector<ScaleSpaceBlob<SiteType<AimsSurface<3,Void> >::type> *> blobvect;
  for (mapit = blobmap.begin();mapit!=blobmap.end();mapit++){
    blobvect.push_back((*mapit).second);
  }
  
  vector<bool> blobtag; // LE VECTEUR QUI DIT POUR CHAQUE BLOB S'IL A ETE BLOQUE/TRAITE OU PAS
  for (uint i=0;i<blobvect.size();i++)
    blobtag.push_back(true);
  
  for (mapit = blobmap.begin();mapit!=blobmap.end();mapit++){
    ScaleSpaceBlob<SiteType<AimsSurface<3,Void> >::type> *ssb=(*mapit).second;
    uint label = ssb->Label();
    if (blobtag[label]){
      if (ssb->GetMeasurements().tValue > threshold){
        printf("%.3f ", ssb->GetMeasurements().tValue);
        // ça passe le seuil je bloque le blob courant et ses ancetres
        bloquerAscendance(ssb, blobtag);
      }
      else {
        blobmap.erase(mapit);
        // ça ne passe pas le seuil je vire le blob de la map
      }
    }
  }
  printf("\n\n%d", sketch.BlobSet().size());
  PrimalSketch<AimsSurface<3, Void>, Texture<float> > outsketch(sketch.Subject(), sketch.scaleSpace(), SURFACE);

  printf("\n\n%d", outsketch.BlobSet().size());
  for (mapit = blobmap.begin();mapit!=blobmap.end();mapit++){
    outsketch.AddBlob((*mapit).second);
  }
  printf("\n\n%d", outsketch.BlobSet().size());
  return outsketch;
}


int main(int argc, const char **argv)
{
  try
    {
      std::string fileInT, fileInM, fileout, fileoutBlobs, motionfile, subject="none",
        graphout, auxmeshpath="", fileLat="", fileLongit="";
      float dt=0.05, tmax, tmin=1.0,filterout=-100000000.0;
      uint intersection_param=10;

      AimsApplication app( argc, argv, "ScaleSpace et grey level blobs d'une surface/texture au format float");

      app.addOption( fileInT, "-t", "inputTexture");
      app.alias( "--inputT", "-t" );
      app.addOption( graphout, "-o", "output primal sketch graph" );
      app.addOption( fileInM, "-m", "inputMesh");
      app.alias( "--inputM", "-m" );
      app.addOption( fileout, "-os", "scale space texture filename");
      app.alias( "--output-scales", "-os" );
      app.addOption( fileoutBlobs, "-ob", "blobs texture filename");
      app.alias( "--output-blobs", "-ob" );
      app.addOption( tmin, "-t1", "minimum scale (default=1.0)", true);
      app.alias( "--scalemin", "-t1" );
      app.addOption( tmax, "-t2", "maximum scale");
      app.alias( "--scalemax", "-t2" );
      app.addOption( dt, "-dt", "time step (default=0.05)", true);
      app.alias( "--deltat", "-dt" );
      app.addOption( auxmeshpath, "-mX", "auxilliary mesh (for the rendering)",true);
      app.alias( "--auxmesh", "-mX" );
      app.addOption( subject, "-sj", "subject name (default : inputImage)", true);
      app.alias ("--subject", "-sj");
      app.alias ("--subject", "-sj");

      app.addOption( motionfile, "--trans", "Transformation matrix to Talairach space", true );
      app.addOption( intersection_param, "-iP", "Intersection condition for grey-level blobs matching across scales", true );
      app.addOption( filterout, "-f", "filter out blobs whose tvalues are under (default=1.0)", true);
      app.addOption( fileLat, "-l", "texture latitude", true);
      app.addOption( fileLongit, "-L", "texture longitude", true);

  
      app.initialize();

      if( graphout.empty() )
        {
          graphout = fileout;
          string	bn = FileUtil::basename( fileout );
          string::size_type pos = bn.rfind( '.' );
          if( pos != string::npos )
            graphout.erase( graphout.length() - bn.length() + pos + 1, 
                            bn.length() - pos );
          graphout += ".arg";
          cout << "output primalsketch: " << graphout << endl;
        }
      cout << "Reading input surface and texture: " << fileInM << " and " << fileInT << endl;
      cout << "Creating up to scale " << tmax << " with geometric sampling of the scales (t=2^n)" << endl;

      Reader<AimsSurfaceTriangle>	r1(fileInM);
      Reader<Texture1d>             r2(fileInT);
      AimsSurfaceTriangle           laMesh;
      Texture1d                     laTexture;
      r1.read(laMesh);
      r2.read(laTexture);
      if (subject=="none")
        subject=fileInT;

      // Elimination des NaN s'il y a :
      float moy=0.0;
      for (uint i=0;i<laTexture.nItem();i++)
        if (laTexture.item(i) == laTexture.item(i))
          moy += laTexture.item(i);
         
      moy /= laTexture.nItem();
      for (uint i=0;i<laTexture.nItem();i++)
        if (laTexture.item(i) != laTexture.item(i))
          laTexture.item(i) = moy;

      cout << "Size=" << laMesh[0].vertex().size() << endl;

      cout << "Ssmoother creation" << endl;

      FiniteElementSmoother<3, float> *smooth;
      smooth=new FiniteElementSmoother<3, float>(dt, &(laMesh[0]));
      ScaleSpace<AimsSurface<3, Void>, Texture<float> > scale_space(&(laMesh[0]), &(laTexture[0]), smooth);
      cout << "Scale-space creation" << endl;
      Texture1d lat,longit;
      std::vector<Point3df> coordinates;
      TimeTexture<float> tmptex;

      if (fileLat != "" && fileLongit != ""){
      
      Reader<Texture1d> r3(fileLat);
      Reader<Texture1d> r4(fileLongit);
      r3.read(lat);
      r4.read(longit);
      for (uint i=0;i<lat[0].nItem();i++){
        coordinates.push_back(Point3df(lat[0].item(i), longit[0].item(i),i));
      }
      scale_space.PutCoordinates(&coordinates);
      }


      scale_space.GenerateDefaultScaleSpace(tmax);

      cout << "Writing scale space in file " << fileout << endl;

      //scale_space.Write(fileout);

      cout << "Computing primal sketch" << endl;

      PrimalSketch<AimsSurface<3, Void>, Texture<float> > sketch(subject, &scale_space, SURFACE);
      
      printf("%.3f, %.3f\n", tmin, tmax);
      sketch.ComputePrimalSketch(tmin, tmax, "", intersection_param);
      std::list<ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type >*> blobList=sketch.BlobSet();

      std::list<ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type >*>::iterator blobIt;
      uint test=0;
      for (blobIt=blobList.begin();blobIt!=blobList.end();blobIt++){
        ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type > ssb(**blobIt);
        GreyLevelBlob<SiteType<AimsSurface<3, Void> >::type > glb(*(ssb.GlBlobRep()));
//         if (glb.GetListePoints().size() == 1) 
        test++;
        
      }
      cout << "nb de blobs : " << test << endl;
//       graphePS(sketch);
      printf("\n");
//       for (float i=-9.0;i<9.0;i++){
      if (pow(filterout+100000000.0,2) > 0.01){
        cout << "filtre: " << filterout << endl;
        sketch=filterPS2(sketch,filterout);
      }
      
//     printf("BOURRIN\n");
//     std::list< ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type>* > *blobLis; blobLis=&(sketch.BlobSet());
    std::list< ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type>* >::iterator itSSBlobs=sketch.BlobSet().begin();
    std::list< ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type>* >::iterator itSSend=sketch.BlobSet().end();
    

    printf("  %d\n", sketch.BlobSet().size());

    printf("FIN\n");
      test=0; 
      for (blobIt=blobList.begin();blobIt!=blobList.end();blobIt++){
        ScaleSpaceBlob<SiteType<AimsSurface<3, Void> >::type > ssb(**blobIt);
        GreyLevelBlob<SiteType<AimsSurface<3, Void> >::type > glb(*(ssb.GlBlobRep()));
//         if (glb.GetListePoints().size() == 1) 
        test++;
        
      }
      cout << "nb de blobs : " << test << endl;
      
      printf("\n");
//       graphePS(sketch);
      cout << "Converting primal sketch into graph" << endl;

      if (auxmeshpath!=""){
        cout << "Auxilliary mesh used for rendering : " << auxmeshpath << endl;
        AimsSurfaceTriangle *tmpmesh;
        tmpmesh = new AimsSurfaceTriangle();
        Reader<AimsSurfaceTriangle> r(auxmeshpath);
        r.read(*tmpmesh);      
        ASSERT((*tmpmesh)[0].vertex().size() == laMesh[0].vertex().size() && (*tmpmesh)[0].polygon().size() == laMesh[0].polygon().size());
        sketch.scaleSpace()->PutAuxMesh(tmpmesh);
      }
      

      Primalsketch2graph<AimsSurface<3, Void>, Texture<float> > translate(&sketch);
      if( !motionfile.empty() ) {
          MotionReader  mr( motionfile );
          Motion  mot;
          mr.read( mot );
          translate.setMotion( mot );
        }

      translate.DoIt();

      Graph *graphSketch=translate.GetGraph();

      aims::Writer<Graph > dataW( graphout );
      dataW.write(*graphSketch);

      cout << "Getting scale-space blob texture" << endl;
      TimeTexture<float> blobTexture;
      blobTexture=GetSSBlobTexture(&sketch);

//       cout << "Getting blob triangulations and writing it" << endl;
//       AimsSurfaceTriangle *triangles;
//       triangles=GetSSBlobTore(&sketch);
//       aims::Writer<AimsSurfaceTriangle> meshW(fileoutBlobs);
//       meshW.write(*triangles);

      cout << "Writing it in file " << fileoutBlobs << endl;
      
      aims::Writer<TimeTexture<float> > blobW(fileoutBlobs);
      blobW.write(blobTexture);
      //
      scale_space.Write(fileout);

      cout << "Finished" << endl;
      cout<<"\a";
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cout << e.what() << endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
