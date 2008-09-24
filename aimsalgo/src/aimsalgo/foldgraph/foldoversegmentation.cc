/* Copyright (c) 1995-2008 CEA
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


#include <aims/foldgraph/foldoversegmentation.h>
#include <graph/graph/graph.h>
#include <aims/bucket/bucket.h>
#include <aims/distancemap/fastmarching.h>
#include <aims/distancemap/downpath.h>
#include <aims/connectivity/component.h>
#include <aims/scalespace/bucketblob.h>
#include <aims/resampling/mask.h>
#include <aims/graph/graphmanip.h>

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
  void splitBucket( rc_ptr<BucketMap<Void> > & out1,
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

}


FoldArgOverSegment::FoldArgOverSegment( Graph* g )
  : _graph( g )
{
}


FoldArgOverSegment::~FoldArgOverSegment()
{
}


Vertex * FoldArgOverSegment::splitVertex( Vertex* v, const Point3d & pos0 )
{
  cout << "splitVertex...\n";
  // get initial buckets
  Point3d pos = pos0;
  rc_ptr<BucketMap<Void> > ss, bottom, other, hjl;
  v->getProperty( "aims_ss", ss );
  v->getProperty( "aims_bottom", bottom );
  v->getProperty( "aims_other", other );
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
    return 0;
  }
  if( !hj->getProperty( "aims_junction", hjl ) )
  {
    cerr << "vertex has no hull_junction: cannot split this way. abort.\n";
    return 0;
  }
  cout << "pos: " << pos << endl;

  // initial point: in bottom line
  BucketMap<Void>::Bucket & bot0 = (*bottom)[0];
  float dmin = numeric_limits<float>::max(), d;
  if( (*bottom)[0].find( pos ) == (*bottom)[0].end() )
  {
    // project it to the nearest bottom point
    BucketMap<Void>::Bucket::const_iterator ib, eb=bot0.end();
    Point3d pmin;
    for( ib=bot0.begin(); ib!=eb; ++ib )
    {
      d = ( pos - ib->first ).norm();
      if( d < dmin )
      {
        dmin = d;
        pmin = ib->first;
      }
    }
    pos = pmin;
    cout << "bottom pos: " << pos << endl;

    /* if( dmin > 5. )
      return 0;*/
  }

  // propagate across simple surface
  rc_ptr<BucketMap<float> > fss( new BucketMap<float> ); // dist map
  rc_ptr<BucketMap<int16_t> > iss( new BucketMap<int16_t> ); // seeds
  iss->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  BucketMap<Void>::Bucket & ss0 = (*ss)[0];
  BucketMap<Void>::Bucket & hj0 = (*hjl)[0];
  BucketMap<Void>::Bucket & oth0 = (*other)[0];
  BucketMap<Void>::Bucket::const_iterator ib, eb=ss0.end();
  BucketMap<int16_t>::Bucket & iss0 = (*iss)[0];
  for( ib=ss0.begin(); ib!=eb; ++ib )
    iss0[ib->first] = 1;
  for( ib=bot0.begin(), eb=bot0.end(); ib!=eb; ++ib )
    iss0[ib->first] = 1;
  for( ib=hj0.begin(), eb=hj0.end(); ib!=eb; ++ib )
    iss0[ib->first] = 1;
  iss0[pos] = 0.;

  rc_ptr<BucketMap<int16_t> > iss2( dilateBucket( *iss ) );

  FastMarching<BucketMap<int16_t> > fm( Connectivity::CONNECTIVITY_26_XYZ );
  fss = fm.doit( iss2, 1, 0, -100 );
  BucketMap<float>::Bucket & fss0 = (*fss)[0];
  BucketMap<float>::Bucket::iterator ibf, ebf, jbf;
  for( ibf=fss0.begin(), ebf=fss0.end(); ibf!=ebf; ++ibf )
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

  // find closest point on hull_junction
  Point3d hjmin;
  dmin = numeric_limits<float>::max();
  for( ib=hj0.begin(), eb=hj0.end(); ib!=eb; ++ib )
  {
    const Point3d & p = ib->first;
    d = fss0[p];
    if( dmin > d )
    {
      dmin = d;
      hjmin = p;
    }
  }
  cout << "hjmin: " << hjmin << endl;
  // split path
  rc_ptr<BucketMap<Void> > splitline( downPath( *fss, hjmin ) );

  // split simple surface

  rc_ptr<BucketMap<Void> > sssplit;
  rc_ptr<BucketMap<Void> > bottomsplit;
  rc_ptr<BucketMap<Void> > hjsplit;
  rc_ptr<BucketMap<Void> > othersplit;
  rc_ptr<BucketMap<Void> > dilline;
  eb=dilline->begin()->second.end();
  int nssdil = 0;
  AimsBucket<Void> sssplb, botsplb, hjsplb, otsplb;
  sssplb.setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  botsplb.setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  hjsplb.setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  otsplb.setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );

  AimsConnectedComponent( sssplb, *ss,
                          Connectivity::CONNECTIVITY_26_XYZ,
                          Void(), true, 0, 0, false );
  unsigned ncss = sssplb.size();
  rc_ptr<BucketMap<Void> > sscomp( new BucketMap<Void>( sssplb ) );
  sscomp->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  cout << "initial components:\n  ss: " << ncss << endl;

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

  cout << "ss split comps: " << sssplb.size() << ", ndil: " << nssdil << endl;
  *sssplit = sssplb;

  int i;
  for( i=0; i<nssdil; ++i ) // re-dilate ss
    sssplit.reset( dilateBucket( *sssplit ) );
  sssplit.reset( mask( *sssplit, *ss ) );
  sssplit.reset( mask( *sssplit, *splitline, false ) );

  if( sssplit->size() <= ncss )
  {
    cout << "could not split the simple surface - split aborting\n";
    return 0;
  }

  // recollect ss bits
  rc_ptr<BucketMap<Void> >
      ss1( new BucketMap<Void> ), ss2( new BucketMap<Void> );
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
      cout << "  bit " << ibm->first << endl;
      maxmatch = 0;
      i = 0;
      for( ibm2=sscomp->begin(), ebm2=sscomp->end(); ibm2!=ebm2; ++ibm2 )
      {
        cout << " -> comp: " << ibm2->first << endl;
        match = bucketMatch( ibm->second, ibm2->second );
        if( match > maxmatch )
        {
          maxmatch = match;
          i = ibm2->first;
        }
      }
      corr[i].insert( ibm->first );
    }
    cout << "correspondance map done\n";
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
      cout << "more than 2 parts in the same split\n";
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
      cout << "kept " << big1 << " and " << big2 << endl;
    }

    // aggregate remaining bits
    ebm2 = ebm;
    ibm = sssplit->begin();
    while( ibm != ebm )
    {
      cout << "aggregate " << ibm->first << "...\n";
      if( nums.find( ibm->first ) != nums.end() )
      {
        cout << "   is a target bit : continue\n";
        ++ibm;
        continue;
      }
      maxmatch = 0;
      i = 0;
      for( ibm2 = sssplit->begin(); ibm2!=ebm; ++ibm2 )
        if( ibm2 != ibm )
        {
          match = bucketMatch( ibm->second, ibm2->second, 2 );
          cout << "test " << ibm2->first << ", match = " << match << endl;
          if( match >= maxmatch )
          {
            cout << "increase max\n";
            maxmatch = match;
            i = ibm2->first;
          }
        }
      ibm2 = sssplit->find( i );
      if( ibm2 == ebm )
        cerr << "error: no match found\n";
      cout << "  -> to bit " << ibm2->first << endl;
      if( ibm == ibm2 )
        cerr << "error: ibm == ibm2\n";
      // add ibm to ibm2 (best matching)
      ibm2->second.insert( ibm->second.begin(), ibm->second.end() );
      cout << "   ... inserted\n";
      ibm2 = ibm;
      ++ibm;
      // delete current item
      sssplit->map<int, std::map<Point3d,Void,BucketMapLess> >::erase( ibm2 );
      cout << "   ... erased\n";
    }
    cout << "ss aggregation done: " << sssplit->size() << " bits at end\n";

    // copy both final bits
    ibm = sssplit->begin();
    (*ss1)[0] = ibm->second;
    ++ibm;
    (*ss2)[0] = ibm->second;
  }

  // voronoi
  rc_ptr<BucketMap<int16_t> > voronoi( new BucketMap<int16_t> );
  voronoi->setSizeXYZT( ss->sizeX(), ss->sizeY(), ss->sizeZ(), ss->sizeT() );
  BucketMap<int16_t>::Bucket & vor = (*voronoi)[0];
  for( ib=ss0.begin(), eb=ss0.end(); ib!=eb; ++ib )
    vor[ ib->first ] = 1;
  for( ib=bot0.begin(), eb=bot0.end(); ib!=eb; ++ib )
    vor[ ib->first ] = 1;
  for( ib=oth0.begin(), eb=oth0.end(); ib!=eb; ++ib )
    vor[ ib->first ] = 1;
  for( ib=hj0.begin(), eb=hj0.end(); ib!=eb; ++ib )
    vor[ ib->first ] = 1;
  for( ib=ss1->begin()->second.begin(), eb=ss1->begin()->second.end(); ib!=eb;
       ++ib )
    vor[ ib->first ] = 10;
  for( ib=ss2->begin()->second.begin(), eb=ss2->begin()->second.end(); ib!=eb;
       ++ib )
    vor[ ib->first ] = 11;
  set<int16_t> work, seeds;
  work.insert( 1 );
  seeds.insert( 10 );
  seeds.insert( 11 );
  voronoi.reset( dilateBucket( *voronoi ) );
  fm.doit( voronoi, work, seeds );
  voronoi = fm.voronoiVol();

  // split ss according to voronoi: definitive pass
  splitBucket( ss1, ss2, *ss, *voronoi, 10 );
  rc_ptr<BucketMap<Void> > bottom1, bottom2;
  splitBucket( bottom1, bottom2, *bottom, *voronoi, 10 );
  rc_ptr<BucketMap<Void> > hj1, hj2;
  splitBucket( hj1, hj2, *hjl, *voronoi, 10 );
  rc_ptr<BucketMap<Void> > other1, other2;
  splitBucket( other1, other2, *other, *voronoi, 10 );
  cout << "split done for all buckets\n";

  // create new Vertex and hull_junction

  Vertex *v2 = _graph->addVertex( v->getSyntax() );
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


rc_ptr<Volume<int16_t> > FoldArgOverSegment::argBackToSkeleton(
  const rc_ptr<Volume<int16_t> > oldskel )
{
  rc_ptr<Volume<int16_t> > skel( new Volume<int16_t>( *oldskel ) );

  // erase all
  Connectivity c( 0, 0, Connectivity::CONNECTIVITY_26_XYZ );
  int x, y, z, dx = oldskel->getSizeX(), dy = oldskel->getSizeY(),
    dz = oldskel->getSizeZ(), i, n = c.nbNeighbors();
  for( z=0; z<dz; ++z )
    for( y=0; y<dy; ++y )
      for( x=0; x<dx; ++x )
      {
        int16_t & val = skel->at( x, y, z );
        switch( val )
        {
        case 30: // bottom/other ?
          val = 11; // brain interior
          break;
        case 60: // simple surface
          for( i=0; i<n; ++i )
          {
            Point3d p( Point3d( x, y, z ) + c.xyzOffset( i ) );
            if( p[0] < 0 || p[1] < 0 || p[2] < 0 || p[0] >= dx || p[1] >= dy
              || p[2] >= dz || oldskel->at( p[0], p[1], p[2] ) == 0 )
              break; // connects exterior or border: keep as is
          }
          if( i == n )
            val = 11; // doesn't connect exterior: erase to interior value
          break;
        case 80: // junction
          val = 11;
          break;
        default:
          break;
        }
      }

  rc_ptr<BucketMap<Void> > bck;
  Graph::const_iterator iv, ev = _graph->end();
  for( iv=_graph->begin(); iv!=ev; ++iv )
  {
    if( (*iv)->getProperty( "aims_ss", bck ) )
      printBucket( skel, bck, 60 );
    if( (*iv)->getProperty( "aims_bottom", bck ) )
      printBucket( skel, bck, 30 );
    if( (*iv)->getProperty( "aims_other", bck ) )
      printBucket( skel, bck, 60 ); // ? or 30 ?
  }

  set<Edge *>::const_iterator ie, ee = _graph->edges().end();
  for( ie=_graph->edges().begin(); ie!=ee; ++ie )
  {
    if( (*ie)->getSyntax() == "junction"
      || (*ie)->getSyntax() == "hull_junction" )
    {
      if( (*ie)->getProperty( "aims_junction", bck ) )
        printBucket( skel, bck, 80 );
    }
    // else: do nothing, they will be recalculated
  }

  return skel;
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


