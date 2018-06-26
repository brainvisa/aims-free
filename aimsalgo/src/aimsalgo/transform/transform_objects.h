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

#ifndef AIMS_REGISTRATION_TRANSFORM_OBJECTS_H
#define AIMS_REGISTRATION_TRANSFORM_OBJECTS_H

#include <cartobase/smart/rcptr.h>
#include <aims/mesh/surface.h>
#include <aims/bucket/bucketMap.h>
#include <aims/fibers/bundles.h>

class Graph;

namespace aims
{

  template <int D>
  void transformMesh( AimsTimeSurface<D, Void> & mesh,
                      Transformation3d & transformation);

  carto::rc_ptr<BucketMap<Void> >
  transformBucket( const BucketMap<Void> & bck, Transformation3d & transformation,
                   const Point3df & vs = Point3df( 0., 0., 0. ) );

  void transformGraph( Graph & graph, Transformation3d & transformation,
                       const Point3df & vs = Point3df( 0., 0., 0. ) );

  /** Apply an arbitrary transform to bundles in stream.
      This is a BundleListener / BundleProducer stream processing class
      which applies vector field deformation to bundle data. It can be
      typically connected to a BundleReader and a BundleWriter.
  */
  class BundleTransformer : public BundleListener, public BundleProducer
  {
  public:
    BundleTransformer( const carto::rc_ptr<Transformation3d>& transformation );
    virtual ~BundleTransformer();

    virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
    virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
    virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                               const FiberInfo & );
    virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                                  const FiberInfo & );
    virtual void newFiberPoint( const BundleProducer &, const BundleInfo &,
                                const FiberInfo &, const FiberPoint & );
    virtual void noMoreBundle( const BundleProducer & );

  private:
    carto::rc_ptr<Transformation3d> _transformation;
  };

} // namespace aims

#endif // !defined(AIMS_REGISTRATION_TRANSFORM_OBJECTS_H)
