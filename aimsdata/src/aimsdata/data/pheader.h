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

#ifndef AIMS_DATA_PHEADER_H
#define AIMS_DATA_PHEADER_H

#include <aims/data/header.h>
#include <cartobase/object/object.h>
#include <cartobase/object/syntax.h>
#include <cartobase/object/property.h>
#include <cartobase/object/headered.h>

namespace aims
{
  namespace internal
  {
    /// wrapper to Headered/PropertySet
    typedef carto::PointerObject<carto::PropertySet> PythonHeaderType;
  }

  class PythonHeader : public internal::PythonHeaderType, public Header
  {
  public:
    PythonHeader();
    PythonHeader( const internal::PythonHeaderType & );
    PythonHeader( const PythonHeader & );
    /// initialization by reference
    PythonHeader( carto::Headered & );
    virtual ~PythonHeader();

    virtual PythonHeader & operator = ( const PythonHeader & );

    virtual const char* id() const;
    virtual bool readMinf( const std::string & filename );
    virtual bool writeMinf( const std::string & filename );
    virtual bool writeMinf( const std::string & filename ) const;
    virtual Header* cloneHeader( bool keepUuid = false ) const;
    virtual void copy( const PythonHeader &, bool keepUuid = false );
    virtual void copy( const carto::Object &, bool keepUuid = false );
    virtual bool isScalar() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isArray() const { return false; }
    virtual bool isDynArray() const { return false; }
    virtual bool isIterator() const { return false; }
    virtual bool isDictionaryIterator() const { return false; }

    static carto::SyntaxSet* syntax();
    static bool writeMinf( const carto::Object & header,
                           const std::string & filename );
    static bool writeMinf( carto::Object & header,
                           const std::string & filename );
    static void copy( const carto::Object & src, carto::Object & dst,
                      bool keepUuid = false );
    static carto::Object cloneHeader( const carto::Object & src,
                                      bool keepUuid = false );
  };

} // namespace aims


namespace carto
{

  template <>
  inline Object Object::value<aims::PythonHeader>()
  {
    return Object( static_cast<GenericObject *>( new aims::PythonHeader ) );
  }

  template <>
  inline Object Object::value( const aims::PythonHeader &v )
  {
    return Object( static_cast<GenericObject *>(
        new aims::PythonHeader( v ) ) );
  }
}

namespace carto {
DECLARE_GENERIC_OBJECT_TYPE( aims::PythonHeader )
}

#endif

