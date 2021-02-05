#include <aims/fibers/fiberOversampler.h>

using namespace std;
using namespace carto;

namespace aims
{

void FiberOversampler::bundleStarted ( const BundleProducer &,
                                  const BundleInfo &  bundleInfo)
{
  BundleProducer::startBundle( bundleInfo );
}


void FiberOversampler::bundleTerminated( const BundleProducer &,
                                    const BundleInfo & bundleInfo)
{
  BundleProducer::terminateBundle( bundleInfo );
}


void FiberOversampler::fiberStarted( const BundleProducer &,
                                const BundleInfo &,
                                const FiberInfo & )
{
  _fiberPoints.clear();
}


void FiberOversampler::newFiberPoint( const BundleProducer &,
                                 const BundleInfo &,
                                 const FiberInfo &,
                                 const FiberPoint & fiberPoint)
{

  _fiberPoints.push_back(fiberPoint);
}


void FiberOversampler::fiberTerminated( const BundleProducer & /* producer */,
                               const BundleInfo & bundleInfo, const FiberInfo & fiberInfo)
{

  BundleProducer::startFiber( bundleInfo, fiberInfo );

  for (int i = 0; i < (int)_fiberPoints.size() - 1; i++)
  {
    BundleProducer::addFiberPoint(bundleInfo, fiberInfo, _fiberPoints[i]);
    float currentFact = _fact;
    for (int j = 0; j < _points; j++)
    {
      _p = interp(_fiberPoints[i], _fiberPoints[i + 1], currentFact);
      BundleProducer::addFiberPoint(bundleInfo, fiberInfo, _p);
      currentFact += _fact;
    }
  }

  BundleProducer::addFiberPoint(bundleInfo, fiberInfo, _fiberPoints[_fiberPoints.size() - 1]);

  BundleProducer::terminateFiber( bundleInfo, fiberInfo );

}

void FiberOversampler::noMoreBundle( const BundleProducer &)
{
  BundleProducer::noMoreBundle();
}

Point3df& FiberOversampler::interp(Point3df p1, Point3df p2, float fact)
{
  _p = (p2-p1)*fact + p1;
  return _p;
}

} // namespace aims
