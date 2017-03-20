
namespace aims
{

  class SparseOrDenseMatrix : carto::RCObject
  {
%TypeHeaderCode
#include <aims/sparsematrix/sparseordensematrix.h>
%End

    public:

      // typedef rc_ptr_Volume_DOUBLE DenseMatrixType;
      typedef rc_ptr_SparseMatrix SparseMatrixType;

      SparseOrDenseMatrix( int size1 = 1, int size2 = 1 );
      SparseOrDenseMatrix( const aims::SparseOrDenseMatrix& other )
        /ReleaseGIL/;
      virtual ~SparseOrDenseMatrix() /ReleaseGIL/;

      void reallocate( int size1, int size2 ) /ReleaseGIL/;

      int getSize1() const;
      int getSize2() const;
      int getNonZeroElementCount() const;

      bool hasElement( int i, int j ) const;

      double operator()( int i, int j ) const;
      void erase_element( int i, int j );
      void set_element( int i, int j, double x );

      void setRow( int s1, const vector_DOUBLE & row ) /ReleaseGIL/;
      void setColumn( int s2, const vector_DOUBLE & column ) /ReleaseGIL/;

      vector_DOUBLE getRow( int i ) const /ReleaseGIL/;
      vector_DOUBLE getColumn( int j ) const /ReleaseGIL/;

      void read( const std::string& filename ) /ReleaseGIL/;
      void write( const std::string& filename, 
        carto::Object options=carto::none() ) const /ReleaseGIL/;

      bool isDense() const;

      SparseMatrixType sparseMatrix();
      rc_ptr_Volume_DOUBLE denseMatrix();

      void setMatrix( SparseMatrixType matrix ) /ReleaseGIL/;
      void setMatrix( rc_ptr_Volume_DOUBLE matrix ) /ReleaseGIL/;

      carto::Object header();
      void setHeader( carto::Object ph );

      SparseMatrixType asSparse( bool copy=false ) const /ReleaseGIL/;
      rc_ptr_Volume_DOUBLE asDense( bool copy=false ) const /ReleaseGIL/;
      void muteToDense() /ReleaseGIL/;
      void muteToSparse() /ReleaseGIL/;
      void muteToOptimalShape() /ReleaseGIL/;
      bool isOptimalShape() const;

      aims::SparseOrDenseMatrix &
        operator += ( const aims::SparseOrDenseMatrix& thing ) /ReleaseGIL/;
      aims::SparseOrDenseMatrix &
        operator -= ( const aims::SparseOrDenseMatrix& thing ) /ReleaseGIL/;
      aims::SparseOrDenseMatrix &
        operator *= ( double x ) /ReleaseGIL/;
      aims::SparseOrDenseMatrix &
        operator /= ( double x ) /ReleaseGIL/;
  };

};


aims::SparseOrDenseMatrix
  operator - ( const aims::SparseOrDenseMatrix & thing ) /ReleaseGIL/;

aims::SparseOrDenseMatrix
  operator + ( const aims::SparseOrDenseMatrix & thing1,
               const aims::SparseOrDenseMatrix & thing2 ) /ReleaseGIL/;

aims::SparseOrDenseMatrix
  operator - ( const aims::SparseOrDenseMatrix & thing1,
               const aims::SparseOrDenseMatrix & thing2 ) /ReleaseGIL/;

aims::SparseOrDenseMatrix
  operator * ( const aims::SparseOrDenseMatrix& thing1,
               const double& thing2 ) /ReleaseGIL/;

aims::SparseOrDenseMatrix
  operator / ( const aims::SparseOrDenseMatrix & thing1,
               const double& thing2 ) /ReleaseGIL/;
