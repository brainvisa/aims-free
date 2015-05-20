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
 *  Data reader class
 */
#ifndef AIMS_IO_FORMAT_H
#define AIMS_IO_FORMAT_H

#include <aims/config/aimsdata_config.h>
#include <aims/def/general.h>
#include <aims/io/datatypecode.h>
#include <string>
#include <map>
#include <vector>
#include <set>


namespace carto
{
  class Mutex;
}

namespace aims
{
  class Finder;

  /**	\class FileFormat fileFormat.h aims/io/fileFormat.h
	Low-level object IO format: each specific format has such a 
	reader / writer */
  template<class T> class FileFormat
  {
  public:
    virtual ~FileFormat();
    virtual bool read( const std::string & filename, T & obj, 
		       const carto::AllocatorContext & context, 
                       carto::Object options ) = 0;
    /** Creates an object and reads it. The default implementation just 
	calls the other read() function. The returned object may be 
	deleted by the standard \c delete */
    virtual T* read( const std::string & filename, 
                     const carto::AllocatorContext & context, 
                     carto::Object options );
    /** the base class has a default implementation which only returns \c false
	so if your new format only supports read operations, you won't have 
	to bother about the write() function */
    virtual bool write( const std::string &, const T &,
      carto::Object options = carto::none() );
  };


  /*!	\class FinderFormat fileFormat.h aims/io/fileFormat.h
	\brief Finder format checker

	Checks a file for a particular format, and if compatible, fills the 
	Finder state variables and header
  */
  class FinderFormat
  {
  public:
    virtual ~FinderFormat();
    virtual bool check( const std::string & filename, Finder & f ) const = 0;
  };


  /**	\class FileFormatDictionary fileFormat.h aims/io/fileFormat.h
	\brief Formats holder for each format of Aims object, used by Reader 
	and Writer

	FileFormatDictionary is a plug-in mechanism to allow new formats to 
	be registered.
	It uses at lower level specific IO classes for each format (JpegFormat
	for JPEG, which in turn use JpegReader and JpegWriter, AimsVidaReader
        for Vida, AimsSpmReader for SPM, etc., and IO classes for
	other object types (meshes, buckets, textures, ...))

	\par Adding new IO formats

	The standard Aims library includes a set of already compiled Readers 
	and writers, 
	for the following types:
	- AimsData<int8_t>
	- AimsData<uint8_t>
	- AimsData<int16_t>
	- AimsData<uint16_t>
	- AimsData<int32_t>
	- AimsData<uint32_t>
	- AimsData<float>
	- AimsData<double>
	- AimsData<cfloat>
	- AimsData<cdouble>
	- AimsData<AimsRGB>
	- AimsData<AimsRGBA>
	- AimsData<Point3df>
	- AimsSurfaceTriangle
	- Texture1d
	- Texture2d
	- AimsBucket<Void>

	If you need to use other types, you'll have to include 
	\c aims/io/reader_d.h and / or \c aims/io/writer_d.h in order to have 
	the template functions definitions, \c aims/io/reader.h and 
	\c aims/io/writer.h are only the class declarations.
	In addition, you may want to specialize the registerBaseFormats() 
	functions of specific FileFormatDictionary specializations.

	\see Reader Writer Finder Process
  */
  template<class T> class FileFormatDictionary
  {
  public:
    static void registerFormat( const std::string & formatID, 
				FileFormat<T>* format, 
				const std::vector<std::string> & extensions,
                                const std::string & before = "" );
    static FileFormat<T>* fileFormat( const std::string & format );
    static const std::map<std::string, std::list<std::string> > & extensions();
    static std::set<std::string> formats();
    /** builds base formats maps. Currently:
	- Volume formats:
	  - SPM
	  - VIDA
	  - DICOM
	  - ECAT
	- Mesh formats:
	  - MESH
	  - TRI
	- Texture formats:
	  - TEX
	- Bucket formats:
	  - BCK
        The formats list is dynamically extensible, you can register your own 
	whenever you want to
    */
    static void registerBaseFormats();
    static std::string fileExtension( const std::string & filename );
    static void init();

  private:
    static std::map<std::string, FileFormat<T>*>	& _formats();
    static std::map<std::string, std::list<std::string> > &
        _extensions();
    static carto::Mutex & mutex();
  };


  class IOObjectTypesDictionary
  {
  public:
    typedef std::set<std::string> (*FormatInfo)();
    static void registerType( const std::string & objtype, 
			      const std::string & datatype, FormatInfo info );
    static bool hasType( const std::string & objtype, 
			 const std::string & datatype );
    static std::map<std::string, std::map<std::string, FormatInfo> > & types();
  };

}

#endif
