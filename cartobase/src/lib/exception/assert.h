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

#ifndef CARTOBASE_EXCEPTION_ASSERT_H
#define CARTOBASE_EXCEPTION_ASSERT_H

#include <cartobase/config/cartobase_config.h>
#include <stdexcept>   // logic_error
#include <string>


namespace carto {


class assert_error : public std::logic_error
{
  public:
    assert_error(const char* assertion, const char* file, int line);
    virtual ~assert_error() throw();

    const char* file() const;
    int line() const;

  private:
    std::string _file;
    int    _line;
};




#undef ASSERT

#ifdef CARTO_DEBUG

// Function called when assert failed (it allows breakpoint setting)
void assert_failed( const char *, const char *, int );
inline void test_assert( bool x, const char * ex, const char *file, int line )
{
  if( !x ) carto::assert_failed( ex, file, line );
}
#else

inline void test_assert( bool x, const char * ex, const char *file, int line )
{
  if( !x ) throw carto::assert_error( ex, file, line );
}
#endif

#define ASSERT( EX ) carto::test_assert( EX, #EX, __FILE__, __LINE__ )


} // namespace aims
#endif

