#include <unistd.h>
#include <fcntl.h>
#include <cartobase/stream/fdinhibitor.h>
#ifdef _WIN32
#include <cartobase/stream/fileutil.h>
#endif
#include <iostream>

using namespace carto;


void	fdinhibitor::close(void)
{
	_fdsave = dup(_fd);

#ifdef _WIN32
	_tmpfile = FileUtil::temporaryFile("fdinhibitor", _fdblocker);
#else
	_fdblocker = ::open( "/dev/null", O_APPEND );
#endif
	dup2(_fdblocker, _fd);
}

void	fdinhibitor::open(void)
{
	::close(_fdblocker);
	dup2(_fdsave, _fd);
	::close(_fdsave);
#ifdef _WIN32
	unlink(_tmpfile.c_str());
#endif
        if( _fd == STDOUT_FILENO )
          std::cout.clear( std::ios_base::goodbit );
        else if( _fd == STDERR_FILENO )
          std::cerr.clear( std::ios_base::goodbit );
        else if( _fd == STDIN_FILENO )
          std::cin.clear( std::ios_base::goodbit );
}
