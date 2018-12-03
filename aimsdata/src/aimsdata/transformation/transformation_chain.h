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

#ifndef AIMS_TRANSFORMATION_TRANSFORMATION_CHAIN_H
#define AIMS_TRANSFORMATION_TRANSFORMATION_CHAIN_H

#include <list>

#include <cartobase/smart/rcptr.h>
#include <aims/transformation/transformation.h>


namespace aims
{
  /** Container for a composition of multiple transformations.

      This container holds a list of transformations, and acts as the
      composition of all transformations. Transformations are composed from the
      front to the back of the list:

      \code
      rc_ptr<TransformationChain3d>(new TransformationChain3d())
      chain->push_back(t1)
      chain->push_back(t2)
      // chain->transform(p) == t2->transform(t1->transform(p))
      \endcode

      \warning{Do not modify the transformations once you have passed them to
      push_back() or push_front(): it is unspecified if the changes will be
      noticed by TransformationChain3d (a reference to the same object may be
      kept internally, or a copy could be made). This behaviour will allow
      optimizations to be implemented (e.g. composing adjacent affine
      transformations by multiplying their matrices).
   */
class TransformationChain3d : public Transformation3d {
public:
  typedef std::list<carto::const_ref<Transformation3d> > ListType;

  /** Add a transformation to the back of the list (applied last) */
  void push_back(const carto::const_ref<Transformation3d>& transformation);
  /** Remove the last transformation from the list */
  void pop_back();
  /** Add a transformation to the front of the list (applied first) */
  void push_front(const carto::const_ref<Transformation3d>& transformation);
  /** Remove the first transformation from the list */
  void pop_front();

  bool isIdentity() const CARTO_OVERRIDE;
  bool invertible() const CARTO_OVERRIDE;
  std::unique_ptr<Transformation3d> getInverse() const CARTO_OVERRIDE;

protected:
  ListType _transformations;

  Point3dd transformDouble( double x, double y, double z ) const CARTO_OVERRIDE;
  Point3df transformFloat( float x, float y, float z ) const CARTO_OVERRIDE;
  Point3dd transformPoint3dd( const Point3dd & pos ) const CARTO_OVERRIDE;
  Point3df transformPoint3df( const Point3df & dir ) const CARTO_OVERRIDE;
};

} // namespace aims

#endif // !defined(AIMS_TRANSFORMATION_TRANSFORMATION_CHAIN_H)
