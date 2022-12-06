%ModuleHeaderCode
#include <aims/vector/vector.h>
#ifndef PYAIMS_AIMSVECTOR_FLOAT_3_DEFINED
#define PYAIMS_AIMSVECTOR_FLOAT_3_DEFINED
typedef AimsVector<float, 3>
  AimsVector_FLOAT_3;
#endif
#include <aims/roi/roiIterator.h>
namespace carto {
typedef carto::rc_ptr< aims::RoiIterator > rc_ptr_RoiIterator;
}
%End


namespace aims
{


  //-------------//
 //  FiberInfo  //
//-------------//

class FiberInfo
{
%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End

public:

  FiberInfo();
  FiberInfo( int id );
  int id() const;
};


  //--------------//
 //  BundleInfo  //
//--------------//

class BundleInfo
{
%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End

public:
  BundleInfo();
  BundleInfo( const std::string &name );
  BundleInfo( int id );
  BundleInfo( int id, const std::string &name );
  int id() const;
  std::string name() const;
};


  //--------------//
 //  FiberPoint  //
//--------------//

typedef Point3df FiberPoint;


  //------------------//
 //  BundleListener  //
//------------------//

class BundleListener
{
%Docstring
Serial processing of bundles.

BundleListener listens events from a :py:class:`BundleProducer` object, typically emitted when a new bundle starts or ends, a fiber starts or ends etc.

To use it, BundleListener has to be subclassed and some of the following methods overloaded:
* bundleStarted
* bundleTerminated
* fiberStarted
* fiberTerminated
* newFiberPoint
* noMoreBundle

To connect the listener to a producer, use :py:meth:`BundleProducer::addBundleListener`.

Inherited classes may inherit both BundleListener and BundleProducer, if they are part of a processing chain.
%End

%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
protected:

  virtual void bundleStarted( const aims::BundleProducer &, const aims::BundleInfo & );
%Docstring
bundleStarted(producer, bundle_info)
%End

  virtual void bundleTerminated( const aims::BundleProducer &, const aims::BundleInfo & );
%Docstring
bundleTerminated(producer, bundle_info)
%End

  virtual void fiberStarted( const aims::BundleProducer &,
                             const aims::BundleInfo &,
                             const aims::FiberInfo & );
%Docstring
fiberStarted(producer, bundle_info, fiber_info)
%End

  virtual void fiberTerminated( const aims::BundleProducer &, const aims::BundleInfo &,
                                const aims::FiberInfo & );
%Docstring
fiberTerminated(producer, bundle_info, fiber_info)
%End

  virtual void newFiberPoint( const aims::BundleProducer &, const aims::BundleInfo &,
                              const aims::FiberInfo &, const aims::FiberPoint & );
%Docstring
newFiberPoint(producer, bundle_info, fiber_info, point)
%End

  virtual void noMoreBundle( const aims::BundleProducer & );
%Docstring
noMoreBundle(producer)
%End

public:
  virtual ~BundleListener();
};


  //------------------//
 //  BundleProducer  //
//------------------//

class BundleProducer
{
%Docstring
Serial processing of bundles.

BundleProducer emits events to a :py:class:`BundleListener` object while walking through a bundles set structure (or a bundles file), typically when a new bundle starts or ends, a fiber starts or ends etc.

To connect the listener to a producer, use :py:meth:`BundleProducer::addBundleListener`.

Inherited classes may inherit both BundleListener and BundleProducer, if they are part of a processing chain.
%End

%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
public:

  void addBundleListener( aims::BundleListener & );
%Docstring
addBundleListener(listener)

Connects a :py:class:`BundleProducer` to a :py:class:`BunsdleListener`.
%End

protected:

  void startBundle( const aims::BundleInfo & );
  void terminateBundle( const aims::BundleInfo & );
  void startFiber( const aims::BundleInfo &, const aims::FiberInfo & );
  void terminateFiber( const aims::BundleInfo &, const aims::FiberInfo & );
  void addFiberPoint( const aims::BundleInfo &, const aims::FiberInfo &,
                      const aims::FiberPoint & );
  void noMoreBundle();
};


  //----------------//
 //  BundleWriter  //
//----------------//


class BundleWriter : aims::BundleListener, carto::PropertySet
{
%Docstring
Writes bundles information to a bundles file.

BundleWriter is a :py:class:`BundleListener`, thus must be connected to a :py:class:`BundleProducer`, and is fed by it.

It will write the ``.bundles`` format.
%End

%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
public:

  BundleWriter();
  void setFileString( const std::string & );
%Docstring
setFileString(filename)

Set the output file name.
%End

  virtual ~BundleWriter();

private:
  BundleWriter( const aims::BundleWriter & );
};


  //----------------//
 //  BundleReader  //
//----------------//

class BundleReader : aims::BundleProducer
{
%Docstring
Reads a bundles file, and emits events while walking through its data.

BundleReader is a :py:class:`BundleProducer`, so can be listened by any :py:class:`BundleListener`.
It may read several bundles/fibers formats, using a lower-level :py:class:`BundleProducer` dedicated to a specific format.

Currently ``.bundles`` (AIMS/Connectomist) and ``.trk`` (Trackvis) formats are supported.
%End

%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
public:

  BundleReader( const std::string & );
  virtual ~BundleReader();

  static set_STRING supportedFormats();
%Docstring
supportedFormats()

return a set of fiber tracts formats names supported by the BundlesReader.
%End

  static set_STRING formatExtensions( const std::string & format = "ALL" );
%Docstring
formatExtensions(format='ALL')

return a set of file extensions associated with the given format. If the format is "ALL" (default) then all extensions of all supported formats are returned. If the format does not exist, an empty set is returned, and no error is issued.
%End

  void read();
%Docstring
read()

read() is the entry point to fibers processing, and triggers the producer machinery.
%End
};


  //-----------------//
 //  BundleToGraph  //
//-----------------//

class BundleToGraph : aims::BundleListener,carto::PropertySet
{
%Docstring
Bundles structure building as a Graph

The :py:class:`Graph` structure is used to represent bundles and fibers when we need to keep their complete structure in memory. This structure is especially used for 3D rendering in Anatomist.

BundleToGraph is a :py:class:`BundleListener`, thus has to be connected to a :py:class:`BundleProducer` to work (typically, a :py:class:`BundleReader`).

Note that the BundleProducer / BundleListener system work as processing chains, and allows to keep only a small set of data in memory at one time. The Graph structure, on the contrary, keeps all information in memory, so may need a large amount of memory.
%End

%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
public:

  BundleToGraph();
  BundleToGraph( Graph & );
  virtual ~BundleToGraph();

//TODO:  const carto::rc_ptr< Graph > &getGraph() const;
};


  //-----------------------//
 //  BundleToGraphWriter  //
//-----------------------//

class BundleToGraphWriter : aims::BundleToGraph
{
%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
public:

  virtual ~BundleToGraphWriter();

  void setFileString( const std::string & );
};


  //------------------//
 //  BundleROISplit  //
//------------------//

class BundleROISplit : aims::BundleProducer, aims::BundleListener
{
%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
public:

  BundleROISplit( carto::rc_ptr_RoiIterator roiIterator,
                  bool keepOriginalBundle = false );
  virtual ~BundleROISplit();
};



  //-------------------//
 //  BundleROISelect  //
//-------------------//

class BundleROISelect : aims::BundleProducer, aims::BundleListener
{
%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
public:

  BundleROISelect( carto::rc_ptr_RoiIterator roiIterator,
                   const std::string &,
                   float defaultROIMinimumOverlap = 0 );
  virtual ~BundleROISelect();
};


  //----------------//
 //  BundleMotion  //
//----------------//

class BundleMotion : aims::BundleProducer, aims::BundleListener
{
%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
public:

  BundleMotion( const std::string & );
  BundleMotion( const Motion & );
  virtual ~BundleMotion();
};


  //------------------//
 //  CurveSelection  //
//------------------//

class CurveSelection : aims::BundleProducer, aims::BundleListener
{
%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
public:

  CurveSelection( double minimumLength );
  virtual ~CurveSelection();
};


  //------------------//
 // BundleSampler //
//------------------//

class BundleSampler : aims::BundleProducer, aims::BundleListener
{
%TypeHeaderCode
#include <aims/fibers/bundleSampler.h>
%End

public:
  BundleSampler(float percent, const std::string &bundle_name,
                const std::string &sampled_bundle_name, int mode);
  BundleSampler(float percent, const std::string &bundle_name,
                const std::string &sampled_bundle_name, int mode,
                bool discarded);
  virtual ~BundleSampler();
};


}; // namespace aims

