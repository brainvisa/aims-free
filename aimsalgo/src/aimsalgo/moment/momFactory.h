
#ifndef AIMS_MOMENT_MOMFACTORY_H
#define AIMS_MOMENT_MOMFACTORY_H

#include <aims/moment/momBase.h>
#include <aims/moment/momNormStgy.h>
#include <aims/moment/momIncStgy.h>


template< class T >
class MomentFactory
{
  public:
  
    MomentFactory() { }
    virtual ~MomentFactory() { }
    
    MomentStrategy< T > *create( MomentBase::MomentType );
};


template< class T > inline
MomentStrategy< T > *MomentFactory< T >::create( MomentBase::MomentType id )
{
  switch( id )
    {
      case MomentBase::Normal:
        return new MomentNormalStrategy< T >();
	break;
      case MomentBase::Incremental:
        return new MomentIncrementalStrategy< T >();
	break;
      default:
        return (MomentStrategy< T > *)0;
	break;
    }
}

#endif
