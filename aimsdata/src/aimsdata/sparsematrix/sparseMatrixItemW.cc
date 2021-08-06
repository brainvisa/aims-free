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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/sparsematrix/sparseMatrixItemW.h>
#include <aims/io/triederItemW.h>

using namespace aims;
using namespace std;


ItemWriter<SparseMatrix>* 
DefaultItemWriter<SparseMatrix>::writer( const string & openmode, bool bswap ) const
{
  if( openmode == "ascii" )
    return( new DefaultAsciiItemWriter<SparseMatrix> );
  if( bswap )
    return( new DefaultBSwapItemWriter<SparseMatrix> );
  return( new DefaultItemWriter<SparseMatrix> );
}


void DefaultItemWriter<SparseMatrix>::write( ostream & is, const SparseMatrix* pitem, 
				       size_t n ) const
{
  const SparseMatrix	*ptr = pitem;
  for( size_t i=0; i<n; ++i )
    write( is, *ptr++ );
}


void DefaultItemWriter<SparseMatrix>::write( ostream & os, 
				       const SparseMatrix & item ) const
{
  
  static DefaultItemWriter<uint32_t> itemW1;
  static DefaultItemWriter<double> itemW2;

  SparseMatrix::const_iterator1 s1;
  SparseMatrix::const_iterator2 s2;
    
  itemW1.write( os, item.getSize1() );
  itemW1.write( os, item.getSize2() );
  itemW1.write( os, item.getNonZeroElementCount() );
  
  for ( s1 = item.begin1(); s1 != item.end1(); s1++ )
  {

    for ( s2 = s1.begin(); s2 != s1.end(); s2++ )
    {

      itemW1.write( os, s1.index1() );
      itemW1.write( os, s2.index2() );
      itemW2.write( os, *s2 );

    }

  }

}


void DefaultAsciiItemWriter<SparseMatrix>::write( ostream & os, 
					    const SparseMatrix & item ) const
{
  
  static DefaultAsciiItemWriter<uint32_t> itemW1;
  static DefaultAsciiItemWriter<double> itemW2;

  SparseMatrix::const_iterator1 s1;
  SparseMatrix::const_iterator2 s2;
    
  itemW1.write( os, item.getSize1() );
  itemW1.write( os, item.getSize2() );
  itemW1.write( os, item.getNonZeroElementCount() );
  
  for ( s1 = item.begin1(); s1 != item.end1(); s1++ )
  {

    for ( s2 = s1.begin(); s2 != s1.end(); s2++ )
    {

      itemW1.write( os, s1.index1() );
      itemW1.write( os, s2.index2() );
      itemW2.write( os, *s2 );

    }

  }

}


void DefaultBSwapItemWriter<SparseMatrix>::write( ostream & os, 
					    const SparseMatrix & item ) const
{
  
  static DefaultBSwapItemWriter<uint32_t> itemW1;
  static DefaultBSwapItemWriter<double> itemW2;

  SparseMatrix::const_iterator1 s1;
  SparseMatrix::const_iterator2 s2;
    
  itemW1.write( os, item.getSize1() );
  itemW1.write( os, item.getSize2() );
  itemW1.write( os, item.getNonZeroElementCount() );
  
  for ( s1 = item.begin1(); s1 != item.end1(); s1++ )
  {

    for ( s2 = s1.begin(); s2 != s1.end(); s2++ )
    {

      itemW1.write( os, s1.index1() );
      itemW1.write( os, s2.index2() );
      itemW2.write( os, *s2 );

    }

  }

}

