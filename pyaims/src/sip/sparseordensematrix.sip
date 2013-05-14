
namespace aims
{

  class SparseOrDenseMatrix : carto::RCObject
  {
%TypeHeaderCode
#include <aims/sparsematrix/sparseordensematrix.h>
%End

    public:

      typedef rc_ptr_Volume_DOUBLE DenseMatrixType;
      typedef rc_ptr_SparseMatrix SparseMatrixType;

      SparseOrDenseMatrix( int size1 = 1, int size2 = 1 );
      SparseOrDenseMatrix( const aims::SparseOrDenseMatrix& other );
      virtual ~SparseOrDenseMatrix();

      void reallocate( int size1, int size2 );

      int getSize1() const;
      int getSize2() const;
      int getNonZeroElementCount() const;

      bool hasElement( int i, int j ) const;

      double operator()( int i, int j ) const;
      void erase_element( int i, int j );
      void set_element( int i, int j, double x );

      void setRow( int s1, const vector_DOUBLE & row );
      void setColumn( int s2, const vector_DOUBLE & column );

      vector_DOUBLE getRow( int i ) const;
      vector_DOUBLE getColumn( int j ) const;

      void read( const std::string& filename );
      void write( const std::string& filename, 
        carto::Object options=carto::none() ) const;

      bool isDense() const;

      SparseMatrixType sparseMatrix();
      DenseMatrixType denseMatrix();

      void setMatrix( SparseMatrixType matrix );
      void setMatrix( DenseMatrixType matrix );

      carto::Object header();
      void setHeader( carto::Object ph );

      SparseMatrixType asSparse( bool copy=false ) const;
      DenseMatrixType asDense( bool copy=false ) const;
      void muteToDense();
      void muteToSparse();
      void muteToOptimalShape();

      aims::SparseOrDenseMatrix &
        operator += ( const aims::SparseOrDenseMatrix& thing );
      aims::SparseOrDenseMatrix &
        operator -= ( const aims::SparseOrDenseMatrix& thing );
      aims::SparseOrDenseMatrix &
        operator *= ( double x );
      aims::SparseOrDenseMatrix &
        operator /= ( double x );
  };

};


aims::SparseOrDenseMatrix
  operator - ( const aims::SparseOrDenseMatrix & thing );

aims::SparseOrDenseMatrix
  operator + ( const aims::SparseOrDenseMatrix & thing1,
               const aims::SparseOrDenseMatrix & thing2 );

aims::SparseOrDenseMatrix
  operator - ( const aims::SparseOrDenseMatrix & thing1,
               const aims::SparseOrDenseMatrix & thing2 );

aims::SparseOrDenseMatrix
  operator * ( const aims::SparseOrDenseMatrix& thing1,
               const double& thing2 );

aims::SparseOrDenseMatrix
  operator / ( const aims::SparseOrDenseMatrix & thing1,
               const double& thing2 );
