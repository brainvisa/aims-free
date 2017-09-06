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

#include <aims/sparsematrix/sparseMatrix.h>
#include <aims/sparsematrix/sparseMatrixItemR.h>
#include <aims/sparsematrix/sparseMatrixItemW.h>
#include <cartobase/object/object_d.h>
#include <fstream>
#include <boost/numeric/ublas/matrix.hpp>

using namespace aims;
using namespace carto;


aims::SparseMatrix::SparseMatrix( int32_t size1, int32_t size2 )
  : _matrix( size1, size2, 0 ), _header( Object::value( PropertySet() ) )
{
}


aims::SparseMatrix::SparseMatrix( const aims::SparseMatrix& other )
  : _matrix( other._matrix ), _header( Object::value( PropertySet() ) )
{
  *_header = *other.header();
}


aims::SparseMatrix::~SparseMatrix()
{
}



aims::SparseMatrix& 
    aims::SparseMatrix::operator=( const aims::SparseMatrix& other )
{

  try
  {

    _matrix = other._matrix;
    *_header = *other.header();

    return *this;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string("aims::SparseMatrix& "
        "aims::SparseMatrix::operator=( "
        "const aims::SparseMatrix& other )") + e.what() );
  }

}

void aims::SparseMatrix::reallocate( int32_t size1, int32_t size2 )
{

  try
  {

    // do not preservce because not implemented in boost !
    _matrix.resize( size1, size2, false );

  }

  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "void "
        "aims::SparseMatrix::reallocate( "
        "int32_t size1, int32_t size2 )" ) + e.what() );
  }

}


aims::SparseMatrix::iterator1 aims::SparseMatrix::begin1()
{

  try
  {

    return _matrix.begin1();

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix::iterator1 "
        "aims::SparseMatrix::begin1()" ) + e.what() );
  }

}


aims::SparseMatrix::iterator1 aims::SparseMatrix::end1()
{

  try
  {

    return _matrix.end1();

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix::iterator1 "
        "aims::SparseMatrix::end1()" ) + e.what() );
  }

}


aims::SparseMatrix::const_iterator1 aims::SparseMatrix::begin1() const
{

  try
  {

    return _matrix.begin1();

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix::iterator1 "
        "aims::SparseMatrix::begin1() const" ) + e.what() );
  }

}


aims::SparseMatrix::const_iterator1 aims::SparseMatrix::end1() const
{

  try
  {

    return _matrix.end1();

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix::const_iterator1 "
        "aims::SparseMatrix::end1() const" ) + e.what() );
  }

}

aims::SparseMatrix::iterator2 aims::SparseMatrix::begin2()
{

  try
  {

    return _matrix.begin2();

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix::iterator2 "
        "aims::SparseMatrix::begin2()" ) + e.what() );
  }

}


aims::SparseMatrix::iterator2 aims::SparseMatrix::end2()
{

  try
  {

    return _matrix.end2();

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix::iterator2 "
        "aims::SparseMatrix::end2()" ) + e.what() );
  }

}


aims::SparseMatrix::const_iterator2 aims::SparseMatrix::begin2() const
{

  try
  {

    return _matrix.begin2();

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix::iterator2 "
        "aims::SparseMatrix::begin2() const" ) + e.what() );
  }

}


aims::SparseMatrix::const_iterator2 aims::SparseMatrix::end2() const
{

  try
  {

    return _matrix.end2();

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix::const_iterator2 "
        "aims::SparseMatrix::end2() const" ) + e.what() );
  }

}

int32_t aims::SparseMatrix::getSize1() const
{

  try
  {

    return ( int32_t )_matrix.size1();

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "int32_t "
        "aims::SparseMatrix::getSize1() const" ) + e.what() );
  }

}


int32_t aims::SparseMatrix::getSize2() const
{

  try
  {

    return ( int32_t )_matrix.size2();

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "int32_t "
        "aims::SparseMatrix::getSize2() const" ) + e.what() );
  }

}


int32_t aims::SparseMatrix::getNonZeroElementCount() const
{

  try
  {

#if BOOST_VERSION < 103300
    boost_sparse_matrix::size_type filled = 0;
    boost_sparse_matrix::const_iterator2 itv2, etv2;
    for (boost_sparse_matrix::const_iterator1 itv = _matrix.begin1 (); itv != _matrix.end1 (); ++ itv)
      for( itv2=itv.begin(), etv2=itv.end(); itv2!=etv2; ++itv2 )
        ++filled;
    return filled;

#else
    return ( int32_t )_matrix.nnz();
#endif

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "int32_t aims::SparseMatrix::getNonZeroElementCount() "
        "const" ) + e.what() );
  }

}


bool aims::SparseMatrix::hasElement( int32_t i, int32_t j ) const
{

  try
  {

#if BOOST_VERSION < 103300
    return _matrix.at_element( i, j ) != 0;
#else
    return _matrix.find_element( i, j );
#endif

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "bool "
        "aims::SparseMatrix::hasElement( "
        "int32_t i, int32_t j ) const" ) + e.what() );
  }


}


aims::SparseMatrix::const_reference 
    aims::SparseMatrix::operator()( int32_t i, int32_t j ) const
{

  try
  {

    return _matrix( i, j );

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "const double& "
        "aims::SparseMatrix::operator()( "
        "int32_t i, int32_t j ) const" ) +  e.what() );
  }

}


aims::SparseMatrix::reference 
    aims::SparseMatrix::operator()( int32_t i, int32_t j )
{

  try
  {

    return _matrix( i, j );

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "double& aims::SparseMatrix::operator()( "
        "int32_t i, int32_t j )" ) + e.what() );
  }

}


bool aims::SparseMatrix::isSquare() const
{

  try
  {

    return ( _matrix.size1() == _matrix.size2() );

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "bool "
        "aims::SparseMatrix::isSquare() const" ) + e.what() );
  }

}


void aims::SparseMatrix::setZero()
{

  try
  {
    _matrix.clear();
  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "void "
        "aims::SparseMatrix::setZero()" ) + e.what() );
  }

}


void aims::SparseMatrix::setIdentity()
{

  try
  {

    _matrix.clear();

    int32_t size = getSize1();

    int32_t s = 0;
    for ( s = 0; s < size; s++ )
    {

        _matrix( s, s ) = 1.0;

    }

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "void "
        "aims::SparseMatrix::setIdentity()" ) + e.what() );
  }

}


void aims::SparseMatrix::setRow( int32_t s1, const std::vector<double>& row )
{

  try
  {

    if ( int32_t(row.size()) != getSize2() )
    {

      throw std::runtime_error( "incoherent size(s)" );

    }
    int32_t s2;
    for ( s2 = 0; s2 < getSize2(); s2++ )
    {

      const double & element = row[s2];
      if( element == 0 )
        erase_element( s1, s2 );
      else
        ( *this )( s1, s2 ) = element;

    }

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "void "
        "aims::SparseMatrix::setRow( int32_t s1, "
        "const std::vector<double>& row )" ) + e.what() );
  }

}


void aims::SparseMatrix::setColumn( int32_t s2, const std::vector<double>& column )
{

  try
  {

    if ( int32_t(column.size()) != getSize1() )
    {

      throw std::runtime_error( "incoherent size(s)" );

    }
    int32_t s1;
    for ( s1 = 0; s1 < getSize1(); s1++ )
    {

      ( *this )( s1, s2 ) = column[ s1 ];

    }

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "void "
        "aims::SparseMatrix::setColumn( "
        "int32_t s2, const std::vector<double>& column )" ) + e.what() );
  }

}


void aims::SparseMatrix::fill( const double& value )
{

  try
  {

    int32_t size1 = getSize1();
    int32_t size2 = getSize2();

    int32_t s1 = 0;
    int32_t s2 = 0;
    for ( s1 = 0; s1 < size1; s1++ )
    {

      for ( s2 = 0; s2 < size2; s2++ )
      {

        _matrix( s1, s2 ) = value;

      }

    }

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "void "
        "aims::SparseMatrix::fill( "
        "const double& value )" ) + e.what() );
  }

}


void aims::SparseMatrix::fill( int32_t offset1,
                               int32_t offset2,
                               int32_t size1,
                               int32_t size2,
                               const double& value )
{

  try
  {

    if ( ( offset1 < 0 ) ||
           ( offset2 < 0 ) ||
           ( offset1 + size1 > getSize1() ) ||
           ( offset2 + size2 > getSize2() ) )

    {

      throw std::runtime_error( "incompatible offset(s)/size(s)" );

    }
    int32_t s1, s2;
    for ( s1 = 0; s1 < size1; s1++ )
    {

      for ( s2 = 0; s2 < size2; s2++ )
      {

        ( *this )( offset1 + s1, offset2 + s2 ) = value;

      }

    }

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "void aims::SparseMatrix::fill( "
        "int32_t offset1, "
        "int32_t offset2, "
        "int32_t size1, "
        "int32_t size2, "
        "const double& value )" ) + e.what() );
  }

}


void aims::SparseMatrix::fill( int32_t offset1,
                               int32_t offset2,
                               const aims::SparseMatrix& other,
                               int32_t size1,
                               int32_t size2 )
{

  try
  {


    if ( ( size1 < 0 ) || ( size2 < 0 ) )
    {

      throw std::runtime_error( "size1 and size2 must be nul or positive" );

    }

    int32_t targetSize1 = ( size1 ? size1 : other.getSize1() );
    int32_t targetSize2 = ( size2 ? size2 : other.getSize2() );

    if ( ( offset1 < 0 ) ||
           ( offset2 < 0 ) ||
           ( offset1 + targetSize1 > getSize1() ) ||
           ( offset2 + targetSize2 > getSize2() ) )
    {

      throw std::runtime_error( "incompatible offset(s)/other matrix size" );

    }
    int32_t s1, s2;
    for ( s1 = 0; s1 < targetSize1; s1++ )
    {

      for ( s2 = 0; s2 < targetSize2; s2++ )
      {

        if ( hasElement( s1, s2 ) )
        {

          ( *this )( offset1 + s1, offset2 + s2 ) = other( s1, s2 );

        }

      }

    }

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "void aims::SparseMatrix::fill( "
        "int32_t offset1, "
        "int32_t offset2, "
        "const aims::SparseMatrix& other, "
        "int32_t size1, "
        "int32_t size2 )" ) + e.what() );
  }

}


void aims::SparseMatrix::setDiagonal( const double& value )
{

  try
  {

    _matrix.clear();
    int32_t s;
    int32_t maximumS = std::min( getSize1(), getSize2() );
    for ( s = 0; s < maximumS; s++ )
    {

      _matrix( s, s ) = value;

    }

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "void "
        "aims::SparseMatrix::setDiagonal( "
        "const double& value )" ) + e.what() );
  }

}


void aims::SparseMatrix::transpose()
{

  try
  {

    aims::SparseMatrix tmp( *this );
    reallocate( tmp.getSize2(), tmp.getSize1() );

    for ( int32_t s1 = 0; s1 < getSize1(); s1++ )
    {

      for ( int32_t s2 = 0; s2 < getSize2(); s2++ )
      {

        if ( tmp.hasElement( s2, s1 ) )
        {

          ( *this )( s1, s2 ) = tmp( s2, s1 );

        }

      }

    }

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "void "
        "aims::SparseMatrix::transpose()" ) +  e.what() );
  }

}


double aims::SparseMatrix::getTrace() const
{

  try
  {

    if ( !isSquare() )
    {

      throw std::runtime_error( "not a square matrix" );

    }

    double trace = 0.0;
    int32_t s;
    for ( s = 0; s < getSize1(); s++ )
    {

      trace += ( *this )( s, s );

    }

    return trace;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "double "
        "aims::SparseMatrix::getTrace() const" ) + e.what() );
  }

}


aims::SparseMatrix aims::SparseMatrix::getTransposition() const
{

  try
  {

    aims::SparseMatrix matrix( *this );
    matrix.transpose();
    return matrix;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix "
        "aims::SparseMatrix::getTransposition() const" ) + e.what() );
  }

}


aims::SparseMatrix 
    aims::SparseMatrix::getComposition( const aims::SparseMatrix& other ) const
{

  try
  {

    if ( getSize2() != other.getSize1() )
    {

      std::runtime_error( "the row count for other matrix is not equal to "
          "the column count for this matrix" );
    }

    aims::SparseMatrix composition( getSize1(), other.getSize2() );

    for ( int32_t s1 = 0; s1 < composition.getSize1(); s1++ )
    {

      for ( int32_t s2 = 0; s2 < composition.getSize2(); s2++ )
      {

        double value = 0.0;
        bool nonZero = false;
        for ( int32_t k = 0; k < getSize2(); k++ )
        {

          if ( this->hasElement( s1, k ) && other.hasElement( k, s2 ) )
          {

            value += ( *this )( s1, k ) * other( k, s2 );
            nonZero = true;

          }

        }

        if ( nonZero )
        {

          composition( s1, s2 ) = value;

        }

      }

    }
    return composition;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix "
        "aims::SparseMatrix::getComposition( "
        "const aims::SparseMatrix& other ) const" ) + e.what() );
  }

}


std::vector<double> 
    aims::SparseMatrix::getComposition( const std::vector<double>& other ) const
{

  try
  {

    if ( getSize2() != int32_t(other.size()) )
    {

      std::runtime_error( "the vector size is not equal to "
          "the column count for this matrix" );
    }

    std::vector<double> composition( getSize1() );

    for ( int32_t s = 0; s < ( int32_t )composition.size(); s++ )
    {

      composition[ s ] = 0.0;
      for ( int32_t k = 0; k < getSize2(); k++ )
      {

        composition[ s ] += ( *this )( s, k ) * other[ k ];

      }

    }
    return composition;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::Vector "
        "aims::SparseMatrix::getComposition( "
        "const aims::Vector& other ) const" ) + e.what() );
  }

}


std::vector<double> aims::SparseMatrix::toVector() const
{

  try
  {

    std::vector<double> result;
    if ( getSize1() == 1 )
    {

      result.resize( getSize2() );
      for ( int32_t s = 0; s < ( int32_t )result.size(); s++ )
      {

        result[ s ] = ( *this )( 0, s );

      }

    }
    else if ( getSize2() == 1 )
    {

      result.resize( getSize1() );
      for ( int32_t s = 0; s < ( int32_t )result.size(); s++ )
      {

        result[ s ] = ( *this )( s, 0 );

      }

    }
    else
    {

      std::runtime_error( "none of the size is equal to 1" );

    }
    return result;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "std::vector "
        "aims::SparseMatrix::toVector() const" ) + e.what() );
  }

}


std::vector<double> aims::SparseMatrix::getRow( int32_t i ) const
{

  try
  {

    std::vector<double> row( getSize2(), 0. );
    boost_sparse_matrix::const_iterator1
      ir = _matrix.find1( 0, i, 0 );
    if( ir != _matrix.end1() && ir.index1() == static_cast<size_t>(i) )
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
    throw std::runtime_error( std::string( "std::vector "
        "aims::SparseMatrix::getRow( "
        "int32_t i ) const" ) + e.what() );
  }

}


std::vector<double> aims::SparseMatrix::getColumn( int32_t j ) const
{

  try
  {

    std::vector<double> column( getSize1() );
    for ( int32_t s = 0; s < getSize1(); s++ )
    {

      column[ s ] = ( *this )( s, j );

    }
    return column;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "std::vector "
        "aims::SparseMatrix::getColumn( "
        "int32_t j ) const" ) + e.what() );
  }

}


// #ifdef SPARSEVECTOR_H
// til::SparseVector<double> aims::SparseMatrix::getSparseRow( int32_t i ) const
// {
// 
//   try
//   {
// 
//     til::SparseVector<double> row( getSize2() );
//     boost_sparse_matrix::const_iterator1
//       ir = _matrix.find1( 0, i, 0 );
//     if( ir != _matrix.end1() && ir.index1() == i )
//     {
//       boost_sparse_matrix::const_iterator2
//         ic, ec = ir.end();
//       long n = 0;
//       for( ic=ir.begin(); ic!=ec; ++ic, ++n )
//         row[ic.index2()] = *ic;
//     }
//     return row;
// 
//   }
//   catch( std::exception & e )
//   {
//     throw std::runtime_error( std::string( "til::SparseVector<double> "
//         "aims::SparseMatrix::getSparseRow( "
//         "int32_t i ) const" ) + e.what() );
//   }
// 
// }
// 
// 
// til::SparseVector<double> aims::SparseMatrix::getSparseColumn( int32_t j ) const
// {
// 
//   try
//   {
// 
//     til::SparseVector<double> column( getSize1() );
//     for ( int32_t s = 0; s < getSize1(); s++ )
//     {
// 
//       column[ s ] = ( *this )( s, j );
// 
//     }
//     return column;
// 
//   }
//   catch( std::exception & e )
//   {
//     throw std::runtime_error( std::string( "til::SparseVector<double> "
//         "aims::SparseMatrix::getSparseColumn( "
//         "int32_t j ) const" ) + e.what() );
//   }
// 
// }
// #endif


void aims::SparseMatrix::erase_element( int32_t i, int32_t j )

{

  try
  {

#if BOOST_VERSION < 103300
    _matrix.erase( i, j );
#else
    _matrix.erase_element( i, j );
#endif

  }
  catch (std::exception & e )
  {
    throw std::runtime_error( std::string( "void "
        "aims::SparseMatrix::erase_element"
        "( int32_t i, int32_t j )" ) + e.what() );
  }

  
}

void aims::SparseMatrix::read( const std::string& filename,
                               const std::string & openmode, 
                               bool bswap )
{
  
  try
  {
    
    std::ifstream is( filename.c_str() );

    aims::DefaultItemReader<aims::SparseMatrix> 
        defaultItemR;
    aims::ItemReader<aims::SparseMatrix>*
        sparseMatrixItemR = defaultItemR.reader( openmode, bswap );

    sparseMatrixItemR->read( is, *this );
    is.close();

    delete sparseMatrixItemR;
    
  }
  catch (std::exception & e )
  {
    throw std::runtime_error( std::string( "void "
        "aims::SparseMatrix::read( const std::string& filename, "
        "const std::string & openmode, "
        "bool bswap )" ) + e.what() );
  }
  
}


void aims::SparseMatrix::write( const std::string& filename,
                                const std::string & openmode, 
                                bool bswap ) const
{
  
  try
  {
    
    std::ofstream os( filename.c_str() );

    aims::DefaultItemWriter<aims::SparseMatrix> 
        defaultItemW;
    aims::ItemWriter<aims::SparseMatrix>*
        sparseMatrixItemW = defaultItemW.writer( openmode, bswap );

    sparseMatrixItemW->write( os, *this );
    os.close();

    delete sparseMatrixItemW;
    
  }
  catch (std::exception & e )
  {
    throw std::runtime_error( std::string( "void "
        "aims::SparseMatrix::write( const std::string& filename, "
        "const std::string & openmode, "
        "bool bswap )" )+  e.what() );
  }
  
}


//
// unary operators + & -
//

// + SparseMatrix
aims::SparseMatrix 
    operator + ( const aims::SparseMatrix& thing )
{

  try
  {

    return thing;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix  "
        "operator + ( const aims::SparseMatrix& thing )" ) +  e.what() );
  }

}

// - SparseMatrix
aims::SparseMatrix 
    aims::operator - ( const aims::SparseMatrix& thing )
{

  try
  {

    aims::SparseMatrix result( thing );
    result *= -1;

    return result;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix "
        "operator - ( const aims::SparseMatrix& thing )" ) + e.what() );
  }

}


aims::SparseMatrix &
    aims::SparseMatrix::operator += ( const aims::SparseMatrix& thing )
{

  try
  {

    if ( ( thing.getSize1() != getSize1() ) ||
           ( thing.getSize2() != getSize2() ) )
    {

      throw std::runtime_error( "incompatible matrix size(s)" );

    }

    _matrix += thing._matrix;

    return *this;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix "
        "operator + ( const aims::SparseMatrix& thing1, "
        "const aims::SparseMatrix& thing2 )" ) + e.what() );
  }

}


aims::SparseMatrix &
    aims::SparseMatrix::operator -= ( const aims::SparseMatrix& thing )
{

  try
  {

    if ( ( thing.getSize1() != getSize1() ) ||
           ( thing.getSize2() != getSize2() ) )
    {

      throw std::runtime_error( "incompatible matrix size(s)" );

    }

    _matrix -= thing._matrix;

    return *this;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix "
        "operator + ( const aims::SparseMatrix& thing1, "
        "const aims::SparseMatrix& thing2 )" ) + e.what() );
  }

}


aims::SparseMatrix &
    aims::SparseMatrix::operator *= ( double x )
{

  _matrix *= x;

  return *this;
}


aims::SparseMatrix &
    aims::SparseMatrix::operator /= ( double x )
{

  _matrix /= x;

  return *this;
}



//
// binary operator +
//

// SparseMatrix + SparseMatrix
aims::SparseMatrix
    aims::operator + ( const aims::SparseMatrix& thing1,
                 const aims::SparseMatrix& thing2 )
{

  try
  {

    if ( ( thing1.getSize1() != thing2.getSize1() ) ||
           ( thing1.getSize2() != thing2.getSize2() ) )
    {

      throw std::runtime_error( "incompatible matrix size(s)" );

    }

    aims::SparseMatrix result( thing1 );
    result += thing2;

    return result;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix "
        "operator + ( const aims::SparseMatrix& thing1, "
        "const aims::SparseMatrix& thing2 )" ) + e.what() );
  }

}

//
// binary operator -
//

// SparseMatrix - SparseMatrix
aims::SparseMatrix
    aims::operator - ( const aims::SparseMatrix& thing1,
                 const aims::SparseMatrix& thing2 )
{

  try
  {

    if ( ( thing1.getSize1() != thing2.getSize1() ) ||
           ( thing2.getSize1() != thing2.getSize2() ) )
    {

      throw std::runtime_error( "incompatible matrix size(s)" );

    }

    aims::SparseMatrix result( thing1 );
    result -= thing2;

    return result;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix "
        "operator - ( const aims::SparseMatrix& thing1, "
        "const aims::SparseMatrix& thing2 )" ) + e.what() );
  }

}

//
// binary operator *
//

// SparseMatrix * SparseMatrix
aims::SparseMatrix
    aims::operator * ( const aims::SparseMatrix& thing1,
                 const aims::SparseMatrix& thing2 )
{

  try
  {

    return thing1.getComposition( thing2 );

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix "
        "operator * ( const aims::SparseMatrix& thing1, "
        "const aims::SparseMatrix& thing2 )" ) + e.what() );
  }

}

// SparseMatrix * std::vector<double>
std::vector<double>
    aims::operator * ( const aims::SparseMatrix& thing1,
                 const std::vector<double>& thing2 )
{

  try
  {

    return thing1.getComposition( thing2 );

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "std::vector<double> "
        "operator * ( const aims::SparseMatrix& thing1, "
        "const std::vector<double>& thing2 )" ) + e.what() );
  }

}

// SparseMatrix * double
aims::SparseMatrix
    aims::operator * ( const aims::SparseMatrix& thing1,
                 const double& thing2 )
{

  try
  {

    aims::SparseMatrix result( thing1 );
    result *= thing2;

    return result;

  }
  catch( std::exception & e )
  {
    throw std::runtime_error( std::string( "aims::SparseMatrix "
        "operator * ( const aims::SparseMatrix& thing1, "
        "const double& thing2 )" ) + e.what() );
  }

}

//
// binary operator /
//

// SparseMatrix / double
aims::SparseMatrix
    aims::operator / ( const aims::SparseMatrix& thing1,
                 const double& thing2 )
{

  try
  {

    aims::SparseMatrix result( thing1 );
    result /= thing2;

    return result;

  }
  catch( std::exception & e )
  {
    
    throw std::runtime_error( 
      std::string( "aims::SparseMatrix "
                   "operator / ( const aims::SparseMatrix& thing1, "
                   "const double& thing2 ) : " ) + e.what() );
  }

}

//
// standard operator(s)
//

std::ostream& std::operator << ( std::ostream& os, 
                                 const aims::SparseMatrix& thing )
{

  for ( int32_t s1 = 0; s1 < thing.getSize1(); s1++ )
  {

    for ( int32_t s2 = 0; s2 < thing.getSize2(); s2++ )
    {

      if ( thing.hasElement( s1, s2 ) )
      {

        os << thing( s1, s2 ) << " ";

      }
      else
      {

        os << "- ";

      }

    }
    if ( s1 != thing.getSize1() - 1 )
    {

      os << std::endl;

    }

  }
  return os;

}


bool std::operator == ( const aims::SparseMatrix& thing1,
                        const aims::SparseMatrix& thing2 )
{
  if( thing1.getSize1() != thing2.getSize1()
    || thing1.getSize2() != thing2.getSize2() )
    return false;

  aims::SparseMatrix::const_iterator1 il1, el1 = thing1.end1(),
    il2, el2 = thing2.end1();
  aims::SparseMatrix::const_iterator2 ic1, ec1, ic2, ec2;

  for( il1=thing1.begin1(), il2=thing2.begin1();
      il1!=el1 && il2!=el2 && il1.index1() == il2.index1(); ++il1, ++il2 )
  {
    for( ic1=il1.begin(), ec1=il1.end(), ic2=il2.begin(), ec2=il2.end();
        ic1!=ec1 && ic2!=ec2 && ic1.index2() == ic2.index2(); ++ic1, ++ic2 )
      if( *ic1 != *ic2 )
        return false;
    if( ic1 != ec1 || ic2 != ec2 )
      return false;
  }
  if( il1 != el1 || il2 != el2 )
    return false;
  return true;
}


void SparseMatrix::setHeader( Object ph )
{
  _header = ph;
}


namespace carto 
{

INSTANTIATE_GENERIC_OBJECT_TYPE( aims::SparseMatrix )

}
