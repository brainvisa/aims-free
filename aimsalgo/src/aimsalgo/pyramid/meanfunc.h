
#ifndef AIMS_PYRAMID_MEANFUNC_H
#define AIMS_PYRAMID_MEANFUNC_H


#include <cartodata/volume/volume.h>
#include <aims/pyramid/pyr-func.h>

//
// class MeanPyramidFunc
//
template <class T>
class MeanPyramidFunc : public PyramidFunc<T>
{
  public:

    MeanPyramidFunc() : PyramidFunc<T>() { }
    virtual ~MeanPyramidFunc() { }

    T doit( carto::VolumeRef<T>& data ) const;
};


template <class T> inline
T MeanPyramidFunc<T>::doit( carto::VolumeRef<T>& data ) const
{
  float mean = 0;
  int dim = data.getSizeX();

  for ( int k = 0; k < dim; k++ )
    mean += float( data( k ) );

  return (T)( mean / dim );
}


#endif
