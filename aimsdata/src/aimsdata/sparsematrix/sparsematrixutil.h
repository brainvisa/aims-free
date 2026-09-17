#ifndef AIMS_SPARSEMATRIX_SPARSEMATRIXUTIL_H
#define AIMS_SPARSEMATRIX_SPARSEMATRIXUTIL_H

#include <aims/sparsematrix/sparseordensematrix.h>

namespace aims
{

  class SparseMatrixUtil
  {
  public:
    static double min( const SparseMatrix & );
    static double max( const SparseMatrix & );
    static double min( const SparseOrDenseMatrix & );
    static double max( const SparseOrDenseMatrix & );
  };

}

#endif

