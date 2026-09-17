#include <pyaims/debugging/debuggingvol.h>

using namespace pyaims;
using namespace carto;
using namespace std;

bool DebuggingVolume::created = false;
bool DebuggingVolume::deleted = false;

DebuggingVolume::DebuggingVolume( int sizeX, int sizeY, int sizeZ, int sizeT )
  : Volume<int16_t>( sizeX, sizeY, sizeZ, sizeT )
{
  cout << "DebuggingVolume created\n" << std::flush;
  created = true;
}


DebuggingVolume::~DebuggingVolume()
{
  cout << "DebuggingVolume deleted\n" << std::flush;
  deleted = true;
}

