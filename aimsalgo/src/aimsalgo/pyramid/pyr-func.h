
#ifndef AIMS_PYRAMID_PYR_FUNC_H
#define AIMS_PYRAMID_PYR_FUNC_H


namespace carto
{
  template <class T> class VolumeRef;
}

//
// class PyramidFunc
//
template <class T>
class PyramidFunc
{
  public:

    PyramidFunc() { }
    virtual ~PyramidFunc() { }

    virtual T doit( carto::VolumeRef<T>& data ) const = 0;
};

#endif


