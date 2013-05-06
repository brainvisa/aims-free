/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
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

/*
 *  Trieder item reader class
 */
#include <aims/sparsematrix/sparseMatrixItemR.h>


using namespace aims;
using namespace carto;
using namespace std;


ItemReader<SparseMatrix>* 
DefaultItemReader<SparseMatrix>::reader( const string & openmode, bool bswap ) const
{
  if( openmode == "ascii" )
    return( new DefaultAsciiItemReader<SparseMatrix> );
  if( bswap )
    return( new DefaultBSwapItemReader<SparseMatrix> );
  return( new DefaultItemReader<SparseMatrix> );
}


void DefaultItemReader<SparseMatrix>::read( istream & is, SparseMatrix* pitem, 
				      size_t n ) const
{
  SparseMatrix	*ptr = pitem;
  for( size_t i=0; i<n; ++i )
    read( is, *ptr++ );
}


void DefaultItemReader<SparseMatrix>::read( istream & is, SparseMatrix & item ) const
{

  static DefaultItemReader<uint32_t> itemR1;
  static DefaultItemReader<double> itemR2;

  uint32_t size1 = 0U;
  uint32_t size2 = 0U;
  uint32_t nonZeroElementCount = 0U;
  uint32_t k, s1, s2;
  double value;

  itemR1.read( is, size1 );
  itemR1.read( is, size2 );
  itemR1.read( is, nonZeroElementCount );

  streampos p = is.tellg(), e;
  is.seekg( 0, ios_base::end );
  e = is.tellg();
  is.seekg( p, ios_base::beg );
  if( nonZeroElementCount > size1 * size2
      || nonZeroElementCount * 16 != e - p )
    throw wrong_format_error( "Wrong format or corrupted .imas format",
                              "<?>" );

  item.reallocate( size1, size2 );

  for ( k = 0; k < nonZeroElementCount; k++ )
  {

    itemR1.read( is, s1 );
    itemR1.read( is, s2 );
    itemR2.read( is, value );
    item( s1, s2 ) = value;

  }

}


void DefaultAsciiItemReader<SparseMatrix>::read( istream & is, SparseMatrix & item ) const
{
  
  static DefaultAsciiItemReader<uint32_t> itemR1;
  static DefaultAsciiItemReader<double> itemR2;

  uint32_t size1 = 0U;
  uint32_t size2 = 0U;
  uint32_t nonZeroElementCount = 0U;
  uint32_t k, s1, s2;
  double value;

  itemR1.read( is, size1 );
  itemR1.read( is, size2 );
  itemR1.read( is, nonZeroElementCount );

  item.reallocate( size1, size2 );

  for ( k = 0; k < nonZeroElementCount; k++ )
  {

    itemR1.read( is, s1 );
    itemR1.read( is, s2 );
    itemR2.read( is, value );
    item( s1, s2 ) = value;

  }

}


void DefaultBSwapItemReader<SparseMatrix>::read( istream & is, SparseMatrix & item ) const
{
  
  static DefaultBSwapItemReader<uint32_t> itemR1;
  static DefaultBSwapItemReader<double> itemR2;

  uint32_t size1 = 0U;
  uint32_t size2 = 0U;
  uint32_t nonZeroElementCount = 0U;
  uint32_t k, s1, s2;
  double value;

  itemR1.read( is, size1 );
  itemR1.read( is, size2 );
  itemR1.read( is, nonZeroElementCount );

  item.reallocate( size1, size2 );

  for ( k = 0; k < nonZeroElementCount; k++ )
  {

    itemR1.read( is, s1 );
    itemR1.read( is, s2 );
    itemR2.read( is, value );
    item( s1, s2 ) = value;

  }

}


