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
