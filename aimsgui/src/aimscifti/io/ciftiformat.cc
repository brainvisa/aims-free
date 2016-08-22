/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL license version 2 under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#include <aims/io/ciftiformat.h>
#include <aims/io/ciftiobject.h>
#include <CiftiException.h>
#include <CiftiFile.h>
#include <MultiDimIterator.h>

#include <iostream>
#include <vector>

using namespace aims;
using namespace carto;
using namespace cifti;
using namespace std;

namespace aims
{

  class CiftiLazyReader : public SparseOrDenseMatrix::MatrixLazyReader
  {
  public:
    CiftiLazyReader( CiftiFile *ciftifile );
    virtual ~CiftiLazyReader();
    virtual bool hasRow( int32_t s1 ) const;
    virtual bool hasColumn( int32_t s2 ) const;
    virtual std::vector<double> *readRow( int32_t s1 );
    virtual std::vector<double> *readColumn( int32_t s2 );
    virtual void freeRow( int32_t s1 )
    { _read_rows.erase( s1 ); _read_cols.clear(); }
    virtual void freeColumn( int32_t s2 )
    { _read_cols.erase( s2 ); _read_rows.clear(); }
    virtual void selectDimension( const vector<int32_t> & dims );

    vector<int32_t> fix_dims( const vector<int32_t> & pos );

  protected:
    std::set<int32_t> _read_rows;
    std::set<int32_t> _read_cols;
    CiftiFile *_cifti_file;
    vector<int32_t> _current_dim;
  };

}

CiftiLazyReader::CiftiLazyReader( CiftiFile *ciftifile )
  : _cifti_file( ciftifile )
{

}


CiftiLazyReader::~CiftiLazyReader()
{
  delete _cifti_file;
}

bool CiftiLazyReader::hasRow( int32_t s1 ) const
{
  return _read_rows.find( s1 ) != _read_rows.end();
}


bool CiftiLazyReader::hasColumn( int32_t s2 ) const
{
  return _read_cols.find( s2 ) != _read_cols.end();
}


std::vector<double> *CiftiLazyReader::readRow( int32_t s1 )
{
  const vector<int64_t>& dims = _cifti_file->getDimensions();
  vector<float> scratchRow( dims[0] );
  vector<double> *aimsrow = new vector<double>( dims[0] );

  // mark this row as read
  _read_rows.insert( s1 );

  vector<int64_t> indexSelect( dims.size() - 1, 1 );
  int64_t i, n = dims.size();
  for( i=2; i<n; ++i )
    indexSelect[i] = _current_dim[i-2];
  indexSelect[0] = s1;

  _cifti_file->getRow( &scratchRow[0], indexSelect );

  for( i=0; i<dims[0]; ++i )
    (*aimsrow)[i] = scratchRow[i];

  return aimsrow;
}


std::vector<double> *CiftiLazyReader::readColumn( int32_t s2 )
{
  // Warning: the CIFTI API doesn allow to read columns on other than the 1st
  // dimensions. The _current_dim position is thus ignored.
  const vector<int64_t>& dims = _cifti_file->getDimensions();
  vector<float> scratchCol( dims[1] );
  vector<double> *aimscol = new vector<double>( dims[1] );

  // for now we only read 2D matrices

  // mark this col as read
  _read_cols.insert( s2 );

  _cifti_file->getColumn( &scratchCol[0], s2 );

  int32_t i;
  for( i=0; i<dims[1]; ++i )
    (*aimscol)[i] = scratchCol[i];

  return aimscol;
}


vector<int32_t> CiftiLazyReader::fix_dims( const vector<int32_t> & pos )
{
  size_t i, n = _current_dim.size();
  if( n == 0 )
    n = _cifti_file->getDimensions().size() - 2;
  if( pos.size() == n )
    return pos;
  vector<int32_t> fixed_pos( n, 0 );
  for( i=0; i<n; ++i )
    fixed_pos[i] = pos[i];
  return fixed_pos;
}


void CiftiLazyReader::selectDimension( const vector<int32_t> & dims )
{
  vector<int32_t> fixed_dims = fix_dims( dims );
  if( fixed_dims != _current_dim )
  {
    _current_dim = fixed_dims;
    _read_rows.clear();
    _read_cols.clear();
  }
}


// ---

bool CiftiFormat::read( const std::string & filename,
                        SparseOrDenseMatrix & vol,
                        const carto::AllocatorContext & context,
                        carto::Object options )
{
  CiftiFile *inputFile = new CiftiFile( filename.c_str() );//on-disk reading

  // header

  string	format = "CIFTI-2";

  CiftiObjectConverter ciftiobj;
  Object cifti_tree = ciftiobj.ciftiToObject( *inputFile );

  // matrix size

  const vector<int64_t>& dims = inputFile->getDimensions();
  vol.reallocate( dims[1], dims[0] );
  vol.muteToSparse();
//   int ndim = dims.size();
//   if( ndim > 2 && dims[2] != 1 )
//   {
//     Volume<double>::Position4Di p( dims[0], dims[1], dims[2], 1 );
//     if( ndim > 3 )
//       p[3] = dims[3];
//     vol.denseMatrix()->reallocate( p );
//   }

  // header
  Object hdr = vol.header();
  hdr->setProperty( "file_type", format );
  hdr->setProperty( "data_type", string("DOUBLE") );
  vector<int> dimsi( dims.begin(), dims.end() );
  hdr->setProperty( "dimensions", dimsi );
  hdr->copyProperties( cifti_tree );

  // .minf
  PythonHeader ph;
  ph.readMinf( filename + ".minf" );
  hdr->copyProperties( Object::reference( ph ) );

  // lazy reader

  CiftiLazyReader *cifti_lazy = new CiftiLazyReader( inputFile );
  vol.setLazyReader( cifti_lazy );

  // matrix contents

//   vector<float> scratchRow(dims[0]); //read a row at a time
// //   vector<double> aimsrow(dims[0]);
//   SparseOrDenseMatrix::DenseMatrixType voldat = vol.denseMatrix();
//   int64_t i, z = 0, t = 0;
//   for (MultiDimIterator<int64_t>
//       iter(vector<int64_t>(dims.begin() + 1, dims.end()));
//       !iter.atEnd();
//       ++iter)
//   {//helper class to iterate over 2D and 3D cifti with the same code - the "+ 1" is to drop the first dimension (row length)
//     inputFile.getRow(&scratchRow[0], *iter);
//     if( ndim >= 3 )
//     {
//       z = (*iter)[1];
//       if( ndim >= 4 )
//         t = (*iter)[2];
//     }
//     double *ptr = &voldat->at( 0, (*iter)[0], z, t );
//     for( i=0; i<dims[0]; ++i )
// //       aimsrow[i] = scratchRow[i];
//       *ptr++ = scratchRow[i];
//     // vol.setRow( (int32_t) (*iter)[0], aimsrow );
//   }

  return true;
}


bool CiftiFormat::write( const std::string & filename,
                        const SparseOrDenseMatrix & vol,
                        carto::Object options )
{
  return false;
}


