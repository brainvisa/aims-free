
namespace aims
{

  /** Points repartition using forces in a given geometry (on a sphere for
      instance).

      adapted from: http://www.nicoptere.net/blog/index.php/2008/09/20/50-distribution-points-sphere-actionscript
      http://www.nicoptere.net/AS3/distribution/Distribute.as

      Points are moved according to forces, using an energy minimization
      method.

      A few parameters have been added, to allow fitting spheres, or "free"
      geometries and forces.

      Linked points can be defined, with different force behaviours.
  */
  class PointsDistribution
  {
%TypeHeaderCode
#include <aims/points_distribution/points_distribution.h>
%End

  public:
    /** Utility force class for sphere_distribution.

        Individual force and energy between 2 points
    */
    class ForceFunction
    {
    public:
      ForceFunction();
      virtual ~ForceFunction();
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
      MoveConstraints();
      virtual ~MoveConstraints();
      /// The default is unconstrained: just move the requested amount
      virtual Point3df position( const Point3df & pt, const Point3df & f,
                                 double step );
    };

    typedef vector_POINT3DF PointSet;
    typedef std::map<unsigned, std::set<unsigned> > LinkSet;

    /** Utility function for sphere_distribution.

        Individual Coulomb force between 2 points
    */
    class CoulombForce : public aims::PointsDistribution::ForceFunction
    {
    public:
      CoulombForce();
      virtual ~CoulombForce();
      /// Coulomb electrostatic force between 2 points (r_vec/r^3)
      virtual Point3df force( const Point3df & p1, const Point3df & p2,
                              bool has_link );
      /// Coulomb energy of the force between 2 points (1/r)
      virtual double energy( const Point3df & p1, const Point3df & p2,
                             bool has_link );
    };

    /** Utility function for sphere_distribution.

        Individual Coulomb force between 2 points, plus a restoring force that
        avoids points expanding away like the universe
    */
    class CoulombAndRestoringForce
      : public aims::PointsDistribution::ForceFunction
    {
    public:
      CoulombAndRestoringForce();
      virtual ~CoulombAndRestoringForce();
      virtual Point3df force( const Point3df & p1, const Point3df & p2,
                              bool has_link );
      virtual double energy( const Point3df & p1, const Point3df & p2,
                             bool has_link );
    };

    /** Move point constrained to a uinit sphere: project the force to be
        tangent to the sphere, move the point, then stick it onto the sphere.
    */
    class SphereMove : public aims::PointsDistribution::MoveConstraints
    {
    public:
      SphereMove();
      virtual ~SphereMove();
      virtual Point3df position( const Point3df & pt, const Point3df & f,
                                 double step );
    };

    /** Constructor.

        parameters
        ----------
        force:
            force / energy function between 2 points. Takes 2 points and a bool
            (has_link), and returns the force vector applied to the 1st point.
            Defaults to a Coulomb force (r_vec/r^3)
        move_constraints:
            applies force to a point, and add additional constraints.
            Takes a point, a force vector, and a step (factor to the force),
            returns the new point position.
            Default: projects the force to be tangent to a unit sphere, and
            renormalizes the resulting point position to stock on the sphere.
    */
    PointsDistribution(
      aims::PointsDistribution::ForceFunction *force /Transfer/ = 0 ,
      aims::PointsDistribution::MoveConstraints *move_constraint /Transfer/
        = 0 );
    ~PointsDistribution();

    /** Set linked points set

        Optional links. Linked points have different forces to make them
        closer. The links dict maps a point num to a set of linked points:
        {0: [1, 3], 1: [0, 5], 2: [4], 3: [0]}
        The map should be symmetrical.
    */
    void set_links( const aims::PointsDistribution::LinkSet & links );
    /// Set the individual force function
    void setForceFunction(
      aims::PointsDistribution::ForceFunction *force /Transfer/ );
    /// Set the individual move constraints function
    void setMoveConstraints(
      aims::PointsDistribution::MoveConstraints *move_constaint /Transfer/ );

    /// Randomly initialize npoints points on a unit sphere
    static aims::PointsDistribution::PointSet init_points( unsigned npoints );

    aims::PointsDistribution::PointSet *get_forces(
      const aims::PointsDistribution::PointSet & pts ) /Factory/;
    double get_coulomb_energy(
      const aims::PointsDistribution::PointSet & pts );
    /** get a points distribution on a sphere.


    parameters
    ----------
    points: list of 3D points (Point3df), or number
        If points is a number, this number of points in initialized randomly
        on a sphere.
    */
    aims::PointsDistribution::PointSet *distribute( const PointSet & pts,
                          unsigned nsteps=100,
                          double step=0.01 );
    /** Same as above but point are randomly initialized on a sphere,
        using the init_points() function
    */
    aims::PointsDistribution::PointSet *distribute( unsigned npoints,
                          unsigned nsteps = 100,
                          double step = 0.01 );
  };

};


