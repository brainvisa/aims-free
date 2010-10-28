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


#ifndef AIMS_PRIMALSKETCH_SCALELEVEL_H
#define AIMS_PRIMALSKETCH_SCALELEVEL_H

#include <cstdlib>
#include <aims/config/aimsalgo_config.h>
#include <aims/texdata/texdata.h>
#include <aims/primalsketch/greyLevelBlob.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/data/data.h>
#include <set>
#include <map>
#include <string>

namespace aims
{

    // Scalelevel Is Templated With Respect To A Geometry And A Texture
    // Same Mechanism Than Textureddata And Extractgreylevelblobs

    // A Scale Level

    template<typename Geom, typename Text> class ScaleLevel {

        public:

            typedef typename SiteType<Geom>::type Site;
            typedef typename TexType<Text>::type Val;

            ScaleLevel(float scale) { _scale=scale; }

            ScaleLevel() {}

            float     Scale() { return _scale; }
            std::map<int, GreyLevelBlob<Site> *> BlobList() { return blobs; }
            std::list<SaddlePoint<Site> *> SaddlePointList() { return saddlePoints; }
            std::list<MaximumPoint<Site> *> MaximumPointList() { return maximumPoints; }

            GreyLevelBlob<Site> *Blob(int label)
            {
                if (blobs.find(label) !=  blobs.end())
                    return blobs[label];
                else
                {
                    std::cerr << "looking for GLB that does not exist..." << std::endl;
                    exit(EXIT_FAILURE);
                }
            }

            TexturedData<Geom, Text > Data() {}

            void DetectBlobs(TexturedData<Geom, Text> *mask, char *stats=0);
            int nbBlobs() { return blobs.size(); }

            ScaleLevel<Geom, Text> & operator = (const ScaleLevel<Geom, Text> & other)
                {_scale=other._scale; blobs=other.blobs;
                saddlePoints=other.saddlePoints; maximumPoints=other.maximumPoints; return *this;}

        private:

            float _scale;

            std::map<int, GreyLevelBlob<Site> *> blobs;
            std::list<SaddlePoint<Site> *> saddlePoints;
            std::list<MaximumPoint<Site> *> maximumPoints;

    };

    // Scale Levels Image Partial Specialisation

    template<typename T> class ScaleLevel<AimsData<T>, AimsData<T> > {

        public:

            typedef typename SiteType<AimsData<T> >::type Site;

            ScaleLevel(float scale, AimsData<T> level) { _scale=scale; _level = level.clone(); _originallevel=NULL; }

            ScaleLevel(float scale, AimsData<T> level, AimsData<T> *originallevel)
                { _scale=scale; _level = level.clone(); _originallevel=originallevel; }

            ~ScaleLevel() {}

            AimsData<T> & Level() { return _level; }


            // Essai
            TexturedData<AimsData<T>, AimsData<T> > Data() {TexturedData<AimsData<T>, AimsData<T> > data(&_level); return data;}
            TexturedData<AimsData<T>, AimsData<T> > OriginalData() {TexturedData<AimsData<T>, AimsData<T> > data(_originallevel); return data;}

            float     Scale() { return _scale; }
            std::map<int, GreyLevelBlob<Site> *> BlobList() { return blobs; }
            std::list<SaddlePoint<Site> *> SaddlePointList() { return saddlePoints; }
            std::list<MaximumPoint<Site> *> MaximumPointList() { return maximumPoints; }

            GreyLevelBlob<Site> *Blob(int label) {if (blobs.find(label) !=  blobs.end())
                                                            return blobs[label];
                                                        else
                                                    {std::cerr << "looking for GLB " << label << " that does not exist at scale " << _scale << std::endl;
                                                            exit(EXIT_FAILURE);}}

            void DetectBlobs(TexturedData<AimsData<T>, AimsData<T> > *mask=0, char *stats=0);
            int nbBlobs() { return blobs.size(); }

            ScaleLevel<AimsData<T>, AimsData<T> > & operator = (const ScaleLevel<AimsData<T>, AimsData<T> > & other)
                {_scale=other._scale; _level=other._level; blobs=other.blobs;
                saddlePoints=other.saddlePoints; maximumPoints=other.maximumPoints; return *this;}

            void WriteBlobImage(std::string fileBlob);

        private:

            float _scale;
            AimsData<T> _level;
            AimsData<T> *_originallevel;
            std::map<int, GreyLevelBlob<Site> *> blobs;
            std::list<SaddlePoint<Site> *> saddlePoints;
            std::list<MaximumPoint<Site> *> maximumPoints;
    };

    // Scale Levels Texture Partial Specialisation

    template<int D, typename T> class ScaleLevel<AimsSurface<D, Void>, Texture<T> >
    {

        public:

            typedef typename SiteType<AimsSurface<D, Void> >::type Site;

            ScaleLevel(float scale, Texture<T> level, AimsSurface<D, Void> *mesh)
            {
            _scale=scale; _level=level; _mesh=mesh; _coordinates=NULL; _originallevel=NULL;
            }
            ScaleLevel(float scale, Texture<T> level, AimsSurface<D, Void> *mesh, std::vector<Point3df> *coords)
            {
            _scale=scale; _level=level; _mesh=mesh; _coordinates=coords; _originallevel=NULL;
            }
            ScaleLevel(const ScaleLevel<AimsSurface<D, Void>, Texture<T> > &other)
            {
            (*this)._scale=other._scale; (*this)._level=other._level; (*this)._mesh=other._mesh; (*this)._coordinates=_coordinates; _originallevel=NULL;
            }
            ScaleLevel(float scale, Texture<T> level, AimsSurface<D, Void> *mesh, Texture<T> *originallevel)
            {
            _scale=scale; _level=level; _mesh=mesh; _coordinates=NULL; _originallevel=originallevel;
            }
            ScaleLevel(float scale, Texture<T> level, AimsSurface<D, Void> *mesh, std::vector<Point3df> *coords, Texture<T> *originallevel)
            {
            _scale=scale; _level=level; _mesh=mesh; _coordinates=coords; _originallevel=originallevel;
            }
            ScaleLevel(const ScaleLevel<AimsSurface<D, Void>, Texture<T> > &other, Texture<T> *originallevel)
            {
            (*this)._scale=other._scale; (*this)._level=other._level; (*this)._mesh=other._mesh; (*this)._coordinates=_coordinates; _originallevel=originallevel;
            }

            ~ScaleLevel() {}


            Texture<T> & Level() { return _level; }
            TexturedData<AimsSurface<D, Void>, Texture<T> > Data() {TexturedData<AimsSurface<D, Void>, Texture<T> > data(_mesh, &_level); return data;}
            TexturedData<AimsSurface<D, Void>, Texture<T> > OriginalData() {TexturedData<AimsSurface<D, Void>, Texture<T> > data(_mesh, _originallevel); return data;}


            float     Scale() { return _scale; }
            AimsSurface<D, Void> *Mesh() { return _mesh; }
            std::map<int, GreyLevelBlob<Site> *> & BlobList() { return blobs; }
            std::list<SaddlePoint<Site> *> & SaddlePointList()
            { return saddlePoints; }
            std::list<MaximumPoint<Site> *> & MaximumPointList()
            { return maximumPoints; }

            GreyLevelBlob<Site> *Blob (int label) {
                if (blobs.find(label) !=  blobs.end())
                    return blobs[label];
                else
                {
                    std::cerr << "looking for GLB " << label << " that does not exist..." << std::endl;
                    exit(EXIT_FAILURE);
                }
            }

            void DetectBlobs(TexturedData<AimsSurface<D, Void>, Texture<T> > *mask=0, char *stats=0);

            int nbBlobs() { return blobs.size(); }
            void PutCoordinates ( std::vector<Point3df> *coord ) { _coordinates = coord; }


            ScaleLevel<AimsSurface<D, Void>, Texture<T> > &
            operator = (const ScaleLevel<AimsSurface<D, Void>, Texture<T> > & other)
            {
            (*this)._scale=other._scale;(*this)._level=other._level;(*this)._mesh=other._mesh; (*this).blobs=other.blobs;
            (*this).saddlePoints=other.saddlePoints; (*this).maximumPoints=other.maximumPoints;  return *this;
            }

        private:

            float _scale;
            Texture<T> _level;
            Texture<T> *_originallevel;
            AimsSurface<D, Void> *_mesh;
            std::vector<Point3df> *_coordinates;

            std::map<int, GreyLevelBlob<Site> *> blobs;
            std::list<SaddlePoint<Site> *> saddlePoints;
            std::list<MaximumPoint<Site> *> maximumPoints;
    };


     //---------------- METHOD/FUNCTION DEFINITIONS ------------------------------

     template<typename T>
     void ScaleLevel<AimsData<T>, AimsData<T> >::DetectBlobs(TexturedData<AimsData<T>, AimsData<T> > *mask, char *stats)
     {
          TexturedData<AimsData<T>, AimsData<T> >textdata(&_level) ;
          TexturedData<AimsData<T>, AimsData<T> >rawtextdata(_originallevel) ;


          ExtractGreyLevelBlobs<AimsData<T>, AimsData<T> > extractor(&textdata, &rawtextdata, mask, stats);

        typename std::map<int, GreyLevelBlob<Site> *>::iterator itBlob;

        //std::cout << "DEBUG: starting detection" << std::endl;
          if (blobs.size() == 0)
          {
               extractor.Run();
            //std::cout << "DEBUG: ran" << std::endl;
               blobs=extractor.GetBlobs();
               saddlePoints=extractor.GetSaddleList();
               maximumPoints=extractor.GetMaxList();
            //std::cout << "DEBUG: Got lists" << std::endl;
            itBlob=blobs.begin();
               for ( ; itBlob != blobs.end(); ++itBlob)
               ((*itBlob).second)->SetScale(_scale);
            //std::cout << "DEBUG: Set scales" << std::endl;
          }
     }

     //-------------------------------------------------------------------

     template<int D, typename T>
     void ScaleLevel<AimsSurface<D, Void>, Texture<T> >::DetectBlobs(TexturedData<AimsSurface<D, Void>, Texture<T> > *mask, char *stats)
     {

        TexturedData<AimsSurface<D, Void>, Texture<T> > textdata(_mesh, &_level, _coordinates) ;
        TexturedData<AimsSurface<D, Void>, Texture<T> > rawtextdata(_mesh, _originallevel, _coordinates) ;

        ExtractGreyLevelBlobs<AimsSurface<D, Void>, Texture<T> > extractor(&textdata, &rawtextdata, mask, stats);

        typename std::map<int, GreyLevelBlob<Site> *>::iterator itBlob;
        if (blobs.size() == 0){
            extractor.Run();
            blobs=extractor.GetBlobs();
            saddlePoints=extractor.GetSaddleList();
            maximumPoints=extractor.GetMaxList();
            itBlob=blobs.begin();
            for ( ; itBlob != blobs.end(); ++itBlob)
                ((*itBlob).second)->SetScale(_scale);
        }

     }

     //-------------------------------------------------------------------

     template<typename T>
     void ScaleLevel<AimsData<T>, AimsData<T> >::WriteBlobImage(std::string fileBlob)
     {
          int x,y,z;
          int sx=_level.dimX(), sy=_level.dimY(), sz=_level.dimZ();
          float     dx=_level.sizeX(), dy=_level.sizeY(), dz=_level.sizeZ();
          AimsData<float> imaBlob(sx,sy,sz);
          typename std::map<int, GreyLevelBlob<Site> *>::iterator itBlob=blobs.begin();;
          typename std::list<SaddlePoint<Site> *>::iterator itSaddle=saddlePoints.begin();
          typename std::list<MaximumPoint<Site> *>::iterator itMax=maximumPoints.begin();
          typename std::set<Site,ltstr_p3d<Site> >::iterator itPoints;
          GreyLevelBlob<Site> *blobby;

          imaBlob.setSizeXYZT(dx,dy,dz);

          for (z=0; z<sz; z++)
               for (y=0; y<sy; y++)
                    for (x=0; x<sx; x++)
                         imaBlob(x,y,z)=0.0;

          for (; itBlob!=blobs.end(); ++itBlob)
          {
               blobby=(*itBlob).second;
               itPoints=blobby->GetListePoints().begin();
               for (; itPoints!=blobby->GetListePoints().end(); ++itPoints)
               {
                    x=int((*itPoints)[0]); y=int((*itPoints)[1]); z=int((*itPoints)[2]);
                    imaBlob(x,y,z)=255.0;
               }
          }

          for (; itSaddle!=saddlePoints.end(); ++itSaddle)
          {
               x=int((*itSaddle)->_node[0]); y=int((*itSaddle)->_node[1]); z=int((*itSaddle)->_node[2]);
               imaBlob(x,y,z)=128.0;
          }

          for (; itMax!=maximumPoints.end(); ++itMax)
          {
               x=int((*itMax)->_node[0]); y=int((*itMax)->_node[1]); z=int((*itMax)->_node[2]);
               imaBlob(x,y,z)=128.0;
          }
          Writer<AimsData<float> > dataW(fileBlob);
          dataW.write(imaBlob);
     }

}

#endif
