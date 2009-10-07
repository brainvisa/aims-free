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

#ifndef CARTOBASE_IO_FORMATDICTIONARY_H
#define CARTOBASE_IO_FORMATDICTIONARY_H

#include <string>
#include <map>
#include <set>
#include <vector>

namespace carto
{
  template <typename T> class FormatReader;
  template <typename T> class FormatWriter;

  /** \brief Formats holder for each format of Cartograph object, used by 
      Reader and Writer

      FormatDictionary is a plug-in mechanism to allow new formats to 
      be registered.\n
      It uses at lower level specific IO classes for each format (GisFormat 
      for GIS, SpmReader for SPM, etc., and IO classes for 
      other object types (meshes, buckets, textures, ...))

      \par Adding new IO formats

      The standard carto/aims libraries include a set of already compiled 
      Readers and writers.

      If you need to use other types, you'll have to include 
      \c cartobase/io/reader_d.h and / or \c cartobase/io/writer_d.h in order 
      to have the template functions definitions, \c cartobase/io/reader.h and 
      \c cartobase/io/writer.h are only the class declarations.
      In addition, you may want to specialize the registerBaseFormats() 
      functions of specific FormatDictionary specializations.

      \see Reader Writer DataSourceInfo Process
  */
  template<typename T>
  class FormatDictionary
  {
  public:
    static void registerFormat( const std::string & formatID, 
				FormatReader<T>* format, 
				const std::vector<std::string> & extensions );
    static void registerFormat( const std::string & formatID, 
				FormatWriter<T>* format, 
				const std::vector<std::string> & extensions );
    static FormatReader<T>* readFormat( const std::string & format );
    static FormatWriter<T>* writeFormat( const std::string & format );
    static const std::multimap<std::string, std::string> & readExtensions();
    static const std::multimap<std::string, std::string> & writeExtensions();
    static std::set<std::string> readFormats();
    static std::set<std::string> writeFormats();
    /** builds base formats maps. 
        The formats list is dynamically extensible, you can register your own 
	whenever you want
    */
    static void registerBaseFormats();
    static void init();

  private:
    static std::map<std::string, FormatReader<T>*>	& _readformats();
    static std::map<std::string, FormatWriter<T>*>	& _writeformats();
    static std::multimap<std::string, std::string>	& _readextensions();
    static std::multimap<std::string, std::string>	& _writeextensions();
  };


  /// Global dictionary of supported object types for IO
  class IOObjectTypesDictionary
  {
  public:
    typedef std::set<std::string> (*FormatInfo)();
    static void registerReadType( const std::string & datatype, 
                                  FormatInfo info );
    static void registerWriteType( const std::string & datatype, 
                                   FormatInfo info );
    static bool hasReadType( const std::string & datatype );
    static bool hasWriteType( const std::string & datatype );
    static std::map<std::string, FormatInfo> & readTypes();
    static std::map<std::string, FormatInfo> & writeTypes();
  };

}

#endif

