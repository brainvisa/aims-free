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

#ifndef CARTOBASE_STREAM_SSTREAM_H
#define CARTOBASE_STREAM_SSTREAM_H

#include <cartobase/config/cartobase_config.h>

#if defined (__GNUC__) && (__GNUC__-0 == 2) && (__GNUC_MINOR__-0 < 96)
// gcc 2.95.2 doesn't provide <sstream>
// gcc 2.95.3, gcc 2.96 and gcc 3.0 have both
// gcc 3.1 warns about <strstream> being obsolete
// gcc 3.2 refuses <strstream>
// (lots of fun...)

// 1st hack: provide a common <cartobase/stream/sstream> include
// (this is what this header does)
// and a way to easily know what we've done
#include <strstream>
#define CARTO_OLDSTYLE_STRSTREAM

// 2nd hack: replace [io]stringstream by [io]strstream
namespace std
{

  typedef istrstream istringstream;
  typedef ostrstream ostringstream;
  typedef strstream stringstream;

}

// This hack supposes you don't use 'expert features' of strstreams
// since the API of strstream and stringstream is different 
// (constructors are already different) !

#else // gcc >= 2.96 and other compilers

// new standard API
#include <sstream>

#endif

#endif
