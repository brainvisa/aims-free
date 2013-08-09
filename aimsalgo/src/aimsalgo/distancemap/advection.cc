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


#include <algorithm>
#include <string>
#include <sstream>
#include <cassert>
#include <cmath>
#include <set>

#include <cartodata/volume/volume.h>
#include <aims/distancemap/advection.h>
#include <aims/connectivity/component.h>
#include <aims/bucket/bucket.h>
#include <aims/io/writer.h>

using carto::Volume;
using carto::VolumeRef;
using aims::BucketMap;

namespace {
  // TODO reset to zero (false)
  const int debug_output = 1;
}

aims::GradientAdvection::GradientAdvection()
  : _verbose(debug_output)
{
}

aims::GradientAdvection::~GradientAdvection()
{
}

void aims::GradientAdvection::setVerbose(bool verbose)
{
  _verbose = verbose;
}

void aims::GradientAdvection::doit(const VolumeRef<int16_t> &vol,
                             const VolumeRef<float> &gradX,
                             const VolumeRef<float> &gradY,
                             const VolumeRef<float> &gradZ,
                             int seed)
{
  std::set<int16_t> seeds;
  seeds.insert(seed);
  doit(vol, gradX, gradY, gradZ, seeds);
}


void aims::GradientAdvection::doit(const VolumeRef<int16_t> &vol,
                             const VolumeRef<float> &gradX,
                             const VolumeRef<float> &gradY,
                             const VolumeRef<float> &gradZ,
                             const std::set<int16_t> &seeds)
{
  // Wouldn't a smaller neighbourhood suffice / be faster?
  const Connectivity cd(0, 0, Connectivity::CONNECTIVITY_26_XYZ);
  const int cdn = cd.nbNeighbors();

  Volume<int16_t> img(vol);
  Volume<float> data1(img.getSizeX(), img.getSizeY(), img.getSizeZ());
  data1.header() = vol->header();
  Volume<float> data2(img.getSizeX(), img.getSizeY(), img.getSizeZ());
  data2.header() = vol->header();
  Volume<float> *src, *dst;
  src = &data1;
  dst = &data2;

  int i = 1;
  for (std::set<int16_t>::const_iterator is = seeds.begin(), es = seeds.end() ;
       is != es ;
       ++is, ++i)
  {
    const int16_t seed_value = *is;
    if(_verbose) {
      std::clog << "aims::GradientAdvection processing seed "
                << i << " / " << seeds.size()
                << " (seed value = " << seed_value << "):" << std::endl;
    }
    BucketMap<Void> dots1, dots2;
    BucketMap<Void> *current_front = &dots1, *next_front = &dots2;
    src->fill(0);
    dst->fill(0);
    std::size_t region_size = 0;

    // Build src image, where each voxel is either -1 (forbidden region), 0
    // (propagation region) or 1 (seed).
    for(int z = 0; z < img.getSizeZ(); ++z)
    for(int y = 0; y < img.getSizeY(); ++y)
    for(int x = 0; x < img.getSizeX(); ++x)
    {
      if((x == 0) || (y == 0) || (z == 0) ||
         (x == img.getSizeX() - 1) ||
         (y == img.getSizeY() - 1) ||
         (z == img.getSizeZ() - 1))
      {
        (*src)(x, y, z) = -1;
        continue;
      }
      if(img(x, y, z) == 0) continue;
      Point3d p(x, y, z);
      if(img(x, y, z) == seed_value)
        (*src)(x, y, z) = 1;
      else {
        (*src)(x, y, z) = -1;
        // Why are the neighbours also forbidden?
        for (int cdi = 0; cdi < cdn; ++cdi)
        {
          Point3d p2 = p + cd.xyzOffset(cdi);
          if((p2[0] < 0) || (p2[0] >= img.getSizeX())) continue;
          if((p2[1] < 0) || (p2[1] >= img.getSizeY())) continue;
          if((p2[2] < 0) || (p2[2] >= img.getSizeZ())) continue;
          (*src)(p2[0], p2[1], p2[2]) = -1;
        }
        continue;
      }
      region_size++;
      std::pair<Point3d, Void> pair;
      pair.first = p;
      (*current_front)[0].insert(pair);
      for (int cdi = 0; cdi < cdn; ++cdi)
      {
        std::pair<Point3d, Void> pair;
        pair.first = p + cd.xyzOffset(cdi);
        (*current_front)[0].insert(pair);
      }
    }
    if(debug_output) {
      Writer<Volume<float> > writer("src.nii");
      writer.write(*src);
    }

    // Propagate the region along the gradient, starting from the seed. A front
    // is used that includes the neighbours of all points added to the region
    // during the previous time step. Each point of the front is added to the
    // region if its local gradient points towards the region.

    // TODO eliminate arbitrary time limit
    for (int t = 0; t < 200; ++t)
    {
      BucketMap<Void>::Bucket &front = (*current_front)[0];
      if(_verbose) {
        std::clog << "  iteration " << t
                  << ", front size = " << front.size() << "..." << std::endl;
      }

      dst->fill(0);
      for(BucketMap<Void>::Bucket::const_iterator ib = front.begin()
            , eb = front.end();
          ib != eb;
          ++ib)
      {
        // For each point of the front, follow the local gradient along a
        // distance of one voxel. If a floating voxel placed there overlaps
        // more than 30% with voxels in the region, add it to the region and
        // propagate the front.
        const float propagation_overlap_threshold = 0.3;

        // TODO work in 3D!
        // TODO can this be made to work with non-cubic voxels?

        const Point3d &p = ib->first;
        const int x = p[0], y = p[1], z = p[2];

        assert(x >= 0 && x < img.getSizeX());
        assert(y >= 0 && y < img.getSizeY());
        assert(z >= 0 && z < img.getSizeZ());
        if((*src)(x, y, z) == -1) continue;

        float gX = gradX(x, y, z);
        float gY = gradY(x, y, z);
        //float gZ = gradZ(x, y, z);
        float gN = std::sqrt(gX * gX + gY * gY/* + gZ * gZ*/);
        // Do not add voxel if gradient is too small, NaN, or Inf.
        if(!std::isnormal(gN)) continue;
        gX /= gN;
        gY /= gN;
        //gZ /= gN;
        float xd = x + gX;
        float yd = y + gY;
        //float zd = z + gZ;
        if(xd < 0.5) xd = 0.5;
        if(yd < 0.5) yd = 0.5;
        //if(zd < 0.5) zd = 0.5;
        if(xd > img.getSizeX() - 1.5) xd = img.getSizeX() - 1.5;
        if(yd > img.getSizeY() - 1.5) yd = img.getSizeY() - 1.5;
        //if(zd > img.getSizeZ() - 1.5) zd = img.getSizeZ() - 1.5;
        int id = static_cast<int>(xd);
        int jd = static_cast<int>(yd);
        //int kd = static_cast<int>(zd);

        // Calculate the overlap between the floating voxel centered on (xd,
        // yd) and each of its four closest integer neighbours.
        float sw, se, nw, ne;
        sw = (1 + id - xd) * (1 + jd - yd);
        se = (xd - id) * (1 + jd - yd);
        nw = (1 + id - xd) * (yd - jd);
        ne = (xd - id) * (yd - jd);

        float e, f, g, h;
        e = (*src)(id, jd, z);
        f = (*src)(id + 1, jd, z);
        g = (*src)(id, jd + 1, z);
        h = (*src)(id + 1, jd + 1, z);
        // c represents the fraction of the floating voxel (xd, yd) that
        // overlaps with the region.
        float c = 0.;
        if(e != -1) c += sw * e;
        if(f != -1) c += se * f;
        if(g != -1) c += nw * g;
        if(h != -1) c += ne * h;

        if(c > propagation_overlap_threshold) {
          (*dst)(x, y, z) = 1;
          region_size++;

          for (int cdi = 0; cdi < cdn; ++cdi)
          {
            std::pair<Point3d, Void> pair;
            /* si deja traite on ne l'ajoute pas */
            Point3d p2 = p + cd.xyzOffset(cdi);
            pair.first = p2;
            if((p2[0] < 0) || (p2[0] >= img.getSizeX())) continue;
            if((p2[1] < 0) || (p2[1] >= img.getSizeY())) continue;
            if((p2[2] < 0) || (p2[2] >= img.getSizeZ())) continue;

            if((*src)(p2[0], p2[1], p2[2]) == 0)
              (*next_front)[0].insert(pair);
          }
        }
        if(debug_output && _verbose) {
          std::clog << "    (" << x << ", " << y << ", " << z << "): "
                    << (*src)(x, y, z) << " -> " << (*dst)(x, y, z) << std::endl;
        }
      }

      for(int z = 0; z < dst->getSizeZ(); ++z)
      for(int y = 0; y < dst->getSizeY(); ++y)
      for(int x = 0; x < dst->getSizeX(); ++x)
      {
        if((*src)(x, y, z) != 0)
          (*dst)(x, y, z) = (*src)(x, y, z);
      }

      if((*next_front)[0].size() == 0) break;
      if(debug_output >= 2) {
        std::ostringstream s;
        s << "plop_" << i << "_" << t << ".nii";
        Writer<Volume<float> > writer(s.str());
        writer.write(*dst);
      }
      front.clear();
      std::swap(current_front, next_front);
      std::swap(src, dst);
    }

    if(_verbose) {
      std::clog << "end region size: " << region_size << " voxels." << std::endl;
    }

    if(debug_output) {
      std::ostringstream s;
      s << "final_" << i << ".nii";
      Writer<Volume<float> > writer(s.str());
      writer.write(*src);
    }
  }
}
