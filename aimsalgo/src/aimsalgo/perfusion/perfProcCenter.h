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



#ifndef AIMS_PERFUSION_PERFPROCCENTER_H
#define AIMS_PERFUSION_PERFPROCCENTER_H

#include <aims/perfusion/perfProcessing.h>
#include <aims/perfusion/perfParam.h>
#include <aims/perfusion/perfFitStgy.h>
#include <aims/perfusion/perfMaps.h>

#include <aims/data/data.h>
#include <aims/bucket/bucket.h>
#include <aims/vector/vector.h>
#include <aims/optimization/lmgamma.h>

#include <list>
#include <map>
#include <string>

namespace aims
{

  class PerfusionProcessingCenter
  {
  public:

    PerfusionProcessingCenter() { initialize();  }
    virtual ~PerfusionProcessingCenter();

    virtual std::map< int, PerfusionProcessing * >& processings() 
    { return mProc; }
    virtual PerfusionParameters& parameters() { return _params; }

    PerfusionMaps *perfMaps() { return pmap; }
  
    void initialize();

    void registerData( carto::rc_ptr<carto::Volume< short > > );

    void doMask( int );
    void doSkip( int );
    void doAifPoints( int );
    void doInjection( int );
    void doQuantification( int );
    void doAif( int );
    void doFit( int );
    void doDeconvolution( int );
    bool doMaps( int );

    void saveMaps( const std::string& );

    void setSelection( std::list< Point4dl >& sel ) { aifSelected = sel; }

    aims::BucketMap< Void >& bucketMask() { return maskBck; }
    std::list< Point4dl >& aifPointList() { return aifPoints; }
    AimsData< float >& quantifiedData() { return dataQuantif; }
    AimsData< float >& aif() { return aifCurve; }
    AimsData< float >& fittedAif() { return aifFit; }
    AimsData< float >& fittedData() { return dataFit; }
    AimsData< float >& deconvolvedData() { return dataDeconv; }
    AimsData< float >& CBF() { return pmap->mapCBF(); }
    AimsData< float >& CBV() { return pmap->mapCBV(); }
    AimsData< float >& MTT() { return pmap->mapMTT(); }
    AimsData< float >& TTP() { return pmap->mapTTP(); }
    AimsData< float >& delay() { return pmap->mapDelay(); }
    AimsData< float >& heterogeneity() { return pmap->maph(); }
    AimsData< float >& BBB() { return pmap->mapBBB(); }

  private:

    PerfusionParameters _params;
    PerfusionMaps *pmap;

    std::map< int, PerfusionProcessing * > mProc;
    std::map< Point3d, LMGamma< float >, bestPoint3d > mFit;
  
    carto::rc_ptr<carto::Volume< short > > _dataIn;
    std::list< Point4dl > aifSelected;
    LMGamma< float > aifParam;

    aims::BucketMap< Void > maskBck;
    std::list< Point4dl > aifPoints;
    AimsData< float > dataQuantif;
    AimsData< float > aifCurve;
    AimsData< float > aifFit;
    AimsData< float > dataFit;
    AimsData< float > dataDeconv;
  };

}

#endif
