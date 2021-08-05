/* Copyright (c) 1995-2017 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/fibers/bundleSampler.h>
#include <time.h>

using namespace std;
using namespace carto;

  //---------------//
 // BundleSampler //
//---------------//

namespace aims
{

//-----------------------------------------------------------------------------
BundleSampler::BundleSampler(float percent, const string &bundle_name,
  const string &sampled_bundle_name, int mode) :
  _percent(percent), _mode(mode), _bundle_name(bundle_name),
  _sampled_bundle_name(sampled_bundle_name)
{
  srand(time(0)); // WARNING: sould maybe remove this here
  _fiber_count = 0;
  _fiber_selected_count = 0;
  _discarded = false;
}

//-----------------------------------------------------------------------------
BundleSampler::BundleSampler(float percent, const string &bundle_name,
  const string &sampled_bundle_name,
  int mode, bool discarded) : _percent(percent), _mode(mode), 
  _bundle_name(bundle_name),
  _sampled_bundle_name(sampled_bundle_name),
  _discarded(discarded)
{
  srand(time(0)); // WARNING: sould maybe remove this here
  _fiber_count = 0;
  _fiber_selected_count = 0;
}

//-----------------------------------------------------------------------------
BundleSampler::~BundleSampler()
{
}


//-----------------------------------------------------------------------------
void BundleSampler::bundleStarted( const BundleProducer &,
                                   const BundleInfo &bundleInfo )
{
  BundleProducer::startBundle( bundleInfo );
  _first_fiber = true;
}


//-----------------------------------------------------------------------------
void BundleSampler::bundleTerminated( const BundleProducer &,
                                      const BundleInfo &bundleInfo )
{
  BundleProducer::terminateBundle( bundleInfo );
}


//-----------------------------------------------------------------------------
void BundleSampler::fiberStarted( const BundleProducer &,
                                   const BundleInfo &bundleInfo,
                                   const FiberInfo &fiberInfo )
{
  _fiber_count++;
  double _rand_num = double(rand()) / RAND_MAX * 100.;

  if( _rand_num < _percent )
  {
    _fiber_selected = true;
  }

  if( _mode && _first_fiber )
  {
    _fiber_selected = true;
  }

  if( _fiber_selected )
  {
    BundleProducer::startFiber( bundleInfo, fiberInfo);
    _first_fiber = false;
    _fiber_selected_count++;
  }
  else if( _discarded )
  {
    _fiber.clear();
  }
}


//-----------------------------------------------------------------------------
void BundleSampler::fiberTerminated( const BundleProducer &,
                                     const BundleInfo &bundleInfo,
                                     const FiberInfo &fiberInfo )
{
  if ( _fiber_selected )
  {
    BundleProducer::terminateFiber( bundleInfo, fiberInfo);
    _fiber_selected = false;
  }
  else if (_discarded)
  {
    _fibers.push_back(_fiber);
  }
}


//-----------------------------------------------------------------------------
void BundleSampler::newFiberPoint(  const BundleProducer &,
                                    const BundleInfo &bundleInfo,
                                    const FiberInfo &fiberInfo,
                                    const FiberPoint &point )
{
  if ( _fiber_selected )
  {
    BundleProducer::addFiberPoint(bundleInfo, fiberInfo, point);
  }
  else if (_discarded)
  {
    _fiber.push_back(point);
  }
}


//-----------------------------------------------------------------------------
void BundleSampler::noMoreBundle( const BundleProducer & )
{

  if (_discarded)
  {
     BundleInfo bundle_tmp("discarded");
     startBundle( bundle_tmp );
     for (Fibers::iterator fib=_fibers.begin(); fib!=_fibers.end(); ++fib)
     {
        FiberInfo fiber_tmp(1);
        startFiber( bundle_tmp, fiber_tmp );
        for (Fiber::iterator pt=(*fib).begin(); pt!=(*fib).end(); ++pt)
        {
          addFiberPoint( bundle_tmp, fiber_tmp, *pt);
        }
        terminateFiber( bundle_tmp, fiber_tmp);
     }
     terminateBundle( bundle_tmp );
  }

  BundleProducer::noMoreBundle();
  // cout << _fiber_selected_count << " fibers selected from a total of " << _fiber_count << " fibers." << endl;
}

} //namespace aims
