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

#include <aims/transformation/transformation_chain.h>

#include <aims/transformation/affinetransformation3d.h>


aims::TransformationChain3d::TransformationChain3d()
  : _transformations()
{
}

void aims::TransformationChain3d::
push_back(const carto::const_ref<Transformation3d>& transformation)
{
  _transformations.push_back(transformation);
}

void aims::TransformationChain3d::pop_back()
{
  _transformations.pop_back();
}

void aims::TransformationChain3d::
push_front(const carto::const_ref<Transformation3d>& transformation)
{
  _transformations.push_front(transformation);
}

void aims::TransformationChain3d::pop_front()
{
  _transformations.pop_front();
}

bool aims::TransformationChain3d::isIdentity() const
{
  for(ListType::const_iterator it = _transformations.begin() ;
      it != _transformations.end() ;
      ++it)
  {
    if(!(*it)->isIdentity()) {
      return false;
    }
  }
  return true;
}

bool aims::TransformationChain3d::invertible() const
{
  for(ListType::const_iterator it = _transformations.begin() ;
      it != _transformations.end() ;
      ++it)
  {
    if(!(*it)->invertible()) {
      return false;
    }
  }
  return true;
}

std::unique_ptr<aims::Transformation3d>
aims::TransformationChain3d::getInverse() const
{
  std::unique_ptr<aims::Transformation3d> ret(new aims::TransformationChain3d);
  aims::TransformationChain3d & new_chain
    = dynamic_cast<aims::TransformationChain3d&>(*ret.get());

  for(ListType::const_reverse_iterator rit = _transformations.rbegin() ;
      rit != _transformations.rend() ;
      ++rit)
  {
    new_chain.push_back(carto::rc_ptr<Transformation3d>(
      (*rit)->getInverse())
    );
  }

  return ret;
}

Point3dd aims::TransformationChain3d::
transformPoint3dd( const Point3dd& point ) const
{
  Point3dd transformed_point = point;
  for(ListType::const_iterator it = _transformations.begin() ;
      it != _transformations.end() ;
      ++it)
  {
    transformed_point = (*it)->transform(transformed_point);
  }
  return transformed_point;
}

Point3df aims::TransformationChain3d::
transformPoint3df( const Point3df& point ) const
{
  Point3df transformed_point = point;
  for(ListType::const_iterator it = _transformations.begin() ;
      it != _transformations.end() ;
      ++it)
  {
    transformed_point = (*it)->transform(transformed_point);
  }
  return transformed_point;
}

Point3dd aims::TransformationChain3d::
transformDouble( double x, double y, double z ) const
{
  return transformPoint3dd(Point3dd(x, y, z));
}

Point3df aims::TransformationChain3d::
transformFloat( float x, float y, float z ) const
{
  return transformPoint3df(Point3df(x, y, z));
}

carto::const_ref<aims::Transformation3d>
aims::TransformationChain3d::simplify() const
{
  // 1. flat_chain is built by applying simplify() recursively to sub-chains,
  // and inserting the elements of the resulting chains in a flat chain.
  TransformationChain3d flat_chain;
  for(ListType::const_iterator it = _transformations.begin() ;
      it != _transformations.end() ;
      ++it)
  {
    carto::const_ref<Transformation3d> current_trans = *it;
    // 1a. Simplify sub-chains
    {
      const TransformationChain3d* current_trans_chain
        = dynamic_cast<const TransformationChain3d*>(current_trans.pointer());
      if(current_trans_chain) {
        current_trans = current_trans_chain->simplify();
      }
    }
    // 1b. Insert the (simplified) transformations into a flat chain
    const TransformationChain3d* current_subchain
      = dynamic_cast<const TransformationChain3d*>(current_trans.pointer());
    if(current_subchain) {
      for(ListType::const_iterator sub_it = current_subchain->_transformations.begin();
          sub_it != current_subchain->_transformations.end();
          ++sub_it) {
        flat_chain.push_back(*sub_it);
      }
    } else {
      if(!current_trans->isIdentity()) {
        flat_chain.push_back(current_trans);
      }
    }
  }

  // 2. Now that we have a flat chain, we compose consecutive affine
  // transformations.
  carto::rc_ptr<TransformationChain3d> new_chain(new TransformationChain3d);
  AffineTransformation3d accumulated_trans; // initialized to identity
  const ListType::const_reverse_iterator rend = flat_chain._transformations.rend();
  for(ListType::const_reverse_iterator rit
        = flat_chain._transformations.rbegin();
      rit != rend;
      ++rit)
  {
    carto::const_ref<Transformation3d> current_trans = *rit;

    const AffineTransformation3d* current_affine_trans
      = dynamic_cast<const AffineTransformation3d*>(current_trans.pointer());
    if(current_affine_trans) {
      accumulated_trans *= *current_affine_trans;
    } else {
      if(!accumulated_trans.isIdentity()) {
        carto::const_ref<AffineTransformation3d> to_insert(new AffineTransformation3d(accumulated_trans));
        new_chain->push_front(to_insert);
        accumulated_trans.setToIdentity();
      }
      new_chain->push_front(current_trans);
    }
  }
  if(!accumulated_trans.isIdentity()) {
    carto::const_ref<AffineTransformation3d> to_insert(new AffineTransformation3d(accumulated_trans));
    new_chain->push_front(to_insert);
  }

  if(new_chain->_transformations.size() == 1) {
    return *new_chain->_transformations.begin();
  }

  return new_chain;
}
