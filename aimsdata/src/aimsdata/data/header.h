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

/*
 *  Header class
 */
#ifndef AIMS_DATA_HEADER_H
#define AIMS_DATA_HEADER_H

#define AIMSDATA_NEW_AIMSDATA

#include <aims/config/aimsdata_config.h>
#include <set>
#include <string>

namespace aims
{

  class AIMSDATA_API Header
  {
  public:
    virtual const char* id() const;
    virtual ~Header()=0;
    virtual Header* cloneHeader( bool keepUuid = false ) const = 0;

    ///	standard file format extension of specialized headers
    virtual std::string extension() const;
    /// possible filename extensions for specialized formats
    virtual std::set<std::string> extensions() const;
    /// extension of given filename (including '.')
    virtual std::string extension( const std::string & filename ) const;
    virtual std::string removeExtension( const std::string & ) const;

    /// read meta-info header
    virtual bool readMinf( const std::string & filename );
    /// write meta-info header, non-const version (may change some attributes)
    virtual bool writeMinf( const std::string & filename );
    /// write meta-info header
    virtual bool writeMinf( const std::string & filename ) const;
  };

}

#endif
