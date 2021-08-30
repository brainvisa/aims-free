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


#ifndef AIMS_ROI_ROISTAT_H
#define AIMS_ROI_ROISTAT_H

#include <aims/config/aimsalgo_config.h>
#include <aims/roi/roi.h>
#include <aims/roi/roistat.h>
#include <aims/roi/roiselector.h>
#include <aims/roi/roigtm.h>
#include <cartodata/volume/volume.h>
#include <aims/resampling/resampler.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/math/mathelem.h>
#include <graph/graph/graph.h>
#include <float.h>

namespace aims
{
  class RoiSelector;
  class RoiGtm;

  class AIMSALGO_API RoiStats : public Graph
  {
  public:

    RoiStats( Resampler< float >* interpolator , RoiSelector* roiSel );
    virtual ~RoiStats( );

    template <typename T>
    void  populate( carto::rc_ptr<carto::Volume<T> > &image, AimsRoi &roi,
                    const AffineTransformation3d &motion );
    std::vector<float> tacByStructName(std::string s);

    void          applyGtm( RoiGtm& rgtm );
    void          streamout(char *out=NULL);
    bool          isGtmCorrected() { return _isGtmCorrected; }
    void          setGtmCorrected() { _isGtmCorrected = true; }

  protected:
    Resampler< float >*   _interpolator;
    RoiSelector*          _roiSel;
    bool                  _isGtmCorrected;

  };


  template <typename T>
  void RoiStats::populate( carto::rc_ptr<carto::Volume<T> > &image,
                           AimsRoi &roi,
                           const AffineTransformation3d &motion )
  {
    std::set< Vertex*> currentRoiStatVert;  //tmp var for current set vert of this
    Vertex*       v;                   //tmp var for vertice of the object "this"
    std::vector< int > roi_st, roi_dt;

    // Tatoo the roi object AND the RoiStat  object with the image name
    roi.setProperty( "last_image_stated", image);
    setProperty("last_image_stated", image);

    // get the maximum bouding box
    std::vector< int > bbmaxTO;
    std::vector< float > vsTO;
    roi.getProperty( "boundingbox_max", bbmaxTO);
    roi.getProperty( "voxel_size", vsTO );

    // Gestion de start et duration compte tenu d'appel monoframe ou multi frame
    std::vector< int > i_st, i_dt;
    carto::PropertySet & hd = image->header();
    if( hd.hasProperty("start_time") )
    {
      hd.getProperty("start_time", i_st);
    }
    else
    {
      ASSERT(image->getSizeT() == 1);   // dynamic series with no start/dur time
      i_st.push_back(0);           // Force val for "static" series
    }
    if ( hd.hasProperty("duration_time") )
    {
      hd.getProperty("duration_time", i_dt);
    }
    else
    {
      ASSERT(image->getSizeT() == 1);   // dynamic series with no start/dur time
      i_dt.push_back(1);           // Force val for "static" series
    }

    // Start gathering data.
    carto::VolumeRef< float > tmpin( image->getSizeX(), image->getSizeY(),
                                     image->getSizeZ() );
    tmpin.setVoxelSize( image->getVoxelSize() );
    tmpin.copyHeaderFrom(image->header());
    const std::list<BaseTree*> & metaRoiList = _roiSel->children();
    carto::rc_ptr<BucketMap<Void> >		bck;
    BucketMap<Void>::Bucket::iterator	ibk, ebk;

    // Level 1 : Start loop on time dimension
    for (int t = 0; t < image->getSizeT(); ++t) // populate may be invoked in byframe
      {                                    // or allframe mode
        // 1-Init des fields start et duration time;
        // 2-Re Copy of one frame in tem image
        // 3-Set in order the interpolation process
        getProperty( "start_time", roi_st );
        roi_st.push_back( i_st[ t ] );
        setProperty("start_time",roi_st);
        getProperty( "duration_time", roi_dt );
        roi_dt.push_back( i_dt[ t ] );
        setProperty("duration_time",roi_dt);

        int x,y,z;
        std::vector<int> dim = tmpin->getSize();
        for( z=0; z<dim[2]; ++z )
          for( y=0; y<dim[1]; ++y )
            for( x=0; x<dim[0]; ++x )
              tmpin(x, y, z) = image->at(x, y, z, t);

        _interpolator->setRef( tmpin );
        carto::VolumeRef< float > tmpout = _interpolator->doit( motion,
                                                        bbmaxTO[0]+1, bbmaxTO[1]+1, bbmaxTO[2]+1,
                                                        Point3df(  vsTO[0],vsTO[1],vsTO[2] )   );

        // Level 2 : Loop on the different MetaRoi of selectionSet
        for (BaseTree::const_iterator metaRoi = metaRoiList.begin();
             metaRoi != metaRoiList.end();
             metaRoi++)
          {
            std::map<int,carto::rc_ptr<BucketMap<Void> > >   corresBuckets;
            std::map<int,carto::rc_ptr<BucketMap<Void> > >::iterator 
              ib, eb = corresBuckets.end();
            Tree*   tmpTree = dynamic_cast< Tree *>( *metaRoi ); // Test 
            std::string surname;(*tmpTree).getProperty("surname", surname);

            // Construct the corresBucket
            const std::list<BaseTree*> & metaRoiChild = (*metaRoi)->children();
            for( BaseTree::const_iterator metaRoiContent = metaRoiChild.begin();
                 metaRoiContent != metaRoiChild.end();
                 ++metaRoiContent )
              {
                tmpTree = dynamic_cast< Tree *>( *metaRoiContent ); // Test 
                std::string tmp;(*tmpTree).getProperty("nomenName", tmp);
                //std::cout << "Debug nomenName>> " << tmp << std::endl;

                std::set<Vertex*> roisv = roi.VertByNameAndDescendantName( tmp );
                for( std::set< Vertex* >::const_iterator j = roisv.begin();
                     j != roisv.end(); ++j )
                  {
                    int l; (*j)->getProperty("roi_label", l);
                    if( (*j)->getProperty( "aims_roi", bck ) )
                      corresBuckets[ l ] = bck;
                    else
                      std::cout << "Bucket not found. Label = "  << l << std::endl ;
                  }
              }

            if (corresBuckets.size() == 0) continue;

            // Lets process the current corresBuckets
            // and construct the vertices of RoiStat.
            currentRoiStatVert = getVerticesWith("surname", surname);

            ASSERT (currentRoiStatVert.size() < 2 ); // only 0 first loop 
            //or 1 -other loops
            if (currentRoiStatVert.size() == 0)
              {
                v = addVertex();
                v->setProperty("surname", surname);
              }
            else v = *( currentRoiStatVert.begin() );

            // Level 3: loop on the corresBuckets in the arg roi and accumulate
            float mean = 0.0;
            float std = 0.0;
            float min = FLT_MAX;
            float max = FLT_MIN;
            int pn2 = 0 ;
            for( ib=corresBuckets.begin(); ib!=eb; ++ib )
              {
                bck = ib->second;
                // 0- Init
                // 1-Retrieve  one bucket : (*v) 
                // 2-Cumul stat
                if( !bck->empty() )
                  for( ibk=bck->begin()->second.begin(),
                        ebk=bck->begin()->second.end(); ibk!=ebk; ++ibk )
                  {
                    float pixel = tmpout( ibk->first );
                    mean += pixel;
                    std  += square( pixel);
                    min = (min > pixel) ? pixel : min;
                    max = (max < pixel) ? pixel : max;
                    ++pn2 ;
                  }
              } // end of Level 3 loop

            //Update the current vertice in the RoiStat object
            mean /= (float) pn2;
            std=(float)sqrt( (double)(std-square(mean)*pn2) / (float)(pn2-1) );

            v->setProperty("voxel_number", pn2 );
            std::vector< float > tac, sac, mac, Mac;
            v->getProperty( "mean_ac", tac);
            tac.push_back( mean );
            v->setProperty("mean_ac", tac);
            v->getProperty( "std_ac", sac);
            sac.push_back( std );
            v->setProperty( "std_ac", sac);
            v->getProperty( "min_ac", mac);
            mac.push_back( min );
            v->setProperty( "min_ac", mac);
            v->getProperty( "max_ac", Mac);
            Mac.push_back( max );
            v->setProperty( "max_ac", Mac);
          } //end of level 2 (loop on different set in the selectionSet)
      } // end level 1 (loop on time)

    setProperty("voxel_volume",vsTO[0]*vsTO[1]*vsTO[2]);
  }
}

#endif
