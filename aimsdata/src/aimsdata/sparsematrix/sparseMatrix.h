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

#ifndef AIMS_SPARSEMATRIX_SPARSEMATRIX_H
#define AIMS_SPARSEMATRIX_SPARSEMATRIX_H

#include <aims/data/pheader.h>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/version.hpp>
#include <string>

namespace aims
{


#if BOOST_VERSION >= 103300
  typedef boost::numeric::ublas::mapped_matrix< double > boost_sparse_matrix;
  typedef boost::numeric::ublas::mapped_vector< double > boost_sparse_vector;
#else
  typedef boost::numeric::ublas::sparse_matrix< double > boost_sparse_matrix;
  typedef boost::numeric::ublas::sparse_vector< double > boost_sparse_vector;
#endif


  class SparseMatrix : public virtual carto::RCObject
  {

    public:

      typedef boost_sparse_matrix::reference
          reference;
      typedef boost_sparse_matrix::const_reference
          const_reference;

      typedef boost_sparse_matrix::iterator1 iterator1;
      typedef boost_sparse_matrix::iterator2 iterator2;

      typedef boost_sparse_matrix::const_iterator1
          const_iterator1;
      typedef boost_sparse_matrix::const_iterator2
          const_iterator2;

      SparseMatrix( int32_t size1 = 1, int32_t size2 = 1 );
      SparseMatrix( const SparseMatrix& other );
      virtual ~SparseMatrix();

      SparseMatrix& operator=( const SparseMatrix& other );

      void reallocate( int32_t size1, int32_t size2 );

      iterator1 begin1();
      iterator1 end1();

      const_iterator1 begin1() const;
      const_iterator1 end1() const;

      iterator2 begin2();
      iterator2 end2();

      const_iterator2 begin2() const;
      const_iterator2 end2() const;

      int32_t getSize1() const;
      int32_t getSize2() const;
      int32_t getNonZeroElementCount() const;

      bool hasElement( int32_t i, int32_t j ) const;

      const_reference operator()( int32_t i, int32_t j ) const; 
      reference operator()( int32_t i, int32_t j ); 

      bool isSquare() const;

      void setZero();
      void setIdentity();//bizarre ?
      void setRow( int32_t s1, const std::vector<double>& row );
      void setColumn( int32_t s2, const std::vector<double>& column );
      void fill( const double& value );
      void fill( int32_t offset1,
                 int32_t offset2,
                 int32_t size1,
                 int32_t size2,
                 const double& value );
      void fill( int32_t offset1,
                 int32_t offset2,
                 const SparseMatrix& other,
                 int32_t size1 = 0,
                 int32_t size2 = 0 );
      void setDiagonal( const double& value );
      void transpose();

      double getTrace() const;
      SparseMatrix getTransposition() const;
      SparseMatrix getComposition( const SparseMatrix& other ) const;
      std::vector<double> getComposition( const std::vector<double>& other ) const;
      std::vector<double> toVector() const;

      std::vector<double> getRow( int32_t i ) const;
      std::vector<double> getColumn( int32_t j ) const;

      template <typename VectorType>
        VectorType getSparseRow( int32_t i ) const;
      template <typename VectorType>
        VectorType getSparseColumn( int32_t i ) const;

      void erase_element( int32_t i, int32_t j );

      /** Avoid using this function, prefer the more standard 
          Reader<SparseMatrix> instead.
      */
      void read( const std::string& filename,
                 const std::string & openmode = "binar", 
                 bool bswap = false );
      /** Avoid using this function, prefer the more standard 
          Writer<SparseMatrix> instead.
      */
      void write( const std::string& filename,
                  const std::string & openmode = "binar", 
                  bool bswap = false ) const;
      aims::SparseMatrix &
        operator += ( const aims::SparseMatrix& thing );
      aims::SparseMatrix &
        operator -= ( const aims::SparseMatrix& thing );
      aims::SparseMatrix &
        operator *= ( double x );
      aims::SparseMatrix &
        operator /= ( double x );

      /// for low-level boost operations
      boost_sparse_matrix & boostMatrix() { return _matrix; }
      const boost_sparse_matrix & boostMatrix() const { return _matrix; }

      carto::Object header() { return _header; }
      const carto::Object header() const { return _header; }
      void setHeader( carto::Object ph );

    protected:

      boost_sparse_matrix _matrix;
      carto::Object  _header;

  };


  template <typename VectorType>
  inline
  VectorType aims::SparseMatrix::getSparseRow( int32_t i ) const
  {

    try
    {

      VectorType row( getSize2() );
      boost_sparse_matrix::const_iterator1
        ir = _matrix.find1( 0, i, 0 );
      if( (ir != _matrix.end1()) && ((int)ir.index1() == i ))
      {
        boost_sparse_matrix::const_iterator2
          ic, ec = ir.end();
        long n = 0;
        for( ic=ir.begin(); ic!=ec; ++ic, ++n )
          row[ic.index2()] = *ic;
      }
      return row;

    }
    catch( std::exception & e )
    {
      throw std::runtime_error( std::string(
          "aims::SparseMatrix::getSparseRow( "
          "int32_t i ) const" ) + e.what() );
    }

  }


  template <typename VectorType>
  inline
  VectorType aims::SparseMatrix::getSparseColumn( int32_t j ) const
  {

    try
    {

      VectorType column( getSize1() );
      for ( int32_t s = 0; s < getSize1(); s++ )
      {

        column[ s ] = ( *this )( s, j );

      }
      return column;

    }
    catch( std::exception & e )
    {
      throw std::runtime_error( std::string(
          "aims::SparseMatrix::getSparseColumn( "
          "int32_t j ) const" ) + e.what() );
    }

  }

//
// unary operators + & -
//

// + SparseMatrix
aims::SparseMatrix 
    operator + ( const aims::SparseMatrix& thing );

// - SparseMatrix
aims::SparseMatrix 
    operator - ( const aims::SparseMatrix& thing );

//
// binary operator +
//

// SparseMatrix + SparseMatrix
aims::SparseMatrix
    operator + ( const aims::SparseMatrix& thing1,
                 const aims::SparseMatrix& thing2 );


//
// binary operator -
//

// SparseMatrix - SparseMatrix
aims::SparseMatrix
    operator - ( const aims::SparseMatrix& thing1,
                 const aims::SparseMatrix& thing2 );


//
// binary operator *
//

// SparseMatrix * SparseMatrix
aims::SparseMatrix
    operator * ( const aims::SparseMatrix& thing1,
                 const aims::SparseMatrix& thing2 );

// SparseMatrix * std::vector
std::vector<double>
    operator * ( const aims::SparseMatrix& thing1,
                 const std::vector<double>& thing2 );

// SparseMatrix * double
aims::SparseMatrix
    operator * ( const aims::SparseMatrix& thing1,
                 const double& thing2 );


//
// binary operator /
//

// SparseMatrix / double
aims::SparseMatrix
    operator / ( const aims::SparseMatrix& thing1,
                 const double& thing2 );

}

namespace std
{

  std::ostream& operator << ( ostream& os, const aims::SparseMatrix& thing );
  bool operator == ( const aims::SparseMatrix& thing1,
                     const aims::SparseMatrix& thing2 );

}


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template<> class DataTypeCode<aims::SparseMatrix>
  {
  public:
    static std::string objectType()
    { return "SparseMatrix"; }
    static std::string dataType()
    { return "DOUBLE"; }
    static std::string name()
    {
      return std::string("SparseMatrix_") + dataType();
    }
  };

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

DECLARE_GENERIC_OBJECT_TYPE( aims::SparseMatrix )

}

#endif
