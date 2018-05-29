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

#ifndef AIMS_MESH_CUTMESH_H
#define AIMS_MESH_CUTMESH_H

#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <vector>

namespace aims
{

  template <typename T>
  class CutMesh
  {
  public:
    CutMesh( const AimsSurfaceTriangle & insurf, const Point4df & plane );
    CutMesh( const std::vector<const AimsSurfaceTriangle *> & insurf,
             const Point4df & plane );
    CutMesh( const std::vector<const AimsSurfaceTriangle *> & insurf,
             const std::vector<std::vector<const TimeTexture<T> *> >
             & intex,
             const Point4df & plane );
    ~CutMesh();

    void setTextures(
      const std::vector<std::vector<carto::rc_ptr<TimeTexture<T> > > >
      & intex );

    /// Perform calculations
    void cut( bool buildborderline = true, bool meshplane = false,
              bool checkplane = true );
    /// build border line mesh only
    void cutBorder( int timestep = 0 );

    /// get output
    std::vector<carto::rc_ptr<AimsSurfaceTriangle> > cutMeshes() const
    { return _cut; }
    /// get output
    std::vector<std::vector<carto::rc_ptr<TimeTexture<T> > > >
      cutTextures() const
    { return _cuttex; }
    /// get output
    carto::rc_ptr<AimsTimeSurface<2, Void> > borderLine() const
    { return _borderline; }
    /// get output
    carto::rc_ptr<AimsSurfaceTriangle> planeMesh() const
    { return _planemesh; }

  private:
    std::vector<const AimsSurfaceTriangle *> _insurf;
    std::vector<carto::rc_ptr<AimsSurfaceTriangle> > _insurfrc;
    std::vector<std::vector<const TimeTexture<T> *> > _intex;
    std::vector<std::vector<carto::rc_ptr<TimeTexture<T> > > > _intexrc;
    Point4df _plane;

    std::vector<carto::rc_ptr<AimsSurfaceTriangle> > _cut;
    std::vector<std::vector<carto::rc_ptr<TimeTexture<T> > > > _cuttex;
    carto::rc_ptr<AimsTimeSurface<2, Void> > _borderline;
    carto::rc_ptr<AimsSurfaceTriangle> _planemesh;
  };

}

#endif
#include <aims/mesh/surface.h>
