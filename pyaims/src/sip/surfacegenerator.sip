
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
  static AimsSurfaceTriangle* cube( const Point3df &, float, 
                                    bool=false ) /Factory/;
  static AimsSurfaceTriangle* cylinder( const carto::GenericObject & ) 
    /Factory/;
  static AimsSurfaceTriangle* cylinder( const Point3df &, const Point3df &, 
                                        float, float, unsigned, bool, 
                                        bool=false ) /Factory/;
  static AimsSurfaceTriangle* cone( const carto::GenericObject & ) /Factory/;
  static AimsSurfaceTriangle* cone( const Point3df &, const Point3df &, float, 
                                    unsigned, bool, bool=false ) /Factory/;
  static AimsSurfaceTriangle* arrow( const carto::GenericObject & ) /Factory/;
  static AimsSurfaceTriangle* arrow( const Point3df &, const Point3df &, float,
                                     float, unsigned, float ) /Factory/;
  static AimsSurfaceTriangle* icosahedron( const carto::GenericObject & ) 
    /Factory/;
  static AimsSurfaceTriangle* icosahedron( const Point3df &, float ) /Factory/;
  static AimsSurfaceTriangle* sphere( const carto::GenericObject & ) /Factory/;
  static AimsSurfaceTriangle* sphere( const Point3df &, float, 
                                      unsigned, bool=false ) /Factory/;
  static AimsSurfaceTriangle* ellipse( const carto::GenericObject & ) /Factory/;
  static AimsSurfaceTriangle* ellipse( const Point3df &, float radius1,
                                       float radius2,
                                       unsigned, bool=false ) /Factory/;
  static AimsSurfaceTriangle* icosphere( const carto::GenericObject & ) /Factory/;
  static AimsSurfaceTriangle* icosphere( const Point3df &, float,
                                         unsigned=320 ) /Factory/;
};

};

