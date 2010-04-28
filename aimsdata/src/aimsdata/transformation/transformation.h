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

#ifndef AIMS_TRANSFORMATION_TRANSFORMATION_H
#define AIMS_TRANSFORMATION_TRANSFORMATION_H

#include <aims/vector/vector.h>
#include <cartobase/smart/rcptr.h>

namespace aims
{

  class Transformation : public virtual carto::RCObject
  {
  public:
    virtual ~Transformation();

  protected:
    Transformation() {}
  };


  class Transformation3d : public Transformation
  {
  public:
    virtual ~Transformation3d();

    virtual Point3dd transform( const Point3dd & pos ) const = 0;
    virtual Point3df transform( const Point3df & dir ) const;
    virtual Point3dd transform( double x, double y, double z ) const;
    virtual Point3df transform( float x, float y, float z ) const;
    virtual Point3dd transformVector( const Point3dd & vec ) const;
    virtual Point3df transformVector( const Point3df & dir ) const;
    virtual Point3dd transformVector( double x, double y, double z ) const;
    virtual Point3df transformVector( float x, float y, float z ) const;
    virtual Point3dd transformNormal( const Point3dd & dir ) const;
    virtual Point3df transformNormal( const Point3df & dir ) const;
    virtual Point3dd transformNormal( double x, double y, double z ) const;
    virtual Point3df transformNormal( float x, float y, float z ) const;
    Point3dd transformUnitNormal( const Point3dd & dir ) const;
    Point3df transformUnitNormal( const Point3df & dir ) const;
    Point3dd transformUnitNormal( double x, double y, double z ) const;
    Point3df transformUnitNormal( float x, float y, float z ) const;

  protected:
    Transformation3d() {}
  };


  inline Point3dd
  Transformation3d::transformUnitNormal( const Point3dd & dir ) const
  {
    return transformNormal( dir ).normalize();
  }


  inline Point3df Transformation3d::transform( const Point3df & pos ) const
  {
    Point3dd transformed = transform( (double) pos[0], (double) pos[1],
                                       (double) pos[2] );
    return Point3df( (float) transformed[0], (float) transformed[1],
                     (float) transformed[2] );
  }


  inline Point3dd
  Transformation3d::transform( double x, double y, double z ) const
  {
    return transform( Point3dd( x, y, z ) );
  }


  inline Point3df
  Transformation3d::transform( float x, float y, float z ) const
  {
    Point3dd transformed = transform( (double) x, (double) y, (double) z );
    return Point3df( (float) transformed[0], (float) transformed[1],
                     (float) transformed[2] );
  }


  inline Point3df
  Transformation3d::transformVector( const Point3df & pos ) const
  {
    Point3dd transformed = transformVector( (double) pos[0], (double) pos[1],
                                            (double) pos[2] );
    return Point3df( (float) transformed[0], (float) transformed[1],
                     (float) transformed[2] );
  }


  inline Point3dd
  Transformation3d::transformVector( double x, double y, double z ) const
  {
    return transformVector( Point3dd( x, y, z ) );
  }


  inline Point3df
  Transformation3d::transformVector( float x, float y, float z ) const
  {
    Point3dd transformed = transformVector( (double) x, (double) y,
                                             (double) z );
    return Point3df( (float) transformed[0], (float) transformed[1],
                     (float) transformed[2] );
  }


  inline Point3df
  Transformation3d::transformNormal( const Point3df & pos ) const
  {
    Point3dd transformed = transformNormal( (double) pos[0], (double) pos[1],
                                             (double) pos[2] );
    return Point3df( (float) transformed[0], (float) transformed[1],
                     (float) transformed[2] );
  }


  inline Point3dd
  Transformation3d::transformNormal( double x, double y, double z ) const
  {
    return transformNormal( Point3dd( x, y, z ) );
  }


  inline Point3df
  Transformation3d::transformNormal( float x, float y, float z ) const
  {
    Point3dd transformed = transformNormal( (double) x, (double) y,
                                             (double) z );
    return Point3df( (float) transformed[0], (float) transformed[1],
                      (float) transformed[2] );
  }


  inline Point3df
  Transformation3d::transformUnitNormal( const Point3df & pos ) const
  {
    Point3dd transformed
        = transformUnitNormal( (double) pos[0], (double) pos[1],
                                (double) pos[2] );
    return Point3df( (float) transformed[0], (float) transformed[1],
                      (float) transformed[2] );
  }


  inline Point3dd
  Transformation3d::transformUnitNormal( double x, double y, double z ) const
  {
    return transformUnitNormal( Point3dd( x, y, z ) );
  }


  inline Point3df
  Transformation3d::transformUnitNormal( float x, float y, float z ) const
  {
    Point3dd transformed = transformUnitNormal( (double) x, (double) y,
                                                (double) z );
    return Point3df( (float) transformed[0], (float) transformed[1],
                      (float) transformed[2] );
  }

}

#endif

