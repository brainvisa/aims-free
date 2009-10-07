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

#ifndef CARTOBASE_FILEDATASOURCE_DATASOURCE_H
#define CARTOBASE_FILEDATASOURCE_DATASOURCE_H

#ifndef _WIN32
#define CARTO_FILEDS_AS_FD
#endif

#ifdef CARTO_FILEDS_AS_FD
#include <cartobase/datasource/fddatasource.h>
#else
#include <cartobase/datasource/cstreamdatasource.h>
#endif

namespace carto
{

  class FileDataSource : 
#ifdef CARTO_FILEDS_AS_FD
    public FDDataSource
#else
    public CStreamDataSource
#endif
  {
  public:
    FileDataSource( const std::string & filename, offset_t offset = 0, 
                    int mode = Read );
    virtual ~FileDataSource();
    virtual std::string url() const;
    offset_t initialOffset() const;

    virtual DataSource* clone() const;
    virtual bool open( int mode );
    virtual bool allowsMemoryMapping() const;
    virtual offset_t size() const;
    virtual offset_t at() const;
    virtual bool at( offset_t pos );

  private:
    std::string _url;
    offset_t    _offset;
  };

}

#endif

