#ifndef AIMS_RESAMPLING_MASK_D_H
#define AIMS_RESAMPLING_MASK_D_H

#include <aims/resampling/mask.h>

namespace aims {

  template <typename T>
  BucketMap<Void>* maskWithVolume( const BucketMap<Void> & src,
                                   const carto::rc_ptr<carto::Volume<T> > m,
                                   const T & val, bool insideval )
  {
    BucketMap<Void>::const_iterator ib, eb = src.end();
    BucketMap<Void>::Bucket::const_iterator i, e;
    BucketMap<Void> *out = new BucketMap<Void>;
    std::vector<float> vs;
    src.header().getProperty( "voxel_size", vs );
    while( vs.size() < 4 )
      vs.push_back( 1. );
    out->setSizeXYZT( vs[0], vs[1], vs[2], vs[3] );
    out->setHeader( src.header() ); // maybe a bad idea ?
    int t;
    for( ib=src.begin(); ib!=eb; ++ib )
    {
      // find timestep
      t = ib->first;
      if( t < m->getSizeT() )
      {
        const BucketMap<Void>::Bucket & bk = ib->second;
        BucketMap<Void>::Bucket & bkout = (*out)[ ib->first ];

        if( insideval )
        {
          for( i=bk.begin(), e=bk.end(); i!=e; ++i )
          {
            const Point3d & pos = i->first;
            if( m->at( pos[0], pos[1], pos[2], t ) == val )
              bkout[ i->first ] = Void();
          }
        }
        else
        {
          for( i=bk.begin(), e=bk.end(); i!=e; ++i )
          {
            const Point3d & pos = i->first;
            if( m->at( pos[0], pos[1], pos[2], t ) != val )
              bkout[ i->first ] = Void();
          }
        }
      }
    }
    return out;
  }

}

#endif
