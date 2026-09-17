
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/distancemap/downpath.h>
#include <aims/connectivity/connectivity.h>


namespace aims
{

template <typename T>
BucketMap<Void>*
downPath( const BucketMap<T> & region, const Point3d & pos )
{
  Point3d p = pos, q, p1;
  BucketMap<Void> *bck = new BucketMap<Void>;
  bck->setSizeXYZT( region.sizeX(), region.sizeY(), region.sizeZ(),
                    region.sizeT() );
  BucketMap<Void>::Bucket & bk = (*bck)[0];
  const typename BucketMap<T>::Bucket & r0 = region.begin()->second;
  typename BucketMap<T>::Bucket::const_iterator ir, er = r0.end();
  Connectivity c( 0, 0, Connectivity::CONNECTIVITY_26_XYZ );
  int i, n = c.nbNeighbors();
  T val, tmpval;
  ir = r0.find( p );
  if( ir == er )
  {
    // cout << "downPath beginning out of bucket\n";
    val = 1e38;
  }
  else
  {
    val = ir->second;
    bk[ p ];
  }
  // cout << "p0: " << p << ", val0:" << val << endl;
  bool moved;

  do
  {
    tmpval = val;
    moved = false;
    for( i=0; i<n; ++i )
    {
      q = p + c.xyzOffset( i );
      ir = r0.find( q );
      if( ir != er && ir->second < tmpval )
      {
        p1 = q;
        tmpval = ir->second;
        moved = true;
      }
    }
    if( moved )
    {
      val = tmpval;
      p = p1;
      bk[ p ];
    }
  }
  while( moved );

  return bck;
}

// ---

template
BucketMap<Void>* downPath( const BucketMap<float> & region,
                           const Point3d & pos );

}

