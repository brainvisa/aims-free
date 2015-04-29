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


#include <aims/mesh/surfacegen.h>
#include <aims/mesh/surfaceOperation.h>
#include <aims/vector/vector.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  struct CubeGen : public SurfaceGenerator::Generator
  {
    virtual AimsSurfaceTriangle* 
    generator( const carto::GenericObject & ) const;
    virtual Object parameters() const;
  };


  struct CylinderGen : public SurfaceGenerator::Generator
  {
    virtual AimsSurfaceTriangle* 
    generator( const carto::GenericObject & ) const;
    virtual Object parameters() const;
  };


  struct ConeGen : public SurfaceGenerator::Generator
  {
    virtual AimsSurfaceTriangle* 
    generator( const carto::GenericObject & ) const;
    virtual Object parameters() const;
  };


  struct ArrowGen : public SurfaceGenerator::Generator
  {
    virtual AimsSurfaceTriangle* 
    generator( const carto::GenericObject & ) const;
    virtual Object parameters() const;
  };


  struct IcosahedronGen : public SurfaceGenerator::Generator
  {
    virtual AimsSurfaceTriangle* 
    generator( const carto::GenericObject & ) const;
    virtual Object parameters() const;
  };


  struct SphereGen : public SurfaceGenerator::Generator
  {
    virtual AimsSurfaceTriangle* 
    generator( const carto::GenericObject & ) const;
    virtual Object parameters() const;
  };


  struct EllipseGen : public SurfaceGenerator::Generator
  {
    virtual AimsSurfaceTriangle*
    generator( const carto::GenericObject & ) const;
    virtual Object parameters() const;
  };


  struct IcosphereGen : public SurfaceGenerator::Generator
  {
    virtual AimsSurfaceTriangle*
    generator( const carto::GenericObject & ) const;
    virtual Object parameters() const;
  };


  struct ParallelepipedGen_wireframe 
    : public SurfaceGenerator::Generator_wireframe
  {
    virtual AimsTimeSurface<2,Void>*
    generator( const carto::GenericObject & ) const;
    virtual Object parameters() const;
  };


  struct CircleGen_wireframe
    : public SurfaceGenerator::Generator_wireframe
  {
    virtual AimsTimeSurface<2,Void>*
    generator( const carto::GenericObject & ) const;
    virtual Object parameters() const;
  };


  struct GridGen_wireframe
    : public SurfaceGenerator::Generator_wireframe
  {
    virtual AimsTimeSurface<2,Void>*
    generator( const carto::GenericObject & ) const;
    virtual Object parameters() const;
  };


  map<string, rc_ptr<SurfaceGenerator::Generator> > & generators()
  {
    static map<string,rc_ptr<SurfaceGenerator::Generator> >	functs;
    if( functs.empty() )
      {
        functs[ "cube"        ] 
          = rc_ptr<SurfaceGenerator::Generator>( new CubeGen );
        functs[ "cylinder"    ] 
          = rc_ptr<SurfaceGenerator::Generator>( new CylinderGen );
        functs[ "cone"        ] 
          = rc_ptr<SurfaceGenerator::Generator>( new ConeGen );
        functs[ "arrow"       ] 
          = rc_ptr<SurfaceGenerator::Generator>( new ArrowGen );
        functs[ "icosahedron" ] 
          = rc_ptr<SurfaceGenerator::Generator>( new IcosahedronGen );
        functs[ "sphere"      ] 
          = rc_ptr<SurfaceGenerator::Generator>( new SphereGen );
        functs[ "ellipse"      ]
          = rc_ptr<SurfaceGenerator::Generator>( new EllipseGen );
        functs[ "icosphere"      ]
          = rc_ptr<SurfaceGenerator::Generator>( new IcosphereGen );
      }
    return functs;
  }


  map<string, rc_ptr<SurfaceGenerator::Generator_wireframe> > & 
    generators_wireframe()
  {
    static map<string,rc_ptr<SurfaceGenerator::Generator_wireframe> >
      functs;
    if( functs.empty() )
    {
      functs[ "parallelepiped" ] 
        = rc_ptr<SurfaceGenerator::Generator_wireframe>( 
          new ParallelepipedGen_wireframe );
      functs[ "circle" ]
        = rc_ptr<SurfaceGenerator::Generator_wireframe>(
          new CircleGen_wireframe );
      functs[ "grid" ]
        = rc_ptr<SurfaceGenerator::Generator_wireframe>(
          new GridGen_wireframe );
    }
    return functs;
  }


  AimsSurfaceTriangle* 
  CubeGen::generator( const carto::GenericObject & x ) const
  {
    return SurfaceGenerator::cube( x );
  }


  Object CubeGen::parameters() const
  {
    Object		d = Object::value( Dictionary() );
    Dictionary	& p = d->value<Dictionary>();
    p[ "center" ] = Object::value( string( "3D position of the center" ) );
    p[ "radius" ] = Object::value( string( "half-length of the edge" ) );
    p[ "smooth" ] 
      = Object::value( string( "(optional) make smooth normals and shared " 
                               "vertices (default: 0)" ) );
    return d;
  }


  AimsSurfaceTriangle* 
  CylinderGen::generator( const carto::GenericObject & x ) const
  {
    return SurfaceGenerator::cylinder( x );
  }


  Object CylinderGen::parameters() const
  {
    Object		d = Object::value( Dictionary() );
    Dictionary	& p = d->value<Dictionary>();
    p[ "point1" ] 
      = Object::value( string( "3D position of the center of the 1st end" ) );
    p[ "point2" ]
      = Object::value( string( "3D position of the center of the 2nd end" ) );
    p[ "radius" ] = Object::value(  string( "radius of the 1st end" ) );
    p[ "radius2" ] 
      = Object::value( string( "(optional) radius of the 2nd end (default: " 
                               "same as radius)" ) );
    p[ "facets" ] 
      = Object::value( string( "(optional) number of facets of the cylinder " 
                               "section (default: 4)" ) );
    p[ "closed" ] 
      = Object::value( string( "(optional) if non-zero, make polygons for " 
                               "the cylinder ends (default: 0)" ) );
    p[ "smooth" ] 
      = Object::value( string( "(optional) make smooth normals and shared " 
                               "vertices (default: 0)" ) );
    return d;
  }


  AimsSurfaceTriangle* 
  ConeGen::generator( const carto::GenericObject & x ) const
  {
    return SurfaceGenerator::cone( x );
  }


  Object ConeGen::parameters() const
  {
    Object		d = Object::value( Dictionary() );
    Dictionary	& p = d->value<Dictionary>();
    p[ "point1" ] = Object::value( string( "3D position of the sharp end" ) );
    p[ "point2" ] 
      = Object::value( string( "3D position of the center of the other " 
                               "end" ) );
    p[ "radius" ] = Object::value( string( "radius of the 2nd end" ) );
    p[ "facets" ] 
      = Object::value( string( "(optional) number of facets of the cone " 
                               "section (default: 4)" ) );
    p[ "closed" ] 
      = Object::value( string( "(optional) if non-zero, make polygons for " 
                               "the cone end (default: 0)" ) );
    p[ "smooth" ] 
      = Object::value( string( "(optional) make smooth normals and shared " 
                               "vertices (default: 0)" ) );
    return d;
  }


  AimsSurfaceTriangle* 
  ArrowGen::generator( const carto::GenericObject & x ) const
  {
    return SurfaceGenerator::arrow( x );
  }


  Object ArrowGen::parameters() const
  {
    Object		d = Object::value( Dictionary() );
    Dictionary	& p = d->value<Dictionary>();
    p[ "point1" ] = Object::value( string( "3D position of the head" ) );
    p[ "point2" ] = Object::value( string( "3D position of the center of the " 
                                           "bottom" ) );
    p[ "radius" ] = Object::value( string( "radius of the head" ) );
    p[ "arrow_radius" ] = Object::value( string( "radius of the tail" ) );
    p[ "arrow_length_factor" ] = Object::value( string( "relative length of " 
                                                        "the head" ) );
    p[ "facets" ] = Object::value( string( "(optional) number of facets of " 
                                           "the cone section (default: 4)" ) );
    return d;
  }


  AimsSurfaceTriangle* 
  IcosahedronGen::generator( const carto::GenericObject & x ) const
  {
    return SurfaceGenerator::icosahedron( x );
  }


  Object IcosahedronGen::parameters() const
  {
    Object		d = Object::value( Dictionary() );
    Dictionary	& p = d->value<Dictionary>();
    p[ "center" ] = Object::value( string( "3D position of the center" ) );
    p[ "radius" ] = Object::value( string( "radius" ) );
    return d;
  }


  AimsSurfaceTriangle* 
  SphereGen::generator( const carto::GenericObject & x ) const
  {
    return SurfaceGenerator::sphere( x );
  }


  Object SphereGen::parameters() const
  {
    Object		d = Object::value( Dictionary() );
    Dictionary	& p = d->value<Dictionary>();
    p[ "center" ] = Object::value( string( "3D position of the center, may " 
                                           "also be specified as 'point1' " 
                                           "parameter" ) );
    p[ "radius" ] = Object::value( string( "radius" ) );
    p[ "facets" ] = Object::value( string( "(optional) number of facets of " 
                                           "the sphere. May also be specified "
                                           "as 'nfacets' parameter "
                                           "(default: 225)" ) );
/*    p[ "smooth" ] = Object::value( string( "(optional) make smooth normals "
                                           "and shared vertices (default: " 
                                           "0)" ) );*/
    p[ "uniquevertices" ] = Object::value( string( "(optional) if set to "
                                           "1, the pole vertices are not "
                                           "duplicated( default: 0)" ) );
    return d;
  }
  
  AimsSurfaceTriangle*
  EllipseGen::generator( const carto::GenericObject & x ) const
  {
      return SurfaceGenerator::ellipse( x );
  }

  Object EllipseGen::parameters() const
  {
    Object      d = Object::value( Dictionary() );
    Dictionary  & p = d->value<Dictionary>();
    p[ "center" ] = Object::value( string( "3D position of the center, may "
    "also be specified as 'point1' "
    "parameter" ) );
    p[ "radius1" ] = Object::value( string( "radius1" ) );
    p[ "radius2" ] = Object::value( string( "radius2" ) );
    p[ "facets" ] = Object::value( string( "(optional) number of facets of "
    "the sphere. May also be specified "
    "as 'nfacets' parameter "
    "(default: 225)" ) );
    /*    p[ "smooth" ] = Object::value( string( "(optional) make smooth normals "
    "and shared vertices (default: "
    "0)" ) );*/
    p[ "uniquevertices" ] = Object::value( string( "(optional) if set to "
    "1, the pole vertices are not "
    "duplicated( default: 0)" ) );
    return d;
  }


  AimsSurfaceTriangle*
  IcosphereGen::generator( const carto::GenericObject & x ) const
  {
    return SurfaceGenerator::icosphere( x );
  }


  Object IcosphereGen::parameters() const
  {
    Object              d = Object::value( Dictionary() );
    Dictionary  & p = d->value<Dictionary>();
    p[ "center" ] = Object::value( string( "3D position of the center, may "
                                           "also be specified as 'point1' "
                                           "parameter" ) );
    p[ "radius" ] = Object::value( string( "radius" ) );
    p[ "facets" ] = Object::value( string( "(optional) minimum number of "
                                           "facets of the sphere. (default: 30)"
                                   ) );
    return d;
  }


  AimsTimeSurface<2,Void>* 
  ParallelepipedGen_wireframe::generator( 
    const carto::GenericObject & x ) const
  {
    return SurfaceGenerator::parallelepiped_wireframe( x );
  }


  Object ParallelepipedGen_wireframe::parameters() const
  {
    Object              d = Object::value( Dictionary() );
    Dictionary  & p = d->value<Dictionary>();
    p[ "boundingbox_min" ] = Object::value( string( 
      "3D position of the lower bounding box" ) );
    p[ "boundingbox_max" ] = Object::value( string( 
      "3D position of the higher bounding box" ) );
    return d;
  }


  AimsTimeSurface<2,Void>*
  CircleGen_wireframe::generator(
    const carto::GenericObject & x ) const
  {
    return SurfaceGenerator::circle_wireframe( x );
  }


  Object CircleGen_wireframe::parameters() const
  {
    Object              d = Object::value( Dictionary() );
    Dictionary  & p = d->value<Dictionary>();
    p[ "center" ] = Object::value( string( "3D position of the center" ) );
    p[ "radius" ] = Object::value( string( "radius of the circle" ) );
    p[ "segments" ] = Object::value( string(
      "(optional) number of segments in the circle polygon (default: 20)" ) );
    p[ "normal" ] = Object::value( string(
      "(optional) normal vector to the circle plane (default: (0, 0, 1))" ) );
    p[ "start_direction" ] = Object::value( string(
      "(optional) vector in the circle plane determining the beginning of "
      "angles (circle ray) (default: (1, 0, 0))" ) );
    p[ "start_angle" ] = Object::value( string(
      "(optional) start angle for incomplete circle (default: 0)" ) );
    p[ "stop_angle" ] = Object::value( string(
      "(default) stop angle for incomplete circle (default: 2*pi)" ) );
    return d;
  }

  AimsTimeSurface<2,Void>*
  GridGen_wireframe::generator(
    const carto::GenericObject & x ) const
  {
    return SurfaceGenerator::grid( x );
  }


  Object GridGen_wireframe::parameters() const
  {
    Object              d = Object::value( Dictionary() );
    Dictionary  & p = d->value<Dictionary>();
    p[ "boundingbox_min" ] = Object::value( string(
      "3D position of the lower bounding box" ) );
    p[ "boundingbox_max" ] = Object::value( string(
      "3D position of the higher bounding box" ) );
    p[ "grid_sampling" ] = Object::value( string(
      "3D sampling interval" ) );
    return d;
  }

}


AimsSurfaceTriangle* 
SurfaceGenerator::generate( const Object params )
{
  return generate( *params );
}


AimsSurfaceTriangle* 
SurfaceGenerator::generate( const GenericObject & params )
{
  string	type = params.getProperty( "type" )->getString();

  const map<string,rc_ptr<Generator> >	& functs = generators();

  map<string,rc_ptr<Generator> >::const_iterator 
    ifn = functs.find( type );
  if( ifn != functs.end() )
    return ifn->second->generator( params );
  else
    {
      throw runtime_error(
        string( "SurfaceGenerator::generate: unknown shape type " ) );
    }
  return 0;
}


AimsTimeSurface<2,Void>* 
SurfaceGenerator::generate_wireframe( const Object params )
{
  return generate_wireframe( *params );
}


AimsTimeSurface<2,Void>*
SurfaceGenerator::generate_wireframe( const GenericObject & params )
{
  string        type = params.getProperty( "type" )->getString();

  const map<string,rc_ptr<Generator_wireframe> >  & 
    functs = generators_wireframe();

  map<string,rc_ptr<Generator_wireframe> >::const_iterator 
    ifn = functs.find( type );
  if( ifn != functs.end() )
    return ifn->second->generator( params );
  else
    {
      cerr << "SurfaceGenerator::generate_wireframe: unknown shape type " 
        << type << endl;
      throw runtime_error(
        string( "SurfaceGenerator::generate_wireframe: unknown shape type " )
        + type );
    }
  return 0;
}


Object SurfaceGenerator::description()
{
  const map<string,rc_ptr<Generator> >	& functs = generators();
  map<string,rc_ptr<Generator> >::const_iterator 
    ifn, efn = functs.end();
  Object				desc = Object::value( ObjectVector() );
  ObjectVector				& l = desc->value<ObjectVector>();
  Dictionary::const_iterator	id, ed;

  for( ifn=functs.begin(); ifn!=efn; ++ifn )
    {
      Object	o = Object::value( Dictionary() );
      Dictionary	& d = o->value<Dictionary>();
      l.push_back( o );
      d[ "type" ] = Object::value( ifn->first );

      Object		dx = ifn->second->parameters();
      Dictionary	& p = dx->value<Dictionary>();
      for( id=p.begin(), ed=p.end(); id!=ed; ++id )
        d[ id->first ] = id->second;
    }
  return desc;
}


void SurfaceGenerator::printDescription( ostream & ostr )
{
  Object                pardesc = description();
  ObjectVector          & pdl = pardesc->value<ObjectVector>();
  ObjectVector::iterator        ipd, epd = pdl.end();
  for( ipd=pdl.begin(); ipd!=epd; ++ipd )
  {
    Dictionary        & d = (*ipd)->value<Dictionary>();
    ostr << "\ntype : " << d[ "type" ]->getString() << endl;
    Dictionary::iterator      id, ed = d.end();
    for( id=d.begin(); id!=ed; ++id )
      if( id->first != "type" )
        ostr << id->first << " : " << id->second->getString() << endl;
  }
}


Object SurfaceGenerator::description_wireframe()
{
  const map<string,rc_ptr<Generator_wireframe> >  & 
    functs = generators_wireframe();
  map<string,rc_ptr<Generator_wireframe> >::const_iterator 
    ifn, efn = functs.end();
  Object                                desc = Object::value( ObjectVector() );
  ObjectVector                          & l = desc->value<ObjectVector>();
  Dictionary::const_iterator    id, ed;

  for( ifn=functs.begin(); ifn!=efn; ++ifn )
  {
    Object    o = Object::value( Dictionary() );
    Dictionary        & d = o->value<Dictionary>();
    l.push_back( o );
    d[ "type" ] = Object::value( ifn->first );

    Object            dx = ifn->second->parameters();
    Dictionary        & p = dx->value<Dictionary>();
    for( id=p.begin(), ed=p.end(); id!=ed; ++id )
      d[ id->first ] = id->second;
  }
  return desc;
}


void SurfaceGenerator::printDescription_wireframe( ostream & ostr )
{
  Object                pardesc = description_wireframe();
  ObjectVector          & pdl = pardesc->value<ObjectVector>();
  ObjectVector::iterator        ipd, epd = pdl.end();
  for( ipd=pdl.begin(); ipd!=epd; ++ipd )
  {
    Dictionary        & d = (*ipd)->value<Dictionary>();
    ostr << "\ntype : " << d[ "type" ]->getString() << endl;
    Dictionary::iterator      id, ed = d.end();
    for( id=d.begin(); id!=ed; ++id )
      if( id->first != "type" )
        ostr << id->first << " : " << id->second->getString() << endl;
  }
}


AimsSurfaceTriangle* SurfaceGenerator::cube( const GenericObject & params )
{
  Object	vp1;
  float		sz;
  bool		smth = false;
  vp1 = params.getProperty( "center" );
  sz = (float) params.getProperty( "radius" )->getScalar();
  try
    {
      smth = (bool) params.getProperty( "smooth" )->getScalar();
    }
  catch( exception & )
    {
    }
  return cube( Point3df( vp1->getArrayItem(0)->getScalar(),
               vp1->getArrayItem(1)->getScalar(),
               vp1->getArrayItem(2)->getScalar() ), sz, smth );
}


AimsSurfaceTriangle* SurfaceGenerator::cube( const Point3df & pos, 
					     float size, bool smooth )
{
  AimsSurfaceTriangle		*surf = new AimsSurfaceTriangle;
  AimsSurface<3,Void>		& s = (*surf)[0];
  vector<Point3df>		& vert = s.vertex();
  vector<Point3df>		& norm = s.normal();
  vector< AimsVector<uint,3> >	& poly = s.polygon();
  Point3df			v = pos;
  float				len = 0.58;

  if( smooth )
    {
      vert.reserve( 8 );
      norm.reserve( 8 );
      poly.reserve( 12 );

      v[0] -= size;
      v[1] -= size;
      v[2] -= size;
      vert.push_back( v );
      v[0] = pos[0] + size;
      vert.push_back( v );
      v[1] = pos[1] + size;
      vert.push_back( v );
      v[0] = pos[0] - size;
      vert.push_back( v );
      v[2] = pos[2] + size;
      vert.push_back( v );
      v[1] = pos[1] - size;
      vert.push_back( v );
      v[0] = pos[0] + size;
      vert.push_back( v );
      v[1] = pos[1] + size;
      vert.push_back( v );

      norm.push_back( Point3df( -len, -len, -len ) );
      norm.push_back( Point3df( len, -len, -len ) );
      norm.push_back( Point3df( len, len, -len ) );
      norm.push_back( Point3df( -len, len, -len ) );
      norm.push_back( Point3df( -len, len, len ) );
      norm.push_back( Point3df( -len, -len, len ) );
      norm.push_back( Point3df( len, -len, len ) );
      norm.push_back( Point3df( len, len, len ) );

      poly.push_back( AimsVector<uint,3>( 0, 2, 1 ) );
      poly.push_back( AimsVector<uint,3>( 0, 3, 2 ) );
      poly.push_back( AimsVector<uint,3>( 0, 1, 5 ) );
      poly.push_back( AimsVector<uint,3>( 1, 6, 5 ) );
      poly.push_back( AimsVector<uint,3>( 1, 2, 7 ) );
      poly.push_back( AimsVector<uint,3>( 1, 7, 6 ) );
      poly.push_back( AimsVector<uint,3>( 2, 3, 4 ) );
      poly.push_back( AimsVector<uint,3>( 2, 4, 7 ) );
      poly.push_back( AimsVector<uint,3>( 0, 5, 3 ) );
      poly.push_back( AimsVector<uint,3>( 3, 5, 4 ) );
      poly.push_back( AimsVector<uint,3>( 4, 5, 6 ) );
      poly.push_back( AimsVector<uint,3>( 6, 7, 4 ) );
    }
  else
    {
      vert.reserve( 24 );
      norm.reserve( 24 );
      poly.reserve( 12 );

      v[0] -= size;
      v[1] -= size;
      v[2] -= size;
      vert.push_back( v );
      vert.push_back( v );
      vert.push_back( v );
      v[0] = pos[0] + size;
      vert.push_back( v );
      vert.push_back( v );
      vert.push_back( v );
      v[1] = pos[1] + size;
      vert.push_back( v );
      vert.push_back( v );
      vert.push_back( v );
      v[0] = pos[0] - size;
      vert.push_back( v );
      vert.push_back( v );
      vert.push_back( v );
      v[2] = pos[2] + size;
      vert.push_back( v );
      vert.push_back( v );
      vert.push_back( v );
      v[1] = pos[1] - size;
      vert.push_back( v );
      vert.push_back( v );
      vert.push_back( v );
      v[0] = pos[0] + size;
      vert.push_back( v );
      vert.push_back( v );
      vert.push_back( v );
      v[1] = pos[1] + size;
      vert.push_back( v );
      vert.push_back( v );
      vert.push_back( v );

      norm.push_back( Point3df( 0, 0, -1 ) );
      norm.push_back( Point3df( 0, -1, 0 ) );
      norm.push_back( Point3df( -1, 0, 0 ) );

      norm.push_back( Point3df( 0, 0, -1 ) );
      norm.push_back( Point3df( 0, -1, 0 ) );
      norm.push_back( Point3df( 1, 0, 0 ) );

      norm.push_back( Point3df( 0, 0, -1 ) );
      norm.push_back( Point3df( 0, 1, 0 ) );
      norm.push_back( Point3df( 1, 0, 0 ) );

      norm.push_back( Point3df( 0, 0, -1 ) );
      norm.push_back( Point3df( 0, 1, 0 ) );
      norm.push_back( Point3df( -1, 0, 0 ) );

      norm.push_back( Point3df( 0, 0, 1 ) );
      norm.push_back( Point3df( 0, 1, 0 ) );
      norm.push_back( Point3df( -1, 0, 0 ) );

      norm.push_back( Point3df( 0, 0, 1 ) );
      norm.push_back( Point3df( 0, -1, 0 ) );
      norm.push_back( Point3df( -1, 0, 0 ) );

      norm.push_back( Point3df( 0, 0, 1 ) );
      norm.push_back( Point3df( 0, -1, 0 ) );
      norm.push_back( Point3df( 1, 0, 0 ) );

      norm.push_back( Point3df( 0, 0, 1 ) );
      norm.push_back( Point3df( 0, 1, 0 ) );
      norm.push_back( Point3df( 1, 0, 0 ) );

      poly.push_back( AimsVector<uint,3>( 0, 6, 3 ) );
      poly.push_back( AimsVector<uint,3>( 0, 9, 6 ) );
      poly.push_back( AimsVector<uint,3>( 1, 4, 16 ) );
      poly.push_back( AimsVector<uint,3>( 4, 19, 16 ) );
      poly.push_back( AimsVector<uint,3>( 5, 8, 23 ) );
      poly.push_back( AimsVector<uint,3>( 5, 23, 20 ) );
      poly.push_back( AimsVector<uint,3>( 7, 10, 13 ) );
      poly.push_back( AimsVector<uint,3>( 7, 13, 22 ) );
      poly.push_back( AimsVector<uint,3>( 2, 17, 11 ) );
      poly.push_back( AimsVector<uint,3>( 11, 17, 14 ) );
      poly.push_back( AimsVector<uint,3>( 12, 15, 18 ) );
      poly.push_back( AimsVector<uint,3>( 18, 21, 12 ) );
    }

  return( surf );
}


AimsSurfaceTriangle* 
SurfaceGenerator::cylinder( const GenericObject & params )
{
  Object	vp1, vp2;
  float		sz, sz2;
  unsigned	nf = 4;
  bool		closed = false;
  bool		smth = false;

  vp1 = params.getProperty( "point1" );
  vp2 = params.getProperty( "point2" );
  sz = (float) params.getProperty( "radius" )->getScalar();
  sz2 = sz;
  try
    {
      sz2 = (float) params.getProperty( "radius2" )->getScalar();
    }
  catch( exception & )
    {
    }
  try
    {
      nf = (unsigned) params.getProperty( "facets" )->getScalar();
    }
  catch( exception & )
    {
    }
  try
    {
      closed = (bool) params.getProperty( "closed" )->getScalar();
    }
  catch( exception & )
    {
    }
  try
    {
      smth = (bool) params.getProperty( "smooth" )->getScalar();
    }
  catch( exception & )
    {
    }
  return cylinder( Point3df( vp1->getArrayItem(0)->getScalar(),
                             vp1->getArrayItem(1)->getScalar(),
                             vp1->getArrayItem(2)->getScalar() ),
                   Point3df( vp2->getArrayItem(0)->getScalar(),
                             vp2->getArrayItem(1)->getScalar(),
                             vp2->getArrayItem(2)->getScalar() ),
                   sz, sz2, nf, closed, smth );
}


AimsSurfaceTriangle* 
SurfaceGenerator::cylinder( const Point3df & p1, const Point3df & p2, 
			    float size, float size2, unsigned nfacets, 
			    bool closed, bool smooth )
{
  if( nfacets == 0 )
    {
      nfacets = (unsigned) ( ( size + size2 + 10 ) * 0.25 );
      if( nfacets < 6 )
	nfacets = 6;
    }

  Point3df	dir = p2 - p1;
  dir.normalize();
  AimsSurfaceTriangle		*mesh = new AimsSurfaceTriangle;
  AimsSurface<3,Void>		& s = (*mesh)[0];
  vector<Point3df>		& vert = s.vertex();
  vector<Point3df>		& norm = s.normal();
  vector< AimsVector<uint,3> >	& poly = s.polygon();
  unsigned			i, nf = nfacets - 1;
  Point3df			plane1, plane2, d;
  float				angle;

  plane1 = vectProduct( dir, Point3df( 0, 0, 1 ) );
  if( plane1.norm() <= 1e-5 )
    plane1 = vectProduct( dir, Point3df( 0, 1, 0 ) );
  plane1.normalize();
  plane2 = vectProduct( dir, plane1 );
  plane2.normalize();

  if( closed )
    {
      if( smooth )
	{
	  vert.reserve( nfacets * 2 + 2 );
	  norm.reserve( nfacets * 2 + 2 );
	  poly.reserve( nfacets * 4);
	}
      else
	{
	  vert.reserve( nfacets * 4 + 2 );
	  norm.reserve( nfacets * 4 + 2 );
	  poly.reserve( nfacets * 4);
	}
    }
  else
    {
      vert.reserve( nfacets * 2 );
      norm.reserve( nfacets * 2 );
      poly.reserve( nfacets * 2 );
    }

  vert.push_back( p1 + plane1 * size );
  vert.push_back( p2 + plane1 * size2 );
  norm.push_back( plane1 );
  norm.push_back( plane1 );

  for( i=0; i<nf; ++i )
    {
      angle = M_PI * (i+1) * 2 / nfacets;
      d = plane1 * (float) cos( angle ) + plane2 * (float) sin( angle );
      norm.push_back( d );
      norm.push_back( d );
      vert.push_back( p1 + d * size );
      vert.push_back( p2 + d * size2 );
      poly.push_back( AimsVector<uint,3>( i*2, i*2+2, i*2+1 ) );
      poly.push_back( AimsVector<uint,3>( i*2+1, i*2+2, i*2+3 ) );
    }

  // last facet
  poly.push_back( AimsVector<uint,3>( nf*2, 0, nf*2+1 ) );
  poly.push_back( AimsVector<uint,3>( nf*2+1, 0, 1 ) );

  if( closed )
    {
      // make ends
      unsigned n = vert.size();
      vert.push_back( p1 );
      vert.push_back( p2 );
      norm.push_back( -dir );
      norm.push_back( dir );
      if( smooth )
	{
	  for( i=0; i<nf; ++i )
	    {
	      poly.push_back( AimsVector<uint,3>( i*2, n, i*2+2 ) );
	      poly.push_back( AimsVector<uint,3>( i*2+1, i*2+3, n+1 ) );
	    }
	  poly.push_back( AimsVector<uint,3>( nf*2, n, 0 ) );
	  poly.push_back( AimsVector<uint,3>( nf*2+1, 1, n+1 ) );
	}
      else
	{
	  for( i=0; i<nf; ++i )
	    {
	      vert.push_back( vert[i*2] );
	      vert.push_back( vert[i*2+1] );
	      norm.push_back( -dir );
	      norm.push_back( dir );
	      poly.push_back( AimsVector<uint,3>( n+i*2+2, n, n+i*2+4 ) );
	      poly.push_back( AimsVector<uint,3>( n+i*2+3, n+i*2+5, n+1 ) );
	    }
	  vert.push_back( vert[nf*2] );
	  vert.push_back( vert[nf*2+1] );
	  norm.push_back( -dir );
	  norm.push_back( dir );
	  poly.push_back( AimsVector<uint,3>( n+nf*2+2, n, n+2 ) );
	  poly.push_back( AimsVector<uint,3>( n+nf*2+3, n+3, n+1 ) );
	}
    }

  return mesh;
}


AimsSurfaceTriangle* SurfaceGenerator::cone( const GenericObject & params )
{
  Object	vp1, vp2;
  float		sz;
  unsigned	nf = 4;
  bool		closed = false;
  bool		smth = false;

  vp1 = params.getProperty( "point1" );
  vp2 = params.getProperty( "point2" );
  sz = (float) params.getProperty( "radius" )->getScalar();
  try
    {
      nf = (unsigned) params.getProperty( "facets" )->getScalar();
    }
  catch( exception & )
    {
    }
  try
    {
      closed = (bool) params.getProperty( "closed" )->getScalar();
    }
  catch( exception & )
    {
    }
  try
    {
      smth = (bool) params.getProperty( "smooth" )->getScalar();
    }
  catch( exception & )
    {
    }
  return cone( Point3df( vp1->getArrayItem(0)->getScalar(),
                         vp1->getArrayItem(1)->getScalar(),
                         vp1->getArrayItem(2)->getScalar() ),
               Point3df( vp2->getArrayItem(0)->getScalar(),
                         vp2->getArrayItem(1)->getScalar(),
                         vp2->getArrayItem(2)->getScalar() ),
               sz, nf, closed, smth );
}


AimsSurfaceTriangle* 
SurfaceGenerator::cone( const Point3df & p1, const Point3df & p2, 
			float size, unsigned nfacets, bool closed, 
			bool smooth )
{
  Point3df	dir = p2 - p1;
  dir.normalize();
  AimsSurfaceTriangle		*mesh = new AimsSurfaceTriangle;
  AimsSurface<3,Void>		& s = (*mesh)[0];
  vector<Point3df>		& vert = s.vertex();
  vector<Point3df>		& norm = s.normal();
  vector< AimsVector<uint,3> >	& poly = s.polygon();
  unsigned			i, nf = nfacets - 1;
  Point3df			plane1, plane2, d;
  float				angle;

  plane1 = vectProduct( dir, Point3df( 0, 0, 1 ) );
  if( plane1 == Point3df( 0, 0, 0 ) )
    plane1 = vectProduct( dir, Point3df( 0, 1, 0 ) );
  plane1.normalize();
  plane2 = vectProduct( dir, plane1 );
  plane2.normalize();

  if( closed )
    {
      if( smooth )
	{
	  vert.reserve( nfacets + 2 );
	  norm.reserve( nfacets + 2 );
	  poly.reserve( nfacets * 2 );
	}
      else
	{
	  vert.reserve( nfacets * 2 + 2 );
	  norm.reserve( nfacets * 2 + 2 );
	  poly.reserve( nfacets * 2 );
	}
    }
  else
    {
      vert.reserve( nfacets + 1 );
      norm.reserve( nfacets + 1 );
      poly.reserve( nfacets );
    }

  d = plane1 * size;
  vert.push_back( p1 );
  vert.push_back( p2 + d );
  norm.push_back( -dir );
  norm.push_back( plane1 );

  for( i=0; i<nf; ++i )
    {
      angle = M_PI * (i+1) * 2 / nfacets;
      d = plane1 * (float) cos( angle ) + plane2 * (float) sin( angle );
      norm.push_back( d );
      d *= size;
      vert.push_back( p2 + d );
      poly.push_back( AimsVector<uint,3>( 0, i+2, i+1 ) );
    }

  // last facet
  poly.push_back( AimsVector<uint,3>( 0, 1, nfacets ) );

  if( closed )
    {
      // make ends
      unsigned n = vert.size();
      vert.push_back( p2 );
      norm.push_back( dir );
      if( smooth )
	{
	  for( i=0; i<nf; ++i )
	    poly.push_back( AimsVector<uint,3>( n, i+1, i+2 ) );
	  poly.push_back( AimsVector<uint,3>( n, nfacets, 1 ) );
	}
      else
	{
	  for( i=0; i<nf; ++i )
	    {
	      vert.push_back( vert[i+1] );
	      norm.push_back( dir );
	      poly.push_back( AimsVector<uint,3>( n, n+i+1, n+i+2 ) );
	    }
	  vert.push_back( vert[nfacets] );
	  norm.push_back( dir );
	  poly.push_back( AimsVector<uint,3>( n, nfacets*2+1, n+1 ) );
	}
    }

  return mesh;
}


AimsSurfaceTriangle* 
SurfaceGenerator::arrow( const GenericObject & params )
{
  Object	vp1, vp2;
  float		sz, sz2, len;
  unsigned	nf = 4;
  vp1 = params.getProperty( "point1" );
  vp2 = params.getProperty( "point2" );
  sz = (float) params.getProperty( "radius" )->getScalar();
  sz2 = (float) params.getProperty( "arrow_radius" )->getScalar();
  len = (float) params.getProperty( "arrow_length_factor" )->getScalar();
  try
    {
      nf = (unsigned) params.getProperty( "facets" )->getScalar();
    }
  catch( exception & )
    {
    }
  return arrow( Point3df( vp1->getArrayItem(0)->getScalar(),
                          vp1->getArrayItem(1)->getScalar(),
                          vp1->getArrayItem(2)->getScalar() ),
                Point3df( vp2->getArrayItem(0)->getScalar(),
                          vp2->getArrayItem(1)->getScalar(),
                          vp2->getArrayItem(2)->getScalar() ),
                sz, sz2, nf, len );
}


AimsSurfaceTriangle* 
SurfaceGenerator::arrow( const Point3df & p1, const Point3df & p2, 
			 float size, float asize, unsigned nfacets, 
			 float alen )
{
  AimsSurfaceTriangle	*mesh, *m2;
  Point3df		bhead = p1 + ( p2 - p1 ) * alen;
  mesh = cone( p1, bhead, asize, nfacets, true );
  m2 = cylinder( bhead, p2, size, size, nfacets, true, false );
  SurfaceManip::meshMerge( *mesh, *m2 );
  delete m2;
  
  return mesh;
}


AimsSurfaceTriangle *
SurfaceGenerator::icosahedron( const GenericObject & params )
{
  Object	vp1;
  float		sz;
  vp1 = params.getProperty( "center" );
  sz = (float) params.getProperty( "radius" )->getScalar();

  return icosahedron( Point3df( vp1->getArrayItem(0)->getScalar(),
                      vp1->getArrayItem(1)->getScalar(),
                      vp1->getArrayItem(2)->getScalar() ), sz );
}


AimsSurfaceTriangle *
SurfaceGenerator::icosahedron( const Point3df & pos, float size )
{
  AimsSurfaceTriangle		*surf = new AimsSurfaceTriangle;
  AimsSurface<3,Void>		& s = (*surf)[0];
  vector<Point3df>		& vert = s.vertex();
  vector<Point3df>		& norm = s.normal();
  vector< AimsVector<uint,3> >	& poly = s.polygon();
  static const float		X = .525731112119133606;
  static const float		Z = .850650808352039932;
  float				x = size * X;
  float				z = size * Z;

  vert.reserve( 12 );
  norm.reserve( 12 );
  poly.reserve( 20 );

  vert.push_back( pos + Point3df( -x,0.0,z ) );
  vert.push_back( pos + Point3df( x,0.0,z ) );
  vert.push_back( pos + Point3df( -x,0.0,-z ) );
  vert.push_back( pos + Point3df( x,0.0,-z ) );
  vert.push_back( pos + Point3df( 0.0,z,x ) );
  vert.push_back( pos + Point3df( 0.0,z,-x ) );
  vert.push_back( pos + Point3df( 0.0,-z,x ) );
  vert.push_back( pos + Point3df( 0.0,-z,-x ) );
  vert.push_back( pos + Point3df( z,x,0.0 ) );
  vert.push_back( pos + Point3df( -z,x,0.0 ) );
  vert.push_back( pos + Point3df( z,-x,0.0 ) );
  vert.push_back( pos + Point3df( -z,-x,0.0 ) );

  norm.push_back( Point3df( -X,0.0,Z ) );
  norm.push_back( Point3df( X,0.0,Z ) );
  norm.push_back( Point3df( -X,0.0,-Z ) );
  norm.push_back( Point3df( X,0.0,-Z ) );
  norm.push_back( Point3df( 0.0,Z,X ) );
  norm.push_back( Point3df( 0.0,Z,-X ) );
  norm.push_back( Point3df( 0.0,-Z,X ) );
  norm.push_back( Point3df( 0.0,-Z,-X ) );
  norm.push_back( Point3df( Z,X,0.0 ) );
  norm.push_back( Point3df( -Z,X,0.0 ) );
  norm.push_back( Point3df( Z,-X,0.0 ) );
  norm.push_back( Point3df( -Z,-X,0.0 ) );

  poly.push_back( AimsVector<uint,3>( 1,4,0 ) );
  poly.push_back( AimsVector<uint,3>( 4,9,0 ) );
  poly.push_back( AimsVector<uint,3>( 4,5,9 ) );
  poly.push_back( AimsVector<uint,3>( 8,5,4 ) );
  poly.push_back( AimsVector<uint,3>( 1,8,4 ) );
  poly.push_back( AimsVector<uint,3>( 1,10,8 ) );
  poly.push_back( AimsVector<uint,3>( 10,3,8 ) );
  poly.push_back( AimsVector<uint,3>( 8,3,5 ) );
  poly.push_back( AimsVector<uint,3>( 3,2,5 ) );
  poly.push_back( AimsVector<uint,3>( 3,7,2 ) );
  poly.push_back( AimsVector<uint,3>( 3,10,7 ) );
  poly.push_back( AimsVector<uint,3>( 10,6,7 ) );
  poly.push_back( AimsVector<uint,3>( 6,11,7 ) );
  poly.push_back( AimsVector<uint,3>( 6,0,11 ) );
  poly.push_back( AimsVector<uint,3>( 6,1,0 ) );
  poly.push_back( AimsVector<uint,3>( 10,1,6 ) );
  poly.push_back( AimsVector<uint,3>( 11,0,9 ) );
  poly.push_back( AimsVector<uint,3>( 2,11,9 ) );
  poly.push_back( AimsVector<uint,3>( 5,2,9 ) );
  poly.push_back( AimsVector<uint,3>( 11,2,7 ) );

  return surf;
}


// Sphere generation code by Manik Bhattacharjee (CNRS UPR640 - LENA)

AimsSurfaceTriangle* 
SurfaceGenerator::sphere( const GenericObject & params )
{
  Object        vp1;
  float         radius;
  unsigned      nfacets = 15*15;
  bool          uniquevert = false;
  // bool          smth = false;

  try
    { 
      vp1 = params.getProperty( "center" );
    }
  catch( exception & )
    {
      vp1 = params.getProperty( "point1" );
    }
  radius = (float) params.getProperty( "radius" )->getScalar();

  try
    {
      nfacets = (unsigned) params.getProperty( "facets" )->getScalar();
    }
  catch( exception & )
    {
      try
        {
          nfacets = (unsigned) params.getProperty( "nfacets" )->getScalar();
        }
      catch( exception & )
        {
        }
    }

  try
    {
      uniquevert = (bool) params.getProperty( "uniquevertices" )->getScalar();
    }
  catch( exception & )
    {
    }

  Point3df p;
  int i = 0;
  Object it;
  for( it=vp1->objectIterator(); it->isValid() && i < 3; ++i, it->next() )
    p[i] = it->currentValue()->getScalar();

  return sphere(p, radius, nfacets, uniquevert );
}


AimsSurfaceTriangle* 
SurfaceGenerator::sphere( const Point3df & p1, float radius, 
                          unsigned nfacets, bool uniquevertices )
{
  int                           j = 0, k = 0, nslices, ncircles;
  float                         normZ, z, currentRadius, angle, angle2;
  AimsSurfaceTriangle           *mesh = new AimsSurfaceTriangle;
  AimsSurface<3,Void>           & s = (*mesh)[0];
  vector<Point3df>              & vert = s.vertex();
  vector<Point3df>              & norm = s.normal();
  vector< AimsVector<uint,3> >  & poly = s.polygon();
  bool                          singlevertex = false, genvertex = true;

  //Checking parameters
  nslices = ncircles = (int)sqrt((float)nfacets);
  if( ncircles <= 3 )
    {
      ncircles = 8;
    }
  if(nslices <= 3)
    {
      nslices = 8;
    }

  //We reserve memory space (more than necessary) to avoid reallocation of
  //vectors

  vert.reserve(nfacets);
  poly.reserve(nfacets);
  //No normals
  norm.clear();

  //Along the Z axis, one circle after the other

  angle2 = -M_PI/2; //From -M_PI/2 to M_PI/2
  for( int v=0; v<ncircles; v++ )
    {
      if(v+1 >= ncircles )//we just need to close the sphere
      {
        angle2 = M_PI/2;
      }

      //Parameters of the circle to create
      normZ = sin(angle2);//normalized Z of the current circle
      z = radius *normZ;// real Z
      //radius of the current circle
      currentRadius = radius*sqrt(1-normZ*normZ);

      k = j + nslices;
      if( uniquevertices )
      {
        genvertex = true;
        if( v == 0 )
        {
          Point3df p( 0., 0., z );
          vert.push_back(p + p1);
          singlevertex = true;
          genvertex = false;
          k = 1;
        }
        else if( v == ncircles - 1 )
        {
          Point3df p( 0., 0., z );
          vert.push_back(p + p1);
          singlevertex = true;
          genvertex = false;
          break;
        }
        else
        {
          if( v == ncircles - 2 )
            singlevertex = true;
          else
            singlevertex = false;
          j = 1 + (v-1) * nslices;
          k = j + nslices;
        }
      }
      else
        singlevertex = false;

      //Creating the circle
      angle = 0; // from 0 to 2*M_PI
      for(int u = 0; u < nslices; u++)
      {
        if( genvertex )
        {
          Point3df p(currentRadius * cos(angle), currentRadius*sin(angle),z);
          vert.push_back(p + p1);
        }

        //Last circle, no need to make the triangles
        if(v+1 >= ncircles)
        {
          j++;
          continue; 
        }

        // Triangles
        if(u +1 >= nslices)//Last point of this circle : triangle between the
          // first and last points of the circle
        {
          if( singlevertex )
          {
            if( v == 0 )
            {
              poly.push_back(AimsVector<uint,3>( 0, k-nslices + 1, k));
              ++k;
            }
            else
            {
              poly.push_back(AimsVector<uint,3>( j, j-nslices + 1, k));
              ++j;
            }
          }
          else
          {
            poly.push_back(AimsVector<uint,3>( j, j-nslices + 1, k));
            poly.push_back(AimsVector<uint,3>( j-nslices + 1,  j+1, k));
            // next point
            ++j;
            ++k;
          }

        }
        else
        {
          if( singlevertex )
          {
            if( v == 0 )
            {
              poly.push_back(AimsVector<uint,3>(0, k +1, k));
              ++k;
            }
            else
            {
              poly.push_back(AimsVector<uint,3>(j, j+1, k ));
              ++j;
            }
          }
          else
          {
            poly.push_back(AimsVector<uint,3>(j, j+1, k ));
            poly.push_back(AimsVector<uint,3>(j+1, k +1, k));
            // next point
            ++j;
            ++k;
          }
        }


        angle += 2*M_PI/(float)nslices;//updating the angle for the next point
        // of the circle
      }
      angle2 += M_PI/(float)(ncircles-1); //updating the angle for the next
      // circle. -1 because both ends of the sphere are circles of radius=0
    }

    // Creating normals
    mesh->updateNormals();

  return mesh;
}

AimsSurfaceTriangle* SurfaceGenerator::ellipse( const carto::GenericObject & params )
{
  Object        vp1;
  float         radius1, radius2;
  unsigned      nfacets = 15*15;
  bool          uniquevert = false;
  // bool          smth = false;

  try
  {
    vp1 = params.getProperty( "center" );
  }
  catch( exception & )
  {
    vp1 = params.getProperty( "point1" );
  }
  radius1 = (float) params.getProperty( "radius1" )->getScalar();
  radius2 = (float) params.getProperty( "radius2" )->getScalar();

  try
  {
    nfacets = (unsigned) params.getProperty( "facets" )->getScalar();
  }
  catch( exception & )
  {
    try
    {
      nfacets = (unsigned) params.getProperty( "nfacets" )->getScalar();
    }
    catch( exception & )
    {
    }
  }

  try
  {
    uniquevert = (bool) params.getProperty( "uniquevertices" )->getScalar();
  }
  catch( exception & )
  {
  }

  Point3df p;
  int i = 0;
  Object it;
  for( it=vp1->objectIterator(); it->isValid() && i < 3; ++i, it->next() )
    p[i] = it->currentValue()->getScalar();

  return ellipse(p, radius1, radius2, nfacets, uniquevert );

}


AimsSurfaceTriangle* SurfaceGenerator::ellipse( const Point3df & p1,
                                                float radius1, float radius2,
                                                unsigned nfacets,
                                                bool uniquevertices )
{
  AimsSurfaceTriangle           *mesh = new AimsSurfaceTriangle;
  mesh = sphere(p1, radius1, nfacets, uniquevertices);
  for ( uint i = 0 ; i < (*mesh)[0].vertex().size() ; i++ )
  {
    (*mesh)[0].vertex()[i] -= p1;
    (*mesh)[0].vertex()[i][1] /= radius1 / radius2;
    (*mesh)[0].vertex()[i] += p1;
  }
  return mesh;
}


AimsSurfaceTriangle* SurfaceGenerator::icosphere(
  const carto::GenericObject & params )
{
  Object        vp1;
  float         radius;
  unsigned      nfacets = 320;

  try
  {
    vp1 = params.getProperty( "center" );
  }
  catch( exception & )
  {
    vp1 = params.getProperty( "point1" );
  }
  radius = (float) params.getProperty( "radius" )->getScalar();

  try
  {
    nfacets = (unsigned) params.getProperty( "facets" )->getScalar();
  }
  catch( exception & )
  {
    try
    {
      nfacets = (unsigned) params.getProperty( "nfacets" )->getScalar();
    }
    catch( exception & )
    {
    }
  }

  Point3df p;
  int i = 0;
  Object it;
  for( it=vp1->objectIterator(); it->isValid() && i < 3; ++i, it->next() )
    p[i] = it->currentValue()->getScalar();

  return icosphere( p, radius, nfacets );
}


AimsSurfaceTriangle* SurfaceGenerator::icosphere( const Point3df & p1,
                                                  float radius,
                                                  unsigned nfacets )
{
  AimsSurfaceTriangle* ico = icosahedron( p1, radius );
  // refine triangles until the needed number of polygons is reached
  while( ico->polygon().size() < nfacets )
  {
    AimsSurfaceTriangle* oldico = ico;
    ico = SurfaceManip::refineMeshTri4( *oldico );
    delete oldico;
  }
  // re-project vertices onto the sphere
  vector<Point3df>::iterator iv, ev = ico->vertex().end();
  Point3df p2;
  for( iv=ico->vertex().begin(); iv!=ev; ++iv )
  {
    p2 = *iv - p1;
    *iv = p1 + p2 / p2.norm() * radius;
  }
  return ico;
}


AimsTimeSurface<2,Void>* SurfaceGenerator::parallelepiped_wireframe(
  const carto::GenericObject & params )
{
  Object        vp1;
  Point3df      corner1, corner2;

  vp1 = params.getProperty( "boundingbox_min" );

  int i = 0;
  Object it;
  for( it=vp1->objectIterator(); it->isValid() && i < 3; ++i, it->next() )
    corner1[i] = it->currentValue()->getScalar();
  if( i != 3 )
    cerr << "parallelepiped_wireframe needs 3 coords in boundingbox_min\n";

  vp1 = params.getProperty( "boundingbox_max" );
  for( i=0, it=vp1->objectIterator(); it->isValid() && i < 3; ++i, it->next() )
    corner2[i] = it->currentValue()->getScalar();
  if( i != 3 )
    cerr << "parallelepiped_wireframe needs 3 coords in boundingbox_max\n";

  return parallelepiped_wireframe( corner1, corner2 );
}


AimsTimeSurface<2,Void>* SurfaceGenerator::parallelepiped_wireframe(
  const Point3df & corner1, const Point3df & corner2 )
{
  AimsTimeSurface<2,Void> *mesh = new AimsTimeSurface<2,Void>;
  AimsSurface<2,Void> & surf = (*mesh)[0];

  vector<Point3df> & vert = surf.vertex();
  vert.reserve( 8 );
  vert.push_back( corner1 );
  vert.push_back( Point3df( corner2[0], corner1[1], corner1[2] ) );
  vert.push_back( Point3df( corner1[0], corner2[1], corner1[2] ) );
  vert.push_back( Point3df( corner1[0], corner1[1], corner2[2] ) );
  vert.push_back( Point3df( corner2[0], corner2[1], corner1[2] ) );
  vert.push_back( Point3df( corner2[0], corner1[1], corner2[2] ) );
  vert.push_back( Point3df( corner1[0], corner2[1], corner2[2] ) );
  vert.push_back( corner2 );

  vector<AimsVector<uint32_t, 2> > & pol = surf.polygon();
  pol.reserve( 12 );
  pol.push_back( AimsVector<uint32_t, 2>( 0, 1 ) );
  pol.push_back( AimsVector<uint32_t, 2>( 0, 2 ) );
  pol.push_back( AimsVector<uint32_t, 2>( 1, 4 ) );
  pol.push_back( AimsVector<uint32_t, 2>( 2, 4 ) );
  pol.push_back( AimsVector<uint32_t, 2>( 0, 3 ) );
  pol.push_back( AimsVector<uint32_t, 2>( 1, 5 ) );
  pol.push_back( AimsVector<uint32_t, 2>( 2, 6 ) );
  pol.push_back( AimsVector<uint32_t, 2>( 4, 7 ) );
  pol.push_back( AimsVector<uint32_t, 2>( 3, 5 ) );
  pol.push_back( AimsVector<uint32_t, 2>( 3, 6 ) );
  pol.push_back( AimsVector<uint32_t, 2>( 5, 7 ) );
  pol.push_back( AimsVector<uint32_t, 2>( 6, 7 ) );

  return mesh;
}


AimsTimeSurface<2,Void>*
SurfaceGenerator::circle_wireframe( const GenericObject & params )
{
  Object        oc, on, od;
  Point3df      center, normal = Point3df( 0, 0, 1 ),
    startdir = Point3df( 1, 0, 0 );
  float         radius, startangle = 0, stopangle = M_PI * 2;
  unsigned      nseg = 20;

  oc = params.getProperty( "center" );

  int i;
  Object it;
  for( i=0, it=oc->objectIterator(); it->isValid() && i < 3; ++i, it->next() )
    center[i] = (float) it->currentValue()->getScalar();
  if( i != 3 )
  {
    cerr << "circle_wireframe needs 3 coords in center\n";
    throw runtime_error( "circle_wireframe needs 3 coords in center" );
  }

  radius = (float) params.getProperty( "radius" )->getScalar();

  try
  {
    on = params.getProperty( "normal" );
    for( i=0, it=on->objectIterator(); it->isValid() && i < 3;
        ++i, it->next() )
      normal[i] = (float) it->currentValue()->getScalar();
  }
  catch( ... )
  {
  }
  if( !on.isNull() && i != 3 )
  {
    cerr << "circle_wireframe needs 3 coords in normal\n";
    throw runtime_error( "circle_wireframe needs 3 coords in normal" );
  }

  try
  {
    nseg = (unsigned) params.getProperty( "segments" )->getScalar();
  }
  catch( ... )
  {
  }

  try
  {
    on = params.getProperty( "start_direction" );
    for( i=0, it=on->objectIterator(); it->isValid() && i < 3;
        ++i, it->next() )
      startdir[i] = (float) it->currentValue()->getScalar();
  }
  catch( ... )
  {
  }
  if( !on.isNull() && i != 3 )
  {
    cerr << "circle_wireframe needs 3 coords in start_direction\n";
    throw runtime_error(
      "circle_wireframe needs 3 coords in start_direction" );
  }

  try
  {
    startangle = (unsigned) params.getProperty( "start_angle" )->getScalar();
  }
  catch( ... )
  {
  }

  try
  {
    stopangle = (unsigned) params.getProperty( "stop_angle" )->getScalar();
  }
  catch( ... )
  {
  }

  return circle_wireframe( center, radius, nseg, normal, startdir, startangle,
    stopangle );
}


AimsTimeSurface<2,Void>* SurfaceGenerator::circle_wireframe(
  const Point3df & center, float radius, unsigned nseg,
  const Point3df & normal, const Point3df & startdir, float startangle,
  float stopangle )
{
  AimsTimeSurface<2,Void> *mesh = new AimsTimeSurface<2,Void>;
  AimsSurface<2, Void> & mesh0 = (*mesh)[0];
  vector<Point3df> & vert = mesh0.vertex();
  vector<AimsVector<uint32_t, 2> > & poly = mesh0.polygon();
  Point3df stdir = startdir;
  stdir.normalize();

  Point3df v3 = crossed( normal, stdir );
  if( v3.norm2() < 1e-5 ) // normal and startdir are the same direction
  {
    stdir = Point3df( 1, 0, 0 );
    v3 = crossed( normal, stdir );
    if( v3.norm2() < 1e-5 )
    {
      stdir = Point3df( 0, 1, 0 );
      v3 = crossed( normal, stdir );
    }
  }
  stdir = crossed( v3, normal );
  stdir.normalize();
  v3.normalize();

  float step = ( stopangle - startangle ) / nseg;
  if( step < 0 )
  {
    step *= -1;
    float tmp = startangle;
    startangle = stopangle;
    stopangle = tmp;
  }

  vert.reserve( nseg + 1 );
  poly.reserve( nseg );

  float angle;
  unsigned i = 0;
  for( angle=startangle; angle<stopangle; angle+=step, ++i )
  {
    vert.push_back( center + stdir * radius * cos( angle )
      + v3 * radius * sin( angle ) );
    poly.push_back( AimsVector<uint32_t, 2>( i, i+1 ) );
  }
  Point3df lastver = center + stdir * radius * cos( stopangle )
    + v3 * radius * sin( stopangle );
  if( ( lastver - vert[0] ).norm2() < 1e-5 ) // closed circle
  {
    poly[nseg-1][1] = 0; // last segment loops to 1st point
  }
  else
    vert.push_back( lastver ); // open circle

  return mesh;
}


AimsTimeSurface<2,Void>* SurfaceGenerator::grid(
  const carto::GenericObject & params )
{
  Object        vp1;
  Point3df      corner1, corner2, sampling;

  vp1 = params.getProperty( "boundingbox_min" );

  int i = 0;
  Object it;
  for( it=vp1->objectIterator(); it->isValid() && i < 3; ++i, it->next() )
    corner1[i] = it->currentValue()->getScalar();
  if( i != 3 )
    cerr << "grid needs 3 coords in boundingbox_min\n";

  vp1 = params.getProperty( "boundingbox_max" );
  for( it=vp1->objectIterator(), i=0; it->isValid() && i < 3; ++i, it->next() )
    corner2[i] = it->currentValue()->getScalar();
  if( i != 3 )
    cerr << "grid needs 3 coords in boundingbox_max\n";

  vp1 = params.getProperty( "grid_sampling" );
  for( it=vp1->objectIterator(), i=0; it->isValid() && i < 3; ++i, it->next() )
    sampling[i] = it->currentValue()->getScalar();
  if( i != 3 )
    cerr << "grid needs 3 coords in grid_sampling\n";

  return grid( corner1, corner2, sampling );
}


AimsTimeSurface<2,Void>* SurfaceGenerator::grid(
  const Point3df & boundingbox_min, const Point3df & boundingbox_max,
  const Point3df & grid_sampling )
{
  AimsTimeSurface<2,Void> *mesh = new AimsTimeSurface<2,Void>;
  AimsSurface<2, Void> & mesh0 = (*mesh)[0];
  vector<Point3df> & vert = mesh0.vertex();
  vector<AimsVector<uint32_t, 2> > & poly = mesh0.polygon();

  int dimx = int( ( boundingbox_max[0] - boundingbox_min[0] )
    / grid_sampling[0] ) + 1;
  int dimy = int( ( boundingbox_max[1] - boundingbox_min[1] )
    / grid_sampling[1] ) + 1;
  int dimz = int( ( boundingbox_max[2] - boundingbox_min[2] )
    / grid_sampling[2] ) + 1;
  int x, y, z, i = 0;

  vert.resize( dimx * dimy * dimz );

  for( z=0; z<dimz; ++z )
    for( y=0; y<dimy; ++y )
      for( x=0; x<dimx; ++x )
      {
        Point3df pos( grid_sampling );
        pos[0] *= x;
        pos[1] *= y;
        pos[2] *= z;
        vert[i++] = boundingbox_min + pos;
      }

  int poly1 = ( dimx - 1 ) * dimy * dimz;
  int poly2 = dimx * ( dimy - 1 ) * dimz;
  int poly3 = dimx * dimy * ( dimz - 1 );

  poly.resize( poly1 + poly2 + poly3 );

  int splane = dimx * dimy;
  i = 0;

  for( z=0; z<dimz; ++z )
    for( y=0; y<dimy; ++y )
      for( x=0; x<dimx - 1; ++x )
      {
        poly[i++] = AimsVector<uint, 2>( x + y * dimx + z * splane,
                                         x + 1 + y * dimx + z * splane );
      }

  for( z=0; z<dimz; ++z )
    for( y=0; y<dimy - 1; ++y )
      for( x=0; x<dimx; ++x )
      {
        poly[i++] = AimsVector<uint, 2>( x + y * dimx + z * splane,
                                         x + dimx + y * dimx + z * splane );
      }

  for( z=0; z<dimz - 1; ++z )
    for( y=0; y<dimy; ++y )
      for( x=0; x<dimx; ++x )
      {
        poly[i++] = AimsVector<uint, 2>( x + y * dimx + z * splane,
                                         x + splane + y * dimx + z * splane );
      }

  return mesh;
}


