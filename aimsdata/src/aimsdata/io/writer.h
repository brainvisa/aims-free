/*
 *  Data reader class
 */
#ifndef AIMS_IO_WRITER_H
#define AIMS_IO_WRITER_H

#include <cartobase/object/object.h>


namespace aims
{

  class GenericWriter
  {
  public:
    inline GenericWriter() {};
    inline GenericWriter( const std::string& filename,
                          carto::Object options = carto::none() ) :
      _filename( filename ), _options( options ) {}
    virtual ~GenericWriter() {}

    /*!	\brief Finds the correct format and writes the object

    If \c format is specified, this format is tried first, so you can use it
    to force the format, otherwise it will be determined from the filename
    extension. If no extension and no format are given, the first working
    format will be used. */
    template <typename T>
    bool write( const T & obj, bool ascii = false,
                const std::string* format = 0 );

    /// set output file name
    inline void setFileName( const std::string &fileName )
    {
      _filename = fileName;
    }
    /// get output file name
    inline const std::string &fileName() const { return _filename; }
    const carto::Object options() const { return _options; }
    void setOptions( carto::Object opt ) { _options = opt; }

    virtual std::string writtenObjectType() const = 0;
    virtual std::string writtenObjectDataType() const = 0;
    virtual std::string writtenObjectFullType() const = 0;

  protected:
    std::string _filename;
    carto::Object _options;

  };


  /**	Generic writer for *every* format of Aims object.
	The Writer classes are built on the same model as the Reader classes
	and share the same FileFormatDictionary with them to store all
	known file formats.

	\see FileFormatDictionary Reader
  */
  template<class T> class Writer : public GenericWriter
  {
  public:
    inline Writer() {};
    inline Writer( const std::string& filename,
                   carto::Object options = carto::none() ) :
      GenericWriter( filename, options ) {}
    virtual ~Writer() {}

    /*!	\brief Finds the correct format and writes the object

    If \c format is specified, this format is tried first, so you can use it
    to force the format, otherwise it will be determined from the filename
    extension. If no extension and no format are given, the first working
    format will be used. */
    virtual bool write( const T & obj, bool ascii = false,
                        const std::string* format = 0 );

    virtual std::string writtenObjectType() const;
    virtual std::string writtenObjectDataType() const;
    virtual std::string writtenObjectFullType() const;
  };

  template <class T>
  inline aims::GenericWriter &
  operator << ( aims::GenericWriter & writer, const T & thing )
  {
    writer.write( thing );
    return writer;
  }


  template <class T>
  inline aims::Writer<T> &
  operator << ( aims::Writer<T> & writer, const T & thing )
  {
    writer.write( thing );
    return writer;
  }


  /*! \brief Finds the correct format and writes the object, global version

  If \c format is specified, this format is tried first, so you can use it
  to force the format, otherwise it will be determined from the filename
  extension. If no extension and no format are given, the first working
  format will be used. */
  template <typename T>
  inline bool write( const T & obj, const std::string & filename,
                     carto::Object options = carto::none(),
                     const std::string* format = 0 )
  {
    Writer<T> w( filename );
    w.setOptions( options );
    return w.write( obj, false, format );
  }

}


#endif


