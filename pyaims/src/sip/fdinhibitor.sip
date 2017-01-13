
namespace carto
{

  class fdinhibitor
  {
%TypeHeaderCode
#include <cartobase/stream/fdinhibitor.h>
%End

%Docstring
      Temporarily closes a file descriptor (typically, stdout/stderr).
      Currently, this low-level implementation doesn't work with all C++
      streams: on some systems C++ streams stay in a bad state afterwards and
      are silent forever. Standard IO streams (cin, cout, cerr) are checked
      and protected against this problem.
%End

  public:
    class ResetCallback
    {
    public:
      virtual ~ResetCallback();
      virtual void operator ()( int fd ) = 0;
    };

    fdinhibitor( int fd, bool permanent=false );
%Docstring
    fdinhibitor(fd, permanent=False)

    fd: file descriptor to inhibit,
    permanent: if false, don't reopen the stream when deleting the fdinhibitor

    usage:
    from soma import aims
    fdi = aims.carto.fdinhibitor(sys.stderr.fileno())
    fdi.close()
    # call a function which prints something on stderr which we don't want to
    # appear
    fdi.open()
    sys.stderr.write('after reopen\n') # works again
%End

    ~fdinhibitor();

    void close();
    void open();
    void notify( int fd );

    static void registerResetCallback(
      const std::string & name,
      carto::fdinhibitor::ResetCallback *cbk /Transfer/ );
    static bool hasResetCallback( const std::string & name );
    static void unregisterResetCallback( const std::string & name );

  };

};

