
class RawConverter_BucketMap_VOID_rc_ptr_Volume_%Template1typecode%
{
%TypeHeaderCode
#include <aims/utility/converter_bucket.h>
#ifndef PYAIMS_RawConverter_BucketMap_VOID_rc_ptr_Volume_%Template1typecode%
#define PYAIMS_RawConverter_BucketMap_VOID_rc_ptr_Volume_%Template1typecode%
typedef carto::RawConverter<aims::BucketMap<Void>,
                            carto::rc_ptr<carto::Volume<%Template1%> > >
  RawConverter_BucketMap_VOID_rc_ptr_Volume_%Template1typecode%;
#endif
%End

public :
  RawConverter_BucketMap_VOID_rc_ptr_Volume_%Template1typecode%(
    bool timeislabel = false, bool withConstantValue = false,
    %Template1% value = 0 );
  void convert( const BucketMap_VOID &in,
                rc_ptr_Volume_%Template1typecode% & out ) const;
  void printToVolume( const BucketMap_VOID &in,
                      rc_ptr_Volume_%Template1typecode% & out,
                      const Point3d & offset = Point3d( 0, 0, 0 ) ) const;
  void setTimeIsLabel( bool x );
  bool timeIsLabel() const;
  %Template1% constValue() const;
  bool hasConstantValue() const;
  void setConstValue( %Template1% x );
  void setHasConstValue( bool x );
};


class RawConverter_rc_ptr_Volume_%Template1typecode%_BucketMap_VOID
{
%TypeHeaderCode
#include <aims/utility/converter_bucket.h>
#ifndef PYAIMS_RawConverter_rc_ptr_Volume_%Template1typecode%_BucketMap_VOID
#define PYAIMS_RawConverter_rc_ptr_Volume_%Template1typecode%_BucketMap_VOID
typedef carto::RawConverter<carto::rc_ptr<carto::Volume<%Template1%> >,
                            aims::BucketMap<Void> >
  RawConverter_rc_ptr_Volume_%Template1typecode%_BucketMap_VOID;
#endif
%End

public:
  RawConverter_rc_ptr_Volume_%Template1typecode%_BucketMap_VOID( bool timeislabel = false );

  void convert( const rc_ptr_Volume_%Template1typecode% &in,
                BucketMap_VOID & out ) const;

  void setTimeIsLabel( bool x );
  bool timeIsLabel() const;
};
