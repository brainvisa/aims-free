
namespace soma
{

  class DataSource : carto::RCObject
  {
%TypeHeaderCode
#include <soma-io/datasource/datasource.h>
%End

  public:
    enum Mode
    {
      Read = 1,
      Write = 2,
      ReadWrite = 3,
    };

    enum IterateMode
    {
      DirectAccess = 1,
      SequentialAccess = 2,
    };

    virtual ~DataSource();

    int mode() const;

    virtual soma::DataSource* clone() const = 0;
    virtual int iterateMode() const = 0;
    virtual bool isOpen() const = 0;
    virtual bool eof() const;
//    virtual bool operator ! () const;
//    virtual operator bool () const;
    virtual bool open ( int mode ) = 0;
    virtual void close() = 0;
    virtual void flush();
    virtual soma::offset_t size() const = 0;
    virtual soma::offset_t at() const = 0;
    virtual bool at( soma::offset_t pos ) = 0;
    virtual bool atEnd() const;
    bool reset();
    virtual long readBlock( char * data, unsigned long maxlen ) = 0;
    virtual long writeBlock( const char * data, unsigned long len ) = 0;
    virtual int getch() = 0;
    virtual int putch( int ch ) = 0;
    virtual bool ungetch( int ch ) = 0;
    virtual bool allowsMemoryMapping() const = 0;
    virtual std::string url() const;

  protected:
    DataSource();
  };


  class FDDataSource : soma::DataSource
  {
  public:
    FDDataSource( int fd = -1, int mode = Read );
    virtual ~FDDataSource();

    virtual soma::DataSource* clone() const;
    virtual int iterateMode() const;
    virtual bool isOpen() const;
    virtual bool open ( int mode );
    virtual void close();
    virtual soma::offset_t size() const;
    virtual soma::offset_t at() const;
    virtual bool at( soma::offset_t pos );
    virtual long readBlock( char* data, unsigned long maxlen );
    virtual long writeBlock( const char* data, unsigned long len );
    virtual int getch();
    virtual int putch( int ch );
    virtual bool ungetch( int ch );
    virtual bool allowsMemoryMapping() const;

    int descriptor() const;     // should it be non-const ?
    void setDescriptor( int fd );
    bool isFile() const;
  };


  class FileDataSource : soma::DataSource
  {
%TypeHeaderCode
#include <soma-io/datasource/filedatasource.h>
%End

  public:
    FileDataSource( const std::string & filename, soma::offset_t offset = 0,
                    int mode = soma::DataSource::Read );
    virtual ~FileDataSource();
    virtual std::string url() const;
    soma::offset_t initialOffset() const;

    virtual soma::DataSource* clone() const;
    virtual bool open( int mode );
    virtual bool allowsMemoryMapping() const;
    virtual soma::offset_t size() const;
    virtual soma::offset_t at() const;
    virtual bool at( soma::offset_t pos );

    virtual int iterateMode() const;
    virtual bool isOpen() const;
    virtual void close();
    virtual long readBlock( char* data, unsigned long maxlen );
    virtual long writeBlock( const char* data, unsigned long len );
    virtual int getch();
    virtual int putch( int ch );
    virtual bool ungetch( int ch );
  };


  class DataSourceList
  {
    public:
      DataSourceList();
      DataSourceList( const rc_ptr_DataSource & ds,
                      const std::string & type = "default" );
      DataSourceList( const soma::DataSourceList & );
//      DataSourceList & operator = ( const soma::DataSourceList & );
      virtual ~DataSourceList();

      bool operator == ( const soma::DataSourceList & ) const;
      bool operator != ( const soma::DataSourceList & ) const;
      bool  empty() const;
      set_STRING types() const;
      int   typecount() const;
      bool  exists( const std::string & ) const;
      bool  empty( const std::string & ) const;
      int   size( const std::string & ) const;

      rc_ptr_DataSource
      dataSource( const std::string & s = "default", int i = 0 );
      void addDataSource( const std::string &,
                          rc_ptr_DataSource );
      void reset();
  };


    class DataSourceInfo
  {
%TypeHeaderCode
#include <soma-io/datasourceinfo/datasourceinfo.h>
%End

    public:
/*
      DataSourceInfo( const carto::Object & header = carto::none(),
                      const soma::DataSourceCapabilities &
                        cap = soma::DataSourceCapabilities(),
                      const soma::DataSourceList &
                        dsl = soma::DataSourceList());
*/
      DataSourceInfo( const rc_ptr_DataSource & ds,
                      const vector_S32 & dim = vector_S32() );
      DataSourceInfo( const rc_ptr_DataSource & ds,
                      carto::Object header );
      DataSourceInfo( const std::string & fname );
      DataSourceInfo( const soma::DataSourceInfo & );
      virtual ~DataSourceInfo();

      carto::Object header();
//      DataSourceCapabilities & capabilities();
//      carto::Object privateIOData();
      soma::DataSourceList & list();
      std::string url() const;
      std::string identifiedFormat() const;
      void setIdentifiedFormat( const std::string & format );
  };


  class DataSourceInfoLoader
  {
%TypeHeaderCode
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
%End

%TypeCode
#include <soma-io/datasource/filedatasource.h>
%End

    public:
      enum State { Unchecked, Ok, Error };
      DataSourceInfoLoader();
      ~DataSourceInfoLoader();

/*      static void registerFormat( const std::string & fmtid,
                                  FormatChecker* format,
                                  const vector_STRING &
                                    extensions );
      static FormatChecker* formatInfo( const std::string & format );
*/

      soma::DataSourceInfo check( soma::DataSourceInfo dsi,
                            carto::Object options = carto::none(),
                            int passbegin = 1, int passend = 3 );
%Docstring
      Finds the right format checker

      It is possible to specify wich passes to process through
      passbegin and passend.
      * pass 1: extension
      * pass 2: empty extension
      * pass 3: all writers
      see DataSourceInfo DataSourceList DataSourceCapabilities

      Parameters
      ----------
      dsi:     DataSourceInfo containing header, DSlist and .
               capabilities. It allows us to have none, some or all
               information already computed.  It is for the
               DSIloader to deal with the *all* case, and for the
               FormatCheckers to deal with *some* and *none* cases.
      options:  A dictionary containing options. They may not be of
                any use to the checker, but soma are
                (resolution_level).

      Returns
      -------
      A DataSourceInfo object containing a header, a list of
      DataSource and a list of Capabilities.
%End

    soma::DataSourceInfo check( const std::string & filename,
                            carto::Object options = carto::none(),
                            int passbegin = 1, int passend = 3 );
%MethodCode
    carto::rc_ptr<soma::DataSource> ds( new soma::FileDataSource( *a0 ) );
    soma::DataSourceInfo dsi( ds );
    sipRes = new soma::DataSourceInfo( sipCpp->check( dsi, *a1, a2, a3 ) );
%End

      State state() const;
      std::string errorMessage() const;
      void launchException();

      static carto::Object readMinf( soma::DataSource & ds,
                                     carto::Object base = carto::none(),
                                     carto::Object options = carto::none() );
//      static carto::SyntaxSet & minfSyntax();
      static set_STRING extensions( const std::string & format );
  };

};

