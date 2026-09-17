// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/utility/boundingbox.h>
#include <aims/resampling/motion.h>

using namespace aims;

BoundingBox::BoundingBox( const Motion* motion )
  : _npoints( 0 ), _minimum( 0, 0, 0 ), _maximum( 0, 0, 0 ),
    _motion( motion ? new Motion( *motion ) : 0 )
{
}


BoundingBox::~BoundingBox()
{
  delete _motion;
}


void BoundingBox::reset()
{
  _npoints = 0;
  _minimum = Point3df( 0, 0, 0 );
  _maximum = Point3df( 0, 0, 0 );
}


void BoundingBox::setMotion( const Motion* motion )
{
  delete _motion;
  if( motion )
    _motion = new Motion( *motion );
  else
    _motion = 0;
}


void BoundingBox::add( const Point3df & pos )
{
  if( _npoints == 0 )
    {
      ++_npoints;
      if( _motion )
        {
          Point3df	mpos;
          mpos = _motion->transform( pos );
          _minimum = mpos;
          _maximum = mpos;
        }
      else
        {
          _minimum = pos;
          _maximum = pos;
        }
      return;
    }
  ++_npoints;
  if( _motion )
    {
      Point3df	mpos;
      mpos = _motion->transform( pos );
      if( mpos[0] < _minimum[0] )
        _minimum[0] = mpos[0];
      if( mpos[1] < _minimum[1] )
        _minimum[1] = mpos[1];
      if( pos[2] < _minimum[2] )
        _minimum[2] = mpos[2];
      if( mpos[0] > _maximum[0] )
        _maximum[0] = mpos[0];
      if( mpos[1] > _maximum[1] )
        _maximum[1] = mpos[1];
      if( mpos[2] > _maximum[2] )
        _maximum[2] = mpos[2];
    }
  else
    {
      if( pos[0] < _minimum[0] )
        _minimum[0] = pos[0];
      if( pos[1] < _minimum[1] )
        _minimum[1] = pos[1];
      if( pos[2] < _minimum[2] )
        _minimum[2] = pos[2];
      if( pos[0] > _maximum[0] )
        _maximum[0] = pos[0];
      if( pos[1] > _maximum[1] )
        _maximum[1] = pos[1];
      if( pos[2] > _maximum[2] )
        _maximum[2] = pos[2];
    }
}


template <typename T>
void BoundingBox::add( const BucketMap<T> & bck )
{
  if( bck.empty() )
    return;
  const typename BucketMap<T>::Bucket	& bk = bck.begin()->second;
  float	vx = bck.sizeX();
  float	vy = bck.sizeY();
  float	vz = bck.sizeZ();
  typename BucketMap<T>::Bucket::const_iterator	ib, eb = bk.end();

  for( ib=bk.begin(); ib!=eb; ++ib )
    add( Point3df( ib->first[0] * vx, ib->first[1] * vy, ib->first[2] * vz ) );
}


template void BoundingBox::add( const BucketMap<Void> & bck );
template void BoundingBox::add( const BucketMap<int16_t> & bck );
template void BoundingBox::add( const BucketMap<int32_t> & bck );


