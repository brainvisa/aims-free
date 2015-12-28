
namespace soma
{

  class Reader_%Template1typecode%
  {
  %TypeHeaderCode
  #include <soma-io/io/reader.h>
  %Template1typeinclude%
  %Template1sipinclude%
  namespace soma
  {
    typedef soma::Reader<%Template1% > Reader_%Template1typecode%;
  }
  %End

  %Docstring
  Soma-IO Reader class. This is a C++ template class, which can read a specific type of data. All supported formats can be read.

  Constructor parameters may be:
  * string: filename
  * rc_ptr_DataSource: generic DataSource
  * rc_ptr_DataSourceInfo: DataSource with info (more optimal when the data source has been identified and the header loaded)
  %End

  public:
    Reader_%Template1typecode%();
    Reader_%Template1typecode%( const std::string & filename );
    Reader_%Template1typecode%( rc_ptr_DataSource ds );
    Reader_%Template1typecode%( rc_ptr_DataSourceInfo dsi );
//    Reader_%Template1typecode%( std::istream & stream );
    virtual ~Reader_%Template1typecode%();

%If (SIPTHROW_BUG)
    virtual bool read( %Template1PyType% & obj,
        carto::Object header = carto::none(),
        int passbegin = 1, int passend = 4 )
        throw ( ) /Factory, ReleaseGIL/;
%End
%If (!SIPTHROW_BUG)
    virtual bool read( %Template1PyType% & obj,
        carto::Object header = carto::none(),
        int passbegin = 1, int passend = 4 )
    throw ( carto::io_error, std::exception ) /Factory/; //, ReleaseGIL/;
%End
%Docstring
    Read qn object from source.
    Two main modes exist:
    * full: read a full new object and return it
    * in-place: read an existing object, in-place

    A multi-pass procedure is used to identify and read the data from the data source:
    * pass 1: format hint
    * pass 2 : extension
    * pass 3 : empty extension
    * pass 4 : all readers

    Parameters:
    -----------
    obj or header:
        in-place mode: %Template1PyType%, object to be read.
        OR:
        full mode: header (Object), optional
    header: Object (optional)
    passbegin: int (optional)
        begin at given step of the multi-pass identification/read procedure
    passend: int (optional)
        end at given step of the multi-pass identification/read procedure

    Returns:
    --------
    in-place mode:
        True upon success
        False upon failure (but readers will more likely throw an exception)
    full mode:
        the read object
%End

    virtual %Template1PyType%* read( carto::Object header = carto::none(),
        int passbegin = 1, int passend = 4 );

    rc_ptr_DataSourceInfo & dataSourceInfo();

    void setAllocatorContext( const carto::AllocatorContext & ac );
    const carto::AllocatorContext & allocatorContext() const;
    void setOptions( carto::Object options );
    carto::Object options();
    rc_ptr_DataSource dataSource();
    void attach( rc_ptr_DataSource ds );
    void attach( const std::string & filename, soma::offset_t offset = 0 );
//    void attach( std::istream & stream );
    void flush();
    void close();
//    static std::string extension( const std::string & filename );
  };


  class Writer_%Template1typecode%
  {
  %TypeHeaderCode
  #include <soma-io/io/writer.h>
  %Template1typeinclude%
  %Template1sipinclude%
  namespace soma
  {
    typedef soma::Writer<%Template1% > Writer_%Template1typecode%;
  }
  %End

  %Docstring
  Soma-IO Writer class. This is a C++ template class, which can write a specific type of data. All supported formats can be written.

  Constructor parameters may be:
  * string: filename
  * rc_ptr_DataSource: generic DataSource
  %End

  public:
    Writer_%Template1typecode%();

    Writer_%Template1typecode%( rc_ptr_DataSource ds );
    Writer_%Template1typecode%( const std::string& filename );
//    Writer_%Template1typecode%( std::ostream & stream );
    virtual ~Writer_%Template1typecode%();

%If (SIPTHROW_BUG)
    virtual bool write( const %Template1PyType% & obj,
                        carto::Object options = carto::none(),
                        int passbegin = 1 , int passend = 4 )
        throw ( ) /Factory, ReleaseGIL/;
%End
%If (!SIPTHROW_BUG)
    virtual bool write( const %Template1PyType% & obj,
                        carto::Object options = carto::none(),
                        int passbegin = 1 , int passend = 4 )
        throw ( carto::io_error, std::exception ) /Factory/; //, ReleaseGIL/;
%End
%Docstring
    Write the given object to the data source (generally files)

    A multi-pass procedure is used to identify and write the data to the data source:
    * pass 1: format hint
    * pass 2 : extension
    * pass 3 : empty extension
    * pass 4 : all writes

    Parameters:
    -----------
    obj: %Template1PyType%
        object to write
    options: Object (optional)
        Options can be passed to specify some writing parameters. The most
        important is the format ("format" property): if it is specified,
        this format is tried first, so you can use it to force the format,
        otherwise it will be determined from the filename extension (if
        available). If no extension and no format are given, the first
        working format will be used.
    passbegin: int (optional)
    passend: int (optional)

    Returns:
    --------
    True upon success
    False upon failure, but an exception is more likely thrown in such case
%End

    virtual std::string writtenObjectType() const;

    rc_ptr_DataSource dataSource();
    void attach( rc_ptr_DataSource ds );
    void attach( const std::string & filename );
//    void attach( std::ostream & stream );

    void flush();
    void close();
};

}; // namespace  soma

