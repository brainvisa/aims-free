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

#ifndef AIMS_UTILITY_BOUNDINGBOX_H
#define AIMS_UTILITY_BOUNDINGBOX_H

#include <aims/vector/vector.h>
#include <aims/bucket/bucket.h>


namespace aims
{
  class AffineTransformation3d;

  class BoundingBox
  {
  public:
    BoundingBox( const AffineTransformation3d* motion = 0 );
    ~BoundingBox();

    void reset();
    /// \c motion will be copied. It's valid to pass a null pointer.
    void setMotion( const AffineTransformation3d* motion );
    const AffineTransformation3d *motion() const;
    /// number of points recorded
    unsigned count() const;
    Point3df minimum() const;
    Point3df maximum() const;
    void add( const Point3df & pos );
    template <typename T>
    void add( const BucketMap<T> & bck );

  private:
    unsigned	_npoints;
    Point3df	_minimum;
    Point3df	_maximum;
    AffineTransformation3d	*_motion;
  };


  inline 
  const AffineTransformation3d *BoundingBox::motion() const
  {
    return _motion;
  }


  inline 
  unsigned BoundingBox::count() const
  {
    return _npoints;
  }


  inline 
  Point3df BoundingBox::minimum() const
  {
    return _minimum;
  }


  inline 
  Point3df BoundingBox::maximum() const
  {
    return _maximum;
  }

}

#endif

