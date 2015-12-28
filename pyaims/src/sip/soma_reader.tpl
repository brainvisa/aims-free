
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

  public:
    Reader_%Template1typecode%();
    Reader_%Template1typecode%( const std::string & filename );
    Reader_%Template1typecode%( rc_ptr_DataSource ds );
    Reader_%Template1typecode%( rc_ptr_DataSourceInfo dsi );
//    Reader_%Template1typecode%( std::istream & stream );
    virtual ~Reader_%Template1typecode%();

    virtual bool read( %Template1PyType% & obj,
        carto::Object header = carto::none(),
        int passbegin = 1, int passend = 4 );
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

  public:
    Writer_%Template1typecode%();

    Writer_%Template1typecode%( rc_ptr_DataSource ds );
    Writer_%Template1typecode%( const std::string& filename );
//    Writer_%Template1typecode%( std::ostream & stream );
    virtual ~Writer_%Template1typecode%();

    virtual bool write( const %Template1PyType% & obj,
                        carto::Object options = carto::none(),
                        int passbegin = 1 , int passend = 4 );

    virtual std::string writtenObjectType() const;

    rc_ptr_DataSource dataSource();
    void attach( rc_ptr_DataSource ds );
    void attach( const std::string & filename );
//    void attach( std::ostream & stream );

    void flush();
    void close();
};

}; // namespace  soma

