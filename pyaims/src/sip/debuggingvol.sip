namespace pyaims
{

  class DebuggingVolume : Volume_S16
  {
%TypeHeaderCode
#include <pyaims/debugging/debuggingvol.h>
%End

    public:
      DebuggingVolume( int sizeX=1, int sizeY=1, int sizeZ=1, int sizeT=1 );
      virtual ~DebuggingVolume();

      static bool created;
      static bool deleted;
  };

};

