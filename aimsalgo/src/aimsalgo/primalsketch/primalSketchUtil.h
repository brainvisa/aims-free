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


#ifndef AIMS_PRIMALSKETCH_PRIMALSKETCHUTIL_H
#define AIMS_PRIMALSKETCH_PRIMALSKETCHUTIL_H

#include <cstdlib>
#include <aims/config/aimsalgo_config.h>
#include <aims/primalsketch/primalSketch.h>
#include <aims/bucket/bucket.h>
#include <aims/graph/graphmanip.h>
#include <aims/mesh/surfacegen.h>
#include <aims/mesh/surfaceOperation.h>
#include <unistd.h>
#include <graph/graph/graph.h>
#include <set>
#include <map>
#include <list>
#include <utility>
#include <math.h>

namespace aims
{
	// Functions that deal with primal sketches

	//------------------------------------- Declarations ----------------------------------------------------------------

	template<typename T> AimsData<short>   *GetSSBlobImage(PrimalSketch<AimsData<T>, AimsData<T> > *sketch);
	template<int D, class T> TimeTexture<float> GetSSBlobTexture(PrimalSketch<AimsSurface<D, Void>, Texture<T> > *sketch);
	template<int D, class T> AimsSurfaceTriangle *GetSSBlobMesh(PrimalSketch<AimsSurface<D, Void>, Texture<T> > *sketch);

	template<typename T> AimsBucket<Void> *GetSSBlobBucket(PrimalSketch<AimsData<T>, AimsData<T> > *sketch);
	template<int D, class T> AimsSurfaceTriangle *GetSSBlobTore(PrimalSketch<AimsSurface<D, Void>, Texture<T> > *sketch);

	template<typename Geom, typename Text> 	void AddBlobsToPSGraph(PrimalSketch<Geom,Text> *sketch, Graph *graph);
	template<typename T> void AddBlobsToPSGraph(PrimalSketch<AimsData<T>, AimsData<T> > *sketch, Graph *graph);
	template<int D, class T> void AddBlobsToPSGraph(PrimalSketch<AimsSurface<D, Void>, Texture<T> > *sketch, Graph *graph);

	//------------------------------------  Definitions ------------------------------------------------------------------

	template<typename T> AimsData<short> *GetSSBlobImage(PrimalSketch<AimsData<T>, AimsData<T> > *sketch)
	{
		typedef Point3d Site;

		AimsData<short> *image;
		int x,y,z,sx,sy,sz, i;
		float dx, dy, dz;
		std::list<ScaleSpaceBlob<Site>*> ssBlobList=sketch->BlobSet();
		std::list<ScaleSpaceBlob<Site>*>::iterator itBlob=ssBlobList.begin();
		GreyLevelBlob<Site>	*glBlob;
		std::set<Site,ltstr_p3d<Site> > points;
		std::set<Site,ltstr_p3d<Site> >::iterator itPoints;

		sx=sketch->scaleSpace()->GetOriginalImage().dimX();
		sy=sketch->scaleSpace()->GetOriginalImage().dimY();
		sz=sketch->scaleSpace()->GetOriginalImage().dimZ();
		dx=sketch->scaleSpace()->GetOriginalImage().sizeX();
		dy=sketch->scaleSpace()->GetOriginalImage().sizeY();
		dz=sketch->scaleSpace()->GetOriginalImage().sizeZ();

		image=new AimsData<short>(sx, sy, sz);
		image->setSizeXYZT(dx,dy,dz);

		for (z=0; z<sz; z++)
			for (y=0; y<sy; y++)
				for (x=0; x<sx; x++)
					(*image)(x,y,z)=0;

		for (; itBlob!=ssBlobList.end(); ++itBlob)
		{
			glBlob=(*itBlob)->GlBlobRep();
			points=glBlob->GetListePoints();
			itPoints=points.begin();
			for (; itPoints!=points.end(); ++itPoints)
			{
				x=(*itPoints)[0]; y=(*itPoints)[1]; z=(*itPoints)[2];
				if ((i=(*image)(x,y,z))==0)
					(*image)(x,y,z)=100 + ((*itBlob)->Label() * 10);
				else
					(*image)(x,y,z)=105 + ((*itBlob)->Label() * 10);
			}
		}

		return image;
	}


	//------------------------------------------------------------------------------------------

	template<int D, class T> TimeTexture<float> GetSSBlobTexture(PrimalSketch<AimsSurface<D, Void>, Texture<T> > *sketch)
	{
		typedef std::pair<Point3df,uint> Site;

		//TexturedData<AimsSurface<D, Void>, Texture<T> > *textBlob;
		std::list<ScaleSpaceBlob<Site>*> ssBlobList=sketch->BlobSet();
		std::list<ScaleSpaceBlob<Site>*>::iterator itBlob=ssBlobList.begin();
		GreyLevelBlob<Site>	*glBlob;
		std::set<Site,ltstr_p3d<Site> > points;
		std::set<Site,ltstr_p3d<Site> >::iterator itPoints;
		//int label;

		//textBlob=new TexturedData<AimsSurface<D, Void>, Texture<T> >(sketch->scaleSpace()->Mesh(), &(sketch->scaleSpace()->GetOriginalImage()));

                std::set<float> scale= sketch->scaleSpace()->GetScaleList();
               

                TimeTexture<float> tex(scale.size(),sketch->scaleSpace()->Mesh()->vertex().size());
                for (uint i=0;i<scale.size();i++)
                  for (uint j=0;j<sketch->scaleSpace()->Mesh()->vertex().size();j++)
                    tex[i].item(j)=0;
                  
                float sc = 0.0f;
                std::set<float>::iterator it;
                uint test=0;
                uint i=0;
                for (; itBlob!=ssBlobList.end(); ++itBlob)
                {
                        //label=(*itBlob)->Label();
			glBlob=(*itBlob)->GlBlobRep();
			points=glBlob->GetListePoints();
			itPoints=points.begin();
//                         if (points.size() == 1){
                          std::list<GreyLevelBlob<Site>*> glBlobs=(*itBlob)->glBlobs;
                          std::list<GreyLevelBlob<Site>*>::iterator glit;
                          for (glit = glBlobs.begin() ; glit != glBlobs.end() ; glit++){
                            points=(*glit)->GetListePoints();
                            itPoints=points.begin();
                            sc = (*glit)->GetScale();
                            for (i=0,it=scale.begin();
                              *it != sc && it!=scale.end();it++,i++) {}

                            for (; itPoints!=points.end(); ++itPoints)
                            {
                              tex[i].item((*itPoints).second)= (*itBlob)->GetMeasurements().t;  //100+test*10;
                            }
                          }
                          test++;
//                         }
//                         uint i=0;
//                         float delta=1000.0;
//                         for (it=scale.begin();it!=scale.end();it++){
//                           if (fabs(*it-(*itBlob)->ScaleRep()) < delta)
//                           {
//                             delta=fabs(*it-(*itBlob)->ScaleRep());
//                             sc = *it;
//                           }
//                         }
//                         cout << "sc:" << sc << " ";
                        for (i=0,it=scale.begin();*it != sc && it!=scale.end();
                          it++,i++) {}

                }

                return tex;
// 		return textBlob->GetTexture();
	}

	//------------------------------------------------------------------------------------------

	template<typename T> AimsBucket<Void> *GetSSBlobBucket(PrimalSketch<AimsData<T>, AimsData<T> > *sketch)
	{
		typedef Point3d Site;
		AimsBucket<Void> *blobBck;
		float dx, dy, dz;
		std::list<ScaleSpaceBlob<Site>*> ssBlobList=sketch->BlobSet();
		std::list<ScaleSpaceBlob<Site>*>::iterator itBlob=ssBlobList.begin();
		GreyLevelBlob<Site>	*glBlob;
		std::set<Site,ltstr_p3d<Site> > points;
		std::set<Site,ltstr_p3d<Site> >::iterator itPoints;
		AimsBucketItem<Void> bckItem;

		dx=sketch->scaleSpace()->GetOriginalImage().sizeX();
		dy=sketch->scaleSpace()->GetOriginalImage().sizeY();
		dz=sketch->scaleSpace()->GetOriginalImage().sizeZ();

		blobBck=new AimsBucket<Void>;
		blobBck->setSizeX(dx);
		blobBck->setSizeY(dy);
		blobBck->setSizeZ(dz);

		for (; itBlob!=ssBlobList.end(); ++itBlob)
		{
			glBlob=(*itBlob)->GlBlobRep();
			points=glBlob->GetListePoints();
			itPoints=points.begin();
			for (; itPoints!=points.end(); ++itPoints)
			{
				bckItem.location()=(*itPoints);
				(*blobBck)[(*itBlob)->Label()].push_back(bckItem);
			}
		}
		return blobBck;
	}

	//------------------------------------------------------------------------------------------
	template<int D, class T> AimsSurfaceTriangle *GetSSBlobMesh(PrimalSketch<AimsSurface<D, Void>, Texture<T> > *sketch, AimsSurfaceTriangle *mesh)
	{
		typedef std::pair<Point3df,uint> Site;		// ATTENTION : cette fonction ne marche que pour les surfaces triangulees
																			// et pas pour D quelconque. C'est un peu verole mais bon...
																			// Peut-?tre le primalSketch de surfaces devrait-il ?tre limit? aux triangulations
		AimsSurfaceTriangle *meshBlob;
		std::list<ScaleSpaceBlob<Site>*> ssBlobList=sketch->BlobSet();
		std::list<ScaleSpaceBlob<Site>*>::iterator ssblobit=ssBlobList.begin();
		std::set<Site,ltstr_p3d<Site> > points;
		
		std::map<int,int> tableNodes;
		int label, index;
                uint count=0, count2=0;
		Point3df coord;
		AimsTimeSurface<3,Void> oneMesh, tempMesh;
		std::vector<Point3df> nodes;
                std::vector<Point3dd>  pts;
		std::vector< AimsVector<uint,3> > poly, allPoly;
		std::vector< AimsVector<uint,3> >::iterator itPoly;
		AimsVector<uint,3> tri;

		meshBlob=new AimsSurfaceTriangle();
                (*mesh)[0].updateNormals();

                std::cout << "==============================" << std::endl;
		for (; ssblobit!=ssBlobList.end(); ++ssblobit ) { //  ON PARCOURT LES SSBLOBS
                  oneMesh=AimsSurfaceTriangle();
                  label=(*ssblobit)->Label();

                  std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bssb n°" << label << "("<< count2++ <<"/" << ssBlobList.size()<<") " << std::flush;

                  std::list<GreyLevelBlob<Site>*>::iterator glblobit=(*ssblobit)->glBlobs.begin();
                  std::map<float,GreyLevelBlob<Site>*> glblobmap;
                  for (;glblobit != (*ssblobit)->glBlobs.end();glblobit++){
                    glblobmap[(*glblobit)->GetScale()] = *glblobit;
                  }
                  std::map<float,GreyLevelBlob<Site>*>::iterator mapit=glblobmap.begin();

                  for (count = 0;count<1 && count <glblobmap.size();count++,mapit++){ // ON PARCOURT LES GLBLOBS DU SSB 
                      tempMesh=AimsSurfaceTriangle();
                      tableNodes.clear();
                      points=(*mapit).second->GetListePoints();
                      points=(*ssblobit)->GlBlobRep()->GetListePoints();
                      std::set<Site,ltstr_p3d<Site> >::iterator itPoints=points.begin();
                      std::set<uint> auxpts;
                      for (; itPoints!=points.end(); ++itPoints){				    
                        index=(*itPoints).second;
                        auxpts.insert(index);
                      }
                      for (uint i=0; i<(*mesh)[0].polygon().size(); i++){
                        if ((auxpts.find((*mesh)[0].polygon()[i][0]) != auxpts.end())		// si le polytgone ne contient que des
                             && (auxpts.find((*mesh)[0].polygon()[i][1]) != auxpts.end())	// points du blob
                             && (auxpts.find((*mesh)[0].polygon()[i][2]) != auxpts.end())){	// on le garde
                          for (uint j=0;j<3;j++){
                            Point3df node((*mesh)[0].vertex()[(*mesh)[0].polygon()[i][j]]);
                            Point3df norm((*mesh)[0].normal()[(*mesh)[0].polygon()[i][j]]);
                            norm *= 0.01;
                            node += norm;
                            tempMesh[0].vertex().push_back(Point3df(node[0],node[1],node[2]));
                          }
                          tempMesh[0].polygon().push_back(AimsVector<uint,3>( tempMesh[0].vertex().size()-3,tempMesh[0].vertex().size()-2,tempMesh[0].vertex().size()-1));
                        }
                      }

                      //                         JUSTE LES CYLINDRES DE LIAISON
//                       if ((*ssblobit)->TopBifurcation()->Type() != APPEAR){
//                         AimsSurfaceTriangle *msh;
//                         std::list<ScaleSpaceBlob<Site>*> topblobs = (*ssblobit)->TopBifurcation()->TopBlobs();
//                         Point3df v1 = (*mesh).vertex()[(*ssblobit)->GlBlobRep()->GetMaximum()->_node.second];
//                         Point3df n1 = (*mesh).normal()[(*ssblobit)->GlBlobRep()->GetMaximum()->_node.second];
//                         n1 *= (*mapit).second->GetScale()*3;
//                         v1 += n1;
//                         std::list<ScaleSpaceBlob<Site>*>::iterator ittb=topblobs.begin(); 
//                         for (; ittb !=topblobs.end();++ittb){
//                           Point3df v2 = (*mesh).vertex()[(*ittb)->GlBlobRep()->GetMaximum()->_node.second];
//                           Point3df n2 = (*mesh).normal()[(*ittb)->GlBlobRep()->GetMaximum()->_node.second];
//                           n2 *= (*ittb)->ScaleMin();
//                           v2 += n2;
//                           msh = SurfaceGenerator::cylinder(v1 ,v2 , 0.1, 0.1, 6,true,true );
//                           SurfaceManip::meshMerge( tempMesh, *msh );
//                         }
//                       }
//                      
                      SurfaceManip::meshMerge( oneMesh, tempMesh );
                  }
                  

                  if (oneMesh[0].polygon().size()!=0){
                    (*meshBlob)[label]=AimsSurface<3,Void>();
                    (*meshBlob)[label].polygon()=oneMesh[0].polygon();
                    (*meshBlob)[label].vertex()=oneMesh[0].vertex();
                    (*meshBlob)[label].updateNormals();
                  }
                  else {
//                   if (true){
                    AimsSurfaceTriangle *msh;
                    Point3df auxnode = (*mesh)[0].vertex()[(*ssblobit)->GlBlobRep()->GetMaximum()->_node.second];
                    msh = SurfaceGenerator::sphere(auxnode , 0.5,6 );
                    (*meshBlob)[label]=AimsSurface<3,Void>();
                    (*meshBlob)[label].polygon()=(*msh).polygon();
                    (*meshBlob)[label].vertex()=(*msh).vertex();
                    (*meshBlob)[label].updateNormals();
                  }

		}
                std::cout << "END" << std::endl;

		return meshBlob;
	}

        template<typename T> AimsBucket<Void> *GetSSBlobMeshBucket(AimsSurfaceTriangle *mesh)
        {
          AimsBucket<Void> *blobBck;
          AimsBucketItem<Void> bckItem;

          blobBck=new AimsBucket<Void>;
          blobBck->setSizeX(1.0);
          blobBck->setSizeY(1.0);
          blobBck->setSizeZ(1.0);

          for (uint i=0; i<(*mesh).size(); i++) {
            for (uint j=0; j<(*mesh)[i].vertex().size(); j++)
            {
              bckItem.location()=static_cast<Point3d>((*mesh)[i].vertex()[j]);
              (*blobBck)[i].push_back(bckItem);
            }
          }
          return blobBck;
        }
        

	//------------------------------------------------------------------------------------------
	template<typename Geom, typename Text> 	void AddBlobsToPSGraph(PrimalSketch<Geom,Text> *sketch, Graph *graph)
	{
	}


	//------------------------------------------------------------------------------------------
	template<typename T> void AddBlobsToPSGraph(PrimalSketch<AimsData<T>, AimsData<T> > *sketch, Graph *graph)	{
          std::set<Vertex * > vertSet;
          std::set<Vertex *>::iterator itVert;
          Vertex *node;
          BucketMap<Void> *bckMap;
          int label;
          aims::GraphManip manip;
          carto::rc_ptr<BucketMap<Void> > ptrBck;
          std::vector<int> bounding_min, bounding_max;
          std::vector<float> resolution;

          bounding_min.push_back(0);
          bounding_min.push_back(0);
          bounding_min.push_back(0);
          bounding_max.push_back(sketch->scaleSpace()->GetOriginalImage().dimX());
          bounding_max.push_back(sketch->scaleSpace()->GetOriginalImage().dimY());
          bounding_max.push_back(sketch->scaleSpace()->GetOriginalImage().dimZ());
          resolution.push_back(sketch->scaleSpace()->GetOriginalImage().sizeX());
          resolution.push_back(sketch->scaleSpace()->GetOriginalImage().sizeY());
          resolution.push_back(sketch->scaleSpace()->GetOriginalImage().sizeZ());

          graph->setProperty( "boundingbox_min", bounding_min );
          graph->setProperty( "boundingbox_max", bounding_max );
          graph->setProperty( "voxel_size", resolution );

          bckMap = new BucketMap<Void>( *GetSSBlobBucket(sketch) );
          bckMap->setSizeX( resolution[0] );
          bckMap->setSizeY( resolution[1] );
          bckMap->setSizeZ( resolution[2] );
          vertSet = graph->vertices();
          for ( itVert = vertSet.begin() ; itVert != vertSet.end() ; ++itVert ) {
            node =*itVert;
            node->getProperty( "index", label );
            ptrBck=carto::rc_ptr<BucketMap<Void> >( new BucketMap<Void> );
            (*ptrBck)[0] = (*bckMap)[label];
            ptrBck->setSizeX( resolution[0] );
            ptrBck->setSizeY( resolution[1] );
            ptrBck->setSizeZ( resolution[2] );
            manip.storeAims( *graph, node, "ssblob", ptrBck );
            node->setProperty( "ssblob_label", label );
          }
	}

	//------------------------------------------------------------------------------------------
	template<int D, class T> void AddBlobsToPSGraph(PrimalSketch<AimsSurface<D, Void>, Texture<T> > *sketch, Graph *graph){
          std::set<Vertex * > vertSet;
          std::set<Vertex *>::iterator itVert;
          Vertex *node;
          AimsSurfaceTriangle *tore;
          BucketMap<Void> *bckMap;

          int label;
          aims::GraphManip manip;
          carto::rc_ptr<AimsSurfaceTriangle> ptrTore;
          carto::rc_ptr<BucketMap<Void> > ptrBck;

          std::vector<int> bounding_min, bounding_max;
          std::vector<float> bounding_minf, bounding_maxf;
          std::vector<float> resolution;
          Point3df mini, maxi;
          
          AimsSurfaceTriangle *mesh;
          mesh = new AimsSurfaceTriangle();

          if (sketch->scaleSpace()->AuxMesh() != NULL){
            mesh = sketch->scaleSpace()->AuxMesh();
          }
          else {
            AimsSurface<3,Void> auxsurf(*(sketch->scaleSpace()->Mesh()));
            (*mesh)[0] = auxsurf;
          }
                    
          for (uint z=0;z<3;z++){ mini[z] = 100000000.0; maxi[z] = -100000000.0; }
                
          for (uint i=0;i<(*mesh)[0].vertex().size();i++)
            for (uint z=0;z<3;z++){
            mini[z] = std::min((*mesh)[0].vertex()[i][z], mini[z]);
            maxi[z] = std::max((*mesh)[0].vertex()[i][z], maxi[z]);
            }

          resolution.push_back(1);
          resolution.push_back(1);
          resolution.push_back(1);
          bounding_min.push_back(-1);
          bounding_min.push_back(-1);
          bounding_min.push_back(-1);
          bounding_max.push_back(1);
          bounding_max.push_back(1);
          bounding_max.push_back(1);
          graph->setProperty("boundingbox_min", bounding_min);
          graph->setProperty("boundingbox_max", bounding_max);
          graph->setProperty("voxel_size", resolution);

          tore=GetSSBlobMesh(sketch, mesh);
          bckMap=new BucketMap<Void>();
          bckMap->setSizeX(resolution[0]);
          bckMap->setSizeY(resolution[1]);
          bckMap->setSizeZ(resolution[2]);
          vertSet=graph->vertices();

          for (itVert=vertSet.begin(); itVert!=vertSet.end(); ++itVert){
            node=*itVert;
            node->getProperty("index", label);
            // Rajouter le bckMap[label] dans les attributs
            ptrTore=carto::rc_ptr<AimsSurfaceTriangle>(new AimsSurfaceTriangle);
            (*ptrTore)[0]=(*tore)[label];
//             for (uint j=0; j< (*tore)[label].vertex().size(); j++){
//               pair<Point3d,Void> aux;
//               aux.first = Point3d((int)(((*tore)[label].vertex()[j][0]+resolution[0]/2.0)/resolution[0]),(int)(((*tore)[label].vertex()[j][1]+resolution[1]/2.0)/resolution[1]),(int)(((*tore)[label].vertex()[j][2]+resolution[2]/2.0)/resolution[2]));
//               (*bckMap)[label].insert(aux);
//             }

            manip.storeAims( *graph, node, "ssblob", ptrTore );
            ptrBck=carto::rc_ptr<BucketMap<Void> >(new BucketMap<Void>);

            (*ptrBck)[0]=(*bckMap)[label];
            ptrBck->setSizeX(resolution[0]);
            ptrBck->setSizeY(resolution[1]);
            ptrBck->setSizeZ(resolution[2]);
//             manip.storeAims( *graph, node, "ssblobbucket", ptrBck );
//             node->setProperty("ssblobbucket_label", label);
            node->setProperty("ssblob_label", label);
          }

	}

	//------------------------------------------------------------------------------------------
	template<int D, class T> AimsSurfaceTriangle *GetSSBlobTore(PrimalSketch<AimsSurface<D, Void>, Texture<T> > *sketch)
	{
		AimsSurfaceTriangle *blobMesh, *blobTore;


		blobMesh=GetSSBlobMesh(sketch);

		// Code from here is a simple extension from the ScaleSpace::surface2Tore function
		// by Arnaud Cachia

  		unsigned  i, t=0, p, t1, t2, t3;
		//AimsSurfaceTriangle::iterator itMesh;
		unsigned		nedge = 0;
  		AimsSurfaceTriangle	*msh, *tmpMesh;

		blobTore=new AimsSurfaceTriangle;

  		//for (itMesh==blobMesh->begin();itMesh!=blobMesh->end();++itMesh)
  		for (t=0; t<blobMesh->size(); t++)
		{
 			const std::vector<Point3df>  vert = (*blobMesh)[t].vertex();
  			const std::vector<AimsVector<uint,3> >  poly = (*blobMesh)[t].polygon();
			std::map<std::pair<unsigned, unsigned>, unsigned> edges;
  			std::map<std::pair<unsigned, unsigned>, unsigned>::iterator ie, eie = edges.end();
			p = poly.size();

  			for( i=0; i<p; ++i )
    		{
      			t1 = poly[i][0];
      			t2 = poly[i][1];
      			t3 = poly[i][2];

				if( t1 < t2 )
					++edges[ std::pair<unsigned, unsigned>( t1, t2 ) ];
      			else
					++edges[ std::pair<unsigned, unsigned>( t2, t1 ) ];
      			if( t1 < t3 )
					++edges[ std::pair<unsigned, unsigned>( t1, t3 ) ];
      			else
					++edges[ std::pair<unsigned, unsigned>( t3, t1 ) ];
      			if( t2 < t3 )
					++edges[ std::pair<unsigned, unsigned>( t2, t3 ) ];
      			else
				++edges[ std::pair<unsigned, unsigned>( t3, t2 ) ];
    		}

			tmpMesh=new AimsSurfaceTriangle;
  			for( ie=edges.begin(); ie!=eie; ++ie)
    			if( (*ie).second == 1 )
      			{
					++nedge;
					msh = SurfaceGenerator::cylinder( vert[(*ie).first.first], vert[(*ie).first.second], 0.3, 0.3, 3, false );
					SurfaceManip::meshMerge( *tmpMesh, *msh );
					delete msh;
      			}
			(*blobTore)[t]=(*tmpMesh)[0];
			delete tmpMesh;
		}

  		return( blobTore );
	}

}

#endif
