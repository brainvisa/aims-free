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

#ifndef AIMS_PRIMALSKETCH_PRIMALSKETCH_H
#define AIMS_PRIMALSKETCH_PRIMALSKETCH_H

#include <cstdlib>
#include <aims/config/aimsalgo_config.h>
#include <aims/primalsketch/scaleSpaceBlob.h>
#include <aims/primalsketch/scalespace.h>
#include <aims/primalsketch/greyLevelBlob.h>
#include <set>
#include <map>
#include <list>
#include <aims/mesh/surfaceOperation.h>
#include <aims/mesh/surfacegen.h>


namespace aims
{

  enum PrimalsketchDataType
    {
      SURFACE,
      IMAGE
    };

     template<typename Geom, typename Text> class PrimalSketch
     {

     protected:
          typedef typename SiteType<Geom>::type Site;
          typedef typename TexType<Text>::type Val;
          
          string _subject;

          ScaleSpace<Geom, Text>   *_scaleSpace;
          std::list<Bifurcation<Site>*> bifurcationList;
          std::list<ScaleSpaceBlob<Site>*> blobList;
          int labelMax; // ssblob label max (=final numbre of blobs); incremented during detection

          float t_min;   // top and bottom scales
          float t_max;

          TexturedData<Geom, Text> *_mask;   // masking : outside the mask, blob are removed

          float min_delta_t; // minimal scale interval for refinements

          int _type;  // is it a  surface or image-based primal sketch ?
                           // default is image

     public:

          PrimalSketch() : _scaleSpace(NULL), _mask(NULL) {}
          PrimalSketch(string subject,  int type) : _subject(subject) , _scaleSpace(NULL), labelMax(0),_mask(NULL), _type(type) {}
          PrimalSketch(string subject, ScaleSpace<Geom, Text> *scaleSpace, int type)
                    : _subject(subject) , _scaleSpace(scaleSpace), labelMax(0), _mask(NULL), _type(type)  {SetMinDt();}
          PrimalSketch(string subject, ScaleSpace<Geom, Text> *scaleSpace, TexturedData<Geom, Text> *mask, int type)
                    : _subject(subject) , _scaleSpace(scaleSpace), labelMax(0), _mask(mask), _type(type)  {SetMinDt();}

          string Subject() {return _subject;}
          int Type() {return _type;}
          void setType(int t){_type = t; }

          void SetMinDt() {if (!_scaleSpace->smoother()->optimal()) min_delta_t=0.5;
                                                       else min_delta_t=_scaleSpace->smoother()->dt();}
          void SetScaleSpace(ScaleSpace<Geom, Text> *scaleSpace)
                                                  {_scaleSpace=scaleSpace;SetMinDt();}
          std::list<Bifurcation<Site>*> BifurcationList() {return bifurcationList;}
          std::list<ScaleSpaceBlob<Site>*> BlobSet() {return blobList;}
          ScaleSpace<Geom, Text>   *scaleSpace() {return _scaleSpace;}

          void AddBlob(ScaleSpaceBlob<Site> *blob) {blobList.push_back(blob);}
          void AddBifurcation(Bifurcation<Site> *bifurcation ) {bifurcationList.push_back(bifurcation);}

          ScaleSpaceBlob<Site>     *Blob(int label);

          void ComputePrimalSketch(float tmin, float tMax, string statFile="", uint intersection_param=10);

          void MatchScaleLevels(float t_up, float t_down, uint intersection_param );

          void ComputeBlobMeasurements(string statName="");

          ScaleSpaceBlob<Site>     *GetSSBlobFromGLBlob(GreyLevelBlob<Site> *glBlob);

     };


     //--------------------------------------------------------------
     // IMPLEMENTATION
     //--------------------------------------------------------------

     template<typename Geom, typename Text>
     ScaleSpaceBlob<typename SiteType<Geom>::type>
        *PrimalSketch<Geom,Text>::Blob(int label)
     {
                typename std::list<ScaleSpaceBlob<Site>*>::iterator itBlobList = blobList.begin();
          for (; itBlobList!=blobList.end(); ++itBlobList)
          {
               if ((*itBlobList).Label() == label)
                    return (*itBlobList);
          }
     }

     //---------------------------------------------------------------------

     template<typename Geom, typename Text> // Pas super optimal mais bon...
     ScaleSpaceBlob<typename SiteType<Geom>::type> *PrimalSketch<Geom,Text>::GetSSBlobFromGLBlob(GreyLevelBlob<Site> *glBlob)
     {
          typename std::list<ScaleSpaceBlob<Site>*>::iterator itBlobList=blobList.begin();
          ScaleSpaceBlob<Site> *ssBlob;
          std::list<GreyLevelBlob<Site>*> glBlobList;
          typename std::list<GreyLevelBlob<Site>*>::iterator
                  itGLBlobList;
          for (; itBlobList!=blobList.end(); ++itBlobList)
          {
               ssBlob=*itBlobList;
               glBlobList=ssBlob->glBlobs;
               itGLBlobList=glBlobList.begin();
               for (; itGLBlobList!=glBlobList.end(); ++itGLBlobList)
                    if (   ((*itGLBlobList)->Label()==glBlob->Label()) &&  ((*itGLBlobList)->GetScale()==glBlob->GetScale()) )
                        return ssBlob;
          }
          // blob not found, return NULL
          std::cout << "SS BLOB label=" << glBlob->Label() << " and scale " << glBlob->GetScale() << " cannot be found...." << std::endl;
          return NULL;
     }

     //---------------------------------------------------------------------

     template<typename Geom, typename Text>
     void PrimalSketch<Geom,Text>::ComputePrimalSketch(float tmin, float tmax, string statFile, uint intersection_param)
     {
          // Algo : On part del'echelle la plus haute et on descend vers l'image originale
          // A chaque niveau : on regarde vers le niveau en dessous et on applique
          //                                           MatchScaleLevels();
          // Si ca marche on descend d'un cran et on recommence
          // Si ca ne marche pas on calcule un niveau intermediaire et on recommence (~recursif)

          std::cout << "Computing primal sketch between tmin=" << tmin << " and tmax=" << tmax << std::endl;

          t_min=tmin;
          t_max=tmax;
        float t_bif=tmax*2;

          if (_scaleSpace==NULL)
          {
               std::cerr << "Trying to compute a primal sketch without a scale-space" << std::endl;
               std::cerr << "Subject : " << _subject;
               exit(EXIT_FAILURE);
          }

          std::set<float> scaleList=_scaleSpace->GetScaleList();
          std::set<float>::reverse_iterator itScaleUp=scaleList.rbegin();
          std::set<float>::reverse_iterator itScaleDown;
          float t_up, t_down;

          for (; (*itScaleUp) != tmax; ++itScaleUp)
          {
               if (itScaleUp==scaleList.rend())
               {
                    std::cerr << "Scale max " << tmax << " does not exist" << std::endl;
                    exit(EXIT_FAILURE);
               }
          }

          ScaleLevel<Geom,Text> *levelUp;
          levelUp=_scaleSpace->Scale(*itScaleUp);
        ScaleLevel<Geom,Text> *levelDown;
          levelDown=_scaleSpace->Scale(tmin);

          int nbBlobs;

          levelUp->DetectBlobs(_mask);

          nbBlobs=levelUp->nbBlobs();

          GreyLevelBlob<Site> *glBlob;
          ScaleSpaceBlob<Site> *ssBlob;
          Bifurcation<Site> *bifurc;
          // scale-space blob initalisation
          std::map<int, GreyLevelBlob<Site> *>
                  listeGLB=levelUp->BlobList();
          typename std::map<int, GreyLevelBlob<Site> *>::iterator
                  itGL=listeGLB.begin();
          for (; itGL != listeGLB.end(); ++itGL)
          {
               glBlob=(*itGL).second;
               ssBlob=new ScaleSpaceBlob<Site>(_subject, labelMax);
               ssBlob->SetScaleMax(*itScaleUp);
               ssBlob->SetScaleMin(t_min);
               ssBlob->AddGreyLevelBlob(glBlob);
               bifurc=new Bifurcation<Site>(DISAPPEAR, t_bif, t_max);
               bifurc->AddBottomBlob(ssBlob);
               ssBlob->SetTopBifurcation(bifurc);
               AddBlob(ssBlob);
               AddBifurcation(bifurc);
               labelMax++;
          }

          // primal sketch computation, top to bottom
          
          for (; (*itScaleUp) > tmin; ++itScaleUp)
          {
               itScaleDown=itScaleUp;
               itScaleDown++;
               t_up=*itScaleUp;
               t_down=*itScaleDown;
               MatchScaleLevels(t_up, t_down, intersection_param);
          }

        // "closing" the primal sketch at bottom

        listeGLB=levelDown->BlobList();
          itGL=listeGLB.begin();
        t_bif=t_min/2.0;
          for (; itGL != listeGLB.end(); ++itGL)
          {
               glBlob=(*itGL).second;
            ssBlob=GetSSBlobFromGLBlob(glBlob);
            bifurc=new Bifurcation<Site>(APPEAR, t_min, t_bif);
            ssBlob->SetBottomBifurcation(bifurc);
            bifurc->AddTopBlob(ssBlob);
            AddBifurcation(bifurc);
          }

          // blob updates

          typename std::list<ScaleSpaceBlob<Site>*>::iterator
                  itBlob=blobList.begin();
          for (; itBlob != blobList.end(); ++itBlob)
          {
               (*itBlob)->ComputeLifeTime();
               (*itBlob)->ComputeScaleRep();
          }
//           typename std::list<ScaleSpaceBlob<Site>*>::iterator itBlob=blobList.begin();
        // blob (multiscale) measurements are computed here.
        
        ComputeBlobMeasurements(statFile);        
        cout << "ssblobs avant : " << blobList.size() << endl;

        // on va faire une texture de blobs

/*        uint nb_scales = _scaleSpace->GetScaleLevels().size();
//         cout << "nb d'échelles :" << nb_scales << " " << _scaleSpace->Scale(0.0)->Mesh()->vertex().size() << endl;
//         AimsSurfaceTriangle *auxmesh; auxmesh = _scaleSpace->AuxMesh();
        TimeTexture<float> blobs(nb_scales, _scaleSpace->Scale(0.0)->Mesh()->vertex().size());
        TimeTexture<float> blobs_t(nb_scales, _scaleSpace->Scale(0.0)->Mesh()->vertex().size());
        for (uint i=0;i<blobs.size();i++)
          for (uint j=0;j<blobs[i].nItem();j++)
            blobs[i].item(j) = 0.0;
        typename std::list<ScaleSpaceBlob<Site>*>::iterator itssb = blobList.begin();
          set<float> scales = _scaleSpace->GetScaleList();

        vector<uint> histoblobs(20);
        
        for (; itssb != blobList.end(); ++itssb){
          ScaleSpaceBlob<Site> *ssb; ssb = *itssb;
          
          
          
          typename std::list<GreyLevelBlob<Site>*>::iterator glBlobit= ssb->glBlobs.begin();
          float deplacement = 0.0;
          float areaevol = 0.0;
          Point3df bar = (*glBlobit)->GetMaximum()->_node.first;
          float area = (*glBlobit)->measurements.area;
          for (; glBlobit != ssb->glBlobs.end(); glBlobit++){
            Point3df old = bar;
            bar = (*glBlobit)->GetMaximum()->_node.first;
            deplacement += sqrt(pow(old[0]-bar[0],2) + pow(old[1]-bar[1],2) + pow(old[2]-bar[2],2));

          }
          cout << "dep: " << deplacement << " " ;
          deplacement = ssb->GetMeasurements().t;
          areaevol = ssb->GetMeasurements().tValue;
          glBlobit= ssb->glBlobs.begin();
          for (; glBlobit != ssb->glBlobs.end(); glBlobit++){
            float sc = (*glBlobit)->GetScale();
            uint indexsc = 0;
            set<float>::iterator scit;
            for (scit=scales.begin();scit!=scales.end() && *scit != sc;scit++,indexsc++){}
            set<Site,ltstr_p3d<Site> > &listepts = (*glBlobit)->GetListePoints();
            typename set<Site,ltstr_p3d<Site> >::iterator ptsit=listepts.begin();
            for (;ptsit!=listepts.end();ptsit++){
              blobs[indexsc].item((*ptsit).second) = deplacement+1.0;
              blobs_t[indexsc].item((*ptsit).second) = areaevol+1.0;
            }
//             blobs[indexsc].item((*glBlobit)->GetMaximum()->_node.second) = deplacement + 1000.0;
              
          }

          
        }*/
//         Writer<TimeTexture<float> > ww("/home/grg/testblobs_t.tex");
//         Writer<TimeTexture<float> > ww1("/home/grg/testblobs_tvalue.tex");
//         ww.write(blobs);
//         ww1.write(blobs_t);


        // ELAGAGE
        typename std::list<Bifurcation<Site>*>::iterator bifit=bifurcationList.begin();
        for (bifit = bifurcationList.begin() ;  bifit != bifurcationList.end() ; bifit++){
          if ((*bifit)->Type() == APPEAR){
            std::list<ScaleSpaceBlob<Site>*> blist = (*bifit)->TopBlobs();
            typename std::list<ScaleSpaceBlob<Site>*>::iterator topit=blist.begin();
            ScaleSpaceBlob<Site> *ssblob1;
            ssblob1 = *topit;
            if ((*ssblob1).GlBlobRep()->GetListePoints().size() == 1) {
              int label1 = ssblob1->Label();
              for (itBlob=blobList.begin(); itBlob != blobList.end(); ++itBlob)
                if ((*itBlob)->Label() == label1) {
                  blobList.erase(itBlob);
                  break;
                }
              
            }

          }
          else if ((*bifit)->Type() == DISAPPEAR){
            std::list<ScaleSpaceBlob<Site>*> blist = (*bifit)->BottomBlobs();
            typename std::list<ScaleSpaceBlob<Site>*>::iterator bottomit=blist.begin();
            ScaleSpaceBlob<Site> *ssblob1;
            ssblob1 = *bottomit;
            if ((*ssblob1).GlBlobRep()->GetListePoints().size()  == 1) {
              int label1 = ssblob1->Label();
              for (itBlob=blobList.begin(); itBlob != blobList.end(); ++itBlob)
                if ((*itBlob)->Label() == label1) {
                  blobList.erase(itBlob);
                  break;
                }
            }
          }
          else if ((*bifit)->Type() == MERGE){
//             cout << "merge" << (*bifit)->BottomBlobs().size()<< ";" << (*bifit)->TopBlobs().size() << endl;
            std::list<ScaleSpaceBlob<Site>*> blist = (*bifit)->BottomBlobs();
            typename std::list<ScaleSpaceBlob<Site>*>::iterator bottomit=blist.begin();
            ScaleSpaceBlob<Site> *ssblob1, *ssblob2;
            ssblob1 = *bottomit;
            bottomit++;
            ssblob2 = *bottomit;
            /* float area1 = (*ssblob1).GetMeasurements().area;
            float area2 = (*ssblob2).GetMeasurements().area; */
//             cout << /*area1 << ";" << area2 << ";" <<*/ max(area1,area2)/min(area1,area2) << endl;
            if ((*ssblob2).GlBlobRep()->GetListePoints().size() == 1) {
              // LE BLOB2 EST PETIT : ON FUSIONNE BLOB1 AVEC TOPBLOB ET TOPBIF DE BLOB2 DEVIENT DISPARITION
              
              ssblob2->TopBifurcation()->setType(DISAPPEAR);
              ssblob2->TopBifurcation()->TopBlobs().clear();
              ssblob2->TopBifurcation()->BottomBlobs().clear();
              ssblob2->TopBifurcation()->AddBottomBlob(ssblob2);
              
              blist = (*bifit)->TopBlobs();
              ScaleSpaceBlob<Site> *topblob = *(blist.begin());
              // RAJOUT DES GLB DE SSBLOB1 
              std::list<GreyLevelBlob<Site>*> glb = ssblob1->glBlobs;
              typename std::list<GreyLevelBlob<Site>*>::iterator glbit = glb.begin();
              for (; glbit != glb.end() ; glbit++)
                topblob->AddGreyLevelBlob(*glbit);
              
              topblob->SetBottomBifurcation(ssblob1->BottomBifurcation());
              topblob->SetScaleMin(ssblob1->ScaleMin());
              
              int label1 = ssblob1->Label();
              for (itBlob=blobList.begin(); itBlob != blobList.end(); ++itBlob)
                if ((*itBlob)->Label() == label1) {
                  blobList.erase(itBlob);
                  break;
                }
            }
            else if ((*ssblob1).GlBlobRep()->GetListePoints().size() == 1){
              // LE BLOB1 EST PETIT : ON FUSIONNE BLOB2 AVEC TOPBLOB ET TOPBIF DE BLOB1 DEVIENT DISPARITION
              
              ssblob1->TopBifurcation()->setType(DISAPPEAR);
              ssblob1->TopBifurcation()->TopBlobs().clear();
              ssblob1->TopBifurcation()->BottomBlobs().clear();
              ssblob1->TopBifurcation()->AddBottomBlob(ssblob1);
              
              blist = (*bifit)->TopBlobs();
              ScaleSpaceBlob<Site> *topblob = *(blist.begin());
              // RAJOUT DES GLB DE SSBLOB2 
              std::list<GreyLevelBlob<Site>*> glb = ssblob2->glBlobs;
              typename std::list<GreyLevelBlob<Site>*>::iterator glbit = glb.begin();
              for (; glbit != glb.end() ; glbit++)
                topblob->AddGreyLevelBlob(*glbit);
              
              topblob->SetBottomBifurcation(ssblob2->BottomBifurcation());
              topblob->SetScaleMin(ssblob2->ScaleMin());
              
              int label2 = ssblob2->Label();
              for (itBlob=blobList.begin(); itBlob != blobList.end(); ++itBlob)
                if ((*itBlob)->Label() == label2) {
                  blobList.erase(itBlob);
                  break;
                }
            }
              
          }
          else if ((*bifit)->Type() == SPLIT){
//             cout << "split" << (*bifit)->TopBlobs().size()<< ";" << (*bifit)->BottomBlobs().size() << endl;
            std::list<ScaleSpaceBlob<Site>*> blist = (*bifit)->TopBlobs();
            typename std::list<ScaleSpaceBlob<Site>*>::iterator topit=blist.begin();
            ScaleSpaceBlob<Site> *ssblob1, *ssblob2;
            ssblob1 = *topit;
            topit++;
            ssblob2 = *topit;
            /* float area1 = (*ssblob1).GetMeasurements().area;
            float area2 = (*ssblob2).GetMeasurements().area; */
//             cout << /*area1 << ";" << area2 << ";" <<*/ max(area1,area2)/min(area1,area2) << endl;
            if ((*ssblob1).GlBlobRep()->GetListePoints().size() == 1) {
              // LE BLOB2 EST PETIT : ON FUSIONNE BLOB1 AVEC BOTTOMBLOB ET BOTTOMBIF DE BLOB2 DEVIENT DISPARITION
              
              ssblob2->BottomBifurcation()->setType(DISAPPEAR);
              ssblob2->BottomBifurcation()->BottomBlobs().clear();
              ssblob2->BottomBifurcation()->TopBlobs().clear();
              ssblob2->BottomBifurcation()->AddTopBlob(ssblob2);
              
              blist = (*bifit)->BottomBlobs();
              ScaleSpaceBlob<Site> *topblob = *(blist.begin());
              // RAJOUT DES GLB DE SSBLOB1 
              std::list<GreyLevelBlob<Site>*> glb = ssblob1->glBlobs;
              typename std::list<GreyLevelBlob<Site>*>::iterator glbit = glb.begin();
              for (; glbit != glb.end() ; glbit++)
                topblob->AddGreyLevelBlob(*glbit);
              
              topblob->SetTopBifurcation(ssblob1->TopBifurcation());
              topblob->SetScaleMin(ssblob1->ScaleMin());
              
              int label1 = ssblob1->Label();
              for (itBlob=blobList.begin(); itBlob != blobList.end(); ++itBlob)
                if ((*itBlob)->Label() == label1) {
                  blobList.erase(itBlob);
                  break;
                }
            }
            else if ((*ssblob1).GlBlobRep()->GetListePoints().size() == 1){
              // LE BLOB1 EST PETIT : ON FUSIONNE BLOB2 AVEC BOTTOMBLOB ET BOTTOMBIF DE BLOB1 DEVIENT DISPARITION
              
              ssblob1->BottomBifurcation()->setType(DISAPPEAR);
              ssblob1->BottomBifurcation()->BottomBlobs().clear();
              ssblob1->BottomBifurcation()->TopBlobs().clear();
              ssblob1->BottomBifurcation()->AddTopBlob(ssblob1);
              
              blist = (*bifit)->BottomBlobs();
              ScaleSpaceBlob<Site> *topblob = *(blist.begin());
              // RAJOUT DES GLB DE SSBLOB2 
              std::list<GreyLevelBlob<Site>*> glb = ssblob2->glBlobs;
              typename std::list<GreyLevelBlob<Site>*>::iterator glbit = glb.begin();
              for (; glbit != glb.end() ; glbit++)
                topblob->AddGreyLevelBlob(*glbit);
              
              topblob->SetTopBifurcation(ssblob2->TopBifurcation());
              topblob->SetScaleMin(ssblob2->ScaleMin());
              
              int label2 = ssblob2->Label();
              for (itBlob=blobList.begin(); itBlob != blobList.end(); ++itBlob)
                if ((*itBlob)->Label() == label2) {
                  blobList.erase(itBlob);
                  break;
                }
            }
              
          }
        }
        
          // blob updates
cout << "ssblobs après : " << blobList.size() << endl;
          
          for (itBlob=blobList.begin(); itBlob != blobList.end(); ++itBlob)
          {
               (*itBlob)->ComputeLifeTime();
               (*itBlob)->ComputeScaleRep();
          }
        
        ComputeBlobMeasurements(statFile);

        
        
     }

     //---------------------------------------------------------------------

     template<typename Geom, typename Text>
     void PrimalSketch<Geom,Text>::MatchScaleLevels(float t_up, float t_down, uint intersection_param)
     {
          float t, t_new;
          int stat1=0, stat2=0, stat3=0;

          std::cout << "Matching " << t_up << " <-> " << t_down << std::endl;

          ScaleLevel<Geom,Text> *levelUp=_scaleSpace->Scale(t_up);
          ScaleLevel<Geom,Text> *levelDown=_scaleSpace->Scale(t_down);
          ScaleSpaceBlob<Site> *ssBlobUp, *ssBlobDown;
          Bifurcation<Site> *bifurc;

          std::cout << "Blob detection at scale " << t_up << std::endl;
          levelUp->DetectBlobs(_mask);
          std::cout << "Blob detection at scale " << t_down << std::endl;
          levelDown->DetectBlobs(_mask);

          std::cout << "\tt=" << t_up << " : " << levelUp->nbBlobs() << " blobs" << std::endl;
          std::cout << "\tt=" << t_down << " : " << levelDown->nbBlobs() << " blobs" << std::endl;

          std::map<int, GreyLevelBlob<Site> *> listeGLB_up=levelUp->BlobList();
          std::map<int, GreyLevelBlob<Site> *> listeGLB_down=levelDown->BlobList();

          // On essaye la maniere (simple mais) brutale

        std::cout << "\tComputing potential links" << std::endl;

          std::map<int, std::set<int> > matchUp;
          std::map<int, std::set<int> > matchDown;
          typename std::map<int, GreyLevelBlob<Site> *>::iterator
                  iteratorUp;
          typename std::map<int, GreyLevelBlob<Site> *>::iterator
                  iteratorDown;
          GreyLevelBlob<Site> *blobUp, *blobDown;
          int labelUp, labelDown;

          iteratorUp=listeGLB_up.begin();
          for (; iteratorUp!=listeGLB_up.end(); ++iteratorUp)
          {
               blobUp=(*iteratorUp).second; labelUp=(*iteratorUp).first;
               matchUp[labelUp]=std::set<int>();
               std::set<Site,ltstr_p3d<Site> > pointsUp=blobUp->GetListePoints();
               iteratorDown=listeGLB_down.begin();
               for (; iteratorDown!=listeGLB_down.end(); ++iteratorDown)
               {
                    blobDown=(*iteratorDown).second; labelDown=(*iteratorDown).first;
                    if (matchDown.find(labelDown) == matchDown.end())
                         matchDown[labelDown]=std::set<int>();
                    // blobs intersect eachother
                    std::set<Site,ltstr_p3d<Site> > pointsInter;
                    std::set<Site,ltstr_p3d<Site> > pointsDown=blobDown->GetListePoints();
                    std::set_intersection(pointsDown.begin(), pointsDown.end(),
                                                            pointsUp.begin(), pointsUp.end(),
                                                            std::inserter(pointsInter, pointsInter.begin()),
                                                            ltstr_p3d<Site>() );
//                     if (2*pointsInter.size()/(pointsUp.size() + pointsDown.size()) >0.5)
                    if (pointsInter.size()>intersection_param)
                    {
                         matchUp[labelUp].insert(labelDown);
                         matchDown[labelDown].insert(labelUp);
                    }
               }
          }

        //-------------------------------------------------------------------
        std::cout << "\tChecking potential refinements" << std::endl;
          std::map<int, std::set<int> >::iterator itMatchUp=matchUp.begin();
          std::map<int, std::set<int> >::iterator itMatchDown=matchDown.begin();
          int refinement=0;
          for (; itMatchUp!=matchUp.end(); ++itMatchUp)
          {
               int nUp=(*itMatchUp).second.size();
               if (nUp > 2)
               {
                    refinement=2;
               }    //---------------------------------------------------------------------
               else if (nUp == 2)
               {
                    std::set<int> setUp=(*itMatchUp).second;
                    std::set<int>::iterator itUp=setUp.begin();
                    for (; itUp!=setUp.end(); ++itUp)
                         if (matchDown[*itUp].size()>1)
                              refinement=1;
               }
               if (refinement>0) break;
          }
          if (refinement==0)
          {
               for (; itMatchDown!=matchDown.end(); ++itMatchDown)
               {
                    if ((*itMatchDown).second.size() > 2)
                         refinement=3;
                    if (refinement>0) break;
               }
          }
          t=exp((log(t_up) + log(t_down))/2.0); t_new=0;
          int i=0;
          while ((i*0.1) <= t) i++;
          t_new=(i-1)*0.1;
          if ((refinement>0) && ((t_up-t_new)>min_delta_t) && ((t_new-t_down)>min_delta_t))
          {
          std::cout << "\tRefinement needed (type " ;
               switch(refinement)
               {
                    case 1 :
                         std::cout << " 2 <-> 2)" << std::endl;
                         stat1++;
                         break;
                    case 2 :
                         std::cout << "split 1->3)" << std::endl;
                         stat2++;
                         break;
                    case 3 :
                         std::cout << "merge 3->1)" << std::endl;
                         stat3++;
                         break;
                    default :
                         std::cout << "unknown)" << std::endl;
                         break;
               }
               std::cout << "\tNew scale : " << t_new << std::endl;
               MatchScaleLevels(t_up, t_new, intersection_param);
               MatchScaleLevels(t_new, t_down, intersection_param);
               std::cout << "\trefinement successful" << std::endl;
          }
          else
          {
               if (refinement >0) std::cout << "\tAccepting matching but refinement needed (type " << refinement << ")" << std::endl;
               else std::cout  << "\tMatching OK" << std::endl;
               itMatchUp=matchUp.begin();
               itMatchDown=matchDown.begin();

            //----- new (sort of) optimised version

            std::cout << "\tResolving matching - version 2" << std::endl;
            std::vector<std::pair<std::set<int>, std::set<int> > > bifurcTable;
            std::set<int> set1, set2;
            for (; itMatchUp!=matchUp.end(); ++itMatchUp)
               {
                labelUp=(*itMatchUp).first;
                    std::set<int> setUp=(*itMatchUp).second;
                if (setUp.size()==0)
                {
                    //cr�ation d'une paire (apparition)
                        set1=std::set<int>();set1.insert(labelUp);
                        set2=setUp;
                        bifurcTable.push_back(std::pair<std::set<int>, std::set<int> >(set1, set2));
                }
                else
                {
                    int checkRepeat=0;
                    std::set<int>::iterator itSetUp;
                    for (itSetUp=setUp.begin(); (itSetUp!=setUp.end()&&(checkRepeat==0)); ++itSetUp)
                    {
                        int blobLab=(*itSetUp);
                        if (matchDown[blobLab].size() > 1)
                            checkRepeat=1;
                    }
                    if (checkRepeat==0)
                    {
                        // cr�ation d'une paire
                        set1=std::set<int>();set1.insert(labelUp);
                            set2=setUp;
                            bifurcTable.push_back(std::pair<std::set<int>, std::set<int> >(set1, set2));
                    }
                    else
                    {
                        // doit on cr�er une nouvelle paire
                        // ou ins�rer dans une existante
                        std::vector<std::pair<std::set<int>, std::set<int> > >::iterator
                            itBifurc=bifurcTable.begin();
                        int flagMerge=0;
                        for (; (itBifurc!=bifurcTable.end())&&(flagMerge==0); ++itBifurc)
                        {
                            std::set<int> setTmp=(*itBifurc).second;
                            std::set<int> setInter;
                              std::set_intersection(setUp.begin(), setUp.end(),
                                                              setTmp.begin(), setTmp.end(),
                                                              std::inserter(setInter, setInter.begin()));
                                if (setInter.size()>0)
                            {
                            //on merge ici
                                (*itBifurc).first.insert(labelUp);
                                itSetUp=setUp.begin();
                                for (; itSetUp!=setUp.end(); ++itSetUp)
                                     (*itBifurc).second.insert(*itSetUp);
                                flagMerge=1;
                            }
                        }
                        if (flagMerge==0)
                        {
                            //creation d'une paire
                            set1=std::set<int>();set1.insert(labelUp);
                                set2=setUp;
                                bifurcTable.push_back(std::pair<std::set<int>, std::set<int> >(set1, set2));
                        }
                    }
                }
            }
            for (; itMatchDown!=matchDown.end(); ++itMatchDown)
            {
                // cas des disparitions non trait� par la boucle pr�c�dente.
                labelDown=(*itMatchDown).first;
                    std::set<int> setDown=(*itMatchDown).second;
                if (setDown.size()==0)
                {
                    set2=std::set<int>();set2.insert(labelDown);
                        set1=setDown;
                    bifurcTable.push_back(std::pair<std::set<int>, std::set<int> >(set1, set2));
                }
            }

            std::vector< std::pair<std::set<int>, std::set<int> > >::iterator itTable=bifurcTable.begin();
            std::pair<std::set<int>, std::set<int> > pairSet;
            //------ end of new version


              std::cout << "\t Table -> Blobs and bifurcations" << std::endl;
               // transformation table -> bifurcations et blobs

               for (; itTable!=bifurcTable.end(); ++itTable)
               {
                    pairSet=(*itTable);
                    set1=pairSet.first;
                    set2=pairSet.second;
                    if (set2.size()==0) // creation
                    {
                         labelUp=*(set1.begin());
                         blobUp=levelUp->Blob(labelUp);
                         ssBlobUp=GetSSBlobFromGLBlob(blobUp);
                         bifurc=new Bifurcation<Site>(APPEAR, t_up, t_down);
                         ssBlobUp->SetScaleMin(t_down);
                         bifurc->AddTopBlob(ssBlobUp);
                         ssBlobUp->SetBottomBifurcation(bifurc);
                         AddBifurcation(bifurc);
                    }
                    else if (set1.size()==0) // annihilation
                    {
                         labelDown=*(set2.begin());
                         blobDown=levelDown->Blob(labelDown);
                         ssBlobDown=new ScaleSpaceBlob<Site>(_subject, labelMax);
                         labelMax++;
                         ssBlobDown->SetScaleMax(t_down);
                         ssBlobDown->SetScaleMin(t_min);
                         ssBlobDown->AddGreyLevelBlob(blobDown);
                         bifurc=new Bifurcation<Site>(DISAPPEAR, t_up, t_down);
                         bifurc->AddBottomBlob(ssBlobDown);
                         ssBlobDown->SetTopBifurcation(bifurc);
                         AddBifurcation(bifurc);
                         AddBlob(ssBlobDown);
                    }
                    else
                    {
                         if ((set1.size()==1) && (set2.size()==1)) // blob growth
                         {
                              labelUp=*(set1.begin());
                              labelDown=*(set2.begin());
                              blobUp=levelUp->Blob(labelUp);
                              ssBlobUp=GetSSBlobFromGLBlob(blobUp);
                              blobDown=levelDown->Blob(labelDown);
                              ssBlobUp->AddGreyLevelBlob(blobDown);
                         }
                         else if ((set1.size()==1) && (set2.size()==2)) // merge
                         {
                              bifurc=new Bifurcation<Site>(MERGE, t_up, t_down);
                              labelUp=*(set1.begin());
                              blobUp=levelUp->Blob(labelUp);
                              ssBlobUp=GetSSBlobFromGLBlob(blobUp);
                              ssBlobUp->SetScaleMin(t_down);
                              bifurc->AddTopBlob(ssBlobUp);
                              ssBlobUp->SetBottomBifurcation(bifurc);
                              std::set<int>::iterator itSet2=set2.begin();                              
                              for (; itSet2!=set2.end(); ++itSet2)
                              {
                                   labelDown=*itSet2;
                                   blobDown=levelDown->Blob(labelDown);
                                   ssBlobDown=new ScaleSpaceBlob<Site>(_subject, labelMax);
                                   labelMax++;                                   
                                   ssBlobDown->SetScaleMax(t_down);
                                   ssBlobDown->SetScaleMin(t_min);
                                   ssBlobDown->AddGreyLevelBlob(blobDown);
                                   bifurc->AddBottomBlob(ssBlobDown);

                                   AddBlob(ssBlobDown);
                                   ssBlobDown->SetTopBifurcation(bifurc);
                              }
                              AddBifurcation(bifurc);
 
                              
                         }
                         else if ((set1.size()==2) && (set2.size()==1)) // split
                         {
                              bifurc=new Bifurcation<Site>(SPLIT, t_up, t_down);
                              labelDown=*(set2.begin());
                              blobDown=levelDown->Blob(labelDown);
                              ssBlobDown=new ScaleSpaceBlob<Site>(_subject, labelMax);
                              labelMax++;
                              ssBlobDown->SetScaleMax(t_down);
                              ssBlobDown->SetScaleMin(t_min);
                              ssBlobDown->AddGreyLevelBlob(blobDown);
                              AddBlob(ssBlobDown);
                              ssBlobDown->SetTopBifurcation(bifurc);
                              bifurc->AddBottomBlob(ssBlobDown);
                              std::set<int>::iterator itSet1=set1.begin();
                              for (; itSet1!=set1.end(); ++itSet1)
                              {
                                   labelUp=*itSet1;
                                   blobUp=levelUp->Blob(labelUp);
                                   ssBlobUp=GetSSBlobFromGLBlob(blobUp);
                                   ssBlobUp->SetScaleMin(t_down);
                                   bifurc->AddTopBlob(ssBlobUp);
                                   ssBlobUp->SetBottomBifurcation(bifurc);
                              }
                              AddBifurcation(bifurc);

                         }
                         else // complex
                         {
                              bifurc=new Bifurcation<Site>(COMPLEX, t_up, t_down);
                              std::set<int>::iterator itSet2=set2.begin();
                              for (; itSet2!=set2.end(); ++itSet2)
                              {
                                   labelDown=*itSet2;
                                   blobDown=levelDown->Blob(labelDown);
                                   ssBlobDown=new ScaleSpaceBlob<Site>(_subject, labelMax);
                                   labelMax++;
                                   ssBlobDown->SetScaleMax(t_down);
                                   ssBlobDown->SetScaleMin(t_min);
                                   ssBlobDown->AddGreyLevelBlob(blobDown);
                                   bifurc->AddBottomBlob(ssBlobDown);
                                   AddBlob(ssBlobDown);
                                   ssBlobDown->SetTopBifurcation(bifurc);
                              }
                              std::set<int>::iterator itSet1=set1.begin();
                              for (; itSet1!=set1.end(); ++itSet1)
                              {
                                   labelUp=*itSet1;
                                   blobUp=levelUp->Blob(labelUp);
                                   ssBlobUp=GetSSBlobFromGLBlob(blobUp);
                                   ssBlobUp->SetScaleMin(t_down);
                                   bifurc->AddTopBlob(ssBlobUp);
                                   ssBlobUp->SetBottomBifurcation(bifurc);
                              }
                              AddBifurcation(bifurc);
                         }
                    }
               }
               std::cout << "\t OK" << std::endl;
          }
     }


     //---------------------------------------------------------------------

     template<typename Geom, typename Text>
     void PrimalSketch<Geom,Text>::ComputeBlobMeasurements(string statName)
    {
        ScaleSpaceBlob<Site> *ssBlob;
        std::map<float, BlobMeasurements > statsM, statsSD;
        FILE *fileStats;
        float t;
        float max_int, mean_int, max_cont, mean_cont, area, areamoy, areavar, tvalue, tv2;
        BlobMeasurements *measurements;
        GreyLevelBlob<Site> *glBlob, *glBlob1, *glBlob2;

       // Computing SSBlob measurements
       // statM contains all the stats for normalizing GLBlobs measurements
       // before integration across scales.

       cout << "Computing multiscale measurements for SSBlobs" << std::endl;

        if (statName.length()>0)
        {
            if ((fileStats=fopen(statName.c_str(), "r"))==NULL)
            {
               cerr << "Problem : cannot open stat file " << statName << endl;
               exit(EXIT_FAILURE);
            }
            cout << "Found stat file, reading it" << endl;
            while (!feof(fileStats))
            {
                fscanf(fileStats, "%f\n", &t); // !!! this defines the stat file format (simple)
                fscanf(fileStats, "%f %f %f %f %f", &max_int, &mean_int, &max_cont, &mean_cont, &area);
                measurements=new BlobMeasurements(max_int, mean_int, max_cont, mean_cont, area);
                statsM.insert(std::pair<float, BlobMeasurements>(t, *measurements));
                fscanf(fileStats, "%f %f %f %f %f", &max_int, &mean_int, &max_cont, &mean_cont, &area);
                measurements=new BlobMeasurements(max_int, mean_int, max_cont, mean_cont, area);
                statsSD.insert(std::pair<float, BlobMeasurements>(t, *measurements));
            }
            fclose(fileStats);
            typename std::list<ScaleSpaceBlob<Site>*>::iterator itSSBlobs=blobList.begin();
            cout << "processing GLblobs for normalisation" << std::endl;
            for (; itSSBlobs!=blobList.end(); ++itSSBlobs)
            {
                ssBlob=*itSSBlobs;
                typename std::list<GreyLevelBlob<Site>*>::iterator itglb;

                for (itglb=ssBlob->glBlobs.begin();itglb!=ssBlob->glBlobs.end();++itglb)
                {
                    glBlob=*itglb;
                    t=glBlob->GetScale();
                    if (statsM.find(t)!=statsM.end())
                    {
                        glBlob->measurements.maxIntensity=(glBlob->measurements.maxIntensity-statsM[t].maxIntensity)/statsSD[t].maxIntensity;
                        glBlob->measurements.meanIntensity=(glBlob->measurements.meanIntensity-statsM[t].meanIntensity)/statsSD[t].meanIntensity;
                        glBlob->measurements.maxContrast=(glBlob->measurements.maxContrast-statsM[t].maxContrast)/statsSD[t].maxContrast;
                        glBlob->measurements.meanContrast=(glBlob->measurements.meanContrast-statsM[t].meanContrast)/statsSD[t].meanContrast;
                        glBlob->measurements.area=(glBlob->measurements.area-statsM[t].area)/statsSD[t].area;
                        if (glBlob->measurements.maxIntensity > 0)
                            glBlob->measurements.maxIntensity=1+glBlob->measurements.maxIntensity;
                        else
                            glBlob->measurements.maxIntensity=exp(glBlob->measurements.maxIntensity);
                        if (glBlob->measurements.meanIntensity > 0)
                            glBlob->measurements.meanIntensity=1+glBlob->measurements.meanIntensity;
                        else
                            glBlob->measurements.meanIntensity=exp(glBlob->measurements.meanIntensity);
                        if (glBlob->measurements.maxContrast > 0)
                            glBlob->measurements.maxContrast=1+glBlob->measurements.maxContrast;
                        else
                            glBlob->measurements.maxContrast=exp(glBlob->measurements.maxContrast);
                        if (glBlob->measurements.meanContrast > 0)
                            glBlob->measurements.meanContrast=1+glBlob->measurements.meanContrast;
                        else
                            glBlob->measurements.meanContrast=exp(glBlob->measurements.meanContrast);
                        if (glBlob->measurements.area > 0)
                            glBlob->measurements.area=1+glBlob->measurements.area;
                        else
                            glBlob->measurements.area=exp(glBlob->measurements.area);
                    }
                    else
                    {
                        std::cerr << "Problem during measurements normalisation !" << std::endl;
                        std::cerr << "Stat file " << statName << " incomplete, can't find scale " << t << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
        typename std::list< ScaleSpaceBlob<Site>* >::iterator itSSBlobs=blobList.begin();
        cout << "Computing multi-scale measurements" << std::endl;

//         FILE *spmV=fopen("/home/grg/spmV.txt", "w");
	uint itest =0;
        for (; itSSBlobs!=blobList.end(); ++itSSBlobs)
        {
// 	    cout << itest << endl;
            ssBlob=*itSSBlobs;
            typename std::list<GreyLevelBlob<Site>*>::iterator itGLBlobs=ssBlob->glBlobs.begin();
            BlobMeasurements measure;
            float t2,t1, dt;
            max_int=0; mean_int=0; max_cont=0; mean_cont=0; area=0; areamoy=0; areavar=0; tvalue=0, tv2 = 0;
            glBlob1=*itGLBlobs;
            t1=glBlob1->GetScale();
            dt=log(ssBlob->TopBifurcation()->tMid())-log(t1);
            // DEBUG
            if (dt< 0)
                std::cout << "PROBLEME DE DT<0" << std::endl;
            // FIN DEBUG
            max_int += dt * (glBlob1->measurements.maxIntensity);
            mean_int += dt * (glBlob1->measurements.meanIntensity);
            max_cont += dt * (glBlob1->measurements.maxContrast);
            mean_cont += dt * (glBlob1->measurements.meanContrast);
            area += dt * (glBlob1->measurements.area);            
            tvalue += dt * (glBlob1->measurements.t);
            
//             cout << dt << ";" << glBlob1->measurements.t << " == " ;

            ++itGLBlobs;
            for (; itGLBlobs!=ssBlob->glBlobs.end(); ++itGLBlobs)
            {
                glBlob2=*itGLBlobs;
                t2=glBlob2->GetScale();
                dt=log(t1)-log(t2);
                max_int += dt * (glBlob2->measurements.maxIntensity + glBlob1->measurements.maxIntensity)/2.0;
                mean_int += dt * (glBlob2->measurements.meanIntensity + glBlob1->measurements.meanIntensity)/2.0;
                max_cont += dt * (glBlob2->measurements.maxContrast + glBlob1->measurements.maxContrast)/2.0;
                mean_cont += dt * (glBlob2->measurements.meanContrast + glBlob1->measurements.meanContrast)/2.0;
                area += dt * (glBlob2->measurements.area + glBlob1->measurements.area)/2.0;
                tvalue += dt * (glBlob2->measurements.t + glBlob1->measurements.t)/2.0;
                
//                 cout << glBlob2->measurements.tValue << " == ";

                glBlob1=glBlob2;
                t1=t2;
            }
            t1=glBlob1->GetScale();
            dt=log(t1) - log(ssBlob->BottomBifurcation()->tMid());
            // DEBUG
            if (dt< 0)
                std::cout << "PROBLEME DE DT<0" << std::endl;
            // FIN DEBUG
            max_int += dt * (glBlob1->measurements.maxIntensity);
            mean_int += dt * (glBlob1->measurements.meanIntensity);
            max_cont += dt * (glBlob1->measurements.maxContrast);
            mean_cont += dt * (glBlob1->measurements.meanContrast);
            area += dt * (glBlob1->measurements.area);
            tvalue += dt * (glBlob1->measurements.t);
            
//             cout << tvalue << endl;

            // getting t-value from original map... NORMALEMENT PLUS BESOIN

            TexturedData<Geom, Text> ima=scaleSpace()->Scale(0.0)->Data();

            glBlob1=ssBlob->GlBlobRep();
            std::set<Site,ltstr_p3d<Site> >  pixels;
            pixels=glBlob1->GetListePoints();
            

            typename std::set<Site, ltstr_p3d<Site> >::iterator itPix;
            float tvmax=-100.0, tvmax2 = -100.;


            float tvalue2 = tvmax * (ssBlob->LifeTime()) ; //* glBlob1->measurements.area;
            float tvalue2bis = tvmax2 * (ssBlob->LifeTime());
//             cout << "\tt" << tvmax <<";" << ssBlob->LifeTime() << ";" << tvalue2 << " ";

            measure=BlobMeasurements(max_int, mean_int, max_cont,mean_cont, area, tvmax, tvalue);

            areamoy +=area;
            areavar +=area*area;
            ssBlob->SetMeasurements(measure);

/*            float tt;
            tt=ssBlob->GetMeasurements().tValue;*/
//             fprintf(spmV, "%.4f\n", tt);

        }
        
        
//         for (itSSBlobs=blobList.end() ; itSSBlobs!=blobList.end(); ++itSSBlobs)
//         {
//             ssBlob=*itSSBlobs;
//             float measure = (ssBlob->GetMeasurements().area - areamoy) / (areavar - areamoy*areamoy);
//             
//             if (measure > 0) ssBlob->GetMeasurements().area =  measure + 1;
//             else ssBlob->GetMeasurements().area = exp(measure);
//         }
//         fclose(spmV);

    }

    //--------------------------------------------------------------------

}

#endif
