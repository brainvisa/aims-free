/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

#include <cstdlib>
#include <aims/connectivity/meshcc_d.h>


using namespace std;


//Instanciation
template
Texture<float> AimsMeshLabelConnectedComponent( const AimsSurface<3,Void> & mesh,
                              const Texture<float> & inittex, float threshold, int mode=1 );

template
Texture<int> AimsMeshLabelConnectedComponent( const AimsSurface<3,Void> & mesh,
                              const Texture<int> & inittex, int threshold, int mode=1 );

template
Texture<short> AimsMeshLabelConnectedComponent( const AimsSurface<3,Void> & mesh,
                              const Texture<short> & inittex, short threshold, int mode=1 );

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
