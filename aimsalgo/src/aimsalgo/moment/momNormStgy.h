
#ifndef AIMS_MOMENT_MOMNORMSTGY_H
#define AIMS_MOMENT_MOMNORMSTGY_H

#include <aims/moment/momStgy.h>


template< class T >
class MomentNormalStrategy : public MomentStrategy< T >
{
  public:
  
    MomentNormalStrategy() : MomentStrategy< T >() { }
    
    void doit( Moment< T > *, carto::rc_ptr<carto::Volume< T > > &, T, int );
    void doit( Moment< T > *, const aims::BucketMap<Void> &, int );

  private:
    void addVoxel2( Moment<T>*, const Point3d & p );
};


template< class T > inline
void MomentNormalStrategy< T >::addVoxel2( Moment< T > *m, const Point3d & p )
{
  double cx = m->cx();
  double cy = m->cy();
  double cz = m->cz();
  double mx = m->m1()[ 0 ] / m->m0();
  double my = m->m1()[ 1 ] / m->m0();
  double mz = m->m1()[ 2 ] / m->m0();

  double tx = (double)p[0] * cx - mx;
  double ty = (double)p[1] * cy - my;
  double tz = (double)p[2] * cz - mz;

  m->m2()[ 0 ] += tx * tx;
  m->m2()[ 1 ] += ty * ty;
  m->m2()[ 2 ] += tz * tz;
  m->m2()[ 3 ] += tx * ty;
  m->m2()[ 4 ] += tx * tz;
  m->m2()[ 5 ] += ty * tz;

  m->m3()[ 0 ] += tx * tx * tx;
  m->m3()[ 1 ] += ty * ty * ty;
  m->m3()[ 2 ] += tz * tz * tz;
  m->m3()[ 3 ] += tx * tx * ty;
  m->m3()[ 4 ] += tx * tx * tz;
  m->m3()[ 5 ] += tx * ty * ty;
  m->m3()[ 6 ] += ty * ty * tz;
  m->m3()[ 7 ] += tx * tz * tz;
  m->m3()[ 8 ] += ty * tz * tz;
  m->m3()[ 9 ] += tx * ty * tz;
}


template< class T > inline
void MomentNormalStrategy< T >::doit( Moment< T > *m,
                                      carto::rc_ptr<carto::Volume< T > > & d,
                                      T label, int )
{
  int x, y, z;
  int dx = d->getSizeX();
  int dy = d->getSizeY();
  int dz = d->getSizeZ();
  T *it;
  long o = d->getStrides()[0];

  double cx = m->cx();
  double cy = m->cy();
  double cz = m->cz();
  double ct = m->ct();

  for ( z=0; z<dz; z++ )
    for ( y=0; y<dy; y++ )
    {
      it = &d->at( 0, y, z );
      for ( x=0; x<dx; x++, it+=o )
        if ( *it == label )
        {
          m->sum()++;

          m->m0()++;

          m->m1()[ 0 ] += (double)x * cx;
          m->m1()[ 1 ] += (double)y * cy;
          m->m1()[ 2 ] += (double)z * cz;
        }
    }

  double mx = m->m1()[ 0 ] / m->m0();
  double my = m->m1()[ 1 ] / m->m0();
  double mz = m->m1()[ 2 ] / m->m0();
  
  m->gravity()[ 0 ] = mx;
  m->gravity()[ 1 ] = my;
  m->gravity()[ 2 ] = mz;

  for ( z=0; z<dz; z++ )
    for ( y=0; y<dy; y++ )
    {
      it = &d->at( 0, y, z );
      for ( x=0; x<dx; x++, it+=o )
        if ( *it == label )
          addVoxel2( m, Point3d( x, y, z ) );
    }

  m->m0() *= ct; // is is OK or should it have been done before ?

  for ( x=0; x<3; x++ ) m->m1()[ x ] *= ct;
  for ( x=0; x<6; x++ ) m->m2()[ x ] *= ct;
  for ( x=0; x<10; x++ ) m->m3()[ x ] *= ct;
}


template <typename T>
void MomentNormalStrategy< T >::doit( Moment< T > *m, 
                                      const aims::BucketMap<Void> & b, int )
{
  const aims::BucketMap<Void>::Bucket	& bk = b.begin()->second;
  aims::BucketMap<Void>::Bucket::const_iterator	ib, eb = bk.end();

  if( bk.size() == 0 )
    return;

  double cx = m->cx();
  double cy = m->cy();
  double cz = m->cz();
  double ct = m->ct();

  m->sum() += bk.size();
  m->m0() += bk.size();

  for( ib=bk.begin(); ib!=eb; ++ib )
    {
      m->m1()[ 0 ] += (double)ib->first[0] * cx;
      m->m1()[ 1 ] += (double)ib->first[1] * cy;
      m->m1()[ 2 ] += (double)ib->first[2] * cz;
    }
  
  double mx = m->m1()[ 0 ] / m->m0();
  double my = m->m1()[ 1 ] / m->m0();
  double mz = m->m1()[ 2 ] / m->m0();
  
  m->gravity()[ 0 ] = mx;
  m->gravity()[ 1 ] = my;
  m->gravity()[ 2 ] = mz;

  for( ib=bk.begin(); ib!=eb; ++ib )
    addVoxel2( m, ib->first );

  m->m0() *= ct;

  int	x;
  for ( x=0; x<3; x++ ) m->m1()[ x ] *= ct;
  for ( x=0; x<6; x++ ) m->m2()[ x ] *= ct;
  for ( x=0; x<10; x++ ) m->m3()[ x ] *= ct;
}

#endif
