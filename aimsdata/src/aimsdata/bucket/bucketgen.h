
#ifndef AIMS_BUCKET_BUCKETGEN_H
#define AIMS_BUCKET_BUCKETGEN_H

#include <aims/bucket/bucketMap.h>
#include <cartobase/object/attributed.h>

namespace aims
{

  /// Factory class for various common shapes of meshes
  class BucketMapGenerator
  {
  public:
    typedef BucketMap<void>* 
    (*GenFunction)( const carto::GenericObject & );

    struct Generator
    {
      virtual ~Generator() {}

      virtual BucketMap<void>* 
      generator( const carto::GenericObject & ) const = 0;
      virtual carto::Object parameters() const = 0;
    };

    /// generic factory function (calls lower-level functions)
    static BucketMap<void>* 
    generate( const carto::GenericObject & params );
    /// generic factory function (calls lower-level functions)
    static BucketMap<void>* generate( carto::Object params );

    /// description of known shapes and parameters (vector of dicts)
    static carto::Object description();

    static BucketMap<void>* quadric( const carto::GenericObject & params );
    static BucketMap<void>* quadric( unsigned order, const std::vector<double> * coefficients, const Point3df & origin, const Point3df & sizes, const Point3df & resolution );
  };

}


#endif
