#ifndef AIMS_FIBERS_FIBEROVERSAMPLERBUNDLELISTENER_H
#define AIMS_FIBERS_FIBEROVERSAMPLERBUNDLELISTENER_H

#include <aims/fibers/bundles.h>


namespace aims
{

class FiberOversampler
  : public aims::BundleListener, public aims::BundleProducer
{
   public:

      FiberOversampler(int points);

      virtual void bundleStarted( const aims::BundleProducer &,
                                  const aims::BundleInfo & );
      virtual void bundleTerminated( const aims::BundleProducer &,
                                     const aims::BundleInfo & );
      virtual void fiberStarted( const aims::BundleProducer &,
                                 const aims::BundleInfo &,
                                 const aims::FiberInfo & );
      virtual void fiberTerminated( const aims::BundleProducer &,
                                    const aims::BundleInfo &,
                                    const aims::FiberInfo & );
      virtual void newFiberPoint( const aims::BundleProducer &,
                                  const aims::BundleInfo &,
                                  const aims::FiberInfo &,
                                  const aims::FiberPoint & );
      virtual void noMoreBundle( const aims::BundleProducer & );

      virtual ~FiberOversampler();

   protected:

    int _points;
    std::vector<Point3df> _fiberPoints;
    Point3df& interp(Point3df p1, Point3df p2, float fact);
    float _fact;
    Point3df _p;
};

inline FiberOversampler::~FiberOversampler()
{
}

inline FiberOversampler::FiberOversampler(int points): _points(points)

{
  _fact = 1.0 / (_points + 1 );
}



} // namespace aims

#endif // ifndef AIMS_FIBERS_FIBEROVERSAMPLERBUNDLELISTENER_H
