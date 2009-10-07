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

#ifndef CARTOBASE_ALLOCATOR_MAPPINGRW_H
#define CARTOBASE_ALLOCATOR_MAPPINGRW_H

#include <cartobase/config/cartobase_config.h>
#include <cartobase/allocator/allocator.h>
#include <map>
#ifdef _WIN32
#include <windows.h>
#endif

namespace carto
{

  class MappingRWAllocator : public LowLevelAllocator
  {
  public:
    virtual ~MappingRWAllocator();

    virtual char* allocate( size_t n, size_t sz, DataSource* ds ) const;
    virtual void deallocate( char* ptr, size_t n, size_t sz ) const;

    bool canDuplicate() const { return false; }

    static const MappingRWAllocator & singleton();

    friend std::ostream& operator << ( std::ostream& os, 
				       const MappingRWAllocator & thing );

  private:
    typedef std::pair<std::string, offset_t> _FileId;

    MappingRWAllocator() : LowLevelAllocator()
    { _allocptr() = 0; }

    static MappingRWAllocator *& _allocptr();
    mutable std::map<char*, _FileId>	_mapPtr2Name;
    mutable std::map<std::string, std::map<offset_t, char*> > 
    _mapName2Ptr;
#ifdef _WIN32
    struct WinHandles
    {
      HANDLE	fileH;
      HANDLE	mapH;
    };
    mutable std::map<char*, WinHandles>		_mapDesc;
#else
    mutable std::map<char*, int>		_mapDesc;
#endif
    mutable std::map<char*, int>		_mapRef;
  };

  std::ostream& operator << ( std::ostream& os, 
			      const MappingRWAllocator & thing );

}

#endif
