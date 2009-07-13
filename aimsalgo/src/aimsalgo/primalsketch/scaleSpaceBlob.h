/*
 *  Copyright (C) 2002-2003 CEA
 *
 *  This software and supporting documentation were developed by
 *   Olivier Coulon
 *   Laboratoire LSIS,Groupe LXAO
 *   ESIL, campus de Luminy, Case 925,
 *   13288 Marseille Cedex 29, France
 */

#ifndef AIMS_PRIMALSKETCH_SCALESPACEBLOB_H
#define AIMS_PRIMALSKETCH_SCALESPACEBLOB_H

#include <cstdlib>
#include <aims/config/aimsalgo_config.h>
#include <aims/primalsketch/greyLevelBlob.h>
#include <set>
#include <map>
#include <list>
#include <utility>

using namespace aims;
using namespace std;

namespace aims
{
  enum PrimalSketchEvents
    {
      APPEAR = 1,
      DISAPPEAR,
      MERGE,
      SPLIT,
      COMPLEX
    };

     template<class T> class ScaleSpaceBlob;
     template<class T> class Bifurcation {

     protected:
          char _type;
          std::list<ScaleSpaceBlob<T>*> topBlobs; // scale(bottomBlobs) < scale(topBlobs)
          std::list<ScaleSpaceBlob<T>*> bottomBlobs; // by convention
          float _tUp;
          float _tDown;
          float _tMid;

     public:

          Bifurcation(char type, float tUp, float tDown) : _type(type), _tUp(tUp), _tDown (tDown){ 

            _tMid=exp((log(_tUp)+log(_tDown))/2.0);

          }
          char Type() {return _type;}
          float tUp() {return _tUp;}
          float tDown() {return _tDown;}
          float tMid() {return _tMid;}
          void setType(char type){ _type = type; }
          void settUp(float tUp){ _tUp = tUp; }
          void settDown(float tDown){ _tDown = tDown; }
          void settMid(float tMid){ _tMid = tMid; }

          void AddTopBlob(ScaleSpaceBlob<T> *blob) {topBlobs.push_back(blob);}
          void AddBottomBlob(ScaleSpaceBlob<T> *blob) {bottomBlobs.push_back(blob);}
          std::list<ScaleSpaceBlob<T>*> TopBlobs() {return topBlobs;}
          std::list<ScaleSpaceBlob<T>*> BottomBlobs() {return bottomBlobs;}
     };

     // Scale space blobs
     // Characterised by an image/subject name (default="subject")
     // and a label. This is for future graph handling and multi-subject
     // matching

     template<class T> class ScaleSpaceBlob {

     protected:

          string _subject;
          int _label;
          //std::list<GreyLevelBlob<T>*> glBlobs;
          float scaleMin;
          float scaleMax;

          float scaleRep; // scale at which the blob will be represented
          GreyLevelBlob<T> *glBlobRep;

          float lifeTime;
          BlobMeasurements measurements;

          Bifurcation<T> *topBifurcation; // scale(topBif)>scale(bottomBif)
          Bifurcation<T> *bottomBifurcation; // by convention

               // Rajouter les mesures, localisation, glBlob representatif, etc...

     public:

          ScaleSpaceBlob() {glBlobs=std::list<GreyLevelBlob<T>*>();}
          ScaleSpaceBlob(string subject) {_subject=subject;glBlobs=std::list<GreyLevelBlob<T>*>();}
          ScaleSpaceBlob(string subject, int label) {_subject=subject; _label=label;glBlobs=std::list<GreyLevelBlob<T>*>();}
          void SetLabel(int label) {label=_label;}
          int Label() {return _label;}

        // test
          std::list<GreyLevelBlob<T>*> glBlobs;

          void SetScaleMin(float t) {scaleMin=t;}
          void SetScaleMax(float t) {scaleMax=t;}
          float ScaleMin() {return scaleMin;}
          float ScaleMax() {return scaleMax;}
          float ScaleRep() {return scaleRep;}
          float LifeTime() {return lifeTime;}
          GreyLevelBlob<T> *GlBlobRep() {return glBlobRep;}

          void SetTopBifurcation(Bifurcation<T> *bif) {topBifurcation=bif;}
          void SetBottomBifurcation(Bifurcation<T> *bif) {bottomBifurcation=bif;}
          Bifurcation<T> *TopBifurcation() {return topBifurcation;}
          Bifurcation<T> *BottomBifurcation() {return bottomBifurcation;}
          //std::list<GreyLevelBlob<T>*> GlBlobs() {return glBlobs;}
          void AddGreyLevelBlob(GreyLevelBlob<T> *blob) {glBlobs.push_back(blob);}
          void ComputeLifeTime();
          void ComputeScaleRep();
          void SetMeasurements(BlobMeasurements meas) {measurements=meas;}
          BlobMeasurements GetMeasurements() {return measurements;}
     };

     // --------------Definitions-------------------------------------------------------

     template<class T> void ScaleSpaceBlob<T>::ComputeLifeTime()
     {
          lifeTime=log(topBifurcation->tMid()) - log(bottomBifurcation->tMid());
     }

     //---------------------------------------

     template<class T> void ScaleSpaceBlob<T>::ComputeScaleRep()
     {
          // Cette definition peut changer, il peut même y en avoir plusieurs
          // temporaire, donc.
          float t, delta=1000.0;
          typename std::list<GreyLevelBlob<T>*>::iterator
                  itBlob=glBlobs.begin();

          scaleRep=exp((log(scaleMin) + log(scaleMax))/2.0);  // the ususal logarithmic scale
//           cout << "sR : " << scaleMin << ";" << scaleMax << ";" << scaleRep ;
          for (; itBlob!=glBlobs.end(); ++itBlob)                // now finding the glBlob that has a scale the  closest
          {
            t=(*itBlob)->GetScale();                        // to scaleRep
//             cout << t << endl;
               if (fabs(t-scaleRep) < delta)
               {
                    delta=fabs(t-scaleRep);
                    glBlobRep=(*itBlob);
               }
          }
//           if (glBlobRep->GetListePoints().size() == 1) cout << "->" << glBlobRep->GetScale() << "test" << endl; else cout << endl;

     }

}

#endif
