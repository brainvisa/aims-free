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

#include <cartobase/smart/scopedptr.h>
#include <iostream>
#include <cartobase/exception/assert.h>


int main() 
{

  std::cout << "testing scoped_ptr default constructor..." << std::endl;
  {
    carto::scoped_ptr<int> x1, x2;
    ASSERT( !x1 );
    ASSERT( !x2 );
    ASSERT( x1 == 0 );
    ASSERT( x2 == 0 );
    ASSERT( x1 == x2 );
  }

  std::cout << "testing scoped_ptr constructor from pointer..." << std::endl;
  {
    carto::scoped_ptr<int> x1( new int );
    carto::scoped_ptr<int> x2( new int );
    ASSERT( x1 );
    ASSERT( x2 );
    ASSERT( x1 != 0 );
    ASSERT( x2 != 0 );
    ASSERT( x1 != x2 );
  }

  std::cout << "testing scoped_ptr constructor from auto_ptr..." << std::endl;
  {
    std::auto_ptr<int> x1( new int );
    ASSERT( x1.get() != 0 );
    {
      carto::scoped_ptr<int> x2( x1 );
      ASSERT( x1.get() == 0 );
      ASSERT( x2 != 0 );
    }
  }

  std::cout << "testing scoped_ptr::reset()..." << std::endl;
  {
    carto::scoped_ptr<int> x1;
    ASSERT( x1 == 0 );
    x1.reset( new int );
    ASSERT( x1 != 0 );
    x1.reset();
    ASSERT( x1 == 0 );
  }
}
