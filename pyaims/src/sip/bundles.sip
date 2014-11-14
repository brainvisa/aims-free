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
%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
protected:

  virtual void bundleStarted( const aims::BundleProducer &, const aims::BundleInfo & ) /AutoGen/;
  virtual void bundleTerminated( const aims::BundleProducer &, const aims::BundleInfo & ) /AutoGen/;
  virtual void fiberStarted( const aims::BundleProducer &, const aims::BundleInfo &,
                             const aims::FiberInfo & ) /AutoGen/;
  virtual void fiberTerminated( const aims::BundleProducer &, const aims::BundleInfo &,
                                const aims::FiberInfo & ) /AutoGen/;
  virtual void newFiberPoint( const aims::BundleProducer &, const aims::BundleInfo &,
                              const aims::FiberInfo &, const aims::FiberPoint & ) /AutoGen/;
  virtual void noMoreBundle( const aims::BundleProducer & ) /AutoGen/;

public:
  virtual ~BundleListener();
};


  //------------------//
 //  BundleProducer  //
//------------------//

class BundleProducer
{
%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
public:

  void addBundleListener( aims::BundleListener & );

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
%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
public:

  BundleWriter();
  void setFileString( const std::string & );
  virtual ~BundleWriter();

private:
  BundleWriter( const aims::BundleWriter & );
};


  //----------------//
 //  BundleReader  //
//----------------//

class BundleReader : aims::BundleProducer
{
%TypeHeaderCode
#include <aims/fibers/bundles.h>
%End
public:

  BundleReader( const std::string & );
  virtual ~BundleReader();

  void read();
};


  //-----------------//
 //  BundleToGraph  //
//-----------------//

class BundleToGraph : aims::BundleListener,carto::PropertySet
{
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


}; // namespace aims