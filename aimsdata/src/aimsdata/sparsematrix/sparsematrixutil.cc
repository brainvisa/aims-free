// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/sparsematrix/sparsematrixutil.h>
#include <cartodata/volume/volumeutil_d.h>

using namespace aims;
using namespace carto;


double SparseMatrixUtil::min( const SparseMatrix & mat )
{
  SparseMatrix::const_iterator1 s1, e1 = mat.end1();
  SparseMatrix::const_iterator2 s2, e2;
  double acc = std::numeric_limits<double>::max();

  for( s1 = mat.begin1(); s1 != e1; ++s1 )
  {
    for( s2 = s1.begin(), e2 = s1.end(); s2 != e2; ++s2 )
    {
      if( *s2 < acc )
        acc = *s2;
    }
  }
  return acc;
}


double SparseMatrixUtil::max( const SparseMatrix & mat )
{
  SparseMatrix::const_iterator1 s1, e1 = mat.end1();
  SparseMatrix::const_iterator2 s2, e2;
  double acc = -std::numeric_limits<double>::max();

  for( s1 = mat.begin1(); s1 != e1; ++s1 )
  {
    for( s2 = s1.begin(), e2 = s1.end(); s2 != e2; ++s2 )
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

