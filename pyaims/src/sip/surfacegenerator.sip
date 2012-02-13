
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
  static carto::Object description();
  static void printDescription();

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
static icosphere( dictionary )
See the description() and printDescription() methods for more information.
%End

  static AimsSurfaceTriangle* icosphere( const Point3df &, float,
                                         unsigned=320 ) /Factory/;
%Docstring
static icosphere( Point3df center, float radius, unsigned nfaces=320 )

Compared to sphere() which performs meridian / parallels tesselation, icosphere over-segments an icosahedron to get equal, equilateral triangles.
%End
};

};

