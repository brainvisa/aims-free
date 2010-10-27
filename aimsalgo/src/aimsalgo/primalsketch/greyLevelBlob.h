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

#ifndef AIMS_PRIMALSKETCH_GREYLEVELBLOB_H
#define AIMS_PRIMALSKETCH_GREYLEVELBLOB_H

#include <cstdlib>
#include <aims/config/aimsalgo_config.h>
#include <iostream>
#include <set>
#include <map>
#include <list>
#include <vector>

namespace aims {
    const int BACKGROUND = 0;

    template<typename T> struct ltstr_p3d { // comparaison de Sites pour conteneurs tri�s (set, map)
        bool operator()(const T p1, const T p2) { return false;}
    };

    template <> struct ltstr_p3d<Point3d> {
        bool operator ()( const Point3d p1, const Point3d p2 ) {
            if ( p1[2] < p2[2] )
                return true;
            else if ( p1[2] > p2[2] )
                return false;
            else {
                if ( p1[1] < p2[1] )
                    return true;
                else if ( p1[1] > p2[1] )
                    return false;
                else
                {
                    if ( p1[0] < p2[0] )
                        return true;
                    else return false;
                }
            }
        }
    };

    template <> struct ltstr_p3d<std::pair<Point3df, uint> > {
        bool operator () ( const std::pair<Point3df, uint> p1,
                           const std::pair<Point3df, uint> p2 ) {
            if ( p1.second < p2.second )
                return true;
            else
                return false;
        }
    };

    template<class T> class MaximumPoint;
    template<class T> class SaddlePoint;
    template<class T> class GreyLevelBlob;

    class BlobMeasurements {

        public:
            BlobMeasurements() {}
            BlobMeasurements(const BlobMeasurements &other) {maxIntensity=other.maxIntensity; meanIntensity=other.meanIntensity; maxContrast=other.maxContrast; meanContrast= other.meanContrast;  area=other.area; t=other.t; tValue=other.tValue;}
            BlobMeasurements(float maxInt, float meanInt, float maxCont, float meanCont, float a, float tv=0.0, float tvalue=0.0):
                maxIntensity(maxInt), meanIntensity(meanInt), maxContrast(maxCont), meanContrast(meanCont), area(a), t(tv), tValue(tvalue) {}

            float maxIntensity;
            float meanIntensity;
            float maxContrast;
            float meanContrast;
            float area;
            float t;
            float tValue;
          
            BlobMeasurements & operator = (const BlobMeasurements & other);
    };

    // Class for grey-level blobs
    // Templated with respect to the type of points:
    // TypeSite<AimsData<T> >::type for images
    // TypeSite<AimsSurface<D, Void> >::type for textures.

    template<class T> class GreyLevelBlob {

        protected:

            int             _label;
            std::set<T,ltstr_p3d<T> >          listePoints;
            SaddlePoint<T>  *saddle;
            MaximumPoint<T> *maximum;
            float           scale;
            bool            grow;

        public:

            GreyLevelBlob ( MaximumPoint<T> *node, int label ) {
                saddle = 0; 
                maximum = node; 
                node->blob = this; 
                grow = true; 
                _label = label; 
                AddPoint (maximum->_node);
            }
            GreyLevelBlob () {
                saddle = 0; 
                maximum = 0; 
                grow = true; 
                _label = BACKGROUND;
            }

            BlobMeasurements    measurements;

            void AddPoint( T node ) { listePoints.insert(node); }
            void StopGrowth() { grow = false; }
            bool CanGrow() { return grow; }
            int  Label() { return _label; }

            std::set<T, ltstr_p3d<T> > & GetListePoints () { return listePoints; }

            void SetSaddle (SaddlePoint<T>  *node) {
                saddle = node; 
                node->AddBlob (this); 
                StopGrowth();
            }

            SaddlePoint<T> *GetSaddle () { return saddle; }
            MaximumPoint<T> *GetMaximum () { return maximum; }
            float GetScale () { return scale; }
            void SetScale (float t) { scale = t; }

            GreyLevelBlob<T> & operator = ( const GreyLevelBlob<T> & other );

    };

    // class for saddle points

    template<class T> class SaddlePoint {

        public:
            T _node;
            std::list<GreyLevelBlob<T>* > blobList;

            SaddlePoint () { }
            SaddlePoint (T node) : _node(node) { }
            ~SaddlePoint () { }

            void AddBlob(GreyLevelBlob<T> *blob) { blobList.push_back(blob); }
            std::list<GreyLevelBlob<T>*> & GetBlobList() { return blobList; }

            SaddlePoint<T> & operator = ( const SaddlePoint<T>& other );

    };

    // class for maxima

    template<class T> class MaximumPoint {

        public:
            T                _node;
            GreyLevelBlob<T> *blob;
            ~MaximumPoint() { }

            MaximumPoint () : blob(0) { }
            MaximumPoint ( T node ) : _node(node), blob(0) { }
            MaximumPoint<T> & operator = ( const MaximumPoint<T>& other );

    };

  // Class that define grey-level blobs algorithm extraction

  template<typename Geom, typename Text> class ExtractGreyLevelBlobs {

    private :

      typedef typename SiteType<Geom>::type Site;
      typedef typename TexType<Text>::type Val;

      TexturedData<Geom,Text> *_texdata;
      TexturedData<Geom,Text> *_rawtexdata;
      TexturedData<Geom, Text> *_mask;      // For convenience, data and mask are same type.
                                                       // This is the only simple solution to keep type genericity
                                                 // Mask is 0 outside, >0 inside
      std::map<int, GreyLevelBlob<Site>* >  blobMap; // map label -> blob
      std::list<SaddlePoint<Site> *>    saddleList;
      std::list<MaximumPoint<Site> *>   maximumList;
      TexturedData<Geom, Text> labelsImage; // mapping site->label (blob, background...)

      char *_stats;        // File name for blob statistics

      void DoMasking(); // if required, use a mask to remove some blobs
      void CheckMask(); // basic check : is the geometry of the mask the same than the data ?

    public :

      ExtractGreyLevelBlobs(TexturedData<Geom,Text> *texdata, TexturedData<Geom,Text> *rawtexdata, TexturedData<Geom,Text> *mask)
      : _texdata(texdata), _rawtexdata(rawtexdata), _mask(mask), _stats(0){ labelsImage=TexturedData<Geom, Text>(*_texdata); CheckMask();}
      ExtractGreyLevelBlobs(TexturedData<Geom,Text> *texdata, TexturedData<Geom,Text> *rawtexdata, TexturedData<Geom,Text> *mask, char *stats)
      : _texdata(texdata), _rawtexdata(rawtexdata), _mask(mask), _stats(stats) { labelsImage=TexturedData<Geom, Text>(*_texdata); CheckMask();}
      
      
      ExtractGreyLevelBlobs(TexturedData<Geom,Text> *texdata)
      : _texdata(texdata), _rawtexdata(NULL), _mask(0), _stats(0) {                    labelsImage=TexturedData<Geom, Text>(*_texdata);}
      ExtractGreyLevelBlobs(TexturedData<Geom,Text> *texdata, TexturedData<Geom,Text> *mask)
      : _texdata(texdata), _rawtexdata(NULL),  _mask(mask), _stats(0) { labelsImage=TexturedData<Geom, Text>(*_texdata); CheckMask();}
      ExtractGreyLevelBlobs(TexturedData<Geom,Text> *texdata, TexturedData<Geom,Text> *mask, char *stats)
      : _texdata(texdata), _rawtexdata(NULL),  _mask(mask), _stats(stats) { labelsImage=TexturedData<Geom, Text>(*_texdata); CheckMask();}
      void Run(); // possibility of masking to exclude an area
  
       void ComputeBlobMeasurements();
       void SetOriginalTexture ( TexturedData<Geom,Text> *rawtexdata ) { _rawtexdata = rawtexdata; }
      TexturedData<Geom,Text> *GetOriginalTexture ( ) { return _rawtexdata; }
    // doit renvoyer une liste de GLB, de maximums et de saddle points
      std::list<SaddlePoint<Site> *> GetSaddleList() { return saddleList; }
      std::list<MaximumPoint<Site> *> GetMaxList() { return  maximumList; }
      std::map<int, GreyLevelBlob<Site> *> GetBlobs() { return blobMap; }
      TexturedData<Geom, Text> GetBlobImage() { return labelsImage; }
      int nbBlobs() { return blobMap.size(); }

  };


 // class that provide some tools for blob management

 template<typename T> class GreyLevelBlobTools {

     private :
     GreyLevelBlob<T> *_blob;

     public :

     GreyLevelBlobTools(GreyLevelBlob<T> *blob) : _blob(blob) {}
     std::vector<float> Boundingbox();
     std::vector<float> Barycenter();

 };

  //---------------------------------------------------------------------------
  //
  // Methods
  //
  //---------------------------------------------------------------------------

    inline
    BlobMeasurements& BlobMeasurements::operator = (const BlobMeasurements & other)
    {
        maxIntensity=other.maxIntensity;
        meanIntensity=other.meanIntensity;
        maxContrast=other.maxContrast;
        meanContrast=other.meanContrast;
        area=other.area;
        t=other.t;
        tValue=other.tValue;
        
     return *this;
    }

      //----------------------------------------------------------------------------------------------------------

     template<class T>
     inline
     SaddlePoint<T>& SaddlePoint<T>::operator = (const SaddlePoint<T> & other)
     {
          _node=other.node;
          return *this;
     }

       //----------------------------------------------------------------------------------------------------------

     template<class T>
     inline
     MaximumPoint<T>& MaximumPoint<T>::operator = (const MaximumPoint<T> & other)
     {
          _node=other.node;
          blob=other.blob;
          return *this;
     }

       //----------------------------------------------------------------------------------------------------------

     template<class T>
     inline
     GreyLevelBlob<T>& GreyLevelBlob<T>::operator = (const GreyLevelBlob<T> & other)
     {
          _label=other._label;
          listePoints=other.listePoints;
          scale=other.scale;
          saddle=other.saddle;
          maximum=other.maximum;
          
          return *this;
     }

       //----------------------------------------------------------------------------------------------------------

    template<typename Geom, typename Text> void ExtractGreyLevelBlobs<Geom, Text>::Run()
    {
        int currentLabel=0, labelBlob;
        MaximumPoint<Site> *maximum;
        SaddlePoint<Site> *saddle;
        GreyLevelBlob<Site> *greyLevelBlob;

        // multimap<intensity, data.site> for intensity based sorting of sites/voxels
        std::multimap<const Val, Site> sortedSites;
        SiteIterator<Geom> ptSite=(*_texdata).siteBegin();

        for ( ; ptSite != (*_texdata).siteEnd(); ++ptSite)
            sortedSites.insert(std::pair<Val, Site>((*_texdata).intensity(*ptSite), (*ptSite)));



        // TexturedData that will contains the label associated with each site,
        // that is:  BACKGROUND, or grey-level blob number.
        // This TexturedData also gives acess to the neighbours of each site

        // Rem: labels should be (int) but will be same type than Text to handle both surfaces
        // and images. Not elegant but that's all I found so far.

        for (ptSite=labelsImage.siteBegin(); ptSite !=  labelsImage.siteEnd(); ++ptSite)
            labelsImage.intensity(*ptSite)=BACKGROUND;
        // GL Blob extraction
        // This is the core algorithm
        typename std::multimap<Val, Site>::reverse_iterator ptSortedSites;

        //std::cout << "DEBUG: extractor has started detection" << std::endl;
        for ( ptSortedSites=sortedSites.rbegin(); ptSortedSites != sortedSites.rend(); ++ptSortedSites) {
            Val intensity=(*_texdata).intensity((*ptSortedSites).second);

            int nbAbove;
            int nbAboveLabels;

            // How many neighbours with higher intensity ?
            
            std::vector<Site> neighb=(*_texdata).neighbours((*ptSortedSites).second); //neighbours
            
            std::vector<Site> above; //neighbours with higher intensity
            std::set<int> aboveLabels; // labels of (above)
            typename std::vector<Site>::iterator ptNeigh=neighb.begin();
            
            for ( ; ptNeigh != neighb.end(); ++ptNeigh){
                
                if ((*_texdata).intensity(*ptNeigh) > intensity){
                    
                    above.push_back(*ptNeigh);
                    aboveLabels.insert(int(labelsImage.intensity(*ptNeigh)));
                }
            }
            
            nbAbove=above.size();
            nbAboveLabels=aboveLabels.size();

            if (nbAbove==0){      // Case 1: local max, new blob
                currentLabel++;
                maximum=new MaximumPoint<Site>((*ptSortedSites).second);
                greyLevelBlob=new GreyLevelBlob<Site>(maximum, currentLabel);
                labelsImage.intensity((*ptSortedSites).second)=(Val) currentLabel;
                maximumList.push_back(maximum);
                blobMap[currentLabel]=(greyLevelBlob);
                
            }
            else if ( (nbAbove==1) && ((labelsImage.intensity(*(above.begin())))==BACKGROUND) ){
                                // Case 2: point in background
                labelsImage.intensity((*ptSortedSites).second)=BACKGROUND;
                
            }

            else if ((nbAbove>1) && (nbAboveLabels>1)){
                               // Case 3: one or several blobs to stop -> Saddle point
                               
                int flagS=0;
                labelsImage.intensity((*ptSortedSites).second)=BACKGROUND;
                std::set<int>::iterator ptLabels=aboveLabels.begin();
                for (; ptLabels != aboveLabels.end(); ++ptLabels)
                if (*ptLabels!=BACKGROUND)
                    if (blobMap[*ptLabels]->CanGrow())
                        {
                            if (flagS==0)
                            {
                        saddle=new SaddlePoint<Site>((*ptSortedSites).second);
                        saddleList.push_back(saddle);
                            blobMap[*ptLabels]->SetSaddle(saddle);
                                flagS=1;
                            }
                            else
                                blobMap[*ptLabels]->SetSaddle(saddle);
                        }
            }
            else                 // Case 4: point belong to a growing blob
            {
                labelBlob=int(labelsImage.intensity(*(above.begin())));
                if ((labelBlob != BACKGROUND) && (blobMap[labelBlob]->CanGrow() ))
                {
                    labelsImage.intensity((*ptSortedSites).second)=(Val) labelBlob;
                    blobMap[labelBlob]->AddPoint((*ptSortedSites).second);
                }
                else
                {
                    labelsImage.intensity((*ptSortedSites).second)=BACKGROUND;
                }
            }
        }
        //std::cout << "DEBUG: found " << blobMap.size() << " blobs" << std::endl;
        //std::cout << "DEBUG: extractor about to enter masking" << std::endl;
        if (_mask!=0) DoMasking();
        //std::cout << "DEBUG: masking done, extractor about to enter measurements" << std::endl;
        ComputeBlobMeasurements();
        //std::cout << "DEBUG: done" << std::endl;
    }

    //----------------------------------------------------------------------------------------------------------

  template<typename Geom, typename Text> void ExtractGreyLevelBlobs<Geom, Text>::DoMasking()
  {
    typename std::list<MaximumPoint<Site> *>::iterator itMax=maximumList.begin(), itPrevious=itMax;
    SaddlePoint<Site> *saddle;
    double EPSILON=0.00001;
    int del=0;

    //std::cout << "DEBUG: found " << blobMap.size() << " blobs" << std::endl;
    //std::cout << "DEBUG: saddle list size : " << saddleList.size() << std::endl;

    for ( ; itMax!=maximumList.end(); ++itMax)
    {
          if (del==1)
          {
              maximumList.erase(itPrevious);
          }
          del=0;
          if (fabs(_mask->intensity((*itMax)->_node)) < EPSILON) // this is to test float=0.0
          {
              blobMap.erase((*itMax)->blob->Label());                // remove the blob from the list.

              saddle=(*itMax)->blob->GetSaddle();                    // check the saddle
            if (saddle != 0) // some blobs can have no saddle if they are on the image border
            {
                  if (saddle->blobList.size()>1)
                  {
                        saddle->blobList.remove((*itMax)->blob);
                  }
                  else
                  {
                        saddle->blobList.remove((*itMax)->blob);
                        saddleList.remove(saddle);
                        delete saddle;
                  }
            }
              delete (*itMax)->blob;        // delete the blob
              del=1;                             // maximum has to be deleted
          }
          itPrevious=itMax;
    }
    if (del==1)
          maximumList.erase(itPrevious);
    //std::cout << "DEBUG: found " << blobMap.size() << " blobs" << std::endl;
  }

    //----------------------------------------------------------------------------------------------------------

  template<typename Geom, typename Text> void ExtractGreyLevelBlobs<Geom, Text>::CheckMask()
  {
          if (_mask != 0)
               if (_mask->NbSites() != _texdata->NbSites())
               {
                    std::cerr << "ExtractGreyLevelBlobs : mask and data do not have the same number of sites !!!" << std::endl;
                    exit(EXIT_FAILURE);
               }
  }

  //----------------------------------------------------------------------------------------------------------

  template<typename Geom, typename Text>
  void
  ExtractGreyLevelBlobs<Geom, Text>::ComputeBlobMeasurements ( ) {
      typename std::map<int, GreyLevelBlob<Site>* >::iterator blobIt;
      GreyLevelBlob<Site> *blob;
      SaddlePoint<Site> *saddle;
      MaximumPoint<Site> *maxi;
      Val saddleInt, maxInt, intensity;
      float maxIntensity, meanIntensity, maxContrast, meanContrast, area;
      int nbPoint;
      FILE *fileStat = 0;

      if ( _stats != 0 ) {
          fileStat = fopen ( _stats, "w" );
          fprintf ( fileStat, "Blob_label max_int mean_int max_cont mean_cont area\n" );
      }

      //std::cout << "DEBUG: going through blob list" << std::endl;
      //std::cout << "DEBUG: Size of list : " << blobMap.size() << std::endl;
      //std::cout << "DEBUG: saddle list size : " << saddleList.size() << std::endl;
      for ( blobIt = blobMap.begin() ; blobIt != blobMap.end() ; ++blobIt ) {
          //std::cout << ((*blobIt).second)->Label(); fflulongsh(stdout);
          //std::cout << "DEBUG : blob " << (*blobIt).first << std::endl;
          maxIntensity = 0.0;
          meanIntensity = 0.0;
          maxContrast = 0.0;
          meanContrast = 0.0;
          nbPoint = 0;
          area = 0.0; // pour l'instant il s'agit du nombre de points
                              // donc tres mauvais pour les surfaces
          blob = (*blobIt).second;
          //std::cout << "DEBUG : getting saddle" << std::endl;
          saddle = blob->GetSaddle();
          //std::cout << "DEBUG : getting maxi" << std::endl;
            maxi=blob->GetMaximum();
            //std::cout << "DEBUG : sites : " << (*_texdata).NbSites() << std::endl;
            //std::cout << "DEBUG : getting max intensity" << std::endl;
            maxInt=(*_texdata).intensity(maxi->_node);
            //std::cout << "DEBUG : getting saddle intensity" << std::endl;
            if (saddle != 0)
            {
                //std::cout << "DEBUG : saddle : " << (saddle) << std::endl;
                   saddleInt=(*_texdata).intensity(saddle->_node);
            }
            else
            {
                //std::cout << "DEBUG : blob " << blob->Label() << " has no saddle " << std::endl;
                //std::cout << " ! " << std::flush;
                saddleInt=0;
            }
               maxContrast=float(maxInt-saddleInt);
               maxIntensity=float(maxInt);



               std::set<Site,ltstr_p3d<Site> > listePoints=blob->GetListePoints();
               typename std::set<Site,ltstr_p3d<Site> >::iterator
                          itPoints=listePoints.begin();
               Site point;

               //std::cout << "DEBUG : going through points" << std::endl;
               for (; itPoints!=listePoints.end(); ++itPoints)
               {
                    point=(*itPoints);
                    intensity=(*_texdata).intensity(point);
                    nbPoint++;
                    area=area+1.0;
                    meanContrast+=float(intensity - saddleInt);
                    meanIntensity+=float(intensity);
               }
               meanContrast/=float(nbPoint);
               meanIntensity/=float(nbPoint);

            //std::cout << " + " << std::flush;
               blob->measurements.maxIntensity=maxIntensity;
               blob->measurements.meanIntensity=meanIntensity;
               blob->measurements.maxContrast=maxContrast;
               blob->measurements.meanContrast=meanContrast;
               blob->measurements.area=area;
               


              if ( GetOriginalTexture() != NULL ) {
                  std::set<Site,ltstr_p3d<Site> >  pixels;
                  pixels = blob->GetListePoints();
  
                  typename std::set<Site, ltstr_p3d<Site> >::iterator itPix;
                  float tvmax = -100.0;
                  for ( itPix = pixels.begin() ; itPix != pixels.end() ; itPix++ ) {
                      if ( float ( GetOriginalTexture()->intensity(*itPix) ) > tvmax )
                          tvmax= float ( GetOriginalTexture()->intensity(*itPix) );
                  }   
                  blob->measurements.t = tvmax;
              }
              else  {
                  std::cout << "BLOBMEASUREMENT = 0.0" << std::endl;
                  blob->measurements.t = 0.0;
              }


              //std::cout << " - " << std::flush;
              if ( fileStat != 0 )
                  fprintf ( fileStat, "%i %.4f %.4f %.4f %.4f %.4f\n", blob->Label(), 
                          maxIntensity, meanIntensity, maxContrast, meanContrast, area);
              //std::cout << " | " << std::flush;
              }
        if ( fileStat != 0 )
            fclose(fileStat);
 }

 //----------------------------------------------------------------------------------------

  // Image specialisation

 template <>
 std::vector<float>
 GreyLevelBlobTools<Point3d>::Boundingbox()
 {

     std::set<Point3d,ltstr_p3d<Point3d> >  listeP=_blob->GetListePoints();
     std::set<Point3d,ltstr_p3d<Point3d> >::iterator pointIt=listeP.begin();

     float x1,x2,y1,y2,z1,z2;

     x1=10000.0; y1=10000.0; z1=10000.0;
     x2=-10000.0; y2=-10000.0; z2=-10000.0;
     std::vector<float> vectF(6);

     for (; pointIt != listeP.end(); ++pointIt)
     {
          float x=(float) (*pointIt)[0];
          float y=(float) (*pointIt)[1];
          float z=(float) (*pointIt)[2];
          if (x<x1) x1=x;
          if (x>x2) x2=x;
          if (y<y1) y1=y;
          if (y>y2) y2=y;
          if (z<z1) z1=z;
          if (z>z2) z2=z;
     }

     vectF[0]=x1; vectF[1]=y1; vectF[2]=z1;
     vectF[3]=x2; vectF[4]=y2; vectF[5]=z2;

     return vectF;
 }

 template <>
     std::vector<float>
     GreyLevelBlobTools<Point3d >::Barycenter()
     {
       Point3d max= _blob->GetMaximum()->_node;
       std::vector<float> triplet;
       triplet.push_back(max[0]);
       triplet.push_back(max[1]);
       triplet.push_back(max[2]);
       return triplet;

     }
 
  // Surface specialisation

 template <>
 std::vector<float>
 GreyLevelBlobTools<std::pair<Point3df,uint> >::Boundingbox()
 {
     std::set<std::pair<Point3df,uint> ,ltstr_p3d<std::pair<Point3df,uint> > >  listeP=_blob->GetListePoints();
     std::set<std::pair<Point3df,uint> ,ltstr_p3d<std::pair<Point3df,uint> > >::iterator pointIt=listeP.begin();

     float x1,x2,y1,y2,z1,z2;

     x1=10000.0; y1=10000.0; z1=10000.0;
     x2=-10000.0; y2=-10000.0; z2=-10000.0;

     std::vector<float> vectF(6);

     for (; pointIt != listeP.end(); ++pointIt)
     {
          float x=(float) (*pointIt).first[0];
          float y=(float) (*pointIt).first[1];
          float z=(float) (*pointIt).first[2];
          if (x<x1) x1=x;
          if (x>x2) x2=x;
          if (y<y1) y1=y;
          if (y>y2) y2=y;
          if (z<z1) z1=z;
          if (z>z2) z2=z;
     }

     vectF[0]=x1; vectF[1]=y1; vectF[2]=z1;
     vectF[3]=x2; vectF[4]=y2; vectF[5]=z2;

     return vectF;
 }



template <>
    std::vector<float>
    GreyLevelBlobTools<std::pair<Point3df,uint> >::Barycenter()
{
  Point3df max= _blob->GetMaximum()->_node.first;
  std::vector<float> triplet;
  triplet.push_back(max[0]);
  triplet.push_back(max[1]);
  triplet.push_back(max[2]);
  return triplet;

}
}
#endif
