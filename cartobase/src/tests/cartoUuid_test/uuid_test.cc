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

#define private public // don't have time to augment the API for testing...
#include <cartobase/uuid/uuid.h>
#include <cartobase/exception/assert.h>


int main()
{

  carto::UUID uuid;
  ASSERT( uuid.isNull() );
  carto::UUID uuid1;
  uuid1.generate();
  ASSERT( !uuid1.isNull() );

#if defined(__linux) || defined(__osf__) || defined(_WIN32)

  ASSERT( ( uuid1.data[ 10 ] & 0x80 ) == 0);

#else

  ASSERT( ( uuid1.data[ 10 ] & 0x80 ) != 0);

#endif

  carto::UUID uuid2;
  uuid2.generate();
  ASSERT( !uuid2.isNull() );

#if defined(__linux) || defined(__osf__) || defined(_WIN32)

  ASSERT( ( uuid2.data[ 10 ] & 0x80 ) == 0);

#else

  ASSERT( ( uuid2.data[ 10 ] & 0x80 ) != 0);

#endif

  ASSERT( !( uuid1 == uuid2 ) );
  std::string input( "1d827ae8-57ca-45bb-871d-c3e080495795" );
  carto::UUID uuid3( input );
  std::string output = uuid3.toString();
  ASSERT( input == output );
  carto::UUID uuid4( uuid );
  ASSERT( uuid4 == uuid );
  uuid1 = uuid;
  ASSERT( uuid1 == uuid );

}
