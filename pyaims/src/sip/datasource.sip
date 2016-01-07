
namespace soma
{

  class DataSource : carto::RCObject
  {
%TypeHeaderCode
#include <soma-io/datasource/datasource.h>
%End

%Docstring
    Abstraction layer for various data sources (file, buffer, socket...).

    It can be seen as a stream, and is inspired by the QIODevice of Qt library.

    Abstract class.
%End

  public:
    enum Mode
    {
      Read = 1,
      Write = 2,
      ReadWrite = 3,
    };
%Docstring

    Mode:
      enum (Read, Write, ReadWrite)
      Read / write mode
%End

    enum IterateMode
    {
      DirectAccess = 1,
      SequentialAccess = 2,
    };
%Docstring

    IterateMode:
      enum (DirectAccess, SequentialAccess)
      Direct / sequential access
%End

    virtual ~DataSource();

    int mode() const;
%Docstring
    access mode(s) (read/write): bitwise OR of Mode values
%End

    virtual soma::DataSource* clone() const = 0 /Factory/;
%Docstring
    duplicate the DataSource object
%End

    virtual int iterateMode() const = 0;
%Docstring
    bitwise combination of possible access: direct (1) or sequential (2) access
%End

    virtual bool isOpen() const = 0;
%Docstring
    tells if the stream is open / ok.
%End

    virtual bool eof() const;
%Docstring
    when the end of stream is reached
%End

//    virtual bool operator ! () const;
//    virtual operator bool () const;
    virtual bool open ( int mode ) = 0;
%Docstring
    open(mode)

    Opens the stream (when supported) in the specified Mode

    Parameters
    ----------
    mode: int
        access Mode: Read (1), Write (2) or ReadWrite (3)

    Returns
    -------
    ok: bool
        if the operation was successful
%End

    virtual void close() = 0;
%Docstring
    Close the stream
%End

    virtual void flush();
%Docstring
    flush the stream for buffered output
%End

    virtual soma::offset_t size() const = 0;
%Docstring
    size (or len) of the data inn stream, in bytes
%End

    virtual soma::offset_t at() const = 0;
%Docstring
    offset = at()

    returns the current position in the stream
%End

    virtual bool at( soma::offset_t pos ) = 0;
%Docstring
    bool at(pos)

    Sets the current position in the stream (when supported).
    Returns True if successful.
%End

    virtual bool atEnd() const;
%Docstring
    True if the current position is at the end of the stream
%End

    bool reset();
%Docstring
    reset the stream and get to the start
%End

    virtual long readBlock( char * data, unsigned long maxlen ) = 0;
%Docstring
    readBlock(data, maxlen)

    Parameters
    ----------
    data: char*
        read buffer (C++)
    maxlen: unsigned long int
        number of bytes to read

    Returns
    -------
    num: long int
        number of bytes actually read
%End

    virtual long writeBlock( const char * data, unsigned long len ) = 0;
%Docstring
    writeBlock(data, len)

    Parameters
    ----------
    data: const char * (C++)
        buffer to be written
    len: unsigned long int
        size of the buffer to be written

    Returns
    -------
    num: long int
        number of bytes actually written
%End

    virtual int getch() = 0;
%Docstring
    read one char and returns it
%End

    virtual int putch( int ch ) = 0;
%Docstring
    putch(ch)

    write one char, and returns it

    Parameters
    ----------
    ch: int
        char to write
%End

    virtual bool ungetch( int ch ) = 0;
%Docstring
    ungetch(ch)

    un-read one char: put it back in the read buffer, and rewind one char
%End

    virtual bool allowsMemoryMapping() const = 0;
%Docstring
    returns True if memory mapping is allowed on this kind of stream
%End

    virtual std::string url() const;
%Docstring
    URL of filename for the stream, when available
%End

    static const rc_ptr_DataSource none();
%Docstring
   An empty ref-counter that is more convenient than calling a
   constructor of rc_ptr_DataSource (useful when calling functions)
%End

protected:
    DataSource();
  };


  class FDDataSource : soma::DataSource
  {
%TypeHeaderCode
#include <soma-io/datasource/fddatasource.h>
%End

%Docstring
    DataSource specialization for file descriptor stream
%End

  public:
    FDDataSource( int fd = -1, int mode = Read );
    virtual ~FDDataSource();

    virtual soma::DataSource* clone() const /Factory/;
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
%Docstring
    get the file descriptor
%End

    void setDescriptor( int fd );
%Docstring
    setDescriptor(fd)

    Set the file descriptor fd (int)
%End

    bool isFile() const;
%Docstring
    True if the DataSource is a file
%End
  };


  class FileDataSource : soma::DataSource
  {
%TypeHeaderCode
#include <soma-io/datasource/filedatasource.h>
%End

%Docstring
    DataSource specialization for a file.

%End

  public:
    FileDataSource( const std::string & filename, soma::offset_t offset = 0,
                    int mode = soma::DataSource::Read );
%Docstring
    FileDataSource(filename, offset=0, mode=aims.soma.DataSource.Read)

    Parameters
    ----------
    filename: string
        file name on the filesystem
    offset: soma.offset_t (optional)
        offset to start reading in the file
    mode: aims.soma.DataSource.Mode (optional)
        read / write mode
%End

    virtual ~FileDataSource();
    virtual std::string url() const;
%Docstring
    file name or URL of the file
%End

    soma::offset_t initialOffset() const;
%Docstring
    initial offset in the input file to start reading
%End

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
%TypeHeaderCode
#include <soma-io/datasource/datasourcelist.h>
%End

%Docstring
  This class allows to manipulate "lists" of pointers to DataSource.

  It has the design of a dictionary in order to sort sources by
  content (header, minf, data, ...). Since those contents depend on
  the format, the keywords used are defined by specific checkers
  and readers.

  see FormatChecker FormatReader

  The only *global* keyword is "default" which is used to store the
  DataSource defining (at construction) a reader.

  see Reader

  .. note::
    The "default" keyword always contains at least one entry, which
    may be empty. I haven't for now found any use to several "default"
    entries.

  Access to a source is done using dataSource(...) methods.
  Sources are ordered by increasing order of insertion and numbering
  starts at 0.

%End

    public:
      DataSourceList();
%Docstring
  DataSourceList():
    Builds a map containing only ("default", empty pointer)

%End

      DataSourceList( const rc_ptr_DataSource & ds,
                      const std::string & type = "default" );
%Docstring
  DataSourceList(ds, type="default"):
    Builds a 1-element map
    This allows to construct easily a 1 element list, useful when
    declaring a Reader which creator takes a source as parameter.

  Parameters
  ----------
  ds: rc_ptr_DataSource
      Element to insert
  type: string
      Category of the source.
      default key is "default": used at Reader construction
%End
      DataSourceList( const soma::DataSourceList & );
//      DataSourceList & operator = ( const soma::DataSourceList & );
      virtual ~DataSourceList();

      bool operator == ( const soma::DataSourceList & ) const;
      bool operator != ( const soma::DataSourceList & ) const;
      bool  empty() const;
%Docstring
    Returns true only if no keyword inserted.
    .. warning:: May return false while no DataSource present
%End

      set_STRING types() const;
%Docstring
    Returns existing keywords.
    .. warning:: There may be existing keywords with no DataSource
%End

      int   typecount() const;
%Docstring
      number of keys in the types dictionary
%End

      bool  exists( const std::string & ) const;
%Docstring
      exists(key)

      True if the key exists in the dictionary
%End

      bool  empty( const std::string & ) const;
%Docstring
      True if there is no key in the dictionary
%End

      int   size( const std::string & ) const;
%Docstring
      size(key)

      number of elements with the key key
%End

      rc_ptr_DataSource
      dataSource( const std::string & key = "default", int i = 0 );
%Docstring
    datasource(key="default", i=0)

    Accessing an element of the list
    If keyword doesn't exist, or is empty, or coordinate is undefined,
    launches exception.

    Parameters
    ----------
    key: string (optional)
        keyword
    i: int (optional)
        number of the element in list. Numbering starts at 0
%End

      void addDataSource( const std::string &,
                          rc_ptr_DataSource );
%Docstring
    addDataSource(key, ds)

    Adds an element to the dictionary
    If new keyword, creates it.

    Parameters
    ----------
    key: string
        keyword
    ds: rc_ptr_DataSource
        DataSource to be added
%End

      void reset();
%Docstring
      clear the dictionary: all keys and contents are removed.
%End
  };


    class DataSourceInfo
  {
%TypeHeaderCode
#include <soma-io/datasourceinfo/datasourceinfo.h>
%End

%Docstring
  Informative object used by IO system

  This object is used by FormatChecker, FormatReader or FormatWriter to
  describe a DataSource. It contains a DataSourceList which contains
  at first a single default DataSource, a Object header and a
  DataSourceCapabilities.

  * The list is built by a FormatChecker and contains all the files
    involved in the reading/writing process (header, data, ...)
  * The header is built by a FormatChecker and contains meta information.
  * The DSC contains properties dependant of the format,
    the specific file, the reading process (partial reading), etc.

  It is possible to fix some or all of these three objects so that
  they are not recomputed by the FormatChecker.

  see DataSourceInfoLoader DataSourceList DataSourceCapabilities

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
%Docstring
  DataSourceInfo(ds, dim=[]):
    Build a DataSourceInfo from a DataSource, and dimensions

    DSList is set as containing only ds pointed by "default" key.
    Capabilities are set uninitialized.
    If dim is given, a header is built with keys size[X,Y,Z,T].
    Else the header is none().

%End

      DataSourceInfo( const rc_ptr_DataSource & ds,
                      carto::Object header );
%Docstring
  DataSourceInfo(ds, header):
    Build a DataSourceInfo from a DataSource and an existing header

%End

      DataSourceInfo( const std::string & fname );
%Docstring
  DataSourceInfo(fname):
    Builds a DataSourceInfo from a filename

  Parameters
  ----------
  ds: rc_ptr_DataSource
  dim: vector_S32
      dimensions (for a volume), up to 4 dimensions
  header: Object
  fname: string
%End

      DataSourceInfo( const soma::DataSourceInfo & );
      virtual ~DataSourceInfo();

      carto::Object header();
%Docstring
      get the header dict of the underlying data
%End

//      DataSourceCapabilities & capabilities();
//      carto::Object privateIOData();
      soma::DataSourceList & list();
%Docstring
      get the DataSourceList of the underlying data
%End

      std::string url() const;
%Docstring
      main filename or URL
%End

      std::string identifiedFormat() const;
%Docstring
      file format name
%End

      void setIdentifiedFormat( const std::string & format );
%Docstring
      setIdentifiedFormat(format)

      force the identified format name
%End
  };


  class DataSourceInfoLoader
  {
%TypeHeaderCode
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
%End

%TypeCode
#include <soma-io/datasource/filedatasource.h>
%End

%Docstring
    Generic information retreiver / checker for all data sources and file
    formats

    It replaces aims.Finder.

    DataSourceInfoLoader provides a plug-in system for new formats and data
    types. Formats are hidden in the plugins and should never be accessed
    directly.

    Usage: check() the DataSourceInfo and then process according to the
    object type ("Volume of S16", "Mesh", "Texture", "Bucket", ...).

    Use the generic Reader once the data type is known, the right format
    will be selected automatically.

    Avoiding manually switching on objects and data types is possible using
    the ReaderAlgorithm interface and presumably, template functions.

    Here is an example of how to use the DataSourceInfoLoader class:

    ::

        from soma import aims
        from soma.aims import soma

        f = soma.DataSourceInfoLoader()
        info = f.check("toto.nii")
        if not info.header():
            print "could not load", info.url()
        else:
          object_type = info.header()["object_type"]
          if object_type == aims.typeCode(aims.Volume_S16):
              vr = soma.Reader_Volume_S16(info)
              try:
                  vol = vr.read()
                  print "volume read"
              except:
                  print "Error loading", info.url()
          elif object_type == aims.typeCode(aims.Object):
              # do your thing
              print "Object"
          else:
              print info.url(), "is of type", object_type, \
                  "which is not handled"

    see DataSourceInfo, Reader, ReaderAlgorithm
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
                            int passbegin = 1, int passend = 3 )
          throw ( carto::io_error, std::exception );
%Docstring
      check(dsi, options=aims.carto.none(), passbegin=1, passend=3)

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
%Docstring
      state of the DataSourceInfoLoader reading trial
%End

      std::string errorMessage() const;
%Docstring
      error message from the last check
%End

      void launchException()
          throw ( carto::io_error, std::exception );
%Docstring
      raise the read exception which was most probably the cause of a check() failure
%End

      static carto::Object readMinf( soma::DataSource & ds,
                                     carto::Object base = carto::none(),
                                     carto::Object options = carto::none() );
%Docstring
      readMinf(ds, base=aims.none(), options=aims.none())

      read a .minf file dictionary

      Parameters
      ----------
      ds: DataSource
          minf file DataSource
      base: Object
          base dict to be completed by .minf information
      options: Object
          options dict

      Returns
      -------
      minf: Object
          the minf file contents as a dictionary
%End

//      static carto::SyntaxSet & minfSyntax();
      static set_STRING extensions( const std::string & format );
%Docstring
      extensions(format)

      Query supported file extensions for a format

      Parameters
      ----------
      format: string
          format to query

      Returns
      -------
      exts: set_STRING
          set of extensions names
%End
  };

};

