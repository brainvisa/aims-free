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


#ifndef AIMS_SURFACE_SURFACEGEN_H
#define AIMS_SURFACE_SURFACEGEN_H

#include <aims/mesh/surface.h>
#include <cartobase/object/attributed.h>

namespace aims
{

  /// Factory class for various common shapes of meshes
  class SurfaceGenerator
  {
  public:
    typedef AimsSurfaceTriangle* 
    (*GenFunction)( const carto::GenericObject & );
    typedef AimsTimeSurface<2,Void>* 
    (*GenFunction_wireframe)( const carto::GenericObject & );

    struct Generator
    {
      virtual ~Generator() {}

      virtual AimsSurfaceTriangle* 
      generator( const carto::GenericObject & ) const = 0;
      virtual carto::Object parameters() const = 0;
    };

    struct Generator_wireframe
    {
      virtual ~Generator_wireframe() {}

      virtual AimsTimeSurface<2,Void>* 
      generator( const carto::GenericObject & ) const = 0;
      virtual carto::Object parameters() const = 0;
    };

    /// generic factory function (calls lower-level functions)
    static AimsSurfaceTriangle* 
    generate( const carto::GenericObject & params );
    /// generic factory function (calls lower-level functions)
    static AimsSurfaceTriangle* generate( carto::Object params );
    static AimsTimeSurface<2,Void>* 
    generate_wireframe( const carto::GenericObject & params );
    /// generic factory function (calls lower-level functions)
    static AimsTimeSurface<2,Void>* generate_wireframe( carto::Object params );

    /// description of known shapes and parameters (vector of dicts)
    static carto::Object description();
    static carto::Object description_wireframe();
    /// outputs the description() dictionary in a more huma-readable way
    static void printDescription( std::ostream & s = std::cout );
    static void printDescription_wireframe( std::ostream & s = std::cout );

    static AimsSurfaceTriangle* cube( const carto::GenericObject & params );
    static AimsSurfaceTriangle* cube( const Point3df & center, float radius, 
                                      bool smoothnormal = false );

    static AimsSurfaceTriangle* 
    cylinder( const carto::GenericObject & params );
    static AimsSurfaceTriangle* cylinder( const Point3df & p1, 
                                          const Point3df & p2, float radius, 
                                          float radius2, unsigned nfacets, 
                                          bool closed, bool smooth = false,
                                          bool smoooth_tube = true );

    static AimsSurfaceTriangle* cone( const carto::GenericObject & params );
    static AimsSurfaceTriangle* cone( const Point3df & arrow, 
                                      const Point3df & base, float radius, 
                                      unsigned nfacets, bool closed, 
                                      bool smooth = false );

    static AimsSurfaceTriangle* 
    arrow( const carto::GenericObject & params );
    static AimsSurfaceTriangle* arrow( const Point3df & arrow, 
                                        const Point3df & base, float radius, 
                                        float arrowradius, unsigned nfacets, 
                                        float arrowlengthfract );

    /** Sphere generation functions by Manik Bhattacharjee (CNRS UPR640 - LENA)

        Example of config file:
        \code
attributes = {
  'type' :  'sphere',
  'point1' : [ 11, 11, -10 ],
  'radius' : 45,
  'nfacets' : 100,
}        \endcode
     */
    static AimsSurfaceTriangle* 
      icosahedron( const carto::GenericObject & params );
    static AimsSurfaceTriangle* icosahedron( const Point3df & center, 
                                             float radius );

    static AimsSurfaceTriangle* 
      sphere( const carto::GenericObject & params );
    static AimsSurfaceTriangle* sphere( const Point3df & p1, float radius, 
                                        unsigned nfacets,
                                        bool uniquevertices = false );
    static AimsSurfaceTriangle*
      ellipse( const carto::GenericObject & params );
    static AimsSurfaceTriangle* ellipse( const Point3df & p1, float radius1,
                                         float radius2,
                                         unsigned nfacets,
                                         bool uniquevertices = false );
    static AimsSurfaceTriangle*
    icosphere( const carto::GenericObject & params );
    static AimsSurfaceTriangle* icosphere( const Point3df & center,
                                           float radius, unsigned nfacets );

    static AimsTimeSurface<2,Void>*
      parallelepiped_wireframe( const carto::GenericObject & params );
    static AimsTimeSurface<2,Void>* parallelepiped_wireframe( 
      const Point3df & boundingbox_min, const Point3df & boundingbox_max );

    static AimsSurfaceTriangle*
      parallelepiped( const carto::GenericObject & params );
    static AimsSurfaceTriangle* parallelepiped(
      const Point3df & boundingbox_min, const Point3df & boundingbox_max,
      bool smooth = false );

    static AimsTimeSurface<2,Void>*
      circle_wireframe( const carto::GenericObject & params );
    /** Circle, or part of circle.
     center, radius are classical.
     nseg is the number of segments in the circle polygon
     normal is a normal vector to the circle plane.
     startdir is a vector in the circle plane determining the beginning
     of angles (circle ray).
     startangle, stopangle are meant to make parts of circle
    */
    static AimsTimeSurface<2,Void>* circle_wireframe(
      const Point3df & center, float radius, unsigned nseg=20,
      const Point3df & normal = Point3df( 0, 0, 1 ),
      const Point3df & startdir = Point3df( 1, 0, 0 ), float startangle = 0,
      float stopangle = M_PI*2 );

    static AimsTimeSurface<2,Void>*
      grid( const carto::GenericObject & params );

    /** Grid wireframe.
     Similar to parallelepiped_wireframe, with a regular grid sampling.
    */
    static AimsTimeSurface<2,Void>* grid(
      const Point3df & boundingbox_min, const Point3df & boundingbox_max,
      const Point3df & grid_sampling );
  };

}


#endif
