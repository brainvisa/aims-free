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

#ifndef AIMS_FIBERS_BUNDLESFUSION_H
#define AIMS_FIBERS_BUNDLESFUSION_H

#include <aims/fibers/bundles.h>


  //------------------//
 // BundlesFusion    //
//------------------//

namespace aims
{

  /** Bundles fusionning: can merge several bundles producers (files...) into
      a single output, merging bundles split accross several producers, but
      having the same name.
      This BundleListener can be plugged (with a single instance) behind
      several BundleProducer (ie readers) instances.
      For merging purposes, all fibers are stored in memory.
  */
  class BundlesFusion : public aims::BundleProducer, public aims::BundleListener
  {
  public:
    typedef std::vector< aims::FiberPoint > Fiber;
    typedef std::vector< Fiber >        Bundle;
    typedef std::vector< Bundle >       BundlesSet;
    typedef std::vector< FiberInfo >    FiberInfoSet;
    typedef std::vector< BundleInfo >   BundleInfoSet;
    typedef std::vector< FiberInfoSet > BundlesFiberInfo;

    BundlesFusion( int, const Fiber & = Fiber(), const Bundle & = Bundle(),
                   const BundlesSet & = BundlesSet() );
    virtual ~BundlesFusion();

  protected:


    virtual void bundleStarted( const aims::BundleProducer &,
                                const aims::BundleInfo & );
    virtual void bundleTerminated( const aims::BundleProducer &,
                                   const aims::BundleInfo & );
    virtual void fiberStarted( const aims::BundleProducer &,
                               const aims::BundleInfo &,
                               const aims::FiberInfo & );
    virtual void fiberTerminated( const aims::BundleProducer &,
                                  const aims::BundleInfo &,
                                  const aims::FiberInfo & );
    virtual void newFiberPoint( const aims::BundleProducer &,
                                const aims::BundleInfo &,
                                const aims::FiberInfo &,
                                const aims::FiberPoint & );
    virtual void noMoreBundle( const aims::BundleProducer & );

  private:

    int _nb_bundles,_num;
    Fiber _fiber;
    Bundle _bundle;
    BundleInfoSet _bundleInfoSet;
    BundlesSet _bundlesSet;
    std::map<std::string, int> _bundles_name;
    FiberInfoSet _fiberInfoSet;
    BundlesFiberInfo _bundlesFiberInfo;
  };

}

#endif

