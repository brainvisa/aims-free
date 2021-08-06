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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <fstream>
#include <aims/roi/roi_diff.h>
#include <aims/roi/roiIterator.h>

using namespace aims;
using namespace carto;
using namespace std;


struct RoiDiff::Private
{
  map<string, DiffStat> stats;
  DiffStat global;
  rc_ptr<Volume<int16_t> > vol2;
  rc_ptr<BucketMap<Void> > unmatching_voxels;
  vector<string> roi_names;
  map<string, int> roi_names_inv;
};


RoiDiff::RoiDiff()
  : d( new Private )
{
}


RoiDiff::~RoiDiff()
{
  delete d;
}


const map<string, RoiDiff::DiffStat> & RoiDiff::statsByLabel() const
{
  return d->stats;
}


const RoiDiff::DiffStat & RoiDiff::globalStats() const
{
  return d->global;
}


rc_ptr<BucketMap<Void> > RoiDiff::mismatch_bucket() const
{
  return d->unmatching_voxels;
}

rc_ptr<Volume<int16_t> > RoiDiff::graph2LabelVolume() const
{
  return d->vol2;
}


const std::vector<std::string> & RoiDiff::roiNames() const
{
  return d->roi_names;
}


const std::map<std::string, int> & RoiDiff::roiNamesInv() const
{
  return d->roi_names_inv;
}


void RoiDiff::diff( const Graph & g1, const Graph & g2 )
{
  rc_ptr<RoiIterator> roi_it2 = getRoiIterator( g2 );

  Point3d dim2 = roi_it2->volumeDimension();
  d->vol2.reset( new Volume<int16_t>( dim2 ) );
  Volume<int16_t> & vol2 = *d->vol2;
  vol2.header().setProperty( "voxel_size", g2.getProperty( "voxel_size" ) );
  vol2.fill( 0 );
  vector<string> & roi_names = d->roi_names;
  roi_names.clear();
  map<string, int> & roi_names_inv = d->roi_names_inv;
  roi_names_inv.clear();
  map<string, int>::const_iterator irn, ern = roi_names_inv.end();
  int num = 0;
  map<string, DiffStat> & stats = d->stats;
  stats.clear();

  for( roi_it2->restart(); roi_it2->isValid(); roi_it2->next() )
  {
    ++num;
    string name = roi_it2->regionName();
    roi_names.push_back( name );
    roi_names_inv[ name ] = num;

    DiffStat & ds = stats[ name ];
    BucketMap<Void>::Bucket & b0 = ds.g2_bucket[0];

    rc_ptr<MaskIterator> mit = roi_it2->maskIterator();
    for( ; mit->isValid(); mit->next() )
    {
      Point3d p = mit->value();
      vol2.at( p ) = num;
      b0[ p ] = Void();
    }
  }

  d->unmatching_voxels.reset( new BucketMap<Void> );
  BucketMap<Void> & g1_not_in_g2 = *d->unmatching_voxels;
  g1_not_in_g2.setSizeX( vol2.getVoxelSize()[0] );
  g1_not_in_g2.setSizeY( vol2.getVoxelSize()[1] );
  g1_not_in_g2.setSizeZ( vol2.getVoxelSize()[2] );
  BucketMap<Void>::Bucket & g1n2 = g1_not_in_g2[0];

  d->global = DiffStat(); // reset
  DiffStat & global = d->global;

  rc_ptr<RoiIterator> roi_it1 = getRoiIterator( g1 );
  for( roi_it1->restart(); roi_it1->isValid(); roi_it1->next() )
  {
    string name = roi_it1->regionName();
    irn = roi_names_inv.find( name );
    if( irn == ern )
    {
      num = roi_names.size() + 1;
      roi_names.push_back( name );
      roi_names_inv[ name ] = num;
    }
    else
      num = irn->second;

    DiffStat & ds = stats[ name ];

    rc_ptr<MaskIterator> mit = roi_it1->maskIterator();
    for( ; mit->isValid(); mit->next() )
    {
      Point3d p = mit->value();
      int16_t value = vol2.at( p );
      if( value == num )
        ++ds.matching_voxels;
      else
      {
        ++ds.unmatching_voxels;
        g1n2[p] = Void();
      }
    }
    if(  ds.matching_voxels + ds.unmatching_voxels + ds.g2_bucket[0].size()
         == 0 )
    {
      stats.erase( name );
      roi_names_inv.erase( name );
      roi_names.erase( roi_names.begin() + ( num - 1 ) ); // assume it is the last
      continue;
    }
    ds.dice = double( ds.matching_voxels ) * 2.
      / ( ds.matching_voxels + ds.unmatching_voxels + ds.g2_bucket[0].size() );

    global.matching_voxels += ds.matching_voxels;
    global.unmatching_voxels += ds.unmatching_voxels;
  }

  global.dice = double( global.matching_voxels )
    / ( global.matching_voxels + global.unmatching_voxels );
}


void RoiDiff::writeCSV( const std::string & name )
{
  ofstream s( name.c_str() );
  writeCSV( s );
}


namespace
{

  void writeTab( ostream & s, const string & str, unsigned width )
  {
    if( width == 0 )
      return;
    s << ", ";
    if( width > str.length() )
    {
      unsigned blanks = width - str.length();
      for( ; blanks > 0; --blanks )
        s << " ";
    }
  }


  void writeCol( ostream & s, unsigned num, unsigned width )
  {
    stringstream ss;
    ss << setw(6) << num;
    s << ss.str();
    writeTab( s, ss.str(), width );
  }


  void writeCol( ostream & s, double num, unsigned width )
  {
    stringstream ss;
    ss << setw(6) << setprecision(3) << num;
    s << ss.str();
    writeTab( s, ss.str(), width );
  }


  void writeCol( ostream & s, const string & str, unsigned width )
  {
    stringstream ss;
    s << str;
    writeTab( s, str, width );
  }

}


void RoiDiff::writeCSV( std::ostream & s )
{
  vector<string> cols( 7 );
  cols[0] = "region";
  cols[1] = "size1";
  cols[2] = "size2";
  cols[3] = "matching";
  cols[4] = "unmatching";
  cols[5] = "match_%";
  cols[6] = "dice";

  unsigned i, n = cols.size();
  vector<unsigned> clength( n );
  for( i=0; i<n; ++i )
    clength[i] = max( unsigned( cols[i].length() ), 8U );
  clength[ clength.size() - 1 ] = 0; // last col is free

  map<string, DiffStat>::iterator ids, eds = d->stats.end();
  for( ids=d->stats.begin(); ids!=eds; ++ids )
  {
    unsigned l = ids->first.length();
    if( l > clength[0] )
      clength[0] = l;
  }

  for( i=0; i<n; ++i )
  {
    writeCol( s, cols[i], clength[i] );
  }
  s << endl;

  for( ids=d->stats.begin(); ids!=eds; ++ids )
  {
    DiffStat & ds = ids->second;
    writeCol( s, ids->first, clength[0] );
    unsigned size1 = ds.matching_voxels + ds.unmatching_voxels;
    writeCol( s, size1, clength[1] );
    writeCol( s, unsigned( ds.g2_bucket[0].size() ), clength[2] );
    writeCol( s, ds.matching_voxels, clength[3] );
    writeCol( s, ds.unmatching_voxels, clength[4] );
    double perc = 0.;
    if( ds.matching_voxels + ds.unmatching_voxels != 0 )
      perc = double(ds.matching_voxels) * 100.
        / ( ds.matching_voxels + ds.unmatching_voxels );
    writeCol( s, perc, clength[5] );
    writeCol( s, ds.dice, clength[6] );
    s << endl;
  }

  writeCol( s, "global", clength[0] );
  writeCol( s, d->global.matching_voxels + d->global.unmatching_voxels,
            clength[1] );
  writeCol( s, d->global.matching_voxels + d->global.unmatching_voxels,
            clength[2] );
  writeCol( s, d->global.matching_voxels, clength[3] );
  writeCol( s, d->global.unmatching_voxels, clength[4] );
  double perc = 0.;
  if( d->global.matching_voxels + d->global.unmatching_voxels != 0 )
    perc = double(d->global.matching_voxels) * 100.
      / ( d->global.matching_voxels + d->global.unmatching_voxels );
  writeCol( s, perc, clength[5] );
  writeCol( s, d->global.dice, clength[6] );
  s << endl;
}

