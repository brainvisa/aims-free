namespace aims {

//---------------------------------------------------------------------------
class MaskIterator : carto::RCObject /Abstract/
{
%TypeHeaderCode
#include <aims/roi/maskIterator.h>
%End

%TypeCode
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy, sipClass_aims_MaskIterator,
    sipTransferObj, sipIsErr, sipCppPtr );
%End

public:

  virtual ~MaskIterator();

  virtual const Point3d value() const = 0;
  virtual const Point3df valueMillimeters() const = 0;

  virtual bool isValid() const = 0;
  virtual void next() = 0;
  virtual void restart() = 0;

  virtual const Point3df voxelSize() const = 0;
  virtual bool contains( const Point3d & ) const = 0;
  virtual bool contains( const Point3df & ) const = 0;
  virtual const Point3d volumeDimension() const = 0;
  std::string regionName() const = 0;
};

}; // namesapce aims

namespace carto {

//---------------------------------------------------------------------------
class rc_ptr_MaskIterator
{
%TypeHeaderCode
#include <aims/roi/maskIterator.h>
namespace carto {
typedef carto::rc_ptr< aims::MaskIterator > rc_ptr_MaskIterator;
}
%End

public:
  rc_ptr_MaskIterator();
  rc_ptr_MaskIterator( aims::MaskIterator * /Transfer/ );
  void reset( aims::MaskIterator * /Transfer/ );
  void release();
  bool isNull() const;
%MethodCode
  sipRes = !sipCpp->get();
%End

  aims::MaskIterator *_get();
%MethodCode
  sipRes = sipCpp->get();
  if( sipRes )
    {
#if SIP_VERSION >= 0x040400
      PyObject *po = 
        sipConvertFromInstance( sipCpp->get(), sipClass_aims_MaskIterator, 0 );
#else
      PyObject *po = sipMapCppToSelfSubClass( sipCpp->get(),
                                      sipClass_aims_MaskIterator );
#endif
      PyObject_SetAttrString( po, "_rc", sipSelf );
    }
  else
    {
      sipIsErr = 1;
      PyErr_SetString( PyExc_TypeError, "Null rc_ptr used" );
    }
%End

};

}; // namesapce carto

namespace aims {

//---------------------------------------------------------------------------
class RoiIterator : carto::RCObject /Abstract/
{
%TypeHeaderCode
#include <aims/roi/roiIterator.h>
%End

%TypeCode
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy, sipClass_aims_RoiIterator,
    sipTransferObj, sipIsErr, sipCppPtr );
%End

public:

  virtual ~RoiIterator();
  virtual bool isValid() const = 0;
  virtual void next() = 0;
  virtual void restart() = 0;
  virtual unsigned count() const = 0;
  virtual std::string regionName() const = 0;
  virtual carto::rc_ptr_MaskIterator maskIterator() const = 0;
};

}; // namesapce aims

namespace carto {

//---------------------------------------------------------------------------
class rc_ptr_RoiIterator
{
%TypeHeaderCode
#include <aims/roi/roiIterator.h>
namespace carto {
typedef carto::rc_ptr< aims::RoiIterator > rc_ptr_RoiIterator;
}
%End

public:
  rc_ptr_RoiIterator();
  rc_ptr_RoiIterator( aims::RoiIterator * /Transfer/ );
  void reset( aims::RoiIterator * /Transfer/ );
  void release();
  bool isNull() const;
%MethodCode
  sipRes = !sipCpp->get();
%End

  aims::RoiIterator *_get();
%MethodCode
  sipRes = sipCpp->get();
  if( sipRes ) {
#if SIP_VERSION >= 0x040400
    PyObject *po = 
      sipConvertFromInstance( sipCpp->get(), sipClass_aims_RoiIterator, 0 );
#else
    PyObject *po = sipMapCppToSelfSubClass( sipCpp->get(),
                                    sipClass_aims_RoiIterator );
#endif
    PyObject_SetAttrString( po, "_rc", sipSelf );
  } else {
    sipIsErr = 1;
    PyErr_SetString( PyExc_TypeError, "Null rc_ptr used" );
  }
%End

};


}; // namespace carto


namespace aims {

//---------------------------------------------------------------------------
carto::rc_ptr_RoiIterator getRoiIterator( std::string & );
carto::rc_ptr_RoiIterator getRoiIterator( const Graph &data );
carto::rc_ptr_RoiIterator getRoiIterator( const AimsData_S16 &data );

carto::rc_ptr_MaskIterator getMaskIterator( std::string & );
}; // namespace aims



  //------------------------------------//
 //  cartodata/wip/roi/siteIterator.h  //
//------------------------------------//


namespace carto {


//-----------------------------------------------------------------------------
class Site /Abstract/
{
%TypeHeaderCode
#include <cartodata/wip/roi/siteIterator.h>
%End

%TypeCode
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy, sipClass_carto_Site,
    sipTransferObj, sipIsErr, sipCppPtr );
%End

public:

  virtual ~Site();

  virtual int size() const = 0;
  virtual double at( int ) const = 0;
  virtual void set( int, double ) = 0;
  
  bool operator == ( const carto::Site & );
  bool operator != ( const carto::Site & );
}; // Site


//-----------------------------------------------------------------------------
class SiteIterator /Abstract/
{

%TypeHeaderCode
#include <cartodata/wip/roi/siteIterator.h>
namespace carto {
typedef carto::rc_ptr< carto::Site > rc_ptr_Site;
}
%End

%TypeCode
#include <pyaims/object/rcptrconv.h>
%End

%ConvertToTypeCode
  return pyaims::standardConvertToTypeCode( sipPy, sipClass_carto_SiteIterator,
    sipTransferObj, sipIsErr, sipCppPtr );
%End


public:
  virtual ~SiteIterator();
  virtual void next() = 0;
  virtual void restart() = 0;
  virtual bool isValid() const = 0;
  virtual rc_ptr_Site getSite() = 0;
  virtual void writeSite() = 0;
};


  //---------------------------//
 //  cartodata/wip/roi/roi.h  //
//---------------------------//


//-----------------------------------------------------------------------------
class Roi : carto::Headered /Abstract/
{
%TypeHeaderCode
#include <cartodata/wip/roi/roi.h>
%End

  virtual ~Roi();
};


//-----------------------------------------------------------------------------
class DiscreteRoi: carto::Roi /Abstract/
{
%TypeHeaderCode
#include <cartodata/wip/roi/roi.h>
namespace carto {
typedef carto::rc_ptr< carto::SiteIterator > rc_ptr_SiteIterator;
}
%End

public:

  virtual ~DiscreteRoi();

  virtual rc_ptr_SiteIterator getSiteIterator() const = 0;
  bool holds( const carto::Site  & );
  int count() const;

};


//-----------------------------------------------------------------------------
class ContinuousRoi: carto::Roi /Abstract/
{
%TypeHeaderCode
#include <cartodata/wip/roi/roi.h>
%End

public:

  virtual ~ContinuousRoi();
  virtual bool contains( const carto::Site & ) const = 0;
};

}; // namespace carto


namespace aims
{

  class RoiDiff
  {
  %TypeHeaderCode
  #include <aims/roi/roi_diff.h>
  %End

  public:
    struct DiffStat
    {
      DiffStat();
      /// nb of voxels from graph1 matching in graph2
      unsigned matching_voxels;
      /// nb of voxels from graph1 not matching in graph2
      unsigned unmatching_voxels;
      double dice;
      /// voxels from graph2
      BucketMap_VOID g2_bucket;
    };

    RoiDiff();
    ~RoiDiff();

    bool diff( const Graph & g1, const Graph & g2 );
    void writeCSV( const std::string & name );

    // const map_STRING_DiffStat & statsByLabel() const;
    const aims::RoiDiff::DiffStat & globalStats() const;
    rc_ptr_BucketMap_VOID mismatch_bucket() const;
    rc_ptr_Volume_S16 graph2LabelVolume() const;
    const vector_STRING & roiNames() const;
    const std::map<std::string, int> & roiNamesInv() const;
    const aims::RoiDiff::DiffStat
      & statsByLabel( const std::string & label ) const;
%MethodCode
  const std::map<std::string, aims::RoiDiff::DiffStat> & stats
    = sipCpp->statsByLabel();
  std::map<std::string, aims::RoiDiff::DiffStat>::const_iterator
    i = stats.find( *a0 );
  if( i == stats.end() )
  {
    sipIsErr = 1;
    PyErr_SetString( PyExc_KeyError, "Label not found" );
  }
  else
    sipRes = const_cast<aims::RoiDiff::DiffStat *>( &i->second );
%End
  };

}; // namespace aims
