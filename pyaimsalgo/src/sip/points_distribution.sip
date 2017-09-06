
namespace aims
{

  class PointsDistribution
  {
%TypeHeaderCode
#include <aims/points_distribution/points_distribution.h>
%End

%Docstring
    Points repartition using forces in a given geometry (on a sphere for
    instance).

    adapted from: http://www.nicoptere.net/blog/index.php/2008/09/20/50-distribution-points-sphere-actionscript
    http://www.nicoptere.net/AS3/distribution/Distribute.as

    Points are moved according to forces, using an energy minimization
    method.

    A few parameters have been added, to allow fitting spheres, or "free"
    geometries and forces.

    Linked points can be defined, with different force behaviours.
%End

  public:
    class ForceFunction
    {
    public:
%Docstring
    Utility force class for sphere_distribution.

        Individual force and energy between 2 points
%End
      ForceFunction();
      virtual ~ForceFunction();
      virtual Point3df force( const Point3df & p1, const Point3df & p2,
                              bool has_link ) = 0;
%Docstring
      force(p1, p2, has_link)

      Force between 2 points

      Parameters
      ----------
      p1: Point3df
          1st point
      p2: Point3df
          2nd point
      has_link: bool
          True if points p1 and p2 are linked and should be closer

      Returns
      -------
      force: Point3df
%End
      virtual double energy( const Point3df & p1, const Point3df & p2,
                             bool has_link ) = 0;
%Docstring
          Energy of the force between 2 points

          Energy functions are the integral of corresponding forces -
          they are used to drive the minimization.
%End
    };

    class MoveConstraints
    {
    public:
%Docstring
        Utility function for sphere_distribution.

        Add force f * step to the returned point position pt, and optionally
        apply some specific position constraints (stick to sphere...)
%End
      MoveConstraints();
      virtual ~MoveConstraints();
      virtual Point3df position( const Point3df & pt, const Point3df & f,
                                 double step );
%Docstring
      position(pt, f, step)

      The default is unconstrained: just move the requested amount

      Parameters
      ----------
      pt: Point3df
          point to be moved
      f: Point3df
          force which applies on the point
      step: float
          move step factor

    Returns
    -------
    new_pos: Point3df
        Position after move and application of constraints
%End
    };

    typedef vector_POINT3DF PointSet;
    typedef std::map<unsigned, std::set<unsigned> > LinkSet;

    class CoulombForce : aims::PointsDistribution::ForceFunction
    {
    public:
%Docstring
        Utility function for sphere_distribution.

        Individual Coulomb force between 2 points
%End
      CoulombForce();
      virtual ~CoulombForce();
      virtual Point3df force( const Point3df & p1, const Point3df & p2,
                              bool has_link );
%Docstring
      Coulomb electrostatic force between 2 points (r_vec/r^3)
%End
      virtual double energy( const Point3df & p1, const Point3df & p2,
                             bool has_link );
%Docstring
      Coulomb energy of the force between 2 points (1/r)
%End
    };

    class CoulombAndRestoringForce
      : aims::PointsDistribution::ForceFunction
    {
    public:
%Docstring
        Utility function for sphere_distribution.

        Individual Coulomb force between 2 points, plus a restoring force that
        avoids points expanding away like the universe
%End
      CoulombAndRestoringForce();
      virtual ~CoulombAndRestoringForce();
      virtual Point3df force( const Point3df & p1, const Point3df & p2,
                              bool has_link );
      virtual double energy( const Point3df & p1, const Point3df & p2,
                             bool has_link );
    };

    class SphereMove : aims::PointsDistribution::MoveConstraints
    {
    public:
%Docstring
        Move point constrained to a uinit sphere: project the force to be
        tangent to the sphere, move the point, then stick it onto the sphere.
%End
      SphereMove();
      virtual ~SphereMove();
      virtual Point3df position( const Point3df & pt, const Point3df & f,
                                 double step );
    };

    PointsDistribution(
      aims::PointsDistribution::ForceFunction *force /Transfer/ = 0 ,
      aims::PointsDistribution::MoveConstraints *move_constraint /Transfer/
        = 0 );
%Docstring
    PointsDistribution(force=None, move_constaint=None)

    parameters
    ----------
    force: aims.PointsDistribution.ForceFunction instance
        force / energy function between 2 points. Takes 2 points and a bool
        (has_link), and returns the force vector applied to the 1st point.
        Defaults to a Coulomb force (r_vec/r^3)
    move_constraints: aims.PointsDistribution.MoveConstraints instance
        applies force to a point, and add additional constraints.
        Takes a point, a force vector, and a step (factor to the force),
        returns the new point position.
        Default: projects the force to be tangent to a unit sphere, and
        renormalizes the resulting point position to stock on the sphere.
%End
    ~PointsDistribution();

    void set_links( const aims::PointsDistribution::LinkSet & links );
%Docstring
        set_links(links)

        Set linked points set

        Optional links. Linked points have different forces to make them
        closer. The links dict maps a point num to a set of linked points:
        {0: [1, 3], 1: [0, 5], 2: [4], 3: [0]}
        The map should be symmetrical.
%End

    void setForceFunction(
      aims::PointsDistribution::ForceFunction *force /Transfer/ );
%Docstring
      setForceFunction(force)

      Set the individual force function

      Parameters
      ----------
      force: aims.PointsDistribution.ForceFunction instance
%End
    void setMoveConstraints(
      aims::PointsDistribution::MoveConstraints *move_constaint /Transfer/ );
%Docstring
      setMoveConstraints(move_constaint)

      Set the individual move constraints function

      Parameters
      ----------
      move_constaint: aims.PointsDistribution.MoveConstraints instance
%End

    static aims::PointsDistribution::PointSet init_points( unsigned npoints );
%Docstring
      init_points(npoints)

      Randomly initialize npoints points on a unit sphere

      Parameters
      ----------
      npoints: int
          number of points to initialize

      Returns
      -------
      points: aims.vector_POINT3DF
          points 3D positions
%End

    aims::PointsDistribution::PointSet *get_forces(
      const aims::PointsDistribution::PointSet & pts ) /Factory/;
    double get_coulomb_energy(
      const aims::PointsDistribution::PointSet & pts );
    aims::PointsDistribution::PointSet *distribute( const PointSet & pts,
                          unsigned nsteps=100,
                          double step=0.01 );
%Docstring
    distribute(pts, nsteps=100, step=0.01)

    get a points distribution on a sphere.


    parameters
    ----------
    points: list of 3D points (Point3df)
    nsteps: max number of optimization iterations
    step: initial move step factor (is adapted during the minimization)

%End

    aims::PointsDistribution::PointSet *distribute( unsigned npoints,
                          unsigned nsteps = 100,
                          double step = 0.01 );
%Docstring
    distribute(npoints, nsteps=100, step=0.01)

    Same as above but point are randomly initialized on a sphere,
    using the init_points() function
%End
  };

};


