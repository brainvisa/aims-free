/*
 * Copyright CEA (2018).
 *
 * Author: Yann Leprince <yann.leprince@cea.fr>.
 */
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
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iterator>

#include <soma-io/allocator/allocator.h>
#include <cartobase/config/verbose.h>
#include <cartodata/volume/volume.h>
#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>


using std::clog;
using std::endl;
using soma::AllocatorStrategy;
using soma::AllocatorContext;
using carto::VolumeRef;
using carto::verbose;


// Anonymous namespace for file-local symbols
namespace
{

const int EXIT_USAGE_ERROR = 2;
std::string program_name;


typedef VolumeRef<Point3df> FFDVolumeRef;


VolumeRef<Point3df> ffd_from_transformed_coord_volumes(
  const VolumeRef<float>& x_volume,
  const VolumeRef<float>& y_volume,
  const VolumeRef<float>& z_volume)
{
  const int size_x = x_volume.getSizeX(),
    size_y = x_volume.getSizeY(),
    size_z = x_volume.getSizeZ();

  assert(y_volume.getSize() == x_volume.getSize()
         && z_volume.getSize() == x_volume.getSize());
  assert(y_volume.getVoxelSize() == x_volume.getVoxelSize()
         && z_volume.getVoxelSize() == x_volume.getVoxelSize());

  const std::vector<float> voxel_size = x_volume.getVoxelSize();
  const float voxel_size_x = voxel_size[0],
    voxel_size_y = voxel_size[1],
    voxel_size_z = voxel_size[2];

  FFDVolumeRef ret(size_x, size_y, size_z);
  ret->copyHeaderFrom(x_volume->header());

  for(int z = 0; z < size_z; ++z)
    for(int y = 0; y < size_y; ++y)
      for(int x = 0; x < size_x; ++x)
      {
        ret->at(x, y, z)[0] = x_volume->at(x, y, z) - x * voxel_size_x;
        ret->at(x, y, z)[1] = y_volume->at(x, y, z) - y * voxel_size_y;
        ret->at(x, y, z)[2] = z_volume->at(x, y, z) - z * voxel_size_z;
      }

  return ret;
}

VolumeRef<Point3df> ffd_from_transformed_multichannel_5d_coord_volume(
  const VolumeRef<float>& multichannel_volume)
{
  const std::vector<int> size = multichannel_volume.getSize();
  const int size_x = size[0], size_y = size[1], size_z = size[2];

  assert(size.size() >= 5);
  assert(size[3] == 1);
  assert(size[4] == 3);

  const std::vector<float> voxel_size = multichannel_volume.getVoxelSize();
  const float voxel_size_x = voxel_size[0],
    voxel_size_y = voxel_size[1],
    voxel_size_z = voxel_size[2];

  FFDVolumeRef ret(size_x, size_y, size_z);
  ret->copyHeaderFrom(multichannel_volume->header());

  for(int z = 0; z < size_z; ++z) {
    for(int y = 0; y < size_y; ++y) {
      for(int x = 0; x < size_x; ++x)
      {
        ret->at(x, y, z)[0] = multichannel_volume->at(x, y, z, 0, 0)
                              - x * voxel_size_x;
        ret->at(x, y, z)[1] = multichannel_volume->at(x, y, z, 0, 1)
                              - y * voxel_size_y;
        ret->at(x, y, z)[2] = multichannel_volume->at(x, y, z, 0, 2)
                              - z * voxel_size_z;
      }
    }
  }

  return ret;
}

VolumeRef<Point3df> ffd_from_transformed_multichannel_4d_coord_volume(
  const VolumeRef<float>& multichannel_volume)
{
  const std::vector<int> size = multichannel_volume.getSize();
  const int size_x = size[0], size_y = size[1], size_z = size[2];

  assert(size.size() >= 4);
  assert(size[3] == 3);

  const std::vector<float> voxel_size = multichannel_volume.getVoxelSize();
  const float voxel_size_x = voxel_size[0],
    voxel_size_y = voxel_size[1],
    voxel_size_z = voxel_size[2];

  FFDVolumeRef ret(size_x, size_y, size_z);
  ret->copyHeaderFrom(multichannel_volume->header());

  for(int z = 0; z < size_z; ++z) {
    for(int y = 0; y < size_y; ++y) {
      for(int x = 0; x < size_x; ++x)
      {
        ret->at(x, y, z)[0] = multichannel_volume->at(x, y, z, 0)
                              - x * voxel_size_x;
        ret->at(x, y, z)[1] = multichannel_volume->at(x, y, z, 1)
                              - y * voxel_size_y;
        ret->at(x, y, z)[2] = multichannel_volume->at(x, y, z, 2)
                              - z * voxel_size_z;
      }
    }
  }

  return ret;
}

} // end of anonymous namespace


int main(const int argc, const char **argv)
{
  aims::Reader<VolumeRef<float> > multichannel_reader;
  aims::Reader<VolumeRef<float> > x_reader, y_reader, z_reader;
  aims::Writer<FFDVolumeRef> ffd_writer;

  program_name = argv[0];
  // Initialize command-line option parsing
  aims::AimsApplication app(
    argc, argv,
    "Compute a displacement field (FFD) from transformed coordinate images.\n"
    "\n"
    "Usage:\n"
    "1. Create images of X, Y, and Z coordinate with:\n"
    "   python2 -m soma.aimsalgo.scripts.create_coordinate_images\n"
    "2. Warp these images using any tool that you want\n"
    "3. Feed the warped images into this program to reconstruct the FFD\n"
    "   displacement field that performs the same warping.\n"
    "4. You can now use AimsRegisterFFDApply to perform the warping."
  );
  app.addOption(multichannel_reader, "--input",
                "transformed multi-channel image of X, Y, and Z coordinates "
                "(4D or 5D)",
                true);
  app.addOption(x_reader, "--input-x",
                "transformed image of X coordinates", true);
  app.addOption(y_reader, "--input-y",
                "transformed image of Y coordinates", true);
  app.addOption(z_reader, "--input-z",
                "transformed image of Z coordinates", true);
  app.addOption(ffd_writer, "--output", "output displacement field");
  app.alias("-i", "--input");
  app.alias("-x", "--input-x");
  app.alias("-y", "--input-y");
  app.alias("-z", "--input-z");
  app.alias("-o", "--output");
  app.alias("-v", "--verbose");

  // Process command-line options
  try
  {
    app.initialize();
  }
  catch(const carto::user_interruption &)
  {
    // Exit after printing e.g. help
    return EXIT_SUCCESS;
  }
  catch(const std::runtime_error &e)
  {
    clog << program_name << ": error processing command-line options: "
         << e.what() << endl;
    return EXIT_USAGE_ERROR;
  }

  bool use_multichannel = !multichannel_reader.fileName().empty();
  bool use_separate_channels = !x_reader.fileName().empty()
    || !y_reader.fileName().empty()
    || !z_reader.fileName().empty();
  if(!(use_multichannel || use_separate_channels)) {
    clog << program_name << ": you must specify either a multi-channel "
      "--input or three separate-chanel --input-[xyz] parameters." << endl;
    return EXIT_USAGE_ERROR;
  }
  if(use_multichannel && use_separate_channels) {
    clog << program_name << ": you must specify either --input or the three"
      "--input-[xyz] parameters, not both." << endl;
    return EXIT_USAGE_ERROR;
  }
  if(use_separate_channels && !(!x_reader.fileName().empty()
                                && !y_reader.fileName().empty()
                                && !z_reader.fileName().empty())) {
    clog << program_name << ": you must specify or all three --input-[xyz]"
      " parameters for the separate-channel input." << endl;
    return EXIT_USAGE_ERROR;
  }

  bool success = false;
  FFDVolumeRef ffd_volume;

  if(use_separate_channels) {
    if(verbose != 0) {
      clog << program_name << ": reading transformed image of X coordinates..."
           << endl;
    }
    x_reader.setAllocatorContext(
      AllocatorContext(AllocatorStrategy::ReadOnly));
    VolumeRef<float> x_volume;
    success = x_reader.read(x_volume);
    if(!success) {
      clog << program_name << ": error reading input file '"
           << x_reader.fileName()
           << "', aborting." << endl;
      return EXIT_FAILURE;
    }

    if(verbose != 0) {
      clog << program_name << ": reading transformed image of Y coordinates..."
           << endl;
    }
    y_reader.setAllocatorContext(
      AllocatorContext(AllocatorStrategy::ReadOnly));
    VolumeRef<float> y_volume;
    success = y_reader.read(y_volume);
    if(!success) {
      clog << program_name << ": error reading input file '"
           << y_reader.fileName()
           << "', aborting." << endl;
      return EXIT_FAILURE;
    }

    if(verbose != 0) {
      clog << program_name << ": reading transformed image of Z coordinates..."
           << endl;
    }
    z_reader.setAllocatorContext(
      AllocatorContext(AllocatorStrategy::ReadOnly));
    VolumeRef<float> z_volume;
    success = z_reader.read(z_volume);
    if(!success) {
      clog << program_name << ": error reading input file '"
           << z_reader.fileName()
           << "', aborting." << endl;
      return EXIT_FAILURE;
    }

    if(y_volume.getSize() != x_volume.getSize()
       || z_volume.getSize() != x_volume.getSize())
    {
      clog << program_name
           << ": error: all input volumes must have the same dimensions" << endl;
      return EXIT_FAILURE;
    }

    if(y_volume->getVoxelSize() != x_volume->getVoxelSize()
       || z_volume->getVoxelSize() != x_volume->getVoxelSize())
    {
      clog << program_name
           << ": error: all input volumes must have the same voxel size" << endl;
      return EXIT_FAILURE;
    }

    if(verbose != 0) {
      clog << program_name << ": generating FFD volume..." << endl;
    }
    ffd_volume = ffd_from_transformed_coord_volumes(
      x_volume, y_volume, z_volume
    );
  } else if(use_multichannel) {
    if(verbose != 0) {
      clog << program_name << ": reading transformed multi-channel image..."
           << endl;
    }
    multichannel_reader.setAllocatorContext(
      AllocatorContext(AllocatorStrategy::ReadOnly));
    VolumeRef<float> multichannel_volume;
    success = multichannel_reader.read(multichannel_volume);
    if(!success) {
      clog << program_name << ": error reading input file '"
           << multichannel_reader.fileName()
           << "', aborting." << endl;
      return EXIT_FAILURE;
    }

    const std::vector<int> size = multichannel_volume.getSize();
    if(size.size() >= 5 && size[3] == 1 && size[4] == 3) {
      if(verbose != 0) {
        clog << program_name << ": generating FFD volume..." << endl;
      }
      ffd_volume = ffd_from_transformed_multichannel_5d_coord_volume(
        multichannel_volume);
    } else if(size.size() >= 4 && size[3] == 3) {
      if(verbose != 0) {
        clog << program_name << ": generating FFD volume..." << endl;
      }
      ffd_volume = ffd_from_transformed_multichannel_4d_coord_volume(
        multichannel_volume);
    } else {
      clog << program_name << ": the multi-channel volume must have size "
              "(X, Y, Z, 1, 3) or (X, Y, Z, 3), but the provided file ("
           << multichannel_reader.fileName()
           << ") has size (";
      for(std::vector<int>::const_iterator it = size.begin();
          it != size.end();
          ++it) {
        if(it != size.begin()) {
          clog << ", ";
        }
        clog << *it;
      }
      clog << "), aborting." << endl;
      return EXIT_FAILURE;
    }
  } else {
    assert(false);
  }

  if(verbose != 0) {
    clog << program_name << ": writing output FFD volume..." << endl;
  }
  {
    bool write_success = ffd_writer.write(ffd_volume);
    if(!write_success) {
      clog << program_name << ": cannot write output volume" << endl;
    }
    success = success && write_success;
  }

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
