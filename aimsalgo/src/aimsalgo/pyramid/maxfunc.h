
#ifndef AIMS_PYRAMID_MAXFUNC_H
#define AIMS_PYRAMID_MAXFUNC_H


#include <cartodata/volume/volume.h>
#include <aims/pyramid/pyr-func.h>

//
// class MaxPyramidFunc
//
template <class T>
class MaxPyramidFunc : public PyramidFunc<T>
{
  public:

    MaxPyramidFunc() : PyramidFunc<T>() { }
    virtual ~MaxPyramidFunc() { }

    T doit( carto::VolumeRef<T>& data ) const;
};


template <class T> inline
T MaxPyramidFunc<T>::doit( carto::VolumeRef<T>& data ) const
{
  return data.max();
}


#endif


