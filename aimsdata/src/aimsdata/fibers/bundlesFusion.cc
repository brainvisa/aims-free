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

#include <aims/fibers/bundlesFusion.h>

using namespace aims;
using namespace std;

//-----------------------------------------------------------------------------
BundlesFusion::BundlesFusion( int nb_bundles, const Fiber &fiber,
                              const Bundle &bundle,
                              const BundlesSet &bundlesSet ) :
  _nb_bundles(nb_bundles),  _fiber(fiber),
  _bundle(bundle),_bundlesSet(bundlesSet)
{
  _num=1;
}


//-----------------------------------------------------------------------------
BundlesFusion::~BundlesFusion()
{
}


//-----------------------------------------------------------------------------
void BundlesFusion::bundleStarted( const BundleProducer &,
                                   const BundleInfo & )
{
  _bundle.clear();
}


//-----------------------------------------------------------------------------
void BundlesFusion::bundleTerminated( const BundleProducer &,
                                      const BundleInfo &bundle )
{
  if( _bundle.empty() )
    return; // skip empty ones

  // look for bundle name in map
  map<string, int>::iterator
    recherche=_bundles_name.find( bundle.name() );
  if(recherche==_bundles_name.end())
  {
    // **if it's a new name, create a new bundle in memory.**
    int ind = _bundles_name.size();
    _bundlesSet.push_back(_bundle);
    _bundles_name[ bundle.name() ] = ind;
  }
  else
  {
    // **if the name already exists, add bundle fibers to existing bundle**
    // matching index
    int ind=recherche->second;
    // add fibers to bundle.
    Bundle & b = _bundlesSet[ ind ];
    b.insert( b.end(), _bundle.begin(), _bundle.end() );
  }
  _bundle.clear();
}


//-----------------------------------------------------------------------------
void BundlesFusion::fiberStarted( const BundleProducer &,
                                  const BundleInfo &,
                                  const FiberInfo & )
{
  _fiber.clear();
}


//-----------------------------------------------------------------------------
void BundlesFusion::fiberTerminated( const BundleProducer &,
                                     const BundleInfo &,
                                     const FiberInfo & )
{
  if( !_fiber.empty() )
  {
    _bundle.push_back(_fiber);
    _fiber.clear();
  }
}


//-----------------------------------------------------------------------------
void BundlesFusion::newFiberPoint( const BundleProducer &,
                                   const BundleInfo &,
                                   const FiberInfo &,
                                   const FiberPoint &point )
{
  _fiber.push_back(point);
}


//-----------------------------------------------------------------------------
void BundlesFusion::noMoreBundle( const BundleProducer & )
{
  //On ecrit les bundles en mémoire en récupérant les noms du dico:
  if(_num<_nb_bundles)
  {
    cout << endl << "end of bundles file: " << _num << ", bundles: "
      << _bundlesSet.size();
    unsigned count = 0;
    for( BundlesSet::iterator bun=_bundlesSet.begin();
        bun!=_bundlesSet.end(); ++bun )
      count += bun->size();
    cout << ", tracts: " << count << endl;
    ++_num;
  }
  else
  {
    // process and re-send events for all reordered fibers
    unsigned count = 0;
    map<string, int>::iterator names=_bundles_name.begin(),
      en = _bundles_name.end();
    for( ; names!=en; ++names )
    {
      const Bundle & bundle = _bundlesSet[ names->second ];
      BundleInfo bundle_tmp(names->first);
      count += bundle.size();
      startBundle( bundle_tmp );
      for(Bundle::const_iterator fib=bundle.begin();fib!=bundle.end();++fib)
      {
        FiberInfo fiber_tmp(1);
        startFiber( bundle_tmp, fiber_tmp );
        for(Fiber::const_iterator pt=(*fib).begin();pt!=(*fib).end();++pt)
          addFiberPoint( bundle_tmp, fiber_tmp,*pt);
        terminateFiber( bundle_tmp, fiber_tmp);
      }
      terminateBundle( bundle_tmp );
    }
    BundleProducer::noMoreBundle();
    cout << endl << "BundlesFusion processing end, total fibers: " << count
      << endl << flush;
    _bundlesSet.clear();
  }
}


