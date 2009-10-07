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



#include <aims/perfusion/perfProcCenter.h>
#include <aims/perfusion/perfMask.h>
#include <aims/perfusion/perfSkip.h>
#include <aims/perfusion/perfAifPoints.h>
#include <aims/perfusion/perfPreInj.h>
#include <aims/perfusion/perfQuantif.h>
#include <aims/perfusion/perfAif.h>
#include <aims/perfusion/perfFit.h>
#include <aims/perfusion/perfDeconv.h>

#include <aims/data/pheader.h>


using namespace aims;
using namespace std;


PerfusionProcessingCenter::~PerfusionProcessingCenter()
{
  delete pmap;
}


void PerfusionProcessingCenter::initialize()
{
  PerfusionSkip *pskip = new PerfusionSkip;
  PerfusionMask *pm = new PerfusionMask;
  PerfusionAifPoints *pap = new PerfusionAifPoints;
  PerfusionPreInjection *ppi = new PerfusionPreInjection;
  PerfusionQuantification *pq = new PerfusionQuantification;
  PerfusionAif *pa = new PerfusionAif;
  PerfusionFit *pf = new PerfusionFit;  
  PerfusionDeconvolution *pd = new PerfusionDeconvolution;
  pmap = new PerfusionMaps;

  mProc[ pskip->id() ] = pskip;
  mProc[ pm->id() ] = pm;
  mProc[ pap->id() ] = pap;
  mProc[ ppi->id() ] = ppi;
  mProc[ pq->id() ] = pq;
  mProc[ pa->id() ] = pa;
  mProc[ pf->id() ] = pf;
  mProc[ pd->id() ] = pd;
  mProc[ pmap->id() ] = pmap;
}


void PerfusionProcessingCenter::registerData( AimsData< short > *d )
{
  _dataIn = d;
  
  if ( d->header() )
    {
      aims::PythonHeader *ahdr;
      ahdr = dynamic_cast< aims::PythonHeader * >( d->header() );
      
      float tmp;
      
      if ( ahdr && ahdr->getProperty( "tr", tmp ) )  _params.setTr( tmp );
      if ( ahdr && ahdr->getProperty( "te", tmp ) )  _params.setTe( tmp );
    }
}


void PerfusionProcessingCenter::doMask( int btn )
{
  PerfusionMask *pm = dynamic_cast< PerfusionMask * >( mProc[ btn ] );
  
  if ( _dataIn )
    {
      _dataIn->setSizeT( _params.tr() / 1000.0f );
      try
	{
	  maskBck = pm->doit( *_dataIn, _params );
	  pm->setDone( true );
	}
      catch( exception & )
	{
	  cerr << "Perfusion doMask failed\n";
	  pm->setDone( false );
	}
    }
}


void PerfusionProcessingCenter::doSkip( int btn )
{
  PerfusionSkip *ps = dynamic_cast< PerfusionSkip * >( mProc[ btn ] );

  if ( _dataIn )
    {
      try
	{
	  int skipValue = ps->value( *_dataIn, _params );
	  _params.setSkip( skipValue );
	  ps->setDone( true );
	}
      catch( exception & )
	{
	  cerr << "Perfusion doSkip failed\n";
	  ps->setDone( false );
	}
    }
}


void PerfusionProcessingCenter::doAifPoints( int btn )
{
  PerfusionAifPoints *p = dynamic_cast< PerfusionAifPoints * >( mProc[ btn ] );

  if ( _dataIn )
    {
      aifPoints = p->search( *_dataIn, maskBck, _params );
      p->setDone( true );
    }
}


void PerfusionProcessingCenter::doInjection( int btn )
{
  PerfusionPreInjection *ppi;
  ppi = dynamic_cast< PerfusionPreInjection * >( mProc[ btn ] );
  
  if ( _dataIn )
    {
      int preInj = ppi->value( *_dataIn, aifSelected, _params );
      _params.setPreInj( preInj );
      ppi->setDone( true );
    }
}


void PerfusionProcessingCenter::doQuantification( int btn )
{
  PerfusionQuantification *pq;
  pq = dynamic_cast< PerfusionQuantification * >( mProc[ btn ] );
  
  if ( _dataIn )
    {
      dataQuantif = pq->doit( *_dataIn, maskBck, _params );
      pq->setDone( true );
    }
}


void PerfusionProcessingCenter::doAif( int btn )
{
  PerfusionAif *pa = dynamic_cast< PerfusionAif * >( mProc[ btn ] );
  
  aifCurve = pa->doit( dataQuantif, aifPoints, aifSelected, _params );
  aifFit = pa->fit();
  aifParam = pa->fitParams();
  pa->setDone( true );
}


void PerfusionProcessingCenter::doFit( int btn )
{
  PerfusionFit *pf = dynamic_cast< PerfusionFit * >( mProc[ btn ] );

  mFit.clear();
  dataFit = pf->doit( dataQuantif, maskBck, _params, mFit );
  pf->setDone( true );
}


void PerfusionProcessingCenter::doDeconvolution( int btn )
{
  PerfusionDeconvolution *pd;
  pd = dynamic_cast< PerfusionDeconvolution * >( mProc[ btn ] );

  dataDeconv = pd->doit( dataFit, aifFit, maskBck, _params );
  pd->setDone( true );
}


bool PerfusionProcessingCenter::doMaps( int btn )
{
  PerfusionMaps *pm = dynamic_cast< PerfusionMaps * >( mProc[ btn ] );

  bool status = pm->doit( dataQuantif, dataFit, aifFit, dataDeconv, maskBck, 
			  aifParam, mFit, _params );
  pm->setDone( true );

  return status;
}


void PerfusionProcessingCenter::saveMaps( const string& filename )
{
  pmap->saveMaps( filename );
}
