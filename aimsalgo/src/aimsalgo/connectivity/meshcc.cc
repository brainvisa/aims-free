#include <cstdlib>
#include <aims/connectivity/meshcc_d.h>


using namespace std;


//Instanciation
template
Texture<float> AimsMeshLabelConnectedComponent( const AimsSurface<3,Void> & mesh,
                              const Texture<float> & inittex, float threshold, int mode=1, int verbose=0 );

template
Texture<int> AimsMeshLabelConnectedComponent( const AimsSurface<3,Void> & mesh,
                              const Texture<int> & inittex, int threshold, int mode=1, int verbose=0 );

template
Texture<short> AimsMeshLabelConnectedComponent( const AimsSurface<3,Void> & mesh,
                              const Texture<short> & inittex, short threshold, int mode=1, int verbose=0 );

template
unsigned AimsMeshLabelNbConnectedComponent( const AimsSurface<3,Void> & mesh,
                             const Texture<short> & inittex, const short lab );


template
unsigned AimsMeshLabelNbConnectedComponent( const AimsSurface<3,Void> & mesh,
                             const Texture<int> & inittex, const int lab );

template
unsigned AimsMeshLabelNbConnectedComponent( const AimsSurface<3,Void> & mesh,
                             const Texture<float> & inittex, const float lab );

// Give a connected map of inittex.
// An initial connex area composed of different labels
// is splitted in sub-area of same labels
template
Texture<short> AimsMeshLabelConnectedComponent2Texture( const AimsSurface<3,Void> & mesh,
                                   const Texture<short> & inittex,  short threshold );

template
Texture<int> AimsMeshLabelConnectedComponent2Texture( const AimsSurface<3,Void> & mesh,
                                   const Texture<int> & inittex,  int threshold );
template
Texture<float> AimsMeshLabelConnectedComponent2Texture( const AimsSurface<3,Void> & mesh,
                                   const Texture<float> & inittex,  float threshold );

template
Texture<short> AimsMeshFilterConnectedComponent(
  const AimsSurface<3,Void> & mesh, const Texture<short> & inittex,
  short threshold,
  const short & background, unsigned long ncomp, unsigned long max_npts,
  float max_surf );
template
Texture<int> AimsMeshFilterConnectedComponent(
  const AimsSurface<3,Void> & mesh, const Texture<int> & inittex,
  int threshold,
  const int & background, unsigned long ncomp, unsigned long max_npts,
  float max_surf );
template
Texture<float> AimsMeshFilterConnectedComponent(
  const AimsSurface<3,Void> & mesh, const Texture<float> & inittex,
  float threshold,
  const float & background, unsigned long ncomp, unsigned long max_npts,
  float max_surf );
