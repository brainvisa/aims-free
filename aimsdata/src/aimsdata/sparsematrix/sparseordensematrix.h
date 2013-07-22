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

#ifndef AIMS_SPARSEMATRIX_SPARSEORDENSEMATRIX_H
#define AIMS_SPARSEMATRIX_SPARSEORDENSEMATRIX_H

#include <aims/data/pheader.h>
#include <aims/sparsematrix/sparseMatrix.h>
#include <cartodata/volume/volume.h>

namespace aims
{


  class SparseOrDenseMatrix : public virtual carto::RCObject
  {

    public:

      typedef carto::VolumeRef<double> DenseMatrixType;
      typedef carto::rc_ptr<SparseMatrix> SparseMatrixType;

      SparseOrDenseMatrix( int32_t size1 = 1, int32_t size2 = 1 );
      SparseOrDenseMatrix( const SparseOrDenseMatrix& other );
      virtual ~SparseOrDenseMatrix();

      SparseOrDenseMatrix& operator=( const SparseOrDenseMatrix& other );

      void reallocate( int32_t size1, int32_t size2 );

      int32_t getSize1() const;
      int32_t getSize2() const;
      int32_t getNonZeroElementCount() const;

      bool hasElement( int32_t i, int32_t j ) const;

      double operator()( int32_t i, int32_t j ) const;
      void erase_element( int32_t i, int32_t j );
      void set_element( int32_t i, int32_t j, double x );

      void setRow( int32_t s1, const std::vector<double>& row );
      void setColumn( int32_t s2, const std::vector<double>& column );

      std::vector<double> getRow( int32_t i ) const;
      std::vector<double> getColumn( int32_t j ) const;

      template <typename VectorType>
        VectorType getSparseRow( int32_t i ) const;
      template <typename VectorType>
        VectorType getSparseColumn( int32_t i ) const;

      void read( const std::string& filename );
      void write( const std::string& filename, 
                  carto::Object options=carto::none() ) const;

      bool isDense() const { return !_densematrix.isNull(); }

      SparseMatrixType sparseMatrix() { return _sparsematrix; }
      const SparseMatrixType sparseMatrix() const { return _sparsematrix; }
      DenseMatrixType denseMatrix() { return _densematrix; }
      const DenseMatrixType denseMatrix() const { return _densematrix; }

      void setMatrix( SparseMatrixType matrix );
      void setMatrix( DenseMatrixType matrix );

      carto::Object header();
      const carto::Object header() const;
      void setHeader( carto::Object ph );

      /// get a shallow or deep copy in sparse shape
      SparseMatrixType asSparse( bool copy=false ) const;
      /// get a shallow or deep copy in dense shape
      DenseMatrixType asDense( bool copy=false ) const;
      void muteToDense();
      void muteToSparse();
      /// mute to sparse or dense according to the less memory consuming
      void muteToOptimalShape();

      SparseOrDenseMatrix &
        operator += ( const SparseOrDenseMatrix& thing );
      SparseOrDenseMatrix &
        operator -= ( const SparseOrDenseMatrix& thing );
      SparseOrDenseMatrix &
        operator *= ( double x );
      SparseOrDenseMatrix &
        operator /= ( double x );

    protected:

      SparseMatrixType _sparsematrix;
      DenseMatrixType _densematrix;
      carto::Object _fakeheader;

  };


  template <typename VectorType>
  inline
  VectorType SparseOrDenseMatrix::getSparseRow( int32_t i ) const
  {
    if( !isDense() )
      return sparseMatrix()->getSparseRow<VectorType>( i );

    // dense case
    VectorType row( denseMatrix()->getSizeX() );
    const double *buf = &denseMatrix().at( 0, i ), *end = &denseMatrix().at( denseMatrix()->getSizeX(), i );
    long inc = &denseMatrix().at( 1, i ) - buf;
    int x;
    for( x=0; buf != end; buf += inc, ++x )
      if( *buf != 0 )
        row[x] = *buf;
    return row;
  }


  template <typename VectorType>
  inline
  VectorType SparseOrDenseMatrix::getSparseColumn( int32_t i ) const
  {
    if( !isDense() )
      return sparseMatrix()->getSparseColumn<VectorType>( i );

    // dense case
    VectorType column( denseMatrix()->getSizeY() );
    const double *buf = &denseMatrix().at( i, 0 ), *end = &denseMatrix().at( i, denseMatrix()->getSizeY() );
    long inc = &denseMatrix().at( i, 1 ) - buf;
    int x;
    for( x=0; buf != end; buf += inc, ++x )
      if( *buf != 0 )
        column[x] = *buf;
    return column;
  }

}



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


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template<> class DataTypeCode<aims::SparseOrDenseMatrix>
  {
  public:
    static std::string objectType()
    { return "SparseOrDenseMatrix"; }
    static std::string dataType()
    { return "DOUBLE"; }
    static std::string name()
    {
      return std::string("SparseOrDenseMatrix_") + dataType();
    }
  };

  DECLARE_GENERIC_OBJECT_TYPE( aims::SparseOrDenseMatrix )

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


#endif
