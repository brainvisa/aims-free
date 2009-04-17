
namespace aims
{

  class FastMarching_Volume_S16 /PyName=FastMarching/
  {
%TypeHeaderCode
#include <aims/distancemap/fastmarching.h>
namespace aims
{
  typedef aims::FastMarching<carto::Volume<int16_t> > FastMarching_Volume_S16;
}
%End

  public:
    FastMarching_Volume_S16(bool = false);
%MethodCode
sipCpp = new aims::FastMarching_Volume_S16( aims::Connectivity::CONNECTIVITY_26_XYZ, a0 );
%End

    ~FastMarching_Volume_S16();
    rc_ptr_Volume_FLOAT doit( rc_ptr_Volume_S16 vol, 
      short worklabel, short inlabel, short outlabel ) /ReleaseGIL/;
    rc_ptr_Volume_FLOAT doit( rc_ptr_Volume_S16 vol,
      const set_S16 & worklabels, const set_S16 & seedlabels ) /ReleaseGIL/;
%If (SIP_THROW_BUG)
    rc_ptr_Volume_FLOAT midInterfaceVol (short label1, short label2) const
      throw ( );
%End
%If (!SIP_THROW_BUG)
    rc_ptr_Volume_FLOAT midInterfaceVol (short label1, short label2) const
      throw ( std::runtime_error, std::exception );
%End
    const BucketMap_FLOAT & midInterface( short label1, short label2 ) const;
    rc_ptr_Volume_S16 voronoiVol() const;
    void setVerbose( bool x );
    bool verbose() const;
    void setSpeedMap( rc_ptr_Volume_FLOAT speed );
    void setInvSpeedMap( rc_ptr_Volume_FLOAT invspeed );
    void clearSpeedMap();

    SIP_PYTUPLE midInterfaceLabels() const;
%MethodCode
  std::vector<std::pair<short, short> > mil = sipCpp->midInterfaceLabels();
  sipRes = PyTuple_New( mil.size() );
  int i, n = (int) mil.size();
  for( i=0; i<n; ++i )
  {
    PyObject *item = PyTuple_New( 2 );
    PyTuple_SetItem( item, 0, PyInt_FromLong( mil[i].first ) );
    PyTuple_SetItem( item, 1, PyInt_FromLong( mil[i].second ) );
    PyTuple_SetItem( sipRes, i, item );
  }
%End
  };


  class FastMarching_BucketMap_S16
  {
%TypeHeaderCode
#include <aims/distancemap/fastmarching.h>
namespace aims
{
  typedef aims::FastMarching<BucketMap<int16_t> > FastMarching_BucketMap_S16;
}
%End

  public:
    FastMarching_BucketMap_S16(bool = false);
%MethodCode
sipCpp = new aims::FastMarching_BucketMap_S16( aims::Connectivity::CONNECTIVITY_26_XYZ, a0 );
%End

    ~FastMarching_BucketMap_S16();
    rc_ptr_BucketMap_FLOAT doit( rc_ptr_BucketMap_S16 vol,
      short worklabel, short inlabel, short outlabel ) /ReleaseGIL/;
    rc_ptr_BucketMap_FLOAT doit( rc_ptr_BucketMap_S16 vol,
      const set_S16 & worklabels, const set_S16 & seedlabels ) /ReleaseGIL/;
%If (SIP_THROW_BUG)
    rc_ptr_Volume_FLOAT midInterfaceVol (short label1, short label2) const
      throw ( );
%End
%If (!SIP_THROW_BUG)
    rc_ptr_Volume_FLOAT midInterfaceVol (short label1, short label2) const
      throw ( std::runtime_error, std::exception );
%End
    const BucketMap_FLOAT & midInterface( short label1, short label2 ) const;
    rc_ptr_BucketMap_S16 voronoiVol() const;
    void setVerbose( bool x );
    bool verbose() const;
    void setSpeedMap( rc_ptr_BucketMap_FLOAT speed );
    void setInvSpeedMap( rc_ptr_BucketMap_FLOAT invspeed );
    void clearSpeedMap();

    SIP_PYTUPLE midInterfaceLabels() const;
%MethodCode
  std::vector<std::pair<short, short> > mil = sipCpp->midInterfaceLabels();
  sipRes = PyTuple_New( mil.size() );
  int i, n = (int) mil.size();
  for( i=0; i<n; ++i )
  {
    PyObject *item = PyTuple_New( 2 );
    PyTuple_SetItem( item, 0, PyInt_FromLong( mil[i].first ) );
    PyTuple_SetItem( item, 1, PyInt_FromLong( mil[i].second ) );
    PyTuple_SetItem( sipRes, i, item );
  }
%End
};

};

