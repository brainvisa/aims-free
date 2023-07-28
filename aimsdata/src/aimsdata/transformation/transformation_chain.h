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
#include <soma-io/transformation/transformation.h>


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
class TransformationChain3d : public soma::Transformation3d
{
public:
  typedef std::list<carto::const_ref<Transformation3d> > ListType;
  typedef ListType::iterator iterator;
  typedef ListType::const_iterator const_iterator;

  TransformationChain3d();
  virtual ~TransformationChain3d();

  /** Add a transformation to the back of the list (applied last) */
  void push_back(const carto::const_ref<Transformation3d>& transformation);
  /** Remove the last transformation from the list */
  void pop_back();
  /** Add a transformation to the front of the list (applied first) */
  void push_front(const carto::const_ref<Transformation3d>& transformation);
  /** Remove the first transformation from the list */
  void pop_front();
  size_t size() const;
  iterator begin() { return _transformations.begin(); }
  const_iterator begin() const { return _transformations.begin(); }
  iterator end() { return _transformations.end(); }
  const_iterator end() const { return _transformations.end(); }

  bool isIdentity() const CARTO_OVERRIDE;
  bool invertible() const CARTO_OVERRIDE;
  std::unique_ptr<soma::Transformation> getInverse() const CARTO_OVERRIDE;
  bool isDirect() const override;

  /** Compute a simpler transformation that is equivalent to the chain.

      The transformation chain is simplified by applying the following rules,
      so that it should provide equivalent results to the original chain, up to
      numerical precision:

      - The simplification is applied recursively to any sub-chain.

      - Transformations of the simplified sub-chains are inserted at the top
        level (i.e. the simplified chain is flat, it contains no sub-chains).

      - Consecutive affine transformations are composed using matrix
        multiplication.

      - Identity transforms (i.e. transforms for which
        Transformation::isIdentity() returns true) are removed from the list.

      - If the simplified chain consists of only one transformation, no chain
        is returned, the contained transformation is returned directly.
        However, the transformation is not necessarily the same instance that
        was inserted in the chain (it will be a copy if it is an
        AffineTransformation3d).

      - If the simplified chain is empty, an empty chain is returned.

      No deep copy is made, so the result can contain pointers to the same
      transformations as the original chain.
   */
  carto::const_ref<soma::Transformation3d> simplify() const;

protected:
  ListType _transformations;

  Point3dd transformDouble( double x, double y, double z ) const CARTO_OVERRIDE;
  Point3df transformFloat( float x, float y, float z ) const CARTO_OVERRIDE;
  Point3dd transformPoint3dd( const Point3dd & pos ) const CARTO_OVERRIDE;
  Point3df transformPoint3df( const Point3df & dir ) const CARTO_OVERRIDE;
  void setReferentialsInHeader();
};


  // Compose Transformation3d using TransformationChain3d
  carto::rc_ptr<soma::Transformation3d>
      operator * ( const carto::rc_ptr<soma::Transformation3d> &
                    transformation3d1,
                   const carto::rc_ptr<soma::Transformation3d> &
                    transformation3d2 );

} // namespace aims


// ---

namespace soma
{

  template <> inline std::string
  DataTypeCode<aims::TransformationChain3d>::objectType()
  {
    return "TransformationChain3d";
  }

  template <> inline std::string
  DataTypeCode<aims::TransformationChain3d>::dataType()
  {
    return "VOID";
  }

  template <> inline std::string
  DataTypeCode<aims::TransformationChain3d>::name()
  {
    return "TransformationChain3d";
  }

}


#endif // !defined(AIMS_TRANSFORMATION_TRANSFORMATION_CHAIN_H)
