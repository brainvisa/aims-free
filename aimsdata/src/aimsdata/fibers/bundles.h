/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef AIMS_FIBERS_BUNDLES_H
#define AIMS_FIBERS_BUNDLES_H

#include <aims/curve/curve.h>
#include <aims/mesh/surface.h>
#include <aims/vector/vector.h>
#include <aims/resampling/motion.h>
#include <aims/io/motionR.h>
#include <aims/roi/roiIterator.h>
#include <graph/graph/graph.h>
#include <cartobase/smart/rcptr.h>
#include <cartobase/object/object.h>
#include <cartobase/object/property.h>
#include <fstream> 
#include <vector>
#include <map>
#include <string>


namespace aims
{


  //----------//
 //  Bundle  //
//----------//

class Bundle : public std::vector< aims::Polyline >
{
};


  //-----------//
 //  Bundles  //
//-----------//

class Bundles : public std::map< std::string, Bundle >
{
};


  //------------------------------------------------//
 //  Froward classes, smart pointers and typedefs  //
//------------------------------------------------//

class Bundle;
class BundleListener;
class BundleProducer;

typedef carto::rc_ptr<Bundle> RBundle;
typedef carto::rc_ptr<BundleListener> RBundleListener;
typedef carto::rc_ptr<BundleProducer> RBundleProducer;

typedef aims::Polyline::Point_t FiberPoint;


  //-------------//
 //  FiberInfo  //
//-------------//

class FiberInfo
{
public:
  
  inline FiberInfo();
  inline FiberInfo( int id );
  inline int id() const;

protected:

  int _id;
};


  //--------------//
 //  BundleInfo  //
//--------------//

class BundleInfo
{
public:
  
  inline BundleInfo();
  inline BundleInfo( const std::string &name );
  inline BundleInfo( int id );
  inline BundleInfo( int id, const std::string &name );
  inline int id() const;
  inline std::string name() const;

protected:

  int _id;
  std::string _name;
};


  //------------------//
 //  BundleListener  //
//------------------//

/** Serial processing of bundles.

    BundleListener listens events from a BundleProducer object, typically emitted when a new bundle starts or ends, a fiber starts or ends etc.

    To use it, BundleListener has to be subclassed and some of the following methods overloaded:
    - bundleStarted
    - bundleTerminated
    - fiberStarted
    - fiberTerminated
    - newFiberPoint
    - noMoreBundle

    To connect the listener to a producer, use BundleProducer::addBundleListener.

    Inherited classes may inherit both BundleListener and BundleProducer, if they are part of a processing chain.
*/
class BundleListener
{
protected:

  friend class BundleProducer;

  virtual void bundleStarted( const BundleProducer &, const BundleInfo & ) ;
  virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
  virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                             const FiberInfo & );
  virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                                const FiberInfo & );
  /** This version of fiberTerminated() is obsolete, and does not get called
      any longer by callbacks. Do not use it in your codes.
  */
  virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                                const FiberInfo &,  FiberPoint *,int & );
  virtual void newFiberPoint( const BundleProducer &, const BundleInfo &,
                              const FiberInfo &, const FiberPoint & );
  virtual void noMoreBundle( const BundleProducer & );

public:
  virtual ~BundleListener();
};


  //------------------//
 //  BundleProducer  //
//------------------//

/** Serial processing of bundles.

    BundleProducer emits events to a BundleListener object while walking through a bundles set structure (or a bundles file), typically when a new bundle starts or ends, a fiber starts or ends etc.

    To connect the listener to a producer, use BundleProducer::addBundleListener.

    Inherited classes may inherit both BundleListener and BundleProducer, if they are part of a processing chain.
*/
class BundleProducer
{
public:

  void addBundleListener( BundleListener & );

protected:
  
  void startBundle( const BundleInfo & );
  void terminateBundle( const BundleInfo & );
  void startFiber( const BundleInfo &, const FiberInfo & );
  void terminateFiber( const BundleInfo &, const FiberInfo & );
  /** This version of fiberTerminated() is obsolete.
      Do not use it in your codes.
  */
  void terminateFiber( const BundleInfo &, const FiberInfo &,  FiberPoint* ,int &);
  void addFiberPoint( const BundleInfo &, const FiberInfo &,
                      const FiberPoint & );
  void noMoreBundle();

private:
  
  typedef std::vector< BundleListener * > BundleListenerList;
  BundleListenerList _bundleListeners;
};



  //----------------//
 //  BundleWriter  //
//----------------//

// WARNING : This class should be a real writer in aimsdata but I prefer to
//           keep it here until the format is a bit more stable.

/** Writes bundles information to a bundles file.

    BundleWriter is a BundleListener, thus must be connected to a BundleProducer, and is fed by it.

    It will write the .bundles format.
*/
class BundleWriter : public virtual BundleListener, public carto::PropertySet
{
public:

  BundleWriter();
  /// set the output file name
  void setFileString( const std::string &fileName );
  virtual ~BundleWriter();

  typedef std::vector< FiberPoint > FiberPoints;

  virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
  virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
  virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                     const FiberInfo & );
  virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                        const FiberInfo & );
  virtual void newFiberPoint( const BundleProducer &, const BundleInfo &,
                      const FiberInfo &, const FiberPoint & );
  virtual void noMoreBundle( const BundleProducer & );
  virtual void checkStreams();
  virtual void addFiber( const BundleProducer &, const BundleInfo &,
                         const FiberInfo &,  FiberPoint*, int &);

protected:
  int _ascii;
  std::string _fileName;
  carto::Object _header;
  std::ofstream _dataFile;
  int _firstFiberInBundle;
  int _writtenFibersCount;
  FiberPoints _fiberPoints;
};



  //----------------//
 //  BundleReader  //
//----------------//

// WARNING : This class should be a real writer in aimsdata but I prefer to
//           keep it here until the format is a bit more stable.

/** Reads a bundles file, and emits events while walking through its data.

    BundleReader is a BundleProducer, so can be listened by any BundleListener.
    It may read several bundles/fibers formats, using a lower-level BundleProducer dedicated to a specific format.

    Currently .bundles (AIMS/Connectomist) and .trk (Trackvis) formats are supported.
*/
class BundleReader : public BundleProducer, public BundleListener
{
public:

  BundleReader( const std::string &fileName );
  virtual ~BundleReader();

  /** read() is the entry point to fibers processing, and triggers the producer machinery.
  */
  void read();
  virtual carto::Object readHeader();

protected:

  virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
  virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
  virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                     const FiberInfo & );
  virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                        const FiberInfo & );
  virtual void newFiberPoint( const BundleProducer &, const BundleInfo &,
                      const FiberInfo &, const FiberPoint & );
  virtual void noMoreBundle( const BundleProducer & );

private:

  std::string _fileName;
};


  //----------------------//
 //  BundleFormatReader  //
//----------------------//

// WARNING : This class should be a real writer in aimsdata but I prefer to
//           keep it here until the format is a bit more stable.

/** Virtual base class for bundles/fibers file formats support
*/
class BundleFormatReader : public BundleProducer
{
public:

  BundleFormatReader();
  virtual ~BundleFormatReader();

  virtual void read() = 0;
  virtual carto::Object readHeader();

private:

  std::string _fileName;
};


/** Connectomist .bundles format reader

    This is a low-level reader class, normally only used through BundleReader. You should not need to use it directly.
*/
class ConnectomistBundlesReader : public BundleFormatReader
{
public:

  ConnectomistBundlesReader( const std::string &fileName );
  virtual ~ConnectomistBundlesReader();

  virtual void read();
  virtual carto::Object readHeader();

private:

  std::string _fileName;
};


  //-----------------//
 //  BundleToGraph  //
//-----------------//

/** Bundles structure building as a Graph

    The Graph structure is used to represent bundles and fibers when we need to keep their complete structure in memory. This structure is especially used for 3D rendering in Anatomist.

    BundleToGraph is a BundleListener, thus has to be connected to a BundleProducer to work (typically, a BundleReader).

    Note that the BundleProducer / BundleListener system work as processing chains, and allows to keep only a small set of data in memory at one time. The Graph structure, on the contrary, keeps all information in memory, so may need a large amount of memory.
*/
class BundleToGraph : public BundleListener, 
                      virtual public carto::PropertySet
{
public:

  BundleToGraph();
  BundleToGraph( Graph & );
  virtual ~BundleToGraph();

  inline const carto::rc_ptr< Graph > &getGraph() const;

protected:

  virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
  virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
  virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                     const FiberInfo & );
  virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                        const FiberInfo & );
  virtual void newFiberPoint( const BundleProducer &, const BundleInfo &,
                      const FiberInfo &, const FiberPoint & );
  virtual void noMoreBundle( const BundleProducer & );
  virtual void parametersValueChanged();
  void parameterChangedSlot( const carto::Object &, const std::string &,
                             const carto::Object & );
  void shuffleBundle();

  Graph *_meshResult;
  carto::rc_ptr< Graph > _rcptrMeshResult;
  float _minimumLength;
  float _width;
  int _numPointsInMeshSection;
  std::vector< Point2df > _section;
  std::vector<int>	_boundingbox_min;
  std::vector<int>	_boundingbox_max;
  bool _normalsAsFiberStartPos;
  Point3df _relatveFiberStartPos;
  bool _autoFiberStartPos;

  Vertex *_currentVertex;
  aims::Polyline _currentCurve;
  carto::rc_ptr< AimsSurfaceTriangle > _currentMesh;
  carto::rc_ptr< AimsTimeSurface<2,Void> > _currentLines;
  size_t _current_fibers_count;
};



  //-----------------------//
 //  BundleToGraphWriter  //
//-----------------------//

class BundleToGraphWriter : public BundleToGraph
{
public:

  inline virtual ~BundleToGraphWriter();

  void setFileString( const std::string & );

protected:
  
  virtual void noMoreBundle( const BundleProducer & );

private:
  
  std::string _fileName;
};


  //------------------//
 //  BundleROISplit  //
//------------------//

class BundleROISplit : public BundleProducer, public BundleListener
{
public:

  BundleROISplit( carto::rc_ptr< aims::RoiIterator > roiIterator,
                  bool keepOriginalBundle = false );
  virtual ~BundleROISplit();

protected:

  virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
  virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
  virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                     const FiberInfo & );
  virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                        const FiberInfo & );
  virtual void newFiberPoint( const BundleProducer &, const BundleInfo &,
                      const FiberInfo &, const FiberPoint & );
  virtual void noMoreBundle( const BundleProducer & );

private:

  carto::rc_ptr< aims::ROILabelMap > _roiLabelMap;

  typedef std::vector< FiberPoint > FiberPoints;
  typedef std::vector< FiberPoints > Fibers;

  std::map< std::string, std::map< std::pair<int,int>, Fibers > > 
    _result;

  int _currentLabel;
  int _lastLabel;
  FiberPoints _currentPoints;
  bool _keepOriginalBundle;
};



  //-------------------//
 //  BundleROISelect  //
//-------------------//

class BundleROISelect : public BundleProducer, public BundleListener
{
public:

  BundleROISelect( carto::rc_ptr< aims::RoiIterator > roiIterator,
                   const std::string &,
                   float defaultROIMinimumOverlap = 0 );
  virtual ~BundleROISelect();

protected:

  virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
  virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
  virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                     const FiberInfo & );
  virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                        const FiberInfo & );
  virtual void newFiberPoint( const BundleProducer &, const BundleInfo &,
                      const FiberInfo &, const FiberPoint & );
  virtual void noMoreBundle( const BundleProducer & );

private:

  carto::rc_ptr< aims::ROILabelMap > _roiLabelMap;

  typedef std::vector< FiberPoint > FiberPoints;

  std::vector< uint32_t > _crossedROI;
  FiberPoints _currentPoints;
  std::map< std::string, std::vector< FiberPoints > > _result;

  struct SelectionRule
  {
    std::string name;
    uint32_t min, max;
    std::vector< bool > mustBeCrossed;
  };
  std::vector< SelectionRule > _selectionRules;
  float _defaultROIMinimumOverlap;
};



  //----------------//
 //  BundleMotion  //
//----------------//

class BundleMotion : public BundleProducer, public BundleListener
{
public:

  BundleMotion( const std::string & );
  BundleMotion( const Motion & );
  virtual ~BundleMotion();

protected:

  virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
  virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
  virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                     const FiberInfo & );
  virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                        const FiberInfo & );
  virtual void newFiberPoint( const BundleProducer &, const BundleInfo &,
                      const FiberInfo &, const FiberPoint & );
  virtual void noMoreBundle( const BundleProducer & );

private:

  Motion _motion;
};


  //------------------//
 //  CurveSelection  //
//------------------//

class CurveSelection : public BundleProducer, public BundleListener
{
public:

  CurveSelection( const double &minimumLength );
  virtual ~CurveSelection();

protected:

  typedef std::vector< FiberPoint > FiberPoints;

  virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
  virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
  virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                     const FiberInfo & );
  virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                        const FiberInfo & );
  virtual void newFiberPoint( const BundleProducer &, const BundleInfo &,
                      const FiberInfo &, const FiberPoint & );
  virtual void noMoreBundle( const BundleProducer & );

private:

  double _minimumLength;
  double _currentLength;
  FiberPoints _currentPoints;
};


  //--------------------//
 //  CurveOrientation  //
//--------------------//

class CurveOrientation : public BundleProducer, public BundleListener
{
public:

  CurveOrientation();
  virtual ~CurveOrientation();

protected:

  typedef std::vector< FiberPoint > FiberPoints;

  virtual void bundleStarted( const BundleProducer &, const BundleInfo & );
  virtual void bundleTerminated( const BundleProducer &, const BundleInfo & );
  virtual void fiberStarted( const BundleProducer &, const BundleInfo &,
                     const FiberInfo & );
  virtual void fiberTerminated( const BundleProducer &, const BundleInfo &,
                        const FiberInfo & );
  virtual void newFiberPoint( const BundleProducer &, const BundleInfo &,
                      const FiberInfo &, const FiberPoint & );
  virtual void noMoreBundle( const BundleProducer & );

private:

  FiberPoint _beginSum;
  aims::Polyline::Scalar_t _count;
  FiberPoints _currentPoints;
};




  //-------------//
 //  FiberInfo  //
//-------------//

//-----------------------------------------------------------------------------
inline FiberInfo::FiberInfo()
{}


//-----------------------------------------------------------------------------
inline FiberInfo::FiberInfo( int id ) :
  _id( id ) 
{}


//-----------------------------------------------------------------------------
inline int FiberInfo::id() const 
{ 
  return _id; 
}



  //--------------//
 //  BundleInfo  //
//--------------//

//-----------------------------------------------------------------------------
inline BundleInfo::BundleInfo()
{}


//-----------------------------------------------------------------------------
inline BundleInfo::BundleInfo( const std::string &name ) :
  _name( name )
{}


//-----------------------------------------------------------------------------
inline BundleInfo::BundleInfo( int id ) :
  _id( id ) 
{}


//-----------------------------------------------------------------------------
inline BundleInfo::BundleInfo( int id, const std::string &name ) :
  _id( id ), _name( name ) 
{}


//-----------------------------------------------------------------------------
inline std::string BundleInfo::name() const 
{ 
  return _name; 
}


//-----------------------------------------------------------------------------
inline int BundleInfo::id() const 
{ 
  return _id; 
}



  //-----------------//
 //  BundleToGraph  //
//-----------------//

//-----------------------------------------------------------------------------
inline const carto::rc_ptr< Graph > &BundleToGraph::getGraph() const
{
  return _rcptrMeshResult;
}


  //-----------------------//
 //  BundleToGraphWriter  //
//-----------------------//

//-----------------------------------------------------------------------------
inline BundleToGraphWriter::~BundleToGraphWriter()
{
}

} // namespace comist

#endif // ifndef AIMS_FIBERS_BUNDLES_H
