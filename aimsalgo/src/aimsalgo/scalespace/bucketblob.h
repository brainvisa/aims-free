/* Copyright (c) 2008 CEA
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



