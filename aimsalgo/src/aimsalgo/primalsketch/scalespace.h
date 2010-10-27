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


#ifndef AIMS_PRIMALSKETCH_SCALESPACE_H
#define AIMS_PRIMALSKETCH_SCALESPACE_H

#include <cstdlib>
#include <aims/config/aimsalgo_config.h>
#include <aims/primalsketch/smoother.h>
#include <aims/primalsketch/scaleLevel.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/data/data.h>
#include <set>
#include <map>
#include <utility>
#include <iostream>
#include <string>

namespace aims
{

    class BaseScaleSpace {
        public:
            BaseScaleSpace() {}
            virtual ~BaseScaleSpace() {}

            virtual float dt() = 0;

        protected:
            int get_timediff(float t1, float t2)
            {
                float	t = t2 - t1;
                double	nbFloat;
                    double	dt = double(this->dt());
                if (dt) nbFloat = double(t) / dt;
                else	nbFloat = int(rint(t));

                if ((rint(nbFloat) - nbFloat) > 0.001 )
                {
                    std::cerr << "diffusionSmoother::doSmoothing : t (="
                        << t << ") must be a multiple of dt (=" << dt
                        << ")" << std::endl;
                    exit(EXIT_FAILURE);
                }
                return int(rint(nbFloat));
            }
    };

    // Scalespace Is Templated With Respect To A Geometry And A Texture
    // Same Mechanism Than Textureddata And Extractgreylevelblobs


    // General ScaleSpace

    template<typename Geom, typename Text>
    class ScaleSpace : public BaseScaleSpace {

        protected:
            float dt() { return _smoother->dt(); };

            std::map<float, ScaleLevel<Geom, Text>*> scales;
                Smoother<Geom, Text> *_smoother;

        public:

            ScaleSpace() : BaseScaleSpace() {_smoother=NULL; }

            virtual ~ScaleSpace() { delete _smoother; }

            void PutSmoother(Smoother<Geom, Text> *blur) {_smoother=blur;}

            ScaleLevel<Geom, Text> * Scale(float t)
            {
                if (scales.find(t) != scales.end()) return scales[t];
                else { AddScale(t); return scales[t]; }
            }

            Text & GetScaleImage(float t) {return Scale(t)->Level();}

            Text & GetOriginalImage() {return Scale(0.0)->Level();}

            std::map<float, ScaleLevel<Geom, Text>*> GetScaleLevels() {return scales;}

            void AddScale(float t);
            void RemoveScale(float t);

            void GenerateDefaultScaleSpace(float tmax)
            { float t; for (t=1; t<=tmax; t=t*2) { std::cout << "Adding scale " << t << std::endl; AddScale(t); } }

            std::set<float> GetScaleList();

            void WriteScale(float t, std::string name);

            void WriteScaleSpace(std::string name);
    };


    // Image Partial Specialisation

    template<typename T>
    class ScaleSpace<AimsData<T>, AimsData<T> > : public BaseScaleSpace {

        protected:
            
            float dt() { return _smoother->dt(); };
            std::map<float, ScaleLevel<AimsData<T>, AimsData<T> >*> scales;
            Smoother<AimsData<T>, AimsData<T> > *_smoother;

        public:

            ScaleSpace() : BaseScaleSpace() { _smoother = NULL; }

            ScaleSpace(AimsData<T> * originalImage, Smoother<AimsData<T>,AimsData<T> > *smoother)
                { PutSmoother(smoother); PutOriginalImage(originalImage); }

            virtual ~ScaleSpace() { }

            void PutSmoother ( Smoother<AimsData<T>, AimsData<T> > *smoother )
                { _smoother = smoother; }

            void PutOriginalImage ( AimsData<T> *originalImage ) {
                ScaleLevel<AimsData<T>, AimsData<T> > *level;
                level = new ScaleLevel<AimsData<T>, AimsData<T> > ( 0.0f, *originalImage );
                scales.insert(std::pair<float, ScaleLevel<AimsData<T>, AimsData<T> >*> ( 0.0f, level ) );
            }

            ScaleLevel<AimsData<T>, AimsData<T> > * Scale (float t) {
                if (scales.find(t) != scales.end()) return scales[t];
                else { AddScale(t); return scales[t];}
            }

            AimsData<T> & GetScaleImage ( float t ) { return Scale(t)->Level(); }

            AimsData<T> & GetOriginalImage() { return Scale(0.0)->Level(); }

            Smoother<AimsData<T>,AimsData<T> > *smoother() { return _smoother; }

            std::set<float> GetScaleList();

            std::map<float, ScaleLevel<AimsData<T>, AimsData<T> >*> GetScaleLevels() 
                { return scales; }

            void AddScale ( float t );
            void AddScale ( float t, AimsData<float> ima );

            void RemoveScale(float t) {if (scales.find(t) != scales.end()) { ScaleLevel<AimsData<T>, AimsData<T> >* lev=scales[t]; scales.erase(t); delete lev;}}

            void GenerateDefaultScaleSpace( float tmax ) {
                float t; 
                for ( t = 1 ; t <= tmax ; t = t*2 ) {
                    std::cout << "Adding scale " << t << std::endl; 
                    AddScale(t);
                } 
                std::cout << "Adding scale" <<  tmax << std::endl; 
                AddScale(tmax);
            }

            void WriteScale(float t, std::string name);

            void Write(std::string name);
            
            void uploadPreviouslyComputedScaleSpace( AimsData<float> &scale_space)  {
                float t = 1.0;
                for ( uint i = 0 ; i < scale_space.dimT() ; i++ ) {
                    std::cout << "Adding scale " << t << std::flush;
                    AimsData<float> scale ( scale_space.dimX(), scale_space.dimY(), scale_space.dimZ(), 1.0 );
                    for ( uint x = 0 ; x < scale_space.dimX() ; x++ )
                        for ( uint y = 0 ; y < scale_space.dimY() ; y++ )
                            for ( uint z = 0 ; z < scale_space.dimZ() ; z++ )
                                 scale( x, y, z, 0 ) = scale_space( x, y, z, i );
                    AddScale( t, scale );
                    t = t * 2;
                    std::cout << " OK" << std::endl;
                }
            }
    };


    // Surface+Texture Partial Specialisation

    template<int D, typename T>
    class ScaleSpace<AimsSurface<D, Void>, Texture<T> > : public BaseScaleSpace {

        protected:
            float dt() { return _smoother->dt(); };
            AimsSurface<D, Void> *_mesh;
            AimsSurfaceTriangle *_auxmesh;

            std::vector<Point3df> *_coordinates;
            Smoother<AimsSurface<D, Void>, Texture<T> > *_smoother;
            std::map<float, ScaleLevel<AimsSurface<D, Void>, Texture<T> >*> scales;

        public:

            ScaleSpace() : BaseScaleSpace() {
                _smoother=NULL; _auxmesh=NULL; _coordinates=NULL;
            }

            ScaleSpace ( AimsSurface<D, Void> *mesh, Texture<T>  *originalTexture, 
                         Smoother<AimsSurface<D, Void>, Texture<T> > *smoother ) {
                _auxmesh=NULL;  
                _coordinates=NULL; 
                PutSmoother(smoother); 
                PutMesh(mesh);
                PutOriginalImage(originalTexture);
            }


            virtual ~ScaleSpace() { }


            void uploadPreviouslyComputedScaleSpace(TimeTexture<float> &tex) {
                float t = 1.0;
                for ( uint i = 1 ; i < tex.size() ; i++ ) {
                    std::cout << "Adding scale " << t << std::flush;
                    AddScale(t, tex[i]);
                    t = t * 2;
                    std::cout << " OK" << std::endl;
                }
            }

            void PutSmoother(Smoother<AimsSurface<D, Void>, Texture<T> > *smoother) {
                _smoother=smoother;
                }
            void PutMesh(AimsSurface<D, Void> *mesh) {_mesh=mesh;}
            void PutAuxMesh(AimsSurfaceTriangle *auxmesh) {_auxmesh=auxmesh;}
            void PutCoordinates(std::vector<Point3df> *coord){_coordinates = coord;}
            void PutOriginalImage ( Texture<T> *originalTexture )
            {
                ScaleLevel<AimsSurface<D, Void>, Texture<T> > *level;
                level = new ScaleLevel<AimsSurface<D, Void>, Texture<T> > ( 0.0f, *originalTexture, _mesh, _coordinates ) ;
                scales.insert ( std::pair<float, ScaleLevel<AimsSurface<D, Void>, Texture<T> >*>(0.0f, level) );
            }

            AimsSurface<D, Void> *Mesh() {return _mesh;}
            AimsSurfaceTriangle *AuxMesh() { return _auxmesh; }

            ScaleLevel<AimsSurface<D, Void>, Texture<T> > * Scale(float t)
            {
                if (scales.find(t) != scales.end()) return scales[t];
                else { AddScale(t); return scales[t];}
            }
            Texture<T> & GetScaleImage(float t) {return Scale(t)->Level();}

            Texture<T> & GetOriginalImage() {return Scale(0.0)->Level();}

            std::set<float> GetScaleList();

            std::map<float, ScaleLevel<AimsSurface<D, Void>, Texture<T> >*> GetScaleLevels() {return scales;}

            Smoother<AimsSurface<D, Void>, Texture<T> > *smoother() {return _smoother;}

            void AddScale(float t);
            void AddScale(float t, Texture<float> tex);
            void RemoveScale(float t) {if (scales.find(t) != scales.end()) {ScaleLevel<AimsSurface<D, Void>, Texture<T> >* lev=scales[t]; scales.erase(t); delete lev;} }

            void GenerateDefaultScaleSpace(float tmax)
            {
                float t;
                for (t=1; t<=tmax; t=t*2)
                {
                std::cout << "Adding scale " << t << std::endl;
                AddScale(t);
                }
                std::cout << "Checking last scale level : " <<  tmax << std::endl;
                AddScale(tmax);
            }

            void WriteScale(float t, std::string name);
            TimeTexture<T> getScaleSpaceTexture( );

            void Write(std::string name);
    };


    //------------------------------------------------------------------------
    //-----------------DEFINITIONS--------------------------------------------
    //------------------------------------------------------------------------

    template<typename T> void ScaleSpace<AimsData<T>, AimsData<T> >::AddScale(
                                                    float t, AimsData<float> ima)
    {
        if (scales.find(t) == scales.end())
        {
            ScaleLevel<AimsData<T>, AimsData<T> > *lisseLevel;
//            AimsData<float> aux(tex.nItem());
//            for (uint i=0;i<tex.nItem();i++)
//                aux.item(i) = tex.item(i);
            lisseLevel = new ScaleLevel<AimsData<T>, AimsData<T>  >(t, ima);
            scales.insert(std::pair<float, ScaleLevel<AimsData<T>, AimsData<T>  >*>(t, lisseLevel));
        }
        else
        {
            std::cout << "Scale " << t << " already computed... " << std::endl;
        }
        return ;
    }
    
    template<int D, typename T> void ScaleSpace<AimsSurface<D, Void>, Texture<T>  >::AddScale(
                                                    float t, Texture<float> tex)
    {
        if (scales.find(t) == scales.end())
        {
            ScaleLevel<AimsSurface<D, Void>, Texture<T> > *lisseLevel;
//            Texture<float> aux(tex.nItem());
//            for (uint i=0;i<tex.nItem();i++)
//                aux.item(i) = tex.item(i);
            lisseLevel = new ScaleLevel<AimsSurface<D, Void>, Texture<T> >(t, tex, _mesh, _coordinates, &GetOriginalImage());
            scales.insert(std::pair<float, ScaleLevel<AimsSurface<D>, Texture<T> >*>(t, lisseLevel));
        }
        else
        {
            std::cout << "Scale " << t << " already computed... " << std::endl;
        }
        return ;
    }

    template<typename T> void ScaleSpace<AimsData<T>, AimsData<T> >::AddScale(float t)
    {
    if (scales.find(t) == scales.end())
    {
        if (!(_smoother->optimal()))
        {
        std::cout << "Smoothing from original image" << std::endl;
            int time = this->get_timediff(0, t);
        AimsData<T> lisse=_smoother->doSmoothing(GetOriginalImage(), time);

        ScaleLevel<AimsData<T>, AimsData<T> > *lisseLevel;
        lisseLevel=new ScaleLevel<AimsData<T>, AimsData<T> >(t,lisse);
        scales.insert(std::pair<float, ScaleLevel<AimsData<T>, AimsData<T> >*>(t, lisseLevel));
        }
        else
        {
            std::cout << "Smoothing from previous scale " << std::flush;

            float tlow, tmax;
            AimsData<T> imageLow;
            typename std::map<float, ScaleLevel<AimsData<T>, AimsData<T> >*>::const_iterator itS = scales.begin();

            for ( ; itS!=scales.end(); ++itS)
            tmax=itS->first;

            if (t<=0)
            return;
            else if (t>tmax)
            {
            typename std::map<float, ScaleLevel<AimsData<T>, AimsData<T> >*>::const_iterator itscales = scales.begin();
            typename std::map<float, ScaleLevel<AimsData<T>, AimsData<T> >*>::const_iterator  itLow;
            float tmax=-1;
            for ( ; itscales != scales.end(); ++itscales)
            {
                if ((*itscales).first >= tmax)
                itLow=itscales;
            }
            tlow=(*itLow).first;
            ScaleLevel<AimsData<T>, AimsData<T> > *nivEch=(*itLow).second;
            imageLow=nivEch->Level();
            }
            else
            {
            typename std::map<float, ScaleLevel<AimsData<T>, AimsData<T> >*>::const_iterator itscales = scales.begin();
            typename std::map<float, ScaleLevel<AimsData<T>, AimsData<T> >*>::const_iterator itnext = itscales;

            for ( ; itnext != scales.end(); ++itscales)
            {
                itnext=itscales;
                ++itnext;
                if (((*itscales).first < t) && ((*itnext).first > t))
                {
                tlow=(*itscales).first;
                ScaleLevel<AimsData<T>, AimsData<T> > *nivEch=(*itscales).second;
                imageLow=nivEch->Level();
                itnext=scales.end();
                }
            }
            }
            std::cout << "t=" << tlow << ", image: " << imageLow.dimX() << "x" << imageLow.dimY() << "x" << imageLow.dimZ() << "x" << imageLow.dimT() << std::endl;

            int time = this->get_timediff(tlow, t);
            AimsData<T> lisse=_smoother->doSmoothing(imageLow, time);
            ScaleLevel<AimsData<T>, AimsData<T> > *lisseLevel;
            lisseLevel=new ScaleLevel<AimsData<T>, AimsData<T> >(t,lisse);
            scales.insert(std::pair<float, ScaleLevel<AimsData<T>, AimsData<T> >*>(t, lisseLevel));
        }
        }

        return;
    }

  //----------------------------

  template<int D, typename T> void ScaleSpace<AimsSurface<D>, Texture<T> >::AddScale(float t)
  {

    if (scales.find(t) == scales.end())
    {
      if (!(_smoother->optimal()))
      {
        std::cout << "Smoothing from original image" << std::endl;
        int time = this->get_timediff(0, t);
        Texture<T> lisse=_smoother->doSmoothing(GetOriginalImage(), time);
        ScaleLevel<AimsSurface<D, Void>, Texture<T> > *lisseLevel;
        lisseLevel=new ScaleLevel<AimsSurface<D, Void>, Texture<T> >(t, lisse, _mesh,_coordinates, &GetOriginalImage());
        scales.insert(std::pair<float, ScaleLevel<AimsSurface<D>, Texture<T> >*>(t, lisseLevel));
      }
      else
      {
        std::cout << "Smoothing from previous scale " << std::flush;

        float tlow, tmax;
        Texture<T> imageLow;
        typename std::map<float, ScaleLevel<AimsSurface<D>, Texture<T> >*>::const_iterator itS = scales.begin();
        for ( ; itS!=scales.end(); ++itS)
          tmax=(*itS).first;

        if (t<=0)
          return;
        else if (t>tmax)
        {
          typename std::map<float, ScaleLevel<AimsSurface<D>, Texture<T> >*>::const_iterator
          itscales = scales.begin();
          typename std::map<float, ScaleLevel<AimsSurface<D>, Texture<T> >*>::const_iterator  itLow;
          float tmax=-1;
          for ( ; itscales != scales.end(); ++itscales)
          {
            if ((*itscales).first >= tmax)
              itLow=itscales;
          }


          tlow=(*itLow).first;
          ScaleLevel<AimsSurface<D>, Texture<T> > *nivEch=(*itLow).second;
          imageLow=nivEch->Level();
        }
        else
        {
          typename std::map<float, ScaleLevel<AimsSurface<D>, Texture<T> >*>::const_iterator
          itscales = scales.begin();
          typename std::map<float, ScaleLevel<AimsSurface<D>, Texture<T> >*>::const_iterator
          itnext = itscales;

          for ( ; itnext != scales.end(); ++itscales)
          {
            itnext=itscales;
            ++itnext;

            if (((*itscales).first < t) && ((*itnext).first > t))
            {
              tlow=(*itscales).first;
              ScaleLevel<AimsSurface<D>, Texture<T> > *nivEch=(*itscales).second;
              imageLow=nivEch->Level();
              itnext=scales.end();
            }
          }
        }
        std::cout << "t = " << tlow << std::endl;

        int time = this->get_timediff(tlow, t);
        
        Texture<T> lisse = _smoother->doSmoothing(imageLow, time);
        ScaleLevel<AimsSurface<D, Void>, Texture<T> > *lisseLevel;
        lisseLevel = new ScaleLevel<AimsSurface<D, Void>, Texture<T> >( t, lisse, _mesh, _coordinates, &GetOriginalImage() );
        scales.insert(std::pair<float, ScaleLevel<AimsSurface<D>, Texture<T> >*>(t, lisseLevel));

      }
    }
    else
    {
      std::cout << "Scale " << t << " already computed... " << std::endl;
    }

    return;
  }

 //----------------------------


 template<typename T> void ScaleSpace<AimsData<T>, AimsData<T> >::WriteScale(float t, std::string name)
 {
 	typename std::map<float, ScaleLevel<AimsData<T>, AimsData<T> >*>::iterator itscales;
	if ((itscales=scales.find(t)) != scales.end())
	{
		Writer<AimsData<T> > dataW(name);
		dataW.write(((*itscales).second)->Level());
	}
	else
	{
		std::cout << "Can't write scale " << t << " that does not exist..." << std::endl;
	}
 }

  //----------------------------

 template<int D, typename T> void ScaleSpace<AimsSurface<D, Void>, Texture<T> >::WriteScale(float t, std::string name)
 {
 	typename std::map<float, ScaleLevel<AimsSurface<D, Void>, Texture<T> > >::iterator itscales;
	if ((itscales=scales.find(t)) != scales.end())
	{
		Writer<TimeTexture<T> > dataW(name);
		TimeTexture<T> textW; textW[0]=((*itscales).second)->Level();
		dataW.write(textW);
	}
	else
	{
		std::cout << "Can't write scale " << t << " that does not exist..." << std::endl;
	}
 }
 
  //----------------------------

 template<typename T> void ScaleSpace<AimsData<T>, AimsData<T> >::Write(std::string name)
 {
 	typename std::map<float, ScaleLevel<AimsData<T>, AimsData<T> >*>::iterator itscales=scales.begin();
	int nbScales=scales.size();
	int sx=GetOriginalImage().dimX(), sy=GetOriginalImage().dimY(), sz=GetOriginalImage().dimZ(), x, y, z;
	float dx=GetOriginalImage().sizeX(),dy=GetOriginalImage().sizeY(),dz=GetOriginalImage().sizeZ();
	AimsData<T> multiEch(sx, sy, sz, nbScales), tmpIma;

	int t=0;
	for ( ; itscales!=scales.end(); ++itscales)
	{
		tmpIma=((*itscales).second)->Level();
		for (z=0; z<sz; ++z)
		{
			for (y=0; y<sy; ++y)
			{
				for (x=0; x<sx; ++x)
				{
					multiEch(x,y,z,t)=tmpIma(x,y,z);
				}
			}
		}
		t++;
	}

	multiEch.setSizeXYZT(dx, dy, dz, 1.0);
	Writer<AimsData<T> > dataW(name);
	dataW.write(multiEch);
 }

 //----------------------------

  template<int D, typename T> TimeTexture<T> ScaleSpace<AimsSurface<D, Void>, Texture<T> >::getScaleSpaceTexture()
 {
    typename std::map<float, ScaleLevel<AimsSurface<D, Void>, Texture<T> >*>::iterator itscales=scales.begin();
    TimeTexture<T> multiEch ;
    int t=0;
    for ( ; itscales!=scales.end(); ++itscales)
    {
        multiEch[t]=((*itscales).second)->Level();
        t++;
    }

    return multiEch;
 }

 template<int D, typename T> void ScaleSpace<AimsSurface<D, Void>, Texture<T> >::Write(std::string name)
 {
	typename std::map<float, ScaleLevel<AimsSurface<D, Void>, Texture<T> >*>::iterator itscales=scales.begin();
	TimeTexture<T> multiEch = getScaleSpaceTexture();
	Writer<TimeTexture<T> > dataW(name);
	dataW.write(multiEch);
 }
 
   //----------------------------

 template<int D, typename T> std::set<float> ScaleSpace<AimsSurface<D, Void>, Texture<T> >::GetScaleList()
 {
	typename std::map<float, ScaleLevel<AimsSurface<D, Void>, Texture<T> >*>::iterator itscales=scales.begin();
	std::set<float> setScales;
	for ( ; itscales!=scales.end(); ++itscales)
	{
		setScales.insert((*itscales).first);
	}
	return setScales;
 }

   //----------------------------

 template<typename T> std::set<float> ScaleSpace<AimsData<T>, AimsData<T> >::GetScaleList()
 {
	typename std::map<float, ScaleLevel<AimsData<T>, AimsData<T> >*>::iterator itscales=scales.begin();
	std::set<float> setScales;
	for ( ; itscales!=scales.end(); ++itscales)
	{
		setScales.insert((*itscales).first);
	}
	return setScales;
 }

}

#endif
