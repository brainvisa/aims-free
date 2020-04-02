
namespace aims
{

class SurfaceGenerator
{
%TypeHeaderCode
#include <aims/mesh/surfacegen.h>
%End

public:
%If (SIPTHROW_BUG)
  static AimsSurfaceTriangle* generate( carto::Object )
    throw( /* std::runtime_error, std::invalid_argument */ ) /Factory/;
%End
%If (!SIPTHROW_BUG)
  static AimsSurfaceTriangle* generate( carto::Object )
    throw( std::runtime_error, std::invalid_argument ) /Factory/;
%End

  static AimsSurfaceTriangle* generate( const carto::GenericObject & )
    throw( /* std::runtime_error, std::invalid_argument */ ) /Factory/;

  static AimsTimeSurface_2_VOID* generate_wireframe( carto::Object )
    throw( std::runtime_error, std::invalid_argument ) /Factory/;

  static AimsTimeSurface_2_VOID* generate_wireframe(
    const carto::GenericObject & )
    throw( /* std::runtime_error, std::invalid_argument */ ) /Factory/;

  static carto::Object description();
  static void printDescription();
  static carto::Object description_wireframe();
  static void printDescription_wireframe();

  static AimsSurfaceTriangle* cube( const carto::GenericObject & ) /Factory/;
%Docstring
static cube( dictionary )
See the description() and printDescription() methods for more information.
%End

  static AimsSurfaceTriangle* cube( const Point3df &, float, 
                                    bool=false ) /Factory/;
%Docstring
static cube( Point3df center, float radius, bool smoothnormals=False )
%End

  static AimsSurfaceTriangle* cylinder( const carto::GenericObject & ) 
    /Factory/;
%Docstring
static cylinder( dictionary )
See the description() and printDescription() methods for more information.
%End

  static AimsSurfaceTriangle* cylinder( const Point3df &, const Point3df &, 
                                        float, float, unsigned, bool, 
                                        bool=false ) /Factory/;
%Docstring
static cylinder( Point3df center1, Point3df center2, float radius1, float radius2, unsigned nfaces, bool closed, bool smooth=False )
%End

  static AimsSurfaceTriangle* cone( const carto::GenericObject & ) /Factory/;
%Docstring
static cone( dictionary )
See the description() and printDescription() methods for more information.
%End

  static AimsSurfaceTriangle* cone( const Point3df &, const Point3df &, float, 
                                    unsigned, bool, bool=false ) /Factory/;
%Docstring
static cone( Point3df arrowhead, Point3df base, float radius, unsigned nfaces, bool closed, bool smooth=False )
%End

  static AimsSurfaceTriangle* arrow( const carto::GenericObject & ) /Factory/;
%Docstring
static arrow( dictionary )
See the description() and printDescription() methods for more information.
%End

  static AimsSurfaceTriangle* arrow( const Point3df &, const Point3df &, float,
                                     float, unsigned, float ) /Factory/;
%Docstring
static arrow( Point3df arrowhead, Point3df base, float headradius, float arrowradius, unsigned nfaces, float arrowlengthfactor )
%End

  static AimsSurfaceTriangle* icosahedron( const carto::GenericObject & ) 
    /Factory/;
%Docstring
static icosahedron( dictionary )
See the description() and printDescription() methods for more information.
%End

  static AimsSurfaceTriangle* icosahedron( const Point3df &, float ) /Factory/;
%Docstring
static icosahedron( Point3df center, float radius )
%End

  static AimsSurfaceTriangle* sphere( const carto::GenericObject & ) /Factory/;
%Docstring
static sphere( dictionary )
See the description() and printDescription() methods for more information.
%End

  static AimsSurfaceTriangle* sphere( const Point3df &, float, 
                                      unsigned, bool=false ) /Factory/;
%Docstring
static sphere( Point3df center, float radius, unsigned nfaces, bool uniquevertices=False )
%End

  static AimsSurfaceTriangle* ellipse( const carto::GenericObject & ) /Factory/;
%Docstring
static ellipse( dictionary )
See the description() and printDescription() methods for more information.
%End

  static AimsSurfaceTriangle* ellipse( const Point3df &, float radius1,
                                       float radius2,
                                       unsigned, bool=false ) /Factory/;
%Docstring
static ellipse( Point3df center, float radius1, float radius2, unsigned nfaces, bool uniquevertices=False )
%End

  static AimsSurfaceTriangle* icosphere( const carto::GenericObject & ) /Factory/;
%Docstring
icosphere( dictionary )
See the description() and printDescription() methods for more information.
%End

  static AimsSurfaceTriangle* icosphere( const Point3df &, float,
                                         unsigned=320 ) /Factory/;
%Docstring
icosphere( Point3df center, float radius, unsigned nfaces=320 )

Compared to sphere() which performs meridian / parallels tesselation, icosphere over-segments an icosahedron to get equal, equilateral triangles.
%End

  static AimsTimeSurface_2_VOID*
  parallelepiped_wireframe( const carto::GenericObject & params ) /Factory/;
  static AimsTimeSurface_2_VOID* parallelepiped_wireframe(
    const Point3df & boundingbox_min, const Point3df & boundingbox_max ) /Factory/;
%Docstring
parallelepiped_wireframe( Point3df boundingbox_min, Point3df boundingbox_max )
%End

  static AimsSurfaceTriangle*
    parallelepiped( const carto::GenericObject & params ) /Factory/;
  static AimsSurfaceTriangle* parallelepiped(
    const Point3df & boundingbox_min, const Point3df & boundingbox_max,
    bool smooth = false ) /Factory/;
%Docstring
parallelepiped(boundingbox_min, boundingbox_max, smooth=False)

Parameters
----------
boundingbox_min: Point3df
    "lower bound" corner position
boundingbox_min: Point3df
    "upper bound" corner position
smooth: bool
    if True, make smooth faces and shared vertices in corners
%End

  static AimsTimeSurface_2_VOID*
  circle_wireframe( const carto::GenericObject & params ) /Factory/;
  static AimsTimeSurface_2_VOID* circle_wireframe(
    const Point3df & center, float radius, unsigned nseg=20,
    const Point3df & normal = Point3df( 0, 0, 1 ),
    const Point3df & startdir = Point3df( 1, 0, 0 ), float startangle = 0,
    float stopangle = M_PI*2 ) /Factory/;
%Docstring
Circle, or part of circle.
  center, radius are classical.
  nseg is the number of segments in the circle polygon
  normal is a normal vector to the circle plane.
  startdir is a vector in the circle plane determining the beginning
  of angles (circle ray).
  startangle, stopangle are meant to make parts of circle
%End

  static AimsTimeSurface_2_VOID*
  grid( const carto::GenericObject & params ) /Factory/;
%Docstring
grid(dict)
See the description() and printDescription() methods for more information.
%End

  static AimsTimeSurface_2_VOID* grid(
    const Point3df & boundingbox_min, const Point3df & boundingbox_max,
    const Point3df & grid_sampling ) /Factory/;
%Docstring
grid(Point3df boundingbox_min, Point3df boundingbox_max, Point3df grid_sampling)
Regular, wireframe grid
%End

};

};

