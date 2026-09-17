
#ifndef AIMS_SCALESPACE_BUCKETBLOB_H
#define AIMS_SCALESPACE_BUCKETBLOB_H

#include <aims/bucket/bucket.h>
#include <aims/connectivity/connectivity.h>


namespace aims
{

  /** Blobs and watershed
   */
  template <typename T>
  class BucketBlobExtractor
  {
  public:
    struct BlobStruct
    {
      BlobStruct() : newblobnum( 0 ), keepallblobs( false ), background( -10 )
      {}
      BucketMap<Void> *blobs;
      int newblobnum;
      std::map<int, int> mergemap;
      // value in this map is the merge position
      std::map<int, Point3d> deadblobs;
      bool keepallblobs;
      int background;
    };

    class StopCriterion
    {
    public:
      virtual ~StopCriterion() {}
#if __GNUC__ == 3 && __GNUC_MINOR__ == 2 // bug in gcc 3.2 (mingw)
      virtual bool stops( const Point3d &, const BlobStruct & bs ) {}
#else
      virtual bool stops( const Point3d &, const BlobStruct & bs ) = 0;
#endif
      virtual bool doesKeepBlob( int, const BlobStruct & ) { return true; }
    };

    class NoStopCriterion : public StopCriterion
    {
    public:
      virtual ~NoStopCriterion() {}
      virtual bool stops( const Point3d &, const BlobStruct & )
      { return false; }
    };

    class OneComponentStopCriterion : public StopCriterion
    {
    public:
      OneComponentStopCriterion
        ( const std::set<Point3d, BucketMapLess> & toJoin );
      virtual ~OneComponentStopCriterion();
      virtual bool stops( const Point3d &, const BlobStruct & bs );
      virtual bool doesKeepBlob( int blob, const BlobStruct & );

      std::set<Point3d, BucketMapLess> pointsToJoin;
      int mainblob;
    };


    class BlobsMerger
    {
    public:
      virtual ~BlobsMerger();
      virtual void merge( const std::set<int> & tomerge, BlobStruct & bs,
                          const Point3d & p );
    };


    class GLBlobsMerger : public BlobsMerger
    {
    public:
      GLBlobsMerger();
      virtual ~GLBlobsMerger();
      virtual void merge( const std::set<int> & tomerge, BlobStruct & bs,
                          const Point3d & p );
    };


    BucketBlobExtractor( StopCriterion* sc, bool minima,
                         bool keepallblobs = false, BlobsMerger* m = 0 );
    ~BucketBlobExtractor();

    void setConnectivity( Connectivity::Type t ) { _connectivity = t; }
    Connectivity::Type connectivity() const { return _connectivity; }
    /** Extracts blobs or watershed.
        \return blobs: the time key is the blob number
     */
    BucketMap<Void> *extractBlobs( const BucketMap<T> & valuemap );

  protected:
    void neighborBlobs( std::set<int> &, const Point3d &, const BlobStruct & );

    StopCriterion *_stop;
    BlobsMerger *_merger;
    bool _minima;
    bool _keepallblobs;
    Connectivity::Type  _connectivity;
  };


  extern template class BucketBlobExtractor<float>;

}

#endif



