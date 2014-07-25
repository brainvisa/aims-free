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

#include <aims/sparsematrix/sparsematrixutil.h>
#include <cartodata/volume/volumeutil_d.h>

using namespace aims;
using namespace carto;


double SparseMatrixUtil::min( const SparseMatrix & mat )
{
  SparseMatrix::const_iterator1 s1;
  SparseMatrix::const_iterator2 s2;
  double acc = std::numeric_limits<double>::max();

  for( s1 = mat.begin1(); s1 != mat.end1(); ++s1 )
  {
    for( s2 = s1.begin(); s2 != s1.end(); ++s2 )
    {
      if( *s2 < acc )
        acc = *s2;
    }
  }
  return acc;
}


double SparseMatrixUtil::max( const SparseMatrix & mat )
{
  SparseMatrix::const_iterator1 s1;
  SparseMatrix::const_iterator2 s2;
  double acc = -std::numeric_limits<double>::max();

  for( s1 = mat.begin1(); s1 != mat.end1(); ++s1 )
  {
    for( s2 = s1.begin(); s2 != s1.end(); ++s2 )
    {
      if( *s2 > acc )
        acc = *s2;
    }
  }
  return acc;
}


double SparseMatrixUtil::min( const SparseOrDenseMatrix & mat )
{
  if( mat.isDense() )
    return VolumeUtil<double>::min( *mat.denseMatrix() );
  else
    return SparseMatrixUtil::min( *mat.sparseMatrix() );
}


double SparseMatrixUtil::max( const SparseOrDenseMatrix & mat )
{
  if( mat.isDense() )
    return VolumeUtil<double>::max( *mat.denseMatrix() );
  else
    return SparseMatrixUtil::max( *mat.sparseMatrix() );
}

