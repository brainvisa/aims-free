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


#ifndef AIMS_POINTS_DISTRIBUTION_POINTS_DISTRIBUTION_H
#define AIMS_POINTS_DISTRIBUTION_POINTS_DISTRIBUTION_H

#include <aims/vector/vector.h>

namespace aims
{

  class PointsDistribution
  {
  public:
    /** Utility force class for sphere_distribution.

        Individual force and energy between 2 points
    */
    class ForceFunction
    {
    public:
      ForceFunction() {}
      virtual ~ForceFunction() {}
      /// Force between 2 points
      virtual Point3df force( const Point3df & p1, const Point3df & p2,
                              bool has_link ) = 0;
      /** Energy of the force between 2 points

          Energy functions are the integral of corresponding forces -
          they are used to drive the minimization.
      */
      virtual double energy( const Point3df & p1, const Point3df & p2,
                             bool has_link ) = 0;
    };

    /** Utility function for sphere_distribution.

        Add force f * step to the returned point position pt, and optionally
        apply some specific position constraints (stick to sphere...)
    */
    class MoveConstraints
    {
    public:
      MoveConstraints() {}
      virtual ~MoveConstraints() {}
      /// The default is unconstrained: just move the requested amount
      virtual Point3df position( const Point3df & pt, const Point3df & f,
                                 double step )
      {
        return pt + f * step;
      }
    };

    typedef std::vector<Point3df> PointSet;
    typedef std::map<unsigned, std::set<unsigned> > LinkSet;

    /** Utility function for sphere_distribution.

        Individual Coulomb force between 2 points
    */
    class CoulombForce : public ForceFunction
    {
    public:
      CoulombForce() : ForceFunction() {}
      virtual ~CoulombForce() {}
      /// Coulomb electrostatic force between 2 points (1/r^2)
      virtual Point3df force( const Point3df & p1, const Point3df & p2,
                              bool has_link );
      /// Coulomb energy of the force between 2 points
      virtual double energy( const Point3df & p1, const Point3df & p2,
                             bool has_link );
    };

    /** Utility function for sphere_distribution.

        Individual Coulomb force between 2 points, plus a restoring force that
        avoids points expanding away like the universe
    */
    class CoulombAndRestoringForce : public ForceFunction
    {
    public:
      CoulombAndRestoringForce() : ForceFunction() {}
      virtual ~CoulombAndRestoringForce() {}
      virtual Point3df force( const Point3df & p1, const Point3df & p2,
                              bool has_link );
      virtual double energy( const Point3df & p1, const Point3df & p2,
                             bool has_link );
    };

    /** Move point constrained to a uinit sphere: project the force to be
        tangent to the sphere, move the point, then stick it onto the sphere.
    */
    class SphereMove : public MoveConstraints
    {
    public:
      SphereMove() : MoveConstraints() {}
      virtual ~SphereMove() {}
      virtual Point3df position( const Point3df & pt, const Point3df & f,
                                 double step );
    };

    PointsDistribution( ForceFunction *force = new CoulombForce,
                        MoveConstraints *move_constraint = new SphereMove );
    ~PointsDistribution();

    void set_links( const LinkSet & links );
    void setForceFunction( ForceFunction *force );
    void setMoveConstraints( MoveConstraints *move_constaint );

    /// Randomly initialize npoints points on a unit sphere
    static PointSet init_points( unsigned npoints );

    PointSet *get_forces( const PointSet & pts );
    double get_coulomb_energy( const PointSet & pts );
    /** get a points distribution on a sphere.

    adapted from: http://www.nicoptere.net/blog/index.php/2008/09/20/50-distribution-points-sphere-actionscript
    http://www.nicoptere.net/AS3/distribution/Distribute.as

    A few parameters have been added, to allow fitting spheres, or "free"
    geometries and forces.

    parameters
    ----------
    points: list of 3D points (Point3df), or number
        If points is a number, this number of points in initialized randomly
        on a sphere.
    links: dict
        Optional links. Linked points have different forces to make them
        closer. The links dict maps a point num to a set of linkes points:
        {0: [1, 3], 1: [0, 5], 2: [4], 3: [0]}
    indiv_force: function
        force function between 2 points. Takes 2 points and a bool (has_link),
        and returns the force vector applied to the 1st point.
        Defaults to a Coulomb force (r_vec/r^3)
    pos_constraints: function
        applies force to a point, and add additional constraints.
        Takes a point, a force vector, and a step (factor to the force),
        returns the new point position.
        Default: projects the force to be tangent to a unit sphere, and
        renormalizes the resulting point position to stock on the sphere.
    indiv_energy: function
        energy function between 2 points. Like the force, takes 2 points and
        a bool (has_link) as args, returns the force energy.
        (actually the opposite of the integral of indiv_force)
        Defaults to Coulomb energy (1/r)
    */
    PointSet *distribute( const PointSet & pts, unsigned nsteps=100,
                          double step=0.01 );
    /** Same as above but point are randomly initialized on a sphere,
        using the init_points() function
    */
    PointSet *distribute( unsigned npoints, unsigned nsteps = 100,
                          double step = 0.01 );

  private:
    ForceFunction *_force_function;
    MoveConstraints *_move_constraints;
    LinkSet _links;
  };

}

#endif



