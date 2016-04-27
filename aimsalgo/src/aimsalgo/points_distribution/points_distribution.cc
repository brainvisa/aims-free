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

#include <aims/points_distribution/points_distribution.h>
#include <cmath>
#include <stdlib.h>

using namespace aims;
using namespace std;


Point3df PointsDistribution::CoulombForce::force( const Point3df & p1,
                                                  const Point3df & p2,
                                                  bool has_link )
{
  Point3df r = p1 - p2;
  double l = r.norm();
  if( l < 1e-5 )
    l = 1e-5;
  l = 1. / (l * l * l);
  Point3df ff = r * l;
  if( has_link )
    // linked refs should be closer
    ff *= 0.1;
  return ff;
}


double PointsDistribution::CoulombForce::energy( const Point3df & p1,
                                                 const Point3df & p2,
                                                 bool has_link )
{
  double l = (p2 - p1).norm();
  if( has_link )
    l *= 10.;
  if( l < 1e-5 )
    l = 1e-5;
  return 1. / l;
}


Point3df PointsDistribution::CoulombAndRestoringForce::force(
  const Point3df & p1, const Point3df & p2, bool has_link )
{
  Point3df r = p1 - p2;
  double l = r.norm();
  if( l < 1e-5 )
    l = 1e-5;
  double attract = 1.;
  if( has_link )
    // linked refs should be closer
    attract = 3.;
  double l2 = .1 / (l * l) - attract * (l + 0.5) + 1.5;
  Point3df ff = r * l2 / l;
  return ff;
}


double PointsDistribution::CoulombAndRestoringForce::energy(
  const Point3df & p1, const Point3df & p2, bool has_link )
{
  double l = (p2 - p1).norm();
  double attract = 1.;
  if( has_link )
    attract = 3.;
  if( l < 1e-5 )
    l = 1e-5;
  double e = 1. / l + attract * l * l / 2. + attract * 0.5;
  return e;
}


Point3df PointsDistribution::SphereMove::position( const Point3df & pt,
                                                   const Point3df & f,
                                                   double step )
{
  double d = f.dot( pt );
  Point3df fp = f - pt * d;
  Point3df pt2 = pt + fp * step;
  pt2.normalize();
  return pt2;
}


PointsDistribution::PointsDistribution(
  ForceFunction *force,
  MoveConstraints *move_constraint )
  : _force_function( force ? force : new CoulombForce ),
    _move_constraints( move_constraint ? move_constraint : new SphereMove )
{
}


PointsDistribution::~PointsDistribution()
{
  delete _move_constraints;
  delete _force_function;
}

void PointsDistribution::set_links( const LinkSet & links )
{
  _links = links;
}


void PointsDistribution::setForceFunction( ForceFunction *force )
{
  if( _force_function != force )
    delete _force_function;
  _force_function = force;
}


void PointsDistribution::setMoveConstraints( MoveConstraints *move_constaint )
{
  if( _move_constraints != move_constaint )
    delete _move_constraints;
  _move_constraints = move_constaint;
}


namespace
{
  inline double frand()
  {
    return ((rand()-(RAND_MAX/2))/(RAND_MAX/2.));
  }
}

vector<Point3df> PointsDistribution::init_points( unsigned npoints )
{
  PointSet pts( npoints );
  unsigned i;
  for( i=0; i<npoints; ++i )
  {
    double lon = frand() * 2. * M_PI;
    double lat = frand() * M_PI - M_PI / 2.;
    double r = cos( lat );
    pts[i] = Point3df( r * cos( lon ), r * sin( lon ), sin( lat ) );
  }
  return pts;
}


namespace
{

  bool has_link( unsigned i, unsigned j,
                 const PointsDistribution::LinkSet & links )
  {
    PointsDistribution::LinkSet::const_iterator il = links.find( i );
    if( il == links.end() )
      return false;
    set<unsigned>::const_iterator is = il->second.find( j );
    if( is != il->second.end() )
        return true;
    return false;
  }

}


PointsDistribution::PointSet *PointsDistribution::get_forces(
  const PointSet & pts )
{
  unsigned np = pts.size();
  PointSet *f = new PointSet( np, Point3df( 0., 0., 0. ) );
  unsigned i, j;
  for( i=0; i<np; ++i )
    for( j=i+1; j<np; ++j )
    {
      Point3df ff = _force_function->force( pts[i], pts[j],
                                            has_link( i, j, _links ) );
      (*f)[i] += ff;
      (*f)[j] -= ff;
    }
  return f;
}


double PointsDistribution::get_coulomb_energy( const PointSet & pts )
{
  unsigned np = pts.size();
  double e = 0.;
  unsigned i, j;
  for( i=0; i<np; ++i )
    for( j=i+1; j<np; ++j )
      e += _force_function->energy( pts[i], pts[j], has_link( i, j, _links ) );
  return e;
}


PointsDistribution::PointSet *PointsDistribution::distribute(
  const PointSet & pts,
  unsigned nsteps, double step )
{
  float minimal_step = 1e-10;
  unsigned np = pts.size();

  PointSet *pp0 = new PointSet( pts );
  PointSet *pp1 = new PointSet( pts );
  double e0 = get_coulomb_energy( *pp0 );
  unsigned k, i;

  for( k=0; k<nsteps; ++k )
  {
    PointSet *f = get_forces( *pp0 );
    for( i=0; i<np; ++i )
      (*pp1)[i] = _move_constraints->position( (*pp0)[i], (*f)[i], step );
    delete f;
    double e = get_coulomb_energy( *pp1 );
    if( e >= e0 )
    {
      step /= 2.;
      if( step < minimal_step )
        break;
      continue;
    }
    else
    {
      PointSet *tmp = pp0;
      pp0 = pp1;
      pp1 = tmp;
      step *= 2.;
      e0 = e;
    }
  }
  delete pp1;
  return pp0;
}


PointsDistribution::PointSet *PointsDistribution::distribute(
  unsigned npoints, unsigned nsteps, double step )
{
  PointSet pts = init_points( npoints );
  return distribute( pts, nsteps, step );
}


