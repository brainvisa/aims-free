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

#include <aims/sparsematrix/sparseordensematrix.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/finder.h>
#include <cartobase/object/object_d.h>

using namespace aims;
using namespace carto;
using namespace std;


SparseOrDenseMatrix::SparseOrDenseMatrix( 
  int32_t size1, int32_t size2 )
  : _sparsematrix( new SparseMatrix( size1, size2 ) ),
  _fakeheader( 0 )
{
}


SparseOrDenseMatrix::SparseOrDenseMatrix( const SparseOrDenseMatrix& other )
{
  if( other.isDense() )
  {
    _sparsematrix.reset( 0 );
    _densematrix.reset( new Volume<double>( *other.denseMatrix() ) );
    _fakeheader = Object::reference( _densematrix->header() );
  }
  else
  {
    _densematrix.reset( 0 );
    _sparsematrix.reset( new SparseMatrix( *other.sparseMatrix() ) );
    _fakeheader.reset( 0 );
  }
}


SparseOrDenseMatrix::~SparseOrDenseMatrix()
{
}


SparseOrDenseMatrix& SparseOrDenseMatrix::operator = ( 
  const SparseOrDenseMatrix& other )
{
  _sparsematrix = other.sparseMatrix();
  _densematrix = other.denseMatrix();
  if( _densematrix.get() )
    _fakeheader = Object::reference( _densematrix->header() );
  else
    _fakeheader.reset( 0 );
}


void SparseOrDenseMatrix::reallocate( int32_t size1, int32_t size2 )
{
  if( isDense() )
    denseMatrix()->reallocate( size1, size2 );
  else
    sparseMatrix()->reallocate( size1, size2 );
}


int32_t SparseOrDenseMatrix::getSize1() const
{
  if( isDense() )
    return denseMatrix()->getSizeY();
  else
    return sparseMatrix()->getSize1();
}


int32_t SparseOrDenseMatrix::getSize2() const
{
  if( isDense() )
    return denseMatrix()->getSizeX();
  else
    return sparseMatrix()->getSize2();
}


int32_t SparseOrDenseMatrix::getNonZeroElementCount() const
{
  if( isDense() )
  {
    int x, y, nx = denseMatrix()->getSizeX(), ny = denseMatrix()->getSizeY();
    const double *buf;
    long inc;
    unsigned long count = 0;

    for( y=0; y<ny; ++ny )
    {
      buf = &denseMatrix()->at( 0, y );
      inc = &denseMatrix()->at( 1, y ) - buf;
      for( x=0; x<nx; ++x, buf += inc )
        if( *buf != 0 )
          ++count;
    }
    return count;
  }
  else
    return sparseMatrix()->getNonZeroElementCount();
}


bool SparseOrDenseMatrix::hasElement( int32_t i, int32_t j ) const
{
  if( isDense() )
    return denseMatrix()->at( j, i ) != 0;
  else
    return sparseMatrix()->hasElement( i, j );
}


double SparseOrDenseMatrix::operator()( int32_t i, int32_t j ) const
{
  if( isDense() )
    return denseMatrix()->at( j, i );
  else
    return (*sparseMatrix())( i, j );
}


void SparseOrDenseMatrix::set_element( int32_t i, int32_t j, double x )
{
  if( isDense() )
    denseMatrix()->at( j, i ) = x;
  else
    (*sparseMatrix())( i, j ) = x;
}


void SparseOrDenseMatrix::erase_element( int32_t i, int32_t j )
{
  if( isDense() )
    denseMatrix()->at( j, i ) = 0;
  else
    sparseMatrix()->erase_element( i, j );
}


void SparseOrDenseMatrix::setRow( int32_t s1, 
                                  const std::vector<double>& row )
{
  if( isDense() )
  {
    double *buf = &denseMatrix().at( 0, s1 );
    long inc = &denseMatrix().at( 1, s1 ) - buf;
    vector<double>::const_iterator i, e = row.end();
    for( i=row.begin(); i!=e; ++i, buf += inc )
      *buf = *i;
  }
  else
    sparseMatrix()->setRow( s1, row );
}


void SparseOrDenseMatrix::setColumn( int32_t s2, 
                                     const std::vector<double>& column )
{
  if( isDense() )
  {
    double *buf = &denseMatrix().at( s2, 0 );
    long inc = &denseMatrix().at( s2, 1 ) - buf;
    vector<double>::const_iterator i, e = column.end();
    for( i=column.begin(); i!=e; ++i, buf += inc )
      *buf = *i;
  }
  else
    sparseMatrix()->setColumn( s2, column );
}


std::vector<double> SparseOrDenseMatrix::getRow( int32_t s1 ) const
{
  if( isDense() )
  {
    vector<double> row( denseMatrix()->getSizeX() );
    const double *buf = &denseMatrix().at( 0, s1 );
    long inc = &denseMatrix().at( 1, s1 ) - buf;
    vector<double>::iterator i, e = row.end();
    for( i=row.begin(); i!=e; ++i, buf += inc )
      *i = *buf;
    return row;
  }
  else
    return sparseMatrix()->getRow( s1 );
}


std::vector<double> SparseOrDenseMatrix::getColumn( int32_t j ) const
{
  if( isDense() )
  {
    vector<double> column( denseMatrix()->getSizeY() );
    const double *buf = &denseMatrix().at( j, 0 );
    long inc = &denseMatrix().at( j, 1 ) - buf;
    vector<double>::iterator i, e = column.end();
    for( i=column.begin(); i!=e; ++i, buf += inc )
      *i = *buf;
    return column;
  }
  else
    return sparseMatrix()->getColumn( j );
}


void SparseOrDenseMatrix::read( const std::string& filename )
{
  Finder f;
  if( !f.check( filename ) )
    throw wrong_format_error( filename );

  int nz;
  vector<int> dims;

  Object hdr = f.headerObject();

  if( hdr->getProperty( "dimensions", dims ) && dims.size() == 2
    && hdr->getProperty( "non_zero_elements", nz ) )
  {
    // the map takes about 8 times the size of a double for an element
    if( nz * sizeof( double ) * 8 < long( dims[0] ) * dims[1] )
    {
      // use sparse
      cout << "read SparseOrDenseMatrix as sparse\n";
      Reader<SparseMatrix> r( filename );
      SparseMatrix* sp = r.read();
      if( sp )
      {
        setMatrix( rc_ptr<SparseMatrix>( sp ) );
        sp->header()->setProperty( "storage_mode", string( "sparse" ) );
        return;
      }
    }
  }

  // use dense
  Reader<Volume<double> > r( filename );
  Volume<double> *dense = r.read();
  if( dense )
  {
    cout << "read SparseOrDenseMatrix as dense\n";
    setMatrix( VolumeRef<double>( dense ) );
    dense->header().setProperty( "storage_mode", string( "dense" ) );
  }

}


void SparseOrDenseMatrix::write( const std::string& filename, 
                                 Object options ) const
{
  if( isDense() )
  {
    Writer<Volume<double> > w( filename, options );
    w.write( *denseMatrix() );
  }
  else
  {
    // WARNING: not implemented: writing as dense while sparse in memory
    Writer<SparseMatrix> w( filename, options );
    w.write( *sparseMatrix() );
  }
}


void SparseOrDenseMatrix::setMatrix( SparseMatrixType matrix )
{
  _densematrix.reset( 0 );
  _sparsematrix = matrix;
  _fakeheader.reset( 0 );
}


void SparseOrDenseMatrix::setMatrix( DenseMatrixType matrix )
{
  _sparsematrix.reset( 0 );
  _densematrix = matrix;
  _fakeheader = Object::reference( _densematrix->header() );
}


Object SparseOrDenseMatrix::header()
{
  if( isDense() )
    return _fakeheader;
  else
    return sparseMatrix()->header();
}


const Object SparseOrDenseMatrix::header() const
{
  if( isDense() )
    return _fakeheader;
  else
    return sparseMatrix()->header();
}


void SparseOrDenseMatrix::setHeader( Object ph )
{
  if( isDense() )
    denseMatrix()->header() = ph->value<PropertySet>();
  else
    sparseMatrix()->header() = ph;
}


namespace carto 
{

INSTANTIATE_GENERIC_OBJECT_TYPE( aims::SparseOrDenseMatrix )

}
