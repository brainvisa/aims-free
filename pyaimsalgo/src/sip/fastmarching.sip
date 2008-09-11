
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
    rc_ptr_Volume_FLOAT midInterfaceVol (short label1, short label2) const
      throw ( std::runtime_error, std::exception );
    rc_ptr_Volume_S16 voronoiVol() const;
    void setVerbose( bool x );
    bool verbose() const;
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
    rc_ptr_Volume_FLOAT midInterfaceVol (short label1, short label2) const
      throw ( std::runtime_error, std::exception );
    rc_ptr_BucketMap_S16 voronoiVol() const;
    void setVerbose( bool x );
    bool verbose() const;
};

};

