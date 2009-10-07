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


#include <aims/perfusion/perfMaps.h>
#include <aims/perfusion/perfCBV.h>
#include <aims/perfusion/perfCBF.h>
#include <aims/perfusion/perfMTT.h>
#include <aims/perfusion/perfTTP.h>
#include <aims/perfusion/perfDelay.h>
#include <aims/perfusion/perfH.h>
#include <aims/perfusion/perfBBB.h>

using namespace aims;
using namespace std;


PerfusionMaps::PerfusionMaps() : PerfusionProcessing( 9 )
{
  CBVMap *cbvmap = new CBVMap();
  CBFMap *cbfmap = new CBFMap();
  MTTMap *mttmap = new MTTMap();
  TTPMap *ttpmap = new TTPMap();
  DelayMap *delaymap = new DelayMap();
  HMap *hmap = new HMap();
  BBBMap *bbbmap = new BBBMap();

  mCalc[ cbvmap->id() ] = cbvmap;
  mCalc[ cbfmap->id() ] = cbfmap;
  mCalc[ mttmap->id() ] = mttmap;
  mCalc[ ttpmap->id() ] = ttpmap;
  mCalc[ delaymap->id() ] = delaymap;
  mCalc[ hmap->id() ] = hmap;
  mCalc[ bbbmap->id() ] = bbbmap;
}


AimsData< float >& PerfusionMaps::mapCBF()
{
  return mCalc[ (int)PerfusionMapBase::cbf ]->map();
}


AimsData< float >& PerfusionMaps::mapCBV()
{
  return mCalc[ (int)PerfusionMapBase::cbv ]->map();
}


AimsData< float >& PerfusionMaps::mapMTT()
{
  return mCalc[ (int)PerfusionMapBase::mtt ]->map();
}


AimsData< float >& PerfusionMaps::mapTTP()
{
  return mCalc[ (int)PerfusionMapBase::ttp ]->map();
}


AimsData< float >& PerfusionMaps::mapDelay()
{
  return mCalc[ (int)PerfusionMapBase::delay ]->map();
}


AimsData< float >& PerfusionMaps::maph()
{
  return mCalc[ (int)PerfusionMapBase::h ]->map();
}


AimsData< float >& PerfusionMaps::mapBBB()
{
  return mCalc[ (int)PerfusionMapBase::bbb ]->map();
}


void PerfusionMaps::setState( int index, bool theState )
{
  mCalc[ index ]->setState( theState );
}


void PerfusionMaps::saveMaps( const string& filename )
{
  map< int, PerfusionMapBase * >::iterator it = mCalc.begin();
  while( it != mCalc.end() )
    {
      if ( it->second->isChecked() )
	it->second->saveMap( filename );

      ++it;
    }
}


bool 
PerfusionMaps::doit( AimsData< float >& q, AimsData< float >& d, 
		     AimsData< float >& aif, AimsData< float >& r, 
		     BucketMap< Void >& mask, LMGamma< float >& aifParam,
		     std::map< Point3d, LMGamma< float >, bestPoint3d >& mFit,
		     PerfusionParameters& pp )
{
  bool isOk = false;
  AimsData< float > *cbvm = 0;
  AimsData< float > *cbfm = 0;

  map< int, PerfusionMapBase * >::iterator it = mCalc.begin();
  while( it != mCalc.end() )
    {
      if ( it->second->isChecked() )
	{
	  it->second->doit( q, d, aif, r, mask, aifParam, mFit, pp, 
			    cbvm, cbfm );
	  isOk = true;

	  if ( it->first == (int)PerfusionMapBase::cbv )
	    cbvm = &mCalc[ (int)PerfusionMapBase::cbv ]->map();
	  if ( it->first == (int)PerfusionMapBase::cbf )
	    cbfm = &mCalc[ (int)PerfusionMapBase::cbf ]->map();
	}

      ++it;
    }

  return isOk;
}
