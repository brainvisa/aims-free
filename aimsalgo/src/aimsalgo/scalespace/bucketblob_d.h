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


#ifndef AIMS_SCALESPACE_BUCKETBLOB_D_H
#define AIMS_SCALESPACE_BUCKETBLOB_D_H

#include <aims/scalespace/bucketblob.h>

namespace aims
{

  template <typename T>
  BucketBlobExtractor<T>::BucketBlobExtractor( StopCriterion* sc, bool minima,
                                               bool keepallblobs,
                                               BlobsMerger* m )
  : _stop( sc ), _merger( m ), _minima( minima ),
    _keepallblobs( keepallblobs ),
    _connectivity( Connectivity::CONNECTIVITY_6_XYZ )
  {
    if( !m )
      _merger = new BlobsMerger;
  }


  template <typename T>
  BucketBlobExtractor<T>::~BucketBlobExtractor()
  {
    delete _stop;
    delete _merger;
  }


  template <typename T>
  void BucketBlobExtractor<T>::neighborBlobs( std::set<int> & blobs,
                                              const Point3d & p,
                                              const BlobStruct & bs )
  {
    BucketMap<Void>::const_iterator ib, eb = bs.blobs->end();
    Connectivity c( 0, 0, _connectivity );
    int i, n = c.nbNeighbors(), b;
    std::map<int, int>::const_iterator im, em = bs.mergemap.end();
    std::set<int>::iterator notdone = blobs.end();
    std::map<int, Point3d>::const_iterator notdead = bs.deadblobs.end();

    for( ib=bs.blobs->begin(); ib!=eb; ++ib )
    {
      b = ib->first;
      for( im=bs.mergemap.find( b ); im!=em; im=bs.mergemap.find( b ) )
        b = im->second;
      if( bs.deadblobs.find( b ) == notdead && blobs.find( b ) == notdone )
        for( i=0; i<n; ++i )
          if( ib->second.find( p + c.xyzOffset(i) ) != ib->second.end() )
          {
            blobs.insert( ib->first );
            break;
          }
    }
  }


  template <typename T>
  BucketMap<Void> *
  BucketBlobExtractor<T>::extractBlobs( const BucketMap<T> & valuemap )
  {
    BlobStruct  bs;
    bs.keepallblobs = _keepallblobs;
    bs.blobs = new BucketMap<Void>;
    bs.blobs->setSizeXYZT( valuemap.sizeX(), valuemap.sizeY(),
                           valuemap.sizeZ(), valuemap.sizeT() );

    // re-order voxels in increasing/decreasing value
    std::multimap<T, Point3d> orderedb;
    const typename BucketMap<T>::Bucket & bk = valuemap.begin()->second;
    typename BucketMap<T>::Bucket::const_iterator  ibk, ebk = bk.end();
    for( ibk=bk.begin(); ibk!=ebk; ++ibk )
      orderedb.insert( std::pair<T, Point3d>( ibk->second, ibk->first ) );

    typename std::multimap<T, Point3d>::iterator im, em = orderedb.end();
    for( im=orderedb.begin(); im!=em; ++im )
    {
      const Point3d & p = im->second;
      std::set<int>   blobs;
      neighborBlobs( blobs, p, bs );
      switch( blobs.size() )
      {
      case 0: // new blob
        (*bs.blobs)[ bs.newblobnum ][ p ] = Void();
        ++bs.newblobnum;
        break;
      case 1: // inside a blob
        (*bs.blobs)[ *blobs.begin() ][ p ] = Void();
        break;
      default: // blobs merge
        _merger->merge( blobs, bs, p );
      }
      if( _stop->stops( p, bs ) )
        break;
    }

    BucketMap<Void>  *res = bs.blobs;
    if( !_keepallblobs )
    {
      res = new BucketMap<Void>;
      res->setSizeXYZT( valuemap.sizeX(), valuemap.sizeY(),
                        valuemap.sizeZ(), valuemap.sizeT() );
      std::map<int, int> trans;
      std::map<int, int>::const_iterator
        im, em = bs.mergemap.end(), em2 = trans.end();
      int b, nb = 0;
      BucketMap<Void>::const_iterator ib, eb = bs.blobs->end();
      for( ib=bs.blobs->begin(); ib!=eb; ++ib )
      {
        b = ib->first;
        for( im=bs.mergemap.find( b ); im!=em; im=bs.mergemap.find( b ) )
          b = im->second;
        if( _stop->doesKeepBlob( b, bs ) )
        {
          im = trans.find( b );
          if( im == em2 )
          {
            trans[b] = nb;
            b = nb;
            ++nb;
          }
          else
            b = im->second;
          (*res)[b].insert( ib->second.begin(), ib->second.end() );
          }
      }
      delete bs.blobs;
    }

    return res;
  }

  //

  template <typename T>
  BucketBlobExtractor<T>::OneComponentStopCriterion::OneComponentStopCriterion
  ( const std::set<Point3d, BucketMapLess> & toJoin )
    : StopCriterion(), pointsToJoin( toJoin ), mainblob( -1 )
  {
  }


  template <typename T>
  BucketBlobExtractor<T>::OneComponentStopCriterion::
  ~OneComponentStopCriterion()
  {
  }


  template <typename T>
  bool BucketBlobExtractor<T>::OneComponentStopCriterion::stops
  ( const Point3d &, const BlobStruct & bs )
  {
    BucketMap<Void>::const_iterator ib, eb = bs.blobs->end();
    std::set<Point3d, BucketMapLess> active = pointsToJoin;
    std::set<Point3d, BucketMapLess>::iterator ip, ip2, ep = active.end();
    std::map<int, int>::const_iterator im, em = bs.mergemap.end();
    int comp = -1, c;
    for( ib=bs.blobs->begin(); ib!=eb && !active.empty(); ++ib )
    {
      for( ip=active.begin(); ip!=ep; )
      {
        if( ib->second.find( *ip ) != ib->second.end() )
        {
          c = ib->first;
          for( im=bs.mergemap.find( c ); im!=em; im=bs.mergemap.find( c ) )
            c = im->second;
          if( comp < 0 )
          {
            comp = c;
            mainblob = c;
          }
          else if( c != comp )
          {
            return false;
          }

          ip2 = ip;
          ++ip;
          active.erase( ip2 );
        }
        else
          ++ip;
      }
    }
    return active.empty();
  }


  template <typename T>
  bool BucketBlobExtractor<T>::OneComponentStopCriterion::doesKeepBlob(
    int blob, const BlobStruct & )
  {
    return blob == mainblob;
  }

  //

  template <typename T>
  BucketBlobExtractor<T>::BlobsMerger::~BlobsMerger()
  {
  }


  template <typename T>
  void BucketBlobExtractor<T>::BlobsMerger::merge( const std::set<int> & blobs,
                                                   BlobStruct & bs,
                                                   const Point3d & p )
  {
    std::set<int>::iterator i = blobs.begin(), e = blobs.end();
    int b0 = *i;
    if( bs.keepallblobs )
    {
      b0 = bs.newblobnum;
      ++bs.newblobnum;
    }
    else
      ++i;
    for( ; i!=e; ++i )
      bs.mergemap[*i] = b0;
    (*bs.blobs)[b0][p] = Void();
  }

  //

  template <typename T>
  BucketBlobExtractor<T>::GLBlobsMerger::GLBlobsMerger()
  : BlobsMerger()
  {
  }


  template <typename T>
  BucketBlobExtractor<T>::GLBlobsMerger::~GLBlobsMerger()
  {
  }


  template <typename T>
  void BucketBlobExtractor<T>::GLBlobsMerger::merge(
    const std::set<int> & blobs, BlobStruct & bs, const Point3d & p )
  {
    std::set<int>::iterator i, e = blobs.end();
    for( i=blobs.begin(); i!=e; ++i )
      if( *i != bs.background )
        bs.deadblobs[*i] = p;
    (*bs.blobs)[bs.background][p] = Void();
  }

}

#endif

