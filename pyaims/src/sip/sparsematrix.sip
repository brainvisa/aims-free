namespace aims
{

  class SparseMatrix : carto::RCObject
  {

%TypeHeaderCode
#include <aims/sparsematrix/sparseMatrix.h>
%End

    public:

      SparseMatrix( int size1 = 1, int size2 = 1 );
      SparseMatrix( const aims::SparseMatrix& other );
      virtual ~SparseMatrix();

      void reallocate( int size1, int size2 );

      int getSize1() const;
      int getSize2() const;
      int getNonZeroElementCount() const;

      bool hasElement( int i, int j ) const;

      double operator()( int i, int j );

      bool isSquare() const;

      void setZero();
      void setIdentity();//bizarre ?
      void setRow( int s1, const vector_DOUBLE& row );
      void setColumn( int s2, const vector_DOUBLE& column );
      void fill( const double& value );
      void fill( int offset1,
                 int offset2,
                 int size1,
                 int size2,
                 const double& value ) /ReleaseGIL/;
      void fill( int offset1,
                 int offset2,
                 const aims::SparseMatrix& other,
                 int size1 = 0,
                 int size2 = 0 );
      void setDiagonal( const double& value ) /ReleaseGIL/;
      void transpose() /ReleaseGIL/;

      double getTrace() const /ReleaseGIL/;
      aims::SparseMatrix getTransposition() const /ReleaseGIL/;
      aims::SparseMatrix getComposition( const aims::SparseMatrix& other ) const /ReleaseGIL/;
      vector_DOUBLE getComposition( const vector_DOUBLE& other ) const /ReleaseGIL/;

      vector_DOUBLE getRow( int i ) const /ReleaseGIL/;
      vector_DOUBLE getColumn( int j ) const /ReleaseGIL/;
      void erase_element( int i, int j );

      void read( const std::string& filename,
                 const std::string & openmode = "binar",
                 bool bswap = false ) /ReleaseGIL/;
      void write( const std::string& filename,
                  const std::string & openmode = "binar",
                  bool bswap = false ) const /ReleaseGIL/;

      carto::Object header();

      aims::SparseMatrix &
        operator += ( const aims::SparseMatrix& thing ) /ReleaseGIL/;
      aims::SparseMatrix &
        operator -= ( const aims::SparseMatrix& thing ) /ReleaseGIL/;
      aims::SparseMatrix &
        operator *= ( double x ) /ReleaseGIL/;
      aims::SparseMatrix &
        operator /= ( double x ) /ReleaseGIL/;
  };

};


aims::SparseMatrix 
    operator - ( const aims::SparseMatrix& thing ) /ReleaseGIL/;

aims::SparseMatrix
    operator + ( const aims::SparseMatrix& thing1,
                 const aims::SparseMatrix& thing2 ) /ReleaseGIL/;

aims::SparseMatrix
    operator - ( const aims::SparseMatrix& thing1,
                 const aims::SparseMatrix& thing2 ) /ReleaseGIL/;

aims::SparseMatrix
    operator * ( const aims::SparseMatrix& thing1,
                 const aims::SparseMatrix& thing2 ) /ReleaseGIL/;

vector_DOUBLE
    operator * ( const aims::SparseMatrix& thing1,
                 const vector_DOUBLE & thing2 ) /ReleaseGIL/;

aims::SparseMatrix
    operator * ( const aims::SparseMatrix& thing1,
                 const double& thing2 ) /ReleaseGIL/;

aims::SparseMatrix
    operator / ( const aims::SparseMatrix& thing1,
                 const double& thing2 ) /ReleaseGIL/;

