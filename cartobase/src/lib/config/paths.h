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

#ifndef CARTOBASE_CONFIG_PATHS_H
#define CARTOBASE_CONFIG_PATHS_H

#include <string>
#include <list>

namespace carto
{

  /** Paths to configuration / shared data directories

  The functions in Paths provide a portable access to the paths of
  useful directories that may depend on the site or user configuration, or on
  environment variables (such as \c BRAINVISA_SHARE or \c HOME ).

  Path is implemented as a namespace rather than a class with static methods
  so that it can easily be extended in upper-level libraries.
  */
  namespace Paths
  {
    /// User home directory
    const std::string& home();
    /// Temporary directory used for general usage
    const std::string& tempDir();
    /** Temporary directory used for memory mapping with copy - this directory
        has better be large
    */
    const std::string& memmap();
    /// The base shared directory for all cartograph / SHFJ data
    const std::string& globalShared();
    /** Shared directory for SHFJ-specific data. Depending on the
        installation and configuration, this may be the same as the
        globalShared() directory.
    */
    const std::string& shfjShared();
    /** Shared resource files search path.
        It typically includes:
        - home() + "/.brainvisa";
        - home();
        - globalShared()
    */
    const std::list<std::string> & resourceSearchPath();
    /** Find a file in the shared resources directories, or in a personal
        directory.
        Files are looked in the search paths given by resourceSearchPath().
        For each search path, the filename is looked in a subdirectory
        containing the project name and version (if not specified, cartobase
        version is used), and then in a subdirectory containing the project
        name without version. For personal directories, a dot may be prepended
        to the project name for use as a hidden directory (classical on Unix),
        like "/home/user/.anatomist/..."
        The function stops at the first match and return the first path.
    */
    std::string findResourceFile( const std::string & filename,
                                  const std::string & project = "",
                                  const std::string & version = "" );
    /** Same as findResourceFile() except that it looks for every occurrence
        of the searched file (or directory).
    */
    std::list<std::string> findResourceFiles( const std::string & filename,
                                              const std::string & project = "",
                                              const std::string & version = ""
                                            );
    /** argv[0], ie executable name and path; only available when a
        CartoApplication has been built */
    const std::string &argv0();
    void setArgv0( const std::string & argv0 );
  }

}


#endif


