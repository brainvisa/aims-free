

  class FileFormat_%Template1typecode% /VirtualErrorHandler=io_err_handler/
  {
%TypeHeaderCode
#include <aims/io/fileFormat.h>
%Template1typeinclude%
%Template1sipinclude%
typedef aims::FileFormat<%Template1% > FileFormat_%Template1typecode%;
%End

  public:
    virtual ~FileFormat_%Template1typecode%();
    virtual bool read( const std::string & filename, %Template1PyType% & obj,
                       const carto::AllocatorContext & context,
                       carto::Object options ) = 0;
    virtual %Template1PyType%* read( const std::string & filename,
                     const carto::AllocatorContext & context,
                     carto::Object options ) /Factory/;
    virtual bool write( const std::string &, const %Template1PyType% &,
      carto::Object options = carto::none() );

  };


  class FileFormatDictionary_%Template1typecode%
  {
%TypeHeaderCode
#include <aims/io/fileFormat.h>
%Template1typeinclude%
%Template1sipinclude%
typedef aims::FileFormatDictionary<%Template1% > FileFormatDictionary_%Template1typecode%;
%End

  public:
    static void registerFormat( const std::string & formatID,
                FileFormat_%Template1typecode%* format /Transfer/,
                const vector_STRING & extensions,
                const std::string & before = "" );
    static void unregisterFormat( const std::string & formatID );
    static FileFormat_%Template1typecode%* fileFormat(
      const std::string & format );
    // static const map_STRING_list_STRING & extensions();
    static set_STRING formats();
    static void registerBaseFormats();
    static std::string fileExtension( const std::string & filename );
    static void init();

  };

