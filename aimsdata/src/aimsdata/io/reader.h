/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef AIMS_IO_READER_H
#define AIMS_IO_READER_H

#include <aims/config/aimsdata_config.h>
#include <aims/def/general.h>
#include <cartobase/object/object.h>
#include <string>


namespace aims
{

  /**	Generic reader for *every* format of Aims object. Still a template 
	class, the data type must be switched at upper-level (see Finder or 
	Process).
	Generic readers offer a plug-in mechanism to allow new formats to 
	be registered.
	It uses at lower level specific readers of each format (GisReader 
	for GIS, AimsVidaReader for Vida, AimsSpmReader for SPM, 
	AimsDicomReader for DICOM, AimsEcatReader for Ecat..., and readers for 
	other object types (meshes, buckets, textures, ...))

	\par Adding new readers

	The standard Aims library includes a set of already compiled formats, 
	using a FileFormatDictionary to provide a common link between Reader 
	and Writer, see FileFormatDictionary for a list

	If you need to use other types, you'll have to include 
	\c aims/io/reader_d.h in order to have the template functions 
	definitions, \c aims/io/reader.h is only the class declarations.
	In addition, you may want to specialize the registerBaseFormats() 
	functions of the FileFormatDictionary classes.

	\see FileFormatDictionary Writer Finder Process
  */
  template<class T> class Reader
  {
  public:
    Reader();
    Reader( const std::string& filename );
    virtual ~Reader() {}

    /*!	\brief Finds the correct format and reads the object. 
      if \c format is specified, this format is tried first, so you can use it
      as a hint if you already kow it (from the Finder check ).
      IF \c frame is specified and non-negative, reads only that frame (one 
      temporal positon) of the file into time position zero of the object

       <b>DON'T USE SINGLE FRAME READING WITH MEMORY MAPPING !</b>*/
    virtual bool read( T & obj, int border=0, const std::string* format = 0, 
		       int frame = -1 );
    /*! \brief Creates and reads an object. 
      This function differs from the read( T&, ... ) function in the way 
      that it creates the object and does not just fill it. This enables 
      to create sub-classes of T (factory-like behaviour) on types that 
      allow subclasses. The object is created by \c new and can be deleted. 
      The default implementation just creates a T and calls the 
      read( T&, ... ) function */
    virtual T* read( int border = 0, const std::string* format = 0, 
		     int frame = -1 );
    /// set input file mode - soon obsolete
    void setMode( carto::AllocatorStrategy::MappingMode mode );
    /// allocator control (not used by every format yet)
    void setAllocatorContext( const carto::AllocatorContext & ac );
    const carto::AllocatorContext & allocatorContext() const;
    /// set input file name
    void setFileName( const std::string &fileName );
    /// get input file name
    inline const std::string &fileName() const { return _filename; }
    void setOptions( carto::Object options );
    carto::Object options() const;
    carto::Object & options();

    static std::string extension( const std::string & filename );

  protected:
    std::string					_filename;
    carto::AllocatorStrategy::MappingMode	_mode;
    carto::AllocatorContext			_alloccontext;
    carto::Object				_options;
  };

}


template <class T>
inline aims::Reader<T> &
operator >> ( aims::Reader<T> & reader, T & thing )
{
  reader.read( thing );
  return reader;
}


#endif
