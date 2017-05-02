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

#ifndef AIMS_FIBERS_BUNDLEREGROUP_H
#define AIMS_FIBERS_BUNDLEREGROUP_H


#include <aims/fibers/bundles.h>


  //---------------//
 // BundleRegroup //
//---------------//

namespace aims
{

typedef std::vector< aims::FiberPoint > Fiber;
typedef std::vector< Fiber > Fibers;
typedef std::vector< Fibers > BundlesSet;


class BundleRegroup : public aims::BundleProducer, public aims::BundleListener
{
public:

  /** If no groupsFileName or stream is specified, use bundles names to 
      regroup.
  */
  BundleRegroup( const std::string &groupsFileName,
                 std::vector< std::string > &excludednames );
  BundleRegroup( const std::string &groupsFileName = std::string() );
  virtual ~BundleRegroup();
  void setStream( std::istream & );

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

  Fiber _fiber;
  Fibers _fibers;
  BundlesSet _bundlesSet;
  std::vector<std::string> _bundles_name;
  std::string _file_name;
  std::ifstream _file_internal;
  std::istream *_file;
  std::vector< std::string > &_excludednames;
  std::vector< std::string > vect;
};

} // namespace aims

#endif // ifndef AIMS_FIBERS_BUNDLEREGROUP_H
