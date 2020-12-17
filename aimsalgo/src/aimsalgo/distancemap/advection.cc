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
#include <aims/utility/converter_volume.h>
#include <aims/distancemap/advection.h>
#include <aims/connectivity/component.h>
#include <aims/bucket/bucket.h>
#include <aims/io/writer.h>

using std::swap;
using carto::VolumeRef;
using aims::BucketMap;

namespace {
  const int debug_output = 0;
}

const float aims::GradientAdvection::default_propagation_threshold = 0.3f;

aims::GradientAdvection::GradientAdvection()
  : _verbose(debug_output),
    _propagation_threshold(default_propagation_threshold)
{
}

aims::GradientAdvection::~GradientAdvection()
{
}

void aims::GradientAdvection::setVerbose(bool verbose)
{
  _verbose = verbose;
}


void aims::GradientAdvection::setPropagationThreshold(float thres)
{
  _propagation_threshold = thres;
}


// Volume<int16_t> is more widely supported than Volume<int8_t>
VolumeRef<int16_t>
aims::GradientAdvection::doit(const VolumeRef<int16_t> &label_vol,
                              const VolumeRef<float> &gradX,
                              const VolumeRef<float> &gradY,
                              const VolumeRef<float> & /*gradZ*/,
                              int16_t seedlabel)
{
  // Wouldn't a smaller neighbourhood suffice / be faster?
  const Connectivity cd(0, 0, Connectivity::CONNECTIVITY_26_XYZ);
  const int cdn = cd.nbNeighbors();

  const int size_x = label_vol->getSizeX();
  const int size_y = label_vol->getSizeY();
  const int size_z = label_vol->getSizeZ();

  if(_verbose) {
    std::clog << "aims::GradientAdvection for seed label "
              << seedlabel << ":" << std::endl;
  }

  VolumeRef<int8_t> src(size_x, size_y, size_z);
  src.header() = label_vol->header();
  VolumeRef<int8_t> dst(size_x, size_y, size_z);
  dst.header() = label_vol->header();

  typedef BucketMap<Void>::Bucket Bucket;
  carto::rc_ptr<BucketMap<Void> >
    current_front(new BucketMap<Void>()),
    next_front(new BucketMap<Void>());

  std::size_t region_size = 0;

  src->fill(0);
  // Build src image, where each voxel is either -1 (forbidden region), 0
  // (propagation region) or 1 (seed).
  for(int z = 0; z < size_z; ++z)
  for(int y = 0; y < size_y; ++y)
  for(int x = 0; x < size_x; ++x)
  {
    if((x == 0) || (y == 0) || (z == 0) ||
       (x == size_x - 1) || (y == size_y - 1) || (z == size_z - 1))
    {
      src(x, y, z) = -1;
      continue;
    }
    if(label_vol(x, y, z) == 0) continue;
    Point3d p(x, y, z);
    if(label_vol(x, y, z) == seedlabel)
      src(x, y, z) = 1;
    else {
      src(x, y, z) = -1;
      // Why are the neighbours also forbidden?
      for (int cdi = 0; cdi < cdn; ++cdi)
      {
        Point3d p2 = p + cd.xyzOffset(cdi);
        if((p2[0] < 0) || (p2[0] >= size_x)) continue;
        if((p2[1] < 0) || (p2[1] >= size_y)) continue;
        if((p2[2] < 0) || (p2[2] >= size_z)) continue;
        src(p2[0], p2[1], p2[2]) = -1;
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
    Writer<VolumeRef<int8_t> > writer("src.nii");
    writer.write(src);
  }

  // Propagate the region along the gradient, starting from the seed. A front
  // is used that includes the neighbours of all points added to the region
  // during the previous time step. Each point of the front is added to the
  // region if its local gradient points towards the region.

  /// \todo eliminate arbitrary time limit
  for (int t = 0; t < 200; ++t)
  {
    Bucket &front = (*current_front)[0];
    if(_verbose) {
      std::clog << "  iteration " << t
                << ", front size = " << front.size() << "..." << std::endl;
    }

    dst->fill(0);
    for(Bucket::const_iterator ib = front.begin(), eb = front.end();
        ib != eb;
        ++ib)
    {
      // For each point of the front, follow the local gradient along a
      // distance of one voxel. If a floating voxel placed there overlaps
      // more than 30% with voxels in the region, add it to the region and
      // propagate the front.
      const float overlap_threshold = _propagation_threshold;

      /// \todo adapt to work in 3D!
      /// \todo can this be made to work with non-cubic voxels?

      const Point3d &p = ib->first;
      const int x = p[0], y = p[1], z = p[2];

      assert(x >= 0 && x < size_x);
      assert(y >= 0 && y < size_y);
      assert(z >= 0 && z < size_z);
      if(src(x, y, z) != 0) continue;

      float gX = gradX(x, y, z);
      float gY = gradY(x, y, z);
      //float gZ = gradZ(x, y, z);
      float gN = std::sqrt(gX * gX + gY * gY/* + gZ * gZ*/);
      // Do not add voxel if gradient is NaN, Inf, or too close to zero.
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
      if(xd > size_x - 1.5) xd = size_x - 1.5;
      if(yd > size_y - 1.5) yd = size_y - 1.5;
      //if(zd > size_z - 1.5) zd = size_z - 1.5;
      int id = static_cast<int>(xd);
      int jd = static_cast<int>(yd);
      //int kd = static_cast<int>(zd);

      // Calculate the overlap area between the floating voxel centered on (xd,
      // yd) and each of its four closest integer neighbours.
      float sw, se, nw, ne;
      sw = (1 + id - xd) * (1 + jd - yd);
      se = (xd - id) * (1 + jd - yd);
      nw = (1 + id - xd) * (yd - jd);
      ne = (xd - id) * (yd - jd);

      float e, f, g, h;
      e = src(id, jd, z);
      f = src(id + 1, jd, z);
      g = src(id, jd + 1, z);
      h = src(id + 1, jd + 1, z);
      // c represents the fraction of the floating voxel (xd, yd) that
      // overlaps with the region.
      float c = 0;
      if(e != -1) c += sw * e;
      if(f != -1) c += se * f;
      if(g != -1) c += nw * g;
      if(h != -1) c += ne * h;

      if(c > overlap_threshold) {
        dst(x, y, z) = 1;
        region_size++;

        for (int cdi = 0; cdi < cdn; ++cdi)
        {
          std::pair<Point3d, Void> pair;
          /* si deja traite on ne l'ajoute pas */
          Point3d p2 = p + cd.xyzOffset(cdi);
          pair.first = p2;
          if((p2[0] < 0) || (p2[0] >= size_x)) continue;
          if((p2[1] < 0) || (p2[1] >= size_y)) continue;
          if((p2[2] < 0) || (p2[2] >= size_z)) continue;

          if(src(p2[0], p2[1], p2[2]) == 0)
            (*next_front)[0].insert(pair);
        }
      }
      if(debug_output && _verbose) {
        std::clog << "    (" << x << ", " << y << ", " << z << "): "
                  << static_cast<int>(src(x, y, z)) << " -> "
                  << static_cast<int>(dst(x, y, z)) << std::endl;
      }
    }

    for(int z = 0; z < size_z; ++z)
    for(int y = 0; y < size_y; ++y)
    for(int x = 0; x < size_x; ++x)
    {
      if(src(x, y, z) != 0)
        dst(x, y, z) = src(x, y, z);
    }

    if((*next_front)[0].size() == 0) break;
    if(debug_output >= 2) {
      std::ostringstream s;
      s << "plop_" << t << ".nii";
      Writer<VolumeRef<int8_t> > writer(s.str());
      writer.write(dst);
    }
    front.clear();
    swap(current_front, next_front);
    swap(src, dst);
  }

  if(_verbose) {
    std::clog << "end region size: " << region_size << " voxels."
              << std::endl;
  }

  carto::Converter<VolumeRef<int8_t>, VolumeRef<int16_t> > converter;
  return *converter(dst);
}

VolumeRef<int16_t>
aims::GradientAdvection::propagate_all_labels(
  const VolumeRef<int16_t> &label_vol,
  const VolumeRef<float> &gradX,
  const VolumeRef<float> &gradY,
  const VolumeRef<float> &gradZ)
{
  // Wouldn't a smaller neighbourhood suffice / be faster?
  const Connectivity cd(0, 0, Connectivity::CONNECTIVITY_26_XYZ);
  const int cdn = cd.nbNeighbors();

  VolumeRef<int16_t> vol(label_vol);
  const int size_x = vol->getSizeX();
  const int size_y = vol->getSizeY();
  const int size_z = vol->getSizeZ();

  if(_verbose) {
    std::clog << "aims::GradientAdvection::propagate_all_labels:" << std::endl;
  }

  typedef BucketMap<Void>::Bucket Bucket;
  carto::rc_ptr<BucketMap<Void> >
    current_front(new BucketMap<Void>()),
    next_front(new BucketMap<Void>());

  if(_verbose) {
    std::clog << "  building initial front..." << std::endl;
  }

  // Initialize the front to the neighbourhood of all seeds.
  for(int z = 0; z < size_z; ++z)
  for(int y = 0; y < size_y; ++y)
  for(int x = 0; x < size_x; ++x)
  {
    if(label_vol(x, y, z) > 0) {
      Point3d p(x, y, z);
      for (int cdi = 0; cdi < cdn; ++cdi) {
        Point3d p2 = p + cd.xyzOffset(cdi);
        if(label_vol(p2[0], p2[1], p2[2]) == 0) {
          std::pair<Point3d, Void> pair;
          pair.first = p2;
          (*current_front)[0].insert(pair);
        }
      }
    }
  }

  VolumeRef<int8_t> value_origin(size_x, size_y, size_z);
  value_origin->header() = label_vol->header();
  value_origin.fill(-128);

  // Propagate the region along the gradient, starting from the seed. A front
  // is used that includes the neighbours of all points added to the region
  // during the previous time step. Each point of the front is added to the
  // region that its local gradient points towards.

  /// \todo eliminate arbitrary time limit
  for (int t = 0; t < 200; ++t)
  {
    Bucket &front = (*current_front)[0];
    if(_verbose) {
      std::clog << "  iteration " << t
                << ", front size = " << front.size() << "..." << std::endl;
    }

    for(Bucket::const_iterator ib = front.begin(), eb = front.end();
        ib != eb;
        ++ib)
    {
      // For each point of the front, follow the local gradient along a
      // distance of one voxel. Round to the nearest integer voxel, and add the
      // current voxel to the region that this voxel belongs to, then propagate
      // the front.

      /// \todo can this be made to work with non-cubic voxels?

      const Point3d &p = ib->first;
      const int x = p[0], y = p[1], z = p[2];

      assert(x >= 0 && x < size_x);
      assert(y >= 0 && y < size_y);
      assert(z >= 0 && z < size_z);
      if(vol(x, y, z) != 0) continue;

      float gX = gradX(x, y, z);
      float gY = gradY(x, y, z);
      float gZ = gradZ(x, y, z);
      //float gN = std::sqrt(gX * gX + gY * gY + gZ * gZ);
      float gN = std::max(std::abs(gX), std::max(std::abs(gY), std::abs(gZ)));
      // Do not add voxel if gradient is NaN, Inf, or too close to zero.
      if(!std::isnormal(gN)) continue;
      gX /= gN;
      gY /= gN;
      gZ /= gN;

      float xd = x + gX;
      float yd = y + gY;
      float zd = z + gZ;
      if(xd < 0.5) xd = 0.5;
      if(yd < 0.5) yd = 0.5;
      if(zd < 0.5) zd = 0.5;
      if(xd > size_x - 1.5) xd = size_x - 1.5;
      if(yd > size_y - 1.5) yd = size_y - 1.5;
      if(zd > size_z - 1.5) zd = size_z - 1.5;
      int id = static_cast<int>(xd + 0.5);
      int jd = static_cast<int>(yd + 0.5);
      int kd = static_cast<int>(zd + 0.5);

      if(debug_output) {
        assert(std::abs(id - x) <= 1);
        assert(std::abs(jd - y) <= 1);
        assert(std::abs(kd - z) <= 1);
        value_origin(x, y, z) = (id - x) + (jd - y) * 2 + (kd - z) * 4;
      }

      if(debug_output && id == x && jd == y && kd == z) {
        std::clog << "    staying same voxel!" << std::endl;
      }

      int16_t pointed_region = vol(id, jd, kd);
      if(pointed_region > 0) {
        vol(x, y, z) = pointed_region;

        // Add neighbours to the next front
        for (int cdi = 0; cdi < cdn; ++cdi)
        {
          Point3d p2 = p + cd.xyzOffset(cdi);
          if((p2[0] < 0) || (p2[0] >= size_x)) continue;
          if((p2[1] < 0) || (p2[1] >= size_y)) continue;
          if((p2[2] < 0) || (p2[2] >= size_z)) continue;

          if(vol(p2[0], p2[1], p2[2]) == 0) {
            std::pair<Point3d, Void> pair;
            pair.first = p2;
            (*next_front)[0].insert(pair);
          }
        }
      }
    }

    if((*next_front)[0].size() == 0) break;
    if(debug_output >= 2) {
      std::ostringstream s;
      s << "vol_" << t << ".nii";
      Writer<VolumeRef<int16_t> > writer(s.str());
      writer.write(vol);
    }
    front.clear();
    swap(current_front, next_front);
  }

  if(debug_output) {
    Writer<VolumeRef<int8_t> > writer("value_origin.nii");
    writer.write(value_origin);
  }

  return vol;
}

VolumeRef<int16_t>
aims::GradientAdvection::descend_scalar_field(
  const VolumeRef<int16_t> &label_vol,
  const VolumeRef<float> &field)
{
  // Wouldn't a smaller neighbourhood suffice / be faster?
  const Connectivity cd(0, 0, Connectivity::CONNECTIVITY_26_XYZ);
  const int cdn = cd.nbNeighbors();

  VolumeRef<int16_t> vol(label_vol);
  const int size_x = vol->getSizeX();
  const int size_y = vol->getSizeY();
  const int size_z = vol->getSizeZ();

  if(_verbose) {
    std::clog << "aims::GradientAdvection::descend_scalar_field:" << std::endl;
  }

  typedef BucketMap<Void>::Bucket Bucket;
  carto::rc_ptr<BucketMap<Void> >
    current_front(new BucketMap<Void>()),
    next_front(new BucketMap<Void>());

  if(_verbose) {
    std::clog << "  building initial front..." << std::endl;
  }

  // Initialize the front to the neighbourhood of all seeds.
  for(int z = 0; z < size_z; ++z)
  for(int y = 0; y < size_y; ++y)
  for(int x = 0; x < size_x; ++x)
  {
    if(label_vol(x, y, z) > 0) {
      Point3d p(x, y, z);
      for (int cdi = 0; cdi < cdn; ++cdi) {
        Point3d p2 = p + cd.xyzOffset(cdi);
        if((p2[0] < 0) || (p2[0] >= size_x)) continue;
        if((p2[1] < 0) || (p2[1] >= size_y)) continue;
        if((p2[2] < 0) || (p2[2] >= size_z)) continue;

        if(label_vol(p2[0], p2[1], p2[2]) == 0) {
          std::pair<Point3d, Void> pair;
          pair.first = p2;
          (*current_front)[0].insert(pair);
        }
      }
    }
  }

  VolumeRef<int8_t> value_origin(size_x, size_y, size_z);
  value_origin->header() = label_vol->header();
  value_origin.fill(-128);

  // Propagate the region along descending values, starting from the seed. A
  // front is used that includes the neighbours of all points added to the
  // region during the previous time step. Each point of the front is added to
  // the region that the highest neighbour belongs to.

  /// \todo eliminate arbitrary time limit
  for (int t = 0; t < 200; ++t)
  {
    Bucket &front = (*current_front)[0];
    if(_verbose) {
      std::clog << "  iteration " << t
                << ", front size = " << front.size() << "..." << std::endl;
    }

    for(Bucket::const_iterator ib = front.begin(), eb = front.end();
        ib != eb;
        ++ib)
    {
      // For each point of the front, follow the local gradient along a
      // distance of one voxel. Round to the nearest integer voxel, and add the
      // current voxel to the region that this voxel belongs to, then propagate
      // the front.

      /// \todo can this be made to work with non-cubic voxels?

      const Point3d &p = ib->first;
      const int x = p[0], y = p[1], z = p[2];

      assert(x >= 0 && x < size_x);
      assert(y >= 0 && y < size_y);
      assert(z >= 0 && z < size_z);
      if(vol(x, y, z) != 0) continue;

      int cdi_max = -1;
      float max_val = field(x, y, z);
      for (int cdi = 0; cdi < cdn; ++cdi)
      {
        Point3d p2 = p + cd.xyzOffset(cdi);
        if((p2[0] < 0) || (p2[0] >= size_x)) continue;
        if((p2[1] < 0) || (p2[1] >= size_y)) continue;
        if((p2[2] < 0) || (p2[2] >= size_z)) continue;

        float p2_val = field(p2[0], p2[1], p2[2]);
        if(p2_val > max_val) {
          max_val = p2_val;
          cdi_max = cdi;
        }
      }

      if(cdi_max == -1) {  // All neighbours are lower
        if(debug_output) {
          value_origin(x, y, z) = 127;
        }
      }
      Point3d p2 = p + cd.xyzOffset(cdi_max);
      int id = p2[0], jd = p2[1], kd = p2[2];

      if(debug_output) {
        value_origin(x, y, z) = (id - x) + (jd - y) * 2 + (kd - z) * 4;
      }

      int16_t pointed_region = vol(id, jd, kd);
      if(pointed_region > 0) {
        if(debug_output && vol(x, y, z) != 0) {
          std::clog << "    (" << x << ", " << y << ", " << z << "): "
                    << vol(x, y, z) << " -> " << pointed_region << std::endl;
        }
        vol(x, y, z) = pointed_region;

        // Add neighbours to the next front
        for (int cdi = 0; cdi < cdn; ++cdi)
        {
          Point3d p2 = p + cd.xyzOffset(cdi);
          if((p2[0] < 0) || (p2[0] >= size_x)) continue;
          if((p2[1] < 0) || (p2[1] >= size_y)) continue;
          if((p2[2] < 0) || (p2[2] >= size_z)) continue;

          if(vol(p2[0], p2[1], p2[2]) == 0) {
            std::pair<Point3d, Void> pair;
            pair.first = p2;
            (*next_front)[0].insert(pair);
          }
        }
      }
    }

    if((*next_front)[0].size() == 0) break;
    if(debug_output >= 2) {
      std::ostringstream s;
      s << "vol_" << t << ".nii";
      Writer<VolumeRef<int16_t> > writer(s.str());
      writer.write(vol);
    }
    front.clear();
    swap(current_front, next_front);
  }

  if(debug_output) {
    Writer<VolumeRef<int8_t> > writer("value_origin.nii");
    writer.write(value_origin);
  }

  return vol;
}
