
#ifndef AIMS_MOMENT_GEOMMOMENT_H
#define AIMS_MOMENT_GEOMMOMENT_H

#include <aims/vector/vector.h>

#include <aims/moment/moment.h>
#include <aims/moment/momBase.h>
#include <aims/moment/momStgy.h>
#include <aims/moment/momFactory.h>


template< class T >
class GeometricMoment : public MomentBase, public Moment< T >
{
  public:
  
    GeometricMoment( MomentType mType=Incremental );

    void setMomentType( MomentType );
  
    void update( double, double, double, int dir=(int)Moment< T >::mAdd );

    // ignore this warning (the API is broken, see comment in Moment<T>)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Woverloaded-virtual"
    virtual void doit( carto::rc_ptr<carto::Volume< T > > &, T,
                       int d=(int)Moment< T >::mAdd );
    virtual void doit( const aims::BucketMap<Void> &, int d=(int)Moment< T >::mAdd );
    #pragma GCC diagnostic pop

    
  private:
  
    MomentStrategy< T > *stgy;
};


template< class T > inline
GeometricMoment< T >::GeometricMoment( MomentType type )
{
  stgy = 0;
  setMomentType( type );
}


template< class T > inline
void GeometricMoment< T >::setMomentType( MomentType type )
{
  delete stgy;
  
  MomentFactory< T > factory;
  stgy = factory.create( type );
}


template< class T > inline
void GeometricMoment< T >::update( double x, double y, double z, int dir )
{
  stgy->update( (Moment< T > *)this, x, y, z, dir );
  Moment< T >::orientation();
}


template< class T > inline
void GeometricMoment< T >::doit( carto::rc_ptr<carto::Volume< T > > & d,
                                 T label, int dir )
{
  int i;

  this->_cx = (double)d->getVoxelSize()[0];
  this->_cy = (double)d->getVoxelSize()[1];
  this->_cz = (double)d->getVoxelSize()[2];
  this->_ct = this->_cx * this->_cy * this->_cz;

  this->_sum = 0.0;
  this->_m0 = 0.0;

  for ( i=0; i<3; i++ )  this->_m1[ i ] = 0.0;
  for ( i=0; i<6; i++ )  this->_m2[ i ] = 0.0;
  for ( i=0; i<10; i++ )  this->_m3[ i ] = 0.0;

  stgy->doit( (Moment< T > *)this, d, label, dir );
  Moment< T >::orientation();
}


template <typename T> inline 
void GeometricMoment< T >::doit( const aims::BucketMap<Void> & b, 
                                        int dir )
{
  int i;

  this->_cx = (double)b.sizeX();
  this->_cy = (double)b.sizeY();
  this->_cz = (double)b.sizeZ();
  this->_ct = this->_cx * this->_cy * this->_cz;

  this->_sum = 0.0;
  this->_m0 = 0.0;

  for ( i=0; i<3; i++ )  this->_m1[ i ] = 0.0;
  for ( i=0; i<6; i++ )  this->_m2[ i ] = 0.0;
  for ( i=0; i<10; i++ )  this->_m3[ i ] = 0.0;

  stgy->doit( (Moment< T > *)this, b, dir );
  Moment< T >::orientation();
}

#endif
