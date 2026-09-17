// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/resampling/mask_d.h>

using namespace aims;
using namespace carto;

BucketMap<Void>* aims::mask( const BucketMap<Void> & src, const BucketMap<Void> & m, bool intersect )
{
  BucketMap<Void>::const_iterator ib, eb = src.end(), ib2, eb2 = m.end();
  BucketMap<Void>::Bucket::const_iterator i, e, j, e2;
  BucketMap<Void> *out = new BucketMap<Void>;
  out->setSizeXYZT( src.sizeX(), src.sizeY(), src.sizeZ(), src.sizeT() );
  out->setHeader( src.header() );
  for( ib=src.begin(); ib!=eb; ++ib )
  {
    // find timestep
    ib2 = m.find( ib->first );
    if( ib2 == eb2 )
      ib2 = m.find( m.rbegin()->first );
    if( ib2 != eb2 )
    {
      const BucketMap<Void>::Bucket & bk = ib->second;
      const BucketMap<Void>::Bucket & bk2 = ib2->second;
      BucketMap<Void>::Bucket & bkout = (*out)[ ib->first ];

      if( intersect )
      {
        for( i=bk.begin(), e=bk.end(), e2=bk2.end(); i!=e; ++i )
          if( bk2.find( i->first ) != e2 )
            bkout[ i->first ] = Void();
      }
      else
        for( i=bk.begin(), e=bk.end(), e2=bk2.end(); i!=e; ++i )
          if( bk2.find( i->first ) == e2 )
            bkout[ i->first ] = Void();
    }
  }
  return out;
}


template BucketMap<Void>*
aims::maskWithVolume<int16_t>( const BucketMap<Void> & src,
                      const carto::rc_ptr<carto::Volume<int16_t> > m,
                      const int16_t & val, bool insideval );

