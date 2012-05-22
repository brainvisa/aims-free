/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
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
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#include <cstdlib>
#include <aims/foldgraph/foldoversegmentation.h>
#include <graph/graph/graph.h>
#include <aims/bucket/bucket.h>
#include <aims/distancemap/fastmarching.h>
#include <aims/distancemap/downpath.h>
#include <aims/connectivity/component.h>
#include <aims/scalespace/bucketblob.h>
#include <aims/resampling/mask.h>
#include <aims/graph/graphmanip.h>
#include <aims/topology/topoClassifier.h>

using namespace aims;
using namespace carto;
using namespace std;


namespace
{

  // grows the bucket 1 voxel thicker
  template <typename T>
  BucketMap<T> *dilateBucket( const BucketMap<T> & in )
  {
    BucketMap<T> *res = new BucketMap<T>;
    res->setSizeXYZT( in.sizeX(), in.sizeY(), in.sizeZ(), in.sizeT() );
    typename BucketMap<T>::const_iterator ib, eb = in.end();
    typename BucketMap<T>::Bucket::const_iterator ibb, ebb, jb, ejb;
    Connectivity cd( 0, 0, Connectivity::CONNECTIVITY_6_XYZ );
    int i, n = cd.nbNeighbors();

    for( ib=in.begin(); ib!=eb; ++ib )
    {
      typename BucketMap<T>::Bucket & bk = (*res)[ib->first];
      ejb = bk.end();
      for( ibb=ib->second.begin(), ebb=ib->second.end(); ibb!=ebb; ++ibb )
      {
        bk[ ibb->first ] = ibb->second;
        for( i=0; i<n; ++i )
        {
          Point3d p( ibb->first + cd.xyzOffset(i) );
          jb = bk.find( p );
          if( jb == ejb )
            bk[ p ] = ibb->second;
        }
      }
    }
    return res;
  }


  /* correspondance measure of neighborhood (max if b1 and b2 are
     really close neighbors, null if they are far).
     If dil is not 0, the initial b1 bucket is dilated dil voxels before
     matching.
  */
  float bucketMatch( const BucketMap<Void>::Bucket & b1,
                     const BucketMap<Void>::Bucket & b2, int dil = 0 )
  {
    rc_ptr<BucketMap<Void> > b1bis;
    const BucketMap<Void>::Bucket *b1p = &b1;
    if( dil != 0 )
    {
      b1bis.reset( new BucketMap<Void> );
      (*b1bis)[0] = b1;
      for( int i=0; i<dil; ++i )
        b1bis.reset( dilateBucket( *b1bis ) );
      b1p = &b1bis->begin()->second;
    }
    BucketMap<Void>::Bucket::const_iterator
        ib1, eb1 = b1p->end(), eb2 = b2.end(), eb3 = b1.end();
    Connectivity cd( 0, 0, Connectivity::CONNECTIVITY_6_XYZ );
    unsigned i, n = cd.nbNeighbors();
    float corresp = 0;
    for( ib1=b1p->begin(); ib1!=eb1; ++ib1 )
    {
      if( b2.find( ib1->first ) != eb2 )
      {
        if( b1bis && b1.find( ib1->first ) != eb3 ) // in intial bucket
          corresp += 3.F;
        else // in dilated bucket
          corresp += 2.F;
      }
      for( i=0; i<n; ++i )
        if( b2.find( ib1->first + cd.xyzOffset(i) ) != eb2 )
          corresp += 1.F;
    }
    return corresp;
  }


  /* splits src bucket into out1 and out2, according to the voronoi.
     voxels of voronoi label label1 will be dispatched to out1, others to out2.
  */
  void _splitBucket( rc_ptr<BucketMap<Void> > & out1,
                     rc_ptr<BucketMap<Void> > & out2,
                     const BucketMap<Void> & src,
                     const BucketMap<int16_t> & voronoi, int16_t label1 )
  {
    const BucketMap<Void>::Bucket & bk = src.begin()->second;
    const BucketMap<int16_t>::Bucket & vor = voronoi.begin()->second;
    BucketMap<Void>::Bucket::const_iterator ib, eb = bk.end();
    out1.reset( new BucketMap<Void> );
    out2.reset( new BucketMap<Void> );
    out1->setSizeXYZT( src.sizeX(), src.sizeY(), src.sizeZ(), src.sizeT() );
    out2->setSizeXYZT( src.sizeX(), src.sizeY(), src.sizeZ(), src.sizeT() );
    BucketMap<Void>::Bucket & o1 = (*out1)[0];
    BucketMap<Void>::Bucket & o2 = (*out2)[0];

    for( ib=bk.begin(); ib!=eb; ++ib )
      if( vor.find( ib->first )->second == label1 )
        o1[ ib->first ];
      else
        o2[ ib->first ];
  }


  struct _nearestpoint
  {
    _nearestpoint() : sqdist( numeric_limits<float>::max() ) {}
    Point3d position;
    int bucket;
    int component;
    float sqdist;
  };


  Point3d findNearestPoint( const Point3d & pos,
                            const BucketMap<Void> & bck )
  {
    const BucketMap<Void>::Bucket & bk0 = bck.begin()->second;
    BucketMap<Void>::Bucket::const_iterator ib, eb = bk0.end();
    float dmin = FLT_MAX, d;
    Point3d pmin = pos, p;
    Point3df vs( bck.sizeX(), bck.sizeY(), bck.sizeZ() );
    for( ib=bk0.begin(); ib!=eb; ++ib )
    {
      p = pos - ib->first;
      d = Point3df( p[0] * vs[0], p[1] * vs[1], p[2] * vs[2] ).norm2();
      if( d < dmin )
      {
        dmin = d;
        pmin = ib->first;
      }
    }
    return pmin;
  }


  /* finds the nearest point of pos in a list of buckets. Only matches a point
     in a connected group of at least nvoxmin voxels (if possible). which will
     contain the bucket number the nearest point was found in.
     If pmins is not null, it will be filled with the nearest point on each
     bucket.
     If distmap is provided, use it instead of the euclidian distance to pos.
  */
  Point3d findNearestPoint( const Point3d & pos,
                            const vector<rc_ptr<BucketMap<Void> > > & bcks,
                            int nvoxmin, int & which,
                            vector<Point3d> *pmins = 0,
                            rc_ptr<BucketMap<float> > distmap
                            = rc_ptr<BucketMap<float> >() )
  {
    int nbk = (int) bcks.size(), i, j;
    vector<AimsBucket<Void> > ccomps( nbk );
    AimsBucket<Void>::const_iterator iab, eab;
    list<AimsBucketItem<Void> >::const_iterator iiab, eiab;
    vector<vector<_nearestpoint> > allnearests( nbk );
    float d2;
    bool hasgood = false;
    int c, nvox, b = 0;
    const BucketMap<float>::Bucket *dmap = 0;
    BucketMap<float>::Bucket::const_iterator idmap, edmap;
    if( distmap )
    {
      dmap = &distmap->begin()->second;
      edmap = dmap->end();
    }
    for( i=0; i<nbk; ++i, ++b )
    {
      if( !pmins ) // merge all buckets
        b = 0;
      // prepare min lists
      vector<_nearestpoint> & nearests = allnearests[b];
      if( nearests.empty() )
      {
        nearests.reserve( nvoxmin + 1 );
        for( j=0; j<=nvoxmin; ++j )
          nearests.push_back( _nearestpoint() );
      }
      // extract connected components
      AimsBucket<Void> & abk = ccomps[i];
      AimsConnectedComponent( abk, *bcks[i], Connectivity::CONNECTIVITY_26_XYZ,
                              Void(), true, 0, 0, false );
      // scan component by component
      for( iab=abk.begin(), eab=abk.end(); iab!=eab; ++iab )
      {
        const list<AimsBucketItem<Void> > & bk = iab->second;
        nvox = bk.size();
        if( hasgood && nvox < nvoxmin )
          continue; // too small component and we already one better
        c = iab->first;
        if( nvox >= nvoxmin )
        {
          nvox = nvoxmin;
          hasgood = true;
        }
        // nearest point for this size of connected component
        _nearestpoint & np = nearests[nvox];
        for( iiab=iab->second.begin(), eiab=iab->second.end(); iiab!=eiab;
          ++iiab )
        {
          if( dmap )
          {
            idmap = dmap->find( iiab->location() );
            if( idmap == edmap )
              continue;
            d2 = idmap->second;
          }
          else
            d2 = (iiab->location() - pos).norm2();
          if( np.sqdist < 0 || d2 < np.sqdist )
          {
            np.sqdist = d2;
            np.position = iiab->location();
            np.bucket = i;
            np.component = c;
          }
        }
      }
    }
    vector<_nearestpoint> pbycc( nvoxmin + 1 );
    which = -1;
    for( j=0; j<b; ++j ) // each bucket if not merged
    {
      for( i=nvoxmin; i>=0; --i ) // each cc size
      {
        _nearestpoint & np = allnearests[j][i];
        if( np.sqdist < numeric_limits<float>::max() )
        {
          if( np.sqdist < pbycc[i].sqdist )
            pbycc[i] = np;
          if( pmins )
            pmins->push_back( np.position );
          break;
        }
      }
      if( i < 0 && pmins )
        // empty bucket...
        pmins->push_back( Point3d( 0, 0, 0 ) );
    }
    for( i=nvoxmin; i>=0; --i ) // each cc size
    {
      _nearestpoint & np = pbycc[i];
      if( np.sqdist < numeric_limits<float>::max() )
      {
        which = np.bucket;
        return np.position;
      }
    }
    // not found: no voxels in buckets !
    return Point3d( 0, 0, 0 );
  }


  // voronoi of seed1 and seed2 into the mask of given buckets
  rc_ptr<BucketMap<int16_t> > foldVoronoi( rc_ptr<BucketMap<Void> > ss,
                                           rc_ptr<BucketMap<Void> > bottom,
                                           rc_ptr<BucketMap<Void> > other,
                                           rc_ptr<BucketMap<Void> > hjl,
                                           rc_ptr<BucketMap<Void> > seed1,
                                           rc_ptr<BucketMap<Void> > seed2 )
  {
    rc_ptr<BucketMap<int16_t> > voronoi( new BucketMap<int16_t> );
    voronoi->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
    BucketMap<int16_t>::Bucket & vor = (*voronoi)[0];
    BucketMap<Void>::Bucket & ss0 = (*ss)[0];
    BucketMap<Void>::Bucket & bot0 = (*bottom)[0];
    BucketMap<Void>::Bucket & hj0 = (*hjl)[0];
    BucketMap<Void>::Bucket & oth0 = (*other)[0];
    BucketMap<Void>::Bucket::const_iterator ib, eb;
    for( ib=ss0.begin(), eb=ss0.end(); ib!=eb; ++ib )
      vor[ ib->first ] = 1;
    for( ib=bot0.begin(), eb=bot0.end(); ib!=eb; ++ib )
      vor[ ib->first ] = 1;
    for( ib=oth0.begin(), eb=oth0.end(); ib!=eb; ++ib )
      vor[ ib->first ] = 1;
    for( ib=hj0.begin(), eb=hj0.end(); ib!=eb; ++ib )
      vor[ ib->first ] = 1;
    for( ib=seed1->begin()->second.begin(), eb=seed1->begin()->second.end();
         ib!=eb; ++ib )
    vor[ ib->first ] = 10;
    for( ib=seed2->begin()->second.begin(), eb=seed2->begin()->second.end();
         ib!=eb; ++ib )
    vor[ ib->first ] = 11;
    set<int16_t> work, seeds;
    work.insert( 1 );
    seeds.insert( 10 );
    seeds.insert( 11 );
    // dilate to overcome the 6-connectivity requirement of fastmarching
    voronoi.reset( dilateBucket( *voronoi ) );
    FastMarching<BucketMap<int16_t> > fm( Connectivity::CONNECTIVITY_26_XYZ );
    fm.doit( voronoi, work, seeds );
    return fm.voronoiVol();
  }


  /* interface line in the same way Vip junctions were defined:
     this will build a 2-voxels thick line
  */
  rc_ptr<BucketMap<Void> > interface( rc_ptr<BucketMap<Void> > ss1,
                                      rc_ptr<BucketMap<Void> > ss2 )
  {
    BucketMap<Void>::Bucket & bss1 = ss1->begin()->second;
    BucketMap<Void>::Bucket & bss2 = ss2->begin()->second;
    BucketMap<Void>::Bucket::iterator ib, eb, ebss2 = bss2.end();
    rc_ptr<BucketMap<Void> > splitline( new BucketMap<Void> );
    splitline->setSizeXYZT( ss1->sizeX(), ss1->sizeY(), ss1->sizeZ(),
                            ss1->sizeT() );
    BucketMap<Void>::Bucket & bsplit = (*splitline)[0];
    Connectivity c( 0, 0, Connectivity::CONNECTIVITY_26_XYZ );
    int nn = c.nbNeighbors(), i;

    for( ib=bss1.begin(), eb=bss1.end(); ib!=eb; ++ib )
    {
      bool added = false;
      for( i=0; i<nn; ++i )
      {
        Point3d p( ib->first + c.xyzOffset(i) );
        if( bss2.find( p ) != ebss2 )
        {
          // interface between ss1 and ss2: add both voxels
          if( !added )
            bsplit[ ib->first ];
          bsplit[ p ];
          added = true;
          // (don't break: other p points from ss2 may be added)
        }
      }
    }
    return splitline;
  }


  int newSplitIndex( Graph & graph )
  {
    int curindex = 0;
    if( !graph.getProperty( "_current_split_index", curindex ) )
    {
      int idx = -1;
      Graph::const_iterator iv, ev = graph.end();
      for( iv=graph.begin(); iv!=ev; ++iv )
        if( (*iv)->getProperty( "splitnode_index", idx )
          && idx + 1 > curindex )
          curindex = idx + 1;
    }
    graph.setProperty( "_current_split_index", curindex + 1 );
    return curindex;
  }

}


FoldArgOverSegment::FoldArgOverSegment( Graph* g )
  : _graph( g )
{
}


FoldArgOverSegment::~FoldArgOverSegment()
{
}


/* Split ss in 2 parts ss1 and ss2 along splitline.
    returns true if it succeeded
*/
bool FoldArgOverSegment::splitSimpleSurface( rc_ptr<BucketMap<Void> > ss,
  rc_ptr<BucketMap<Void> > splitline,
  rc_ptr<BucketMap<Void> > & ss1,
  rc_ptr<BucketMap<Void> > & ss2 )
{
  rc_ptr<BucketMap<Void> > sssplit;
  rc_ptr<BucketMap<Void> > dilline;
  int nssdil = 0;
  AimsBucket<Void> sssplb;
  sssplb.setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );

  AimsConnectedComponent( sssplb, *ss,
                        Connectivity::CONNECTIVITY_26_XYZ,
                        Void(), true, 0, 0, false );
  unsigned ncss = sssplb.size();
  rc_ptr<BucketMap<Void> > sscomp( new BucketMap<Void>( sssplb ) );
  sscomp->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  cout << "initial ss components: " << ncss << endl;

  /* try to break ss into more than its initial number of connected
    components, by masking it with an increaslingly dilated splitline
  */
  do
  {
    if( !dilline )
      dilline.reset( dilateBucket( *splitline ) );
    else
    {
      cout << "Dilating split line a bit more...\n";
      dilline.reset( dilateBucket( *dilline ) );
    }
    if( sssplb.size() <= ncss )
    {
      sssplit.reset( mask( *ss, *dilline, false ) );
      sssplb.clear();
      AimsConnectedComponent( sssplb, *sssplit,
                              Connectivity::CONNECTIVITY_26_XYZ,
                              Void(), true, 0, 0, false );
      ++nssdil;
    }
  }
  while( ( sssplb.size() <= ncss && nssdil < 5 ) );

  cout << "ss split comps: " << sssplb.size() << ", ndil: " << nssdil
    << endl;
  if( sssplb.size() <= ncss )
  {
    cout << "could not split the simple surface - split aborting\n";
    return false;
  }
  *sssplit = sssplb;

  /* is this needed ?
  // re-dilate ss
  int i;
  for( i=0; i<nssdil; ++i )
    sssplit.reset( dilateBucket( *sssplit ) );
  sssplit.reset( mask( *sssplit, *ss ) );
  sssplit.reset( mask( *sssplit, *splitline, false ) );
  */

  // recollect ss bits
  ss1.reset( new BucketMap<Void> );
  ss2.reset( new BucketMap<Void> );
  ss1->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  ss2->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  BucketMap<Void>::iterator ibm, ebm, ibm2, ebm2;
  if( sssplit->size() == 2 ) // simple case: exactly 2 bits
  {
    ibm = sssplit->begin();
    (*ss1)[0] = ibm->second;
    ++ibm;
    (*ss2)[0] = ibm->second;
  }
  else // complex case of multiple bits where only one has split
  {
    cout << "ss in many pieces - re-aggregating...\n";
    // determine bits correspondance before/after split
    Connectivity c( 0, 0, Connectivity::CONNECTIVITY_26_XYZ );
    map<int, set<int> > corr;
    float match, maxmatch = 0;
    int i;
    for( ibm=sssplit->begin(), ebm=sssplit->end(); ibm!=ebm; ++ibm )
    {
      // cout << "  bit " << ibm->first << endl;
      maxmatch = 0;
      i = 0;
      for( ibm2=sscomp->begin(), ebm2=sscomp->end(); ibm2!=ebm2; ++ibm2 )
      {
        // cout << " -> comp: " << ibm2->first << endl;
        match = bucketMatch( ibm->second, ibm2->second );
        if( match > maxmatch )
        {
          maxmatch = match;
          i = ibm2->first;
        }
      }
      corr[i].insert( ibm->first );
    }
    // cout << "correspondance map done\n";
    i = 0;
    map<int, set<int> >::iterator ic, ec = corr.end();
    for( ic=corr.begin(); ic!=ec; ++ic )
      if( ic->second.size() > 1 )
      {
        i = ic->first;
        break;
      }
    set<int> & nums = ic->second;
    set<int>::iterator  is, es = nums.end();
    if( nums.size() > 2 )
    { // keep only the 2 biggest bits
      // cout << "more than 2 parts in the same split\n";
      unsigned sbig1 = 0, sbig2 = 0, s;
      int big1 = 0, big2 = 0;
      for( is=nums.begin(); is!=es; ++is )
      {
        s = (*sssplit)[*is].size();
        if( s > sbig1 )
        {
          sbig2 = sbig1;
          big2 = big1;
          sbig1 = s;
          big1 = *is;
        }
        else if( s > sbig2 )
        {
          sbig2 = s;
          big2 = *is;
        }
      }
      nums.clear();
      nums.insert( big1 );
      nums.insert( big2 );
      // cout << "kept " << big1 << " and " << big2 << endl;
    }

    // aggregate remaining bits
    ebm2 = ebm;
    ibm = sssplit->begin();
    while( ibm != ebm )
    {
      // cout << "aggregate " << ibm->first << "...\n";
      if( nums.find( ibm->first ) != nums.end() )
      {
        // cout << "   is a target bit : continue\n";
        ++ibm;
        continue;
      }
      maxmatch = 0;
      i = 0;
      for( ibm2 = sssplit->begin(); ibm2!=ebm; ++ibm2 )
        if( ibm2 != ibm )
        {
          match = bucketMatch( ibm->second, ibm2->second, 2 );
          // cout << "test " << ibm2->first << ", match = " << match << endl;
          if( match >= maxmatch )
          {
            // cout << "increase max\n";
            maxmatch = match;
            i = ibm2->first;
          }
        }
      ibm2 = sssplit->find( i );
      if( ibm2 == ebm )
      {
        cerr << "error: no match found\n";
        return false;
      }
      // cout << "  -> to bit " << ibm2->first << endl;
      if( ibm == ibm2 )
      {
        cerr << "error: ibm == ibm2\n";
        return false;
      }
      // add ibm to ibm2 (best matching)
      ibm2->second.insert( ibm->second.begin(), ibm->second.end() );
      // cout << "   ... inserted\n";
      ibm2 = ibm;
      ++ibm;
      // delete current item
      sssplit->map<int, std::map<Point3d,Void,BucketMapLess> >::erase(
        ibm2 );
      // cout << "   ... erased\n";
    }
    cout << "ss aggregation done: " << sssplit->size() << " bits at end\n";

    // copy both final bits
    ibm = sssplit->begin();
    (*ss1)[0] = ibm->second;
    ++ibm;
    (*ss2)[0] = ibm->second;
  }

  return true;
}


rc_ptr<BucketMap<Void> > FoldArgOverSegment::splitLineOnBucket(
  rc_ptr<BucketMap<Void> > bucket, const list<Point3d> & pts )
{
  Point3df vs( bucket->sizeX(), bucket->sizeY(), bucket->sizeZ() );

  // topological classification to find bucket boundaries
  TopologicalClassifier< BucketMap<Void> > topoClass;
  rc_ptr< BucketMap<int16_t> > clas = topoClass.doit( *bucket );
  BucketMap<int16_t>::Bucket & cbk = (*clas)[0];
  BucketMap<int16_t>::Bucket::const_iterator icb, ecb = cbk.end();

  // find nearest boundary point to first and last ones
  // TODO: for now, just using euclidian distance
  float dmin1 = FLT_MAX, dmin2 = FLT_MAX, dx;
  Point3d pmin1, pmin2;
  // snap points to bucket
  list<Point3d> plist = pts;
  list<Point3d>::iterator ip, ep = plist.end();
  for( ip=plist.begin(); ip!=ep; ++ip )
    *ip = findNearestPoint( *ip, *bucket );
  Point3d p1 = *plist.begin(), p2 = *plist.rbegin();
  for( icb=cbk.begin(); icb!=ecb; ++icb )
    if( icb->second == TopologicalClassificationMeaning::BorderPoint )
    {
      Point3d p = icb->first - p1;
      dx = Point3df( p[0] * vs[0], p[1] * vs[1], p[2] * vs[2] ).norm2();
      if( dx < dmin1 )
      {
        dmin1 = dx;
        pmin1 = icb->first;
      }
      p = icb->first - p2;
      dx = Point3df( p[0] * vs[0], p[1] * vs[1], p[2] * vs[2] ).norm2();
      if( dx < dmin2 )
      {
        dmin2 = dx;
        pmin2 = icb->first;
      }
    }
  if( dmin1 == FLT_MAX )
    return rc_ptr<BucketMap<Void> >( 0 ); // cound not find boundaries
  // add boundaries points to list
  if( pmin1 != p1 )
    plist.insert( plist.begin(), pmin1 );
  if( pmin2 != p2 )
    plist.push_back( pmin2 );
  // cout << "pmin1: " << pmin1 << ", pmin2: " << pmin2 << endl;

  // propagate across bucket surface
  rc_ptr<BucketMap<float> > fbk( new BucketMap<float> ); // dist map
  rc_ptr<BucketMap<int16_t> > iss( new BucketMap<int16_t> ); // seeds
  iss->setSizeXYZT( bucket->sizeX(), bucket->sizeY(), bucket->sizeZ(),
                    bucket->sizeT() );
  BucketMap<Void>::Bucket & bk0 = (*bucket)[0];
  BucketMap<Void>::Bucket::const_iterator ib, eb=bk0.end();
  BucketMap<int16_t>::Bucket & iss0 = (*iss)[0];
  for( ib=bk0.begin(); ib!=eb; ++ib )
    iss0[ib->first] = 0;
  int16_t i = 1;
  set<int16_t> seeds;
  set<int16_t> work;
  work.insert( 0 );
  for( ip=plist.begin(); ip!=ep; ++ip, ++i )
  {
    iss0[*ip] = i;
    seeds.insert( i );
  }

  // dilate because the fast marching expects 6-connectivity
  rc_ptr<BucketMap<int16_t> > iss2( dilateBucket( *iss ) );

  // distance map between points of plist
  FastMarching<BucketMap<int16_t> > fm( Connectivity::CONNECTIVITY_26_XYZ,
                                        true );
  fbk = fm.doit( iss2, work, seeds );
  BucketMap<float>::Bucket & fbk0 = (*fbk)[0];
  BucketMap<float>::Bucket::iterator ibf, ebf, jbf;
  // cancel dilation: mask with initial region
  for( ibf=fbk0.begin(), ebf=fbk0.end(); ibf!=ebf; )
  {
    if( bk0.find( ibf->first ) == eb )
    {
      jbf = ibf;
      ++ibf;
      fbk0.erase( jbf );
    }
    else
      ++ibf;
  }

  // reconstruct split line
  rc_ptr< BucketMap<Void> > splitline( new BucketMap<Void> );
  splitline->setSizeXYZT( bucket->sizeX(), bucket->sizeY(), bucket->sizeZ(),
                          bucket->sizeT() );
  BucketMap<Void>::Bucket & sline0 = (*splitline)[0];
  vector<BucketMap<float> > regions( plist.size()+1 );
  rc_ptr<BucketMap<int16_t> > voro = fm.voronoiVol();
  // split distance map into voronoi regions
  for( icb=(*voro)[0].begin(), ecb=(*voro)[0].end(); icb!=ecb; ++icb )
  {
    ibf = fbk0.find( icb->first );
    if( ibf != ebf )
      regions[ icb->second ][0][icb->first] = fbk0[icb->first];
  }

  ip = plist.begin();
  i = 1;
  Point3d p0 = *ip;
  for( ++ip; ip!=ep; ++ip, ++i )
  {
    Point3d pmin;
    float dmin;
    // find nearest point on each voronoi interface between successive points
    try
    {
      const BucketMap<float> & mint = fm.midInterface( i, i+1 );
      BucketMap<float>::Bucket::const_iterator ibf2,
        ebf2 = mint.begin()->second.end();
      dmin = FLT_MAX;
      for( ibf2=mint.begin()->second.begin(); ibf2!=ebf2; ++ibf2 )
        if( ibf2->second < dmin && fbk0.find( ibf2->first ) != ebf )
        {
          dmin = ibf2->second;
          pmin = ibf2->first;
        }
    }
    catch( ... )
    {
      // no interface between p0 and current point
      cout << "no interface between seeds " << i << " and " << i+1 << endl;
      return rc_ptr<BucketMap<Void> >( 0 );
    }
    // follow "gradient" to get line from pmin to p0 and p
    // (restricted to voronoi regions)
    BucketMap<Void> *line = downPath( regions[i], pmin );
    sline0.insert( (*line)[0].begin(), (*line)[0].end() );
    delete line;
    line = downPath( regions[i+1], pmin );
    sline0.insert( (*line)[0].begin(), (*line)[0].end() );
    delete line;
    p0 = *ip;
  }

  return splitline;
}


rc_ptr< BucketMap<Void> >
FoldArgOverSegment::findSplitLine( Vertex* v, const list<Point3d> & pts )
{
  cout << "splitVertex along dotted line...\n";
  // get initial buckets
  rc_ptr<BucketMap<Void> > ss;
  if( !v->getProperty( "aims_ss", ss ) || !ss )
  {
    cerr << "This fold seems not to have simple surface voxels. Aborting."
      << endl;
    return rc_ptr<BucketMap<Void> >( 0 );
  }

  return splitLineOnBucket( ss, pts );
}


Vertex * FoldArgOverSegment::splitVertex( Vertex* v, const list<Point3d> & pts,
                                          size_t minsize )
{
  rc_ptr<BucketMap<Void> > splitline = findSplitLine( v, pts );
  return splitVertex( v, splitline, minsize );
}


rc_ptr< BucketMap<Void> >
FoldArgOverSegment::findSplitLine( Vertex* v, const list<Point3df> & pts )
{
  // get initial buckets
  rc_ptr<BucketMap<Void> > ss;
  if( !v->getProperty( "aims_ss", ss ) || !ss )
  {
    cerr << "This fold seems not to have simple surface voxels. Aborting."
      << endl;
    return rc_ptr<BucketMap<Void> >( 0 );
  }
  Point3df vs( ss->sizeX(), ss->sizeY(), ss->sizeZ() );
  list<Point3d> plist;
  list<Point3df>::const_iterator ip, ep = pts.end();
  for( ip=pts.begin(); ip!=ep; ++ip )
    plist.push_back( Point3d( int16_t( round( (*ip)[0] / vs[0] ) ),
                              int16_t( round( (*ip)[1] / vs[1] ) ),
                              int16_t( round( (*ip)[2] / vs[2] ) ) ) );

  return findSplitLine( v, plist );
}


Vertex * FoldArgOverSegment::splitVertex( Vertex* v,
                                          const list<Point3df> & pts,
                                          size_t minsize )
{
  rc_ptr< BucketMap<Void> > splitline = findSplitLine( v, pts );
  return splitVertex( v, splitline, minsize );
}


rc_ptr< BucketMap<Void> >
FoldArgOverSegment::findSplitLine( Vertex* v, const Point3d & pos0 )
{
  rc_ptr<BucketMap<Void> > splitline;

  // get initial buckets
  Point3d pos = pos0;
  rc_ptr<BucketMap<Void> > ss, bottom, other, hjl;
  if( !v->getProperty( "aims_ss", ss ) || !ss )
  {
    cerr << "This fold seems not to have simple surface voxels. Aborting."
      << endl;
    return splitline;
  }
  if( !v->getProperty( "aims_bottom", bottom ) || !bottom )
  {
    cerr << "This fold seems not to have bottom line voxels. Aborting."
      << endl;
    return splitline;
  }
  if( !v->getProperty( "aims_other", other ) || !other )
  {
    other.reset( new BucketMap<Void> );
    other->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  }
  Vertex::const_iterator  ie, ee=v->end();
  Edge  *hj = 0;
  for( ie=v->begin(); ie!=ee; ++ie )
    if((*ie)->getSyntax() == "hull_junction" )
    {
      hj = *ie;
      break;
    }
  if( !hj )
  {
    cerr << "cannot split fold with no hull junction" << endl;
    return splitline;
  }
  if( !hj->getProperty( "aims_junction", hjl ) || !hjl )
  {
    cerr << "vertex has no hull_junction: cannot split this way. abort.\n";
    return splitline;
  }

  // initial point: nearest in ss (if not already there)
  vector<rc_ptr<BucketMap<Void> > > bklines(1);
  bklines[0] = ss;
  int nearestbk;
  Point3d pmin = findNearestPoint( pos, bklines, 5, nearestbk );
  if( nearestbk < 0 )
  {
    cerr << "Cannot find closest point in simple surface (!). Aborting."
      << endl;
    return splitline;
  }
  pos = pmin;
  cout << "ss pos: " << pmin << endl;

  // propagate across simple surface
  rc_ptr<BucketMap<float> > fss( new BucketMap<float> ); // dist map
  rc_ptr<BucketMap<int16_t> > iss( new BucketMap<int16_t> ); // seeds
  iss->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  BucketMap<Void>::Bucket & ss0 = (*ss)[0];
  BucketMap<Void>::Bucket & bot0 = (*bottom)[0];
  BucketMap<Void>::Bucket & hj0 = (*hjl)[0];
  BucketMap<Void>::Bucket::const_iterator ib, eb=ss0.end();
  BucketMap<int16_t>::Bucket & iss0 = (*iss)[0];
  for( ib=ss0.begin(); ib!=eb; ++ib )
    iss0[ib->first] = 1;
  for( ib=bot0.begin(), eb=bot0.end(); ib!=eb; ++ib )
    iss0[ib->first] = 1;
  for( ib=hj0.begin(), eb=hj0.end(); ib!=eb; ++ib )
    iss0[ib->first] = 1;
  iss0[pos] = 0.;

  // dilate because the fast marching expects 6-connectivity
  rc_ptr<BucketMap<int16_t> > iss2( dilateBucket( *iss ) );

  FastMarching<BucketMap<int16_t> > fm( Connectivity::CONNECTIVITY_26_XYZ );
  fss = fm.doit( iss2, 1, 0, -100 );
  BucketMap<float>::Bucket & fss0 = (*fss)[0];
  BucketMap<float>::Bucket::iterator ibf, ebf, jbf;
  // cancel dilation: mask with initial region
  for( ibf=fss0.begin(), ebf=fss0.end(); ibf!=ebf; )
  {
    if( iss0.find( ibf->first ) == iss0.end() )
    {
      jbf = ibf;
      ++ibf;
      fss0.erase( jbf );
    }
    else
      ++ibf;
  }

  // find closest point on bottom and hull_junction
  bklines[0] = bottom;
  bklines.push_back( hjl );
  vector<Point3d> nears;
  pmin = findNearestPoint( pos, bklines, 5, nearestbk, &nears, fss );
  if( nearestbk < 0 || nears.size() != 2 )
  {
    cerr << "Cannot find closest point in bottom and hull junction lines. "
      "Aborting." << endl;
    return splitline;
  }

  // split path
  // from bottom
  rc_ptr<BucketMap<Void> > spline1( downPath( *fss, nears[0] ) );
  // and from hj
  splitline.reset( downPath( *fss, nears[1] ) );
  (*splitline)[0].insert( (*spline1)[0].begin(), (*spline1)[0].end() );

  return splitline;
}


Vertex * FoldArgOverSegment::splitVertex( Vertex* v, const Point3d & pos0,
                                          size_t minsize )
{
  rc_ptr<BucketMap<Void> > splitline = findSplitLine( v, pos0 );
  return splitVertex( v, splitline, minsize );
}


Vertex *FoldArgOverSegment::splitVertex(
  Vertex *v, rc_ptr< BucketMap<Void> > splitline, size_t minsize )
{
  if( splitline.isNull() )
    return 0; // split failed

  // get initial buckets
  rc_ptr<BucketMap<Void> > ss, bottom, other, hjl, merged;
  if( !v->getProperty( "aims_ss", ss ) || !ss )
  {
    cerr << "This fold seems not to have simple surface voxels. Aborting."
      << endl;
    return 0;
  }
  if( !v->getProperty( "aims_bottom", bottom ) || !bottom )
  {
    bottom.reset( new BucketMap<Void> );
    bottom->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  }
  if( !v->getProperty( "aims_other", other ) || !other )
  {
    other.reset( new BucketMap<Void> );
    other->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  }
  Vertex::const_iterator  ie, ee=v->end();
  Edge  *hj = 0;
  for( ie=v->begin(); ie!=ee; ++ie )
    if((*ie)->getSyntax() == "hull_junction" )
    {
      hj = *ie;
      break;
    }
  if( !hj || !hj->getProperty( "aims_junction", hjl ) || !hjl )
  {
    hjl.reset( new BucketMap<Void> );
    hjl->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  }

  // split simple surface
  rc_ptr<BucketMap<Void> > ss1, ss2;
  if( !splitSimpleSurface( ss, splitline, ss1, ss2 ) )
    return 0;

  // voronoi of ss parts
  rc_ptr<BucketMap<int16_t> > voronoi = foldVoronoi( ss, bottom, other, hjl,
                                                     ss1, ss2 );

  // split ss according to voronoi: definitive pass
  _splitBucket( ss1, ss2, *ss, *voronoi, 10 );
  rc_ptr<BucketMap<Void> > bottom1, bottom2;
  _splitBucket( bottom1, bottom2, *bottom, *voronoi, 10 );
  rc_ptr<BucketMap<Void> > hj1, hj2;
  _splitBucket( hj1, hj2, *hjl, *voronoi, 10 );
  rc_ptr<BucketMap<Void> > other1, other2;
  _splitBucket( other1, other2, *other, *voronoi, 10 );
  cout << "split done for all buckets\n";

  if( minsize > 0
      && ( (*ss1)[0].size() < minsize || (*ss2)[0].size() < minsize ) )
  {
    cerr << "Splitting will result in too small regions: " << (*ss1)[0].size()
      << " and " << (*ss2)[0].size() << " voxels. Minimum size is "
      << minsize << ". Aborting." << endl;
    return 0;
  }

  // recalculate split line in the same way other junctions were defined:
  splitline = interface( ss1, ss2 );

  // create new Vertex and hull_junction

  Vertex *v2 = _graph->addVertex( v->getSyntax() );
  // mark both nodes as originated from the same one
  int splitindex = 0;
  if( !v->getProperty( "splitnode_index", splitindex ) )
  {
    splitindex = newSplitIndex( *_graph );
    v->setProperty( "splitnode_index", splitindex );
  }
  v2->setProperty( "splitnode_index", splitindex );
  // change graph version for 3.3 minimum
  string gvs;
  vector<int> gver( 2, 0 );
  if( _graph->getProperty( "datagraph_VERSION", gvs ) )
  {
    string::size_type x = 0, y = 0;
    int i = 0;
    for( i=0; i<2 && y != string::npos; ++i )
    {
      y = gvs.find( '.', x );
      stringstream( gvs.substr( x, y-x ) ) >> gver[i];
      x = y+1;
    }
  }
  if( gver[0] < 3 || ( gver[0] == 3 && gver[1] < 3 ) )
  {
    _graph->setProperty( "datagraph_VERSION", "3.3" );
    _graph->setProperty( "datagraph_compatibility_model_VERSION", "3.3" );
    _graph->setProperty( "CorticalFoldArg_VERSION", "3.3" );
  }

  GraphManip::storeAims( *_graph, v2, "aims_ss", ss2 );
  GraphManip::storeAims( *_graph, v2, "aims_bottom", bottom2 );
  GraphManip::storeAims( *_graph, v2, "aims_other", other2 );
  // change existing buckets in v/hj
  GraphManip::storeAims( *_graph, v, "aims_ss", ss1 );
  GraphManip::storeAims( *_graph, v, "aims_bottom", bottom1 );
  GraphManip::storeAims( *_graph, v, "aims_other", other1 );
  GraphManip::storeAims( *_graph, hj, "aims_junction", hj1 );
  Edge::const_iterator  iv = hj->begin();
  if( *iv == v )
    ++iv;
  Vertex *hull = *iv;
  Edge *hje2 = _graph->addEdge( v2, hull, hj->getSyntax() );
  GraphManip::storeAims( *_graph, hje2, "aims_junction", hj2 );
  Edge *junc = _graph->addEdge( v, v2, "junction" );
  GraphManip::storeAims( *_graph, junc, "aims_junction", splitline );

  // fill mandatory properties
  string label;
  if( v->getProperty( "name", label ) )
    v2->setProperty( "name", label );
  if( v->getProperty( "label", label ) )
    v2->setProperty( "label", label );
  int pnum = (*ss2)[0].size() + (*bottom2)[0].size() + (*other2)[0].size();
  vector<float> vs;
  _graph->getProperty( "voxel_size", vs );
  while( vs.size() < 3 )
    vs.push_back( 1 );
  float voxvol = vs[0] * vs[1] * vs[2];
  v2->setProperty( "point_number", pnum );
  v2->setProperty( "size", pnum * voxvol );
  v2->setProperty( "ss_point_number", (int) (*ss2)[0].size() );
  v2->setProperty( "bottom_point_number", (int) (*bottom2)[0].size() );
  v2->setProperty( "other_point_number", (int) (*other2)[0].size() );
  // find a free number for skeleton_label
  Graph::const_iterator ig, eg = _graph->end();
  int skeleton_label = 290, skl2 = 0; // conventional starting value: 300
  int index = 0, index2 = 0;
  for( ig=_graph->begin(); ig!=eg; ++ig )
  {
    if( (*ig)->getProperty( "skeleton_label", skl2 ) && skl2 > skeleton_label )
      skeleton_label = skl2;
    if( (*ig)->getProperty( "index", index2 ) && index2 > index )
      index = index2;
  }
  skeleton_label += 10;
  ++index;
  v2->setProperty( "skeleton_label", skeleton_label );
  v2->setProperty( "index", index );
  pnum = (*ss1)[0].size() + (*bottom1)[0].size() + (*other1)[0].size();
  v->setProperty( "point_number", pnum );
  v->setProperty( "size", pnum * voxvol );
  v->setProperty( "ss_point_number", (int) (*ss1)[0].size() );
  v->setProperty( "bottom_point_number", (int) (*bottom1)[0].size() );
  v->setProperty( "other_point_number", (int) (*other1)[0].size() );
  try
  {
    // remove mesh: it's not the right one anymore
    v->removeProperty( "aims_Tmtktri" );
    if( v->hasProperty( "Tmtktri_label" ) )
      v->removeProperty( "Tmtktri_label" );
    if( v->hasProperty( "Tmtktri_filename" ) )
      v->removeProperty( "Tmtktri_filename" );
  }
  catch( ... )  {}
  hj->setProperty( "point_number", (int) (*hjl)[0].size() );
  hj->setProperty( "size", (*hjl)[0].size() * voxvol );
  hje2->setProperty( "point_number", (int) (*hj2)[0].size() );
  hje2->setProperty( "size", (*hj2)[0].size() * voxvol );
  junc->setProperty( "point_number", (int) (*splitline)[0].size() );
  junc->setProperty( "size", (*splitline)[0].size() * voxvol );

  /* separate relations (cortical, junctions, pli de passage)
     - junctions, plidepassage: assign them to the closest node
     - cortical: for now just remove them: the voronoi must be rebuit
       afterwards
  */
  Edge *edge;
  ie=v->begin();
  bool inc;
  while( ie!=ee )
  {
    edge = *ie;
    inc = true;
    if( edge != junc && ( edge->getSyntax() == "junction"
      || edge->getSyntax() == "plidepassage" ) )
    {
      rc_ptr<BucketMap<Void> > ebk;
      if( edge->getProperty( string( "aims_" ) + edge->getSyntax(), ebk ) )
      {
        float m1 = bucketMatch( (*ebk)[0], (*ss1)[0], 0 );
        float m2 = bucketMatch( (*ebk)[0], (*ss2)[0], 0 );
        if( m2 > m1 ) // relation is closer to v2 than the original v
        {
          // so transfer relation to v2 rather than v
          cout << "transfer rel " << edge << " / " << edge->getSyntax()
            << " to new vertex\n";
          Edge::const_iterator iiv;
          Vertex *vorg = 0, *vdest = 0;
          iiv=edge->begin();
          if( *iiv == v )
          {
            vorg = v2;
            ++iiv;
            vdest = *iiv;
          }
          else
          {
            vorg = *iiv;
            vdest = v2;
          }
          Edge *e2 = _graph->addEdge( vorg, vdest, edge->getSyntax() );
          Object iie;
          for( iie = edge->objectIterator(); iie->isValid(); iie->next() )
            e2->setProperty( iie->key(), iie->currentValue() );
          ++ie;
          inc = false;
          _graph->removeEdge( edge );
        }
      }
    }
    else if( edge->getSyntax() == "cortical" )
    {
      ++ie;
      inc = false;
      _graph->removeEdge( edge );
    }
    if( inc )
      ++ie;
  }

  cout << "splitVertex end\n";
  return v2;
}


namespace
{

  void printBucket( rc_ptr<Volume<int16_t> > vol,
                    const rc_ptr<BucketMap<Void> > bck, int16_t value )
  {
    const BucketMap<Void>::Bucket & bk = bck->begin()->second;
    BucketMap<Void>::Bucket::const_iterator i, e = bk.end();

    for( i=bk.begin(); i!=e; ++i )
    {
      const Point3d & p = i->first;
      vol->at( p[0], p[1], p[2] ) = value;
    }
  }

}


void FoldArgOverSegment::printSplitInSkeleton( rc_ptr<Volume<int16_t> > skel,
                                               const Vertex* v1,
                                               const Vertex* v2 )
{
  Vertex::const_iterator ie, ee = v1->end();
  Edge::const_iterator iv;
  for( ie=v1->begin(); ie!=ee; ++ie )
    if( (*ie)->getSyntax() == "junction" )
    {
      iv = (*ie)->begin();
      if( *iv != v2 )
        ++iv;
      if( *iv == v2 )
      {
        rc_ptr<BucketMap<Void> > bck;
        if( (*ie)->getProperty( "aims_junction", bck ) )
          printBucket( skel, bck, 80 );
        break;
      }
    }
}


int FoldArgOverSegment::subdivizeVertex( Vertex* v, float piecelength,
                                         size_t minsize,
                                         set<Vertex *> * newvertices )
{
  rc_ptr<BucketMap<Void> > ss, bottom, other, hjl;
  if( !v->getProperty( "aims_ss", ss ) || !ss )
  {
    cerr << "This fold seems not to have simple surface voxels. Aborting."
        << endl;
    return 0;
  }
  const BucketMap<Void>::Bucket & ss0 = ss->begin()->second;
  size_t totalsize = ss0.size();
  cout << "ss size: " << totalsize << endl;

  rc_ptr<BucketMap<int16_t> > iss( new BucketMap<int16_t> );
  iss->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  BucketMap<Void>::Bucket::const_iterator ib, eb = ss0.end();
  BucketMap<int16_t>::Bucket & iss0 = (*iss)[0];
  Connectivity c( 0, 0, Connectivity::CONNECTIVITY_6_XYZ );
  int i, n = c.nbNeighbors();
  for( ib=ss0.begin(); ib!=eb; ++ib )
  {
    iss0[ib->first] = 1;
    for( i=0; i<n; ++i )
      iss0[ ib->first + c.xyzOffset( i ) ] = 1;
  }
  BucketMap<int16_t>::Bucket dilsave = iss0;
  // random starting point
  iss0.begin()->second = 2;
  FastMarching<BucketMap<int16_t> > fm( Connectivity::CONNECTIVITY_26_XYZ );
  rc_ptr<BucketMap<float> > distmap = fm.doit( iss, 1, 2, 0 );
  BucketMap<float>::Bucket & distmap0 = (*distmap)[0];
  // max distance point
  float dmax = 0, dist;
  Point3d pmax;
  for( ib=ss0.begin(); ib!=eb; ++ib )
  {
    dist = distmap0[ib->first];
    if( dist >= dmax )
    {
      dmax = dist;
      pmax = ib->first;
    }
  }
  // 2nd distance map for max distance
  iss0 = dilsave;
  iss0[ pmax ] = 2;
  distmap = fm.doit( iss, 1, 2, 0 );
  BucketMap<float>::Bucket & distmap1 = (*distmap)[0];
  dmax = 0;
  Point3d pmax2;
  for( ib=ss0.begin(); ib!=eb; ++ib )
  {
    dist = distmap1[ib->first];
    if( dist >= dmax )
    {
      dmax = dist;
      pmax2 = ib->first;
    }
  }
  cout << "fold length: " << dmax << endl;
  cout << "exrtremities: " << pmax << ", " << pmax2 << endl;
  // path from pmax2 to pmax
  rc_ptr<BucketMap<Void> > path( downPath( *distmap, pmax2 ) );
  cout << "path length: " << (*path)[0].size() << endl;
  unsigned nb_pieces = (unsigned) dmax / piecelength;
  cout << "target num of pieces: " << nb_pieces << endl;
  if( nb_pieces < 2 )
  {
    cout << "no need to split." << endl;
    return 0;
  }

  // find closest point to the 1st cut point
  unsigned icut = 1;
  Vertex *v2 = 0;
  do
  {
    float pdist = dmax / nb_pieces * icut;
    cout << "cut at: " << pdist << endl;
    float dmax2 = numeric_limits<float>::max();
    for( ib=path->begin()->second.begin(), eb=path->begin()->second.end();
         ib!=eb; ++ib )
    {
      dist = fabs( distmap1[ib->first] - pdist );
      if( dist < dmax2 )
      {
        dmax2 = dist;
        pmax = ib->first;
      }
    }
    cout << "try to cut at pos: " << pmax << endl;
    v2 = splitVertex( v, pmax, minsize );
    ++icut;
  } // until a cut has succeeded or all attempts have failed
  while( !v2 && icut < nb_pieces );

  if( !v2 )
    return 0;
  if( newvertices )
    newvertices->insert( v2 );
  // restart recursively for both v and v2
  return 1 + subdivizeVertex( v, piecelength, minsize, newvertices )
      + subdivizeVertex( v2, piecelength, minsize, newvertices );
}


int FoldArgOverSegment::subdivizeGraph( float piecelength, size_t minsize,
                                        set<Vertex *> * newvertices )
{
  set<Vertex *> vert = _graph->vertices(); // copy vertex list
  set<Vertex *>::iterator iv, ev = vert.end();
  int nv = 0;
  for( iv=vert.begin(); iv!=ev; ++iv )
    nv += subdivizeVertex( *iv, piecelength, minsize, newvertices );
  return nv;
}

