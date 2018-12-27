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

std::unique_ptr<soma::Transformation3d>
aims::TransformationChain3d::getInverse() const
{
  std::unique_ptr<soma::Transformation3d> ret(new aims::TransformationChain3d);
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
