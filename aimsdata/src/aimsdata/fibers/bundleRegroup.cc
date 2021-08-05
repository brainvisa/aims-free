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

#include <aims/fibers/bundleRegroup.h>

using namespace std;

//------------------//
// BundleRegroup //
//------------------//

namespace aims
{

BundleRegroup::BundleRegroup( const string &groupsFileName, std::vector< string > &excludednames ) 
             : _file_name(groupsFileName),
               _file( 0 ),
               _excludednames(excludednames)
{
  if( !_file_name.empty() )
  {
    _file_internal.open( _file_name.c_str() );
    _file = &_file_internal;
    if (_file_internal.is_open()) cout << "Groups file opened: " << _file_name << endl;
  }
}
//-----------------------------------------------------------------------------

BundleRegroup::BundleRegroup( const string &groupsFileName )
             : _file_name(groupsFileName),
               _file( 0 ),
               _excludednames(vect)
{
  if( !_file_name.empty() )
  {
    _file_internal.open( _file_name.c_str() );
    _file = &_file_internal;
    if (_file_internal.is_open()) cout << "Groups file opened: " << _file_name << endl;
  }
}

//-----------------------------------------------------------------------------
BundleRegroup::~BundleRegroup()
{
  if( _file_internal.is_open() )
    _file_internal.close();
}


//-----------------------------------------------------------------------------
void BundleRegroup::bundleStarted( const BundleProducer &,
                                   const BundleInfo & )
{

}

//-----------------------------------------------------------------------------
void BundleRegroup::bundleTerminated( const BundleProducer &,
                                      const BundleInfo & )
{

}


//-----------------------------------------------------------------------------
void BundleRegroup::fiberStarted( const BundleProducer &,
                                  const BundleInfo &,
                                  const FiberInfo & )
{
  _fiber.clear();
}


//-----------------------------------------------------------------------------
void BundleRegroup::fiberTerminated( const BundleProducer &, 
                                     const BundleInfo & bundleInfo,
                                     const FiberInfo & )
{
  _fibers.clear();

  // read new fiber bundle name
  string new_bundle_name;
  if( _file )
    *_file >> new_bundle_name;
  else
    new_bundle_name = bundleInfo.name();
  //*_file >> new_bundle_name;
  //cout << new_bundle_name << endl;

  // look for the new bundle name in the existing bundle name list
  vector<string>::iterator recherche = find(_bundles_name.begin(), _bundles_name.end(), new_bundle_name);
  if ( recherche == _bundles_name.end() )
  {
    // if the new bundle name is new, add it to the list
    _fibers.push_back(_fiber);
    _bundlesSet.push_back(_fibers);
    _bundles_name.push_back( new_bundle_name );
  }
  else
  {
    // if the bundle name already exists, add the fiber to the corresponding new name
    int ind = recherche - _bundles_name.begin();
     _bundlesSet[ind].push_back(_fiber);
  }
}


void BundleRegroup::setStream( istream & stm )
{
  _file = &stm;
}


//-----------------------------------------------------------------------------
void BundleRegroup::newFiberPoint( const BundleProducer &, 
                                   const BundleInfo &,
                                   const FiberInfo &, 
                                   const FiberPoint &point )
{
  _fiber.push_back(point);
}


//-----------------------------------------------------------------------------
void BundleRegroup::noMoreBundle( const BundleProducer & )
{
    // on procede à l'ecriture du fichier de sortie
    vector<string>::iterator names = _bundles_name.begin();
    for (BundlesSet::iterator bun = _bundlesSet.begin(); bun!=_bundlesSet.end(); ++bun, ++names)
    {
      bool include = true;
      for (int i = 0; i < (int)_excludednames.size(); i++)
      {
        if (_excludednames[i] == *names)
        {
           include = false;
        }
      }

      if (include)
      {
        BundleInfo bundle_tmp(*names);
        startBundle( bundle_tmp );
        for (Fibers::iterator fib=(*bun).begin(); fib!=(*bun).end(); ++fib)
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
    }
    BundleProducer::noMoreBundle();
    cout<<endl<<"fin de l'écriture!!"<<endl<<flush;
}

} //namespace aims
