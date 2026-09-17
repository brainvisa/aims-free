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
