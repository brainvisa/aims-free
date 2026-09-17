
#ifndef AIMS_PYRAMID_MEDFUNC_H
#define AIMS_PYRAMID_MEDFUNC_H


#include <cartodata/volume/volumeutil.h>
#include <aims/pyramid/pyr-func.h>

//
// class MedianPyramidFunc
//
template <class T>
class MedianPyramidFunc : public PyramidFunc<T>
{
  public:

    MedianPyramidFunc() : PyramidFunc<T>() { }
    virtual ~MedianPyramidFunc() { }

    T doit( carto::VolumeRef<T>& data ) const;
};


template <class T> inline
T MedianPyramidFunc<T>::doit( carto::VolumeRef<T>& data ) const
{
  sort( data );
  
  return data( data.getSizeX() / 2 );
}



#endif


