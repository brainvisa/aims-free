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

// This file is only here for Doxygen (doc generation tool)
// Nothing to do with the code, in fact...

namespace carto
{

/*! \mainpage CartoData documentation

  \anchor cartodata

  Cartograph - data package: standard data structures and IO system for 
  image processing.


  \section cartodata_provides Provided libraries and commands:

  - cartodata library


  \section cartodata_requirements Requirements

  CartoData relies on \ref somaio "Soma-IO" and \ref cartobase "CartoBase" and
  so needs their requirements.

  Optionally, cartodata may make use of the following libraries if available:
  - blitz++: http://blitz.sourceforge.net/ for efficient volume accessors
    and array API


  \section cartodata_building Building:

  Use <a href="http://www.cmake.org/">CMake</a>, a cross-platform and open-source build system. More information on <a href="https://brainvisa.info/brainvisa-cmake/compile_existing.html">The BrainVISA-cmake build system</a>. See also <a href="https://brainvisa.info/web/infrastructure.html#sources">this page on BrainVISA website</a>.

  \section cartodata_maincontents Main contents

  - \ref cartovolumes
  - \ref cartovolumes_io : IO types and formats implementations on volumes

  <!--- <b><a href="changelog.html">Change Log</a></b> -->

*/


/*! \page cartovolumes Volumes concept philosophy

  Volumes rely on the allocator system of somaio: see \ref allocators.

  First, a Volume is built using any allocator method. This means a DataSource 
  and an AllocatorContext are provided to it. It can remain unallocated (no 
  data buffer) if it is not used directly, but its data values should not be 
  accessed until it is allocated.

  \section sub_volume Volumes and sub-volumes

  A subvolume can be opened into an existing volume. This used to be a
  separate class, VolumeView, up to aims-4.3, but has been merged with
  the regular Volume class in aims-4.4.
  A view is used either to operate on smaller data blocks than the whole volume
  itself, or to simulate the "volume with borders" concept, or to "paste" a 
  volume (buffer or file) into a larger one. Access to views is granted from
  one (view) volume into another one, using the Volume::refVolume(),
  and Volume::posInRefVolume() methods.
  Depending on the AllocatorContext and DataSource used for both the bigger
  volume and the view, different behaviours can be achieved.
  In the following we name "Volume" the "larger" volume, and "View" the view
  in the former.

  - Volume allocated and linked to an existing DataSource (file or filled 
    buffer), and View with no DataSource:\n
    This is the classical subvolume (region) case. All data for the whole 
    volume are allocated (possibly using memory mapping), and the View
    is only useful to work on a restriced portion of the data, or to write / 
    copy the View to another file or buffer.\n
    The AllocationContext of the View is not used in this case, the data
    block always belongs to the bigger Volume.

  - Volume allocated with no DataSource, and View linked to an existing
    DataSource:\n
    This case is the "volume with borders" case: the buffer is allocated with 
    borders included in the bigger Volume, and data are loaded directly in 
    the View window. Work can be done in the View with no care of
    going in the borders (when borders are large enough).\n
    Another use of this is loading blocks of different files into a bigger 
    volume without allocating nor copying a second volume.\n
    The AllocationContext of the View is not used in this case, the data
    block always belongs to the bigger Volume.

  - Volume allocated and linked to an existing a DataSource, and View
    also linked to an existing DataSource:\n
    In this case the DataSource of the View will be used to load data
    into it and replace data from the original Volume (the Volume must be 
    allocated in read/write mode). This case only differs from the previous 
    one (borders) in that the "borders" are filled by the contents of another 
    data source.

  - Volume unallocated but linked to an existing DataSource, and View
    allocated with no DataSource:\n
    This is used to allocate and load only a part of a file: only the view 
    is allocated (using its own AllocatorContext) and loaded from the bigger 
    Volume Datasource: offsets in a large file correspond to the bigger 
    Volume. If allocated read/write with memory mapping, changes will 
    also affect the file. Otherwise, saving the whole modified volume is 
    not possible at present.\n
    We may think about using the same trick for writing: writing from the 
    View, but using the output DataSource of the Volume, would save
    only the viewed part, or copy the original file outside of it. But this 
    will not be implemented at the beginning.\n
    \warning As buffers are actually held in the View, several views
    into the same unallocated Volume will not share data: this means that 
    overlaps will be duplicated and independent in each view. This is not the 
    case when using views on an allocated Volume.

  - Volume unallocated with no DataSource:\n
    Whatever the SubVolume in it, I think this configuration is useless. The 
    View will act as a full Volume, the bigger one won't be used.

  Here I am speaking of Volume and View implemented as two separate
  classes, but they might be implemented in the same class: a Volume would 
  be a view into another if it holds a reference to another (bigger) Volume.

  In any case, a View is a complete Volume and has all functionalities
  of a Volume. This includes the capability to allow other views into it.

  A View holds a reference-counting pointer to the Volume it is open on.
  So the original bigger Volume cannot be deleted while a View looking
  into it is still living.

  Using cleverly the different combinations of allocators and DataSource on 
  Volumes and Views allows to implement every memory-saving operations
  on very large files: memory mapping, block-by-block sequential reading, etc.

  \attention What will a View become if the Volume it views in resizes
  or is reallocated ?

  \section volume_iteration Iterating over volumes

  There are several ways to iterate over the voxels of a volume:

  - Accessors: volume.at( position ).
    If the number of dimensions is fixed (typically 3, or 4) then the
    programmer may write the required number of nested loops:
    \code
    Volume<float> vol( 10, 10, 10 ); // fill volume
    std::vector<int> size = vol.getSize();
    float sum = 0.;
    for( int z=0; z<size[2]; ++z )
      for( int y=0; y<size[1]; ++y )
        for( int x=0; x<size[0]; ++x )
          sum += vol.at( x, y, z );
    \endcode

  - iterators: the iterator returned by volume.begin() will iterate over all
    dimensions. Depending on the implementation chosen at compilation time
    (blitz++ support or not), the behavior will be different:
    with blitz++ support, iterators will iterate as expected over each "actual"
    voxel of the volume, but with reduced efficiency. Witout blitz++ support,
    iterators will act like pointers: they will not take into account
    borders in the case of a view into a larger volume. Thus it will make too
    many iterations, and thus be wrong, in this case.
    \code
    Volume<float> vol( 10, 10, 10 ); // fill volume
    Volume<float>::iterator it, e = vol.end();
    float sum = 0.;
    for( it=vol.begin(); it!=e; ++it )
      sum += *it;
    \endcode

  - N-D iterators:  if the volume contains an arbitrary number of dimensions,
    then the nested loops cannot be hard-coded in the iterating code. The
    NDIterator and line_NDIterator classes in the CartoBase library provide
    convenient ways of walking through a Volume:
    \code
    Volume<float> vol( 10, 10, 10 ); // fill volume
    float sum = 0.;
    NDIterator<float> it( &vol.at( 0 ), vol.getSize(), vol.getStrides() );
    for( ; !it.ended(); ++it )
      sum += *it;
    \endcode
    or, using line_NDIterator:
    \code
    Volume<float> vol( 10, 10, 10 ); // fill volume
    float *p, *pp;
    float sum = 0.;
    line_NDIterator<float> it( &vol.at( 0 ), vol.getSize(), vol.getStrides(), true );
    for( ; !it.ended(); ++it )
    {
      p = &*it;
      for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ) )
        ++(*p);
    }
    \endcode

  In terms of performance, the impact depends on the "weight" of the operation
  performed with each voxel, the lighter this operation, the more impacted it
  is by the iteration overhead. For a very simple operation (like incrementing
  each voxel value), if we take the Volume::iterator performance as reference
  1., we can expect approximately:

  Iteration type                        | Perf. | N-D support | view support
  --------------------------------------|-------|-------------|-------------
  Volume::iterator with blitz++ support |   1.  |     yes     |     yes
  pointers                              |  10.  |     yes     |      no
  accessors with blitz++ support        |  5-10 |      no     |     yes
  NDIterator                            |   1.  |     yes     |     yes
  line_NDIterator                       |  10.  |     yes     |     yes

  Thus in most cases the best compromise between performance and flexibility
  (support for the general N-dimension case, support for views inside larger
  volumes) is using line_NDIterator.

  For comparison, here are some tests results on a Core i7-4600U 2.1GHz CPU (laptop) using different compilers on linux and trying different types of implementations, all running on the same machine. The reference (factor 1.) was actually 4.2e+08 voxels/s using iterators with gcc 4.9. The implementation choice was taken as the best between implementations (marked with a star), and depends on the compiler. It can be seen that there are striking behavior changes between compilers. Moreover it seems that there are also different behaviors using different machines / processors, so the choices we have made might not be the best ones for every machine. However for now they are hard-coded using preprocessor #if directives.

  Iteration type                  | gcc 4.9 | gcc 6  | clang 3.8 | clang 6
  --------------------------------|---------|--------|-----------|----------
  iterator                        |   1.    |  1.    |    0.9    |   0.8
  pointers                        |  10.9   | 10.9   |   10.6    |  11.2
  accessors (raw 4D)              |   4.6 * |  4.6 * |    5.4    |   5.4
  accessors (blitz)               |   4.5   |  4.6   |   10.7 *  |  11.  *
  vector accessor (loop)          |   0.2   |  2.1 * |    3.8 *  |   5.4 *
  vector accessor (switch on dim) |   3.5 * |  1.2   |    0.5    |   0.5
  vector accessor (blitz)         |   0.3   |  0.7   |    0.17   |   0.2
  NDIterator                      |   0.9   |  1.2   |    1.2    |   1.2
  line_NDIterator                 |  10.7   | 10.4   |   10.6    |  11.


  \section volume_orient Volumes axes orientation

  A volume is not just an array. Each index in the array corresponds to a direction in space. By default, the first axis (X) corresponds to right->left head orientation, the second (Y) is anterior -> posterior, the third (Z) is superior -> inferior. Remaining axes are free of interpretation.

  Axes orientation has a code using 3 letters for the initials of the axes directions: see [this AIMS document](../../aimsdata/user_doc/coordinates_systems.html) and http://www.grahamwideman.com/gw/brain/orientation/orientterms.htm.

  Using this convention, the default orientation in Cartodata / AIMS is "LPI".

  Before CartoData 5.2 this LPI orientation was fixed.

  In CartoData 5.2 the orientation can be changed.

  Note that there are several axes orientations at different places and levels:

  - disk storage voxels orientation: voxels are stored on disk in a certain order, corresponding to certain axes. Some file formats impose a given order, others (like NIFTI) allow other orientations.

  - physical memory orientation (or "memory layout"): voxels are stored in computer memory in a possibly different order, with axes in a different order or orientation.

  - array indices orientation: each index in the Volume structure array (ie volume(x, y, z) correspond to an axis with a given orientation.

  Array indices and memory orientation can differ, using strides. Starting in Cartodata 5.2, strides can also be negative, which means we can switch between increasing and decreasing axes directions without copying voxels data.

  Thus "loading in LPI orientation" may mean 2 different things: load in LPI memory layout, meaning that voxels are contiguous in the R->L direction, or it may mean that the first index increases from right to left. We choose to take the second convention: "orientation" means indices axes orientation, whatever the actual order of voxels in memory. The voxels order is thus called "memory layout".

  \b Querying orientation

  - Axes (indices) orientation can be queries using:

    To know the orientation of a Volume, and to help transfroming to other orientations, the Referential class can help:
    \code
    const Referential & ref = vol.referential();
    std::cout << ref.orientationStr() << std::endl;

    // or using a vector of constants:
    std::vector<int> orient_v = vol.referential.axesOrientation();

    rc_ptr<Transformation3d> trans = ref.toOrientation( "ASR" );
    \endcode
    Note that Referential gives only information about the indices axes, not on the memory layout (which is handled by strides).

  - Memory layout orientation:
    \code
    std::vector<int> orient_v = vol.memoryLayoutOrientation();
    // then if needed:
    std::string orient = vol.referential().orientationStr( orient_v );
    \endcode

  - Storage (disk) layout orientation:

    Note that the storage layout may be undefined if the volume header property "storage_to_memory" is not defined.
    \code
    std::vector<int> orient_v = vol.storageLayoutOrientation();
    // then if needed:
    std::string orient = vol.referential().orientationStr( orient_v );
    \endcode

  \b Flipping

  To flip a volume to a different orientation, we may use:

  \code
  Volume<float> vol( 10, 10, 10 ); // volume in LPI orientation convention
  vol.flipToOrientation( "ASR" );
  \endcode
  After this operation we will use different indices: vol(z, y, x).

  However voxels are left untouched in this operation: only strides have been modified.

  If we want to actually copy voxels in a given orientation, we should use:
  \code
  Volume<float> vol( 10, 10, 10 ); // volume in LPI orientation convention
  vol.flipToOrientation( "ASR", "ASR" );
  \endcode

  and it's possible to specify a memory layout different from the indices orientation:
  \code
  Volume<float> vol( 10, 10, 10 ); // volume in LPI orientation convention
  vol.flipToOrientation( "ASR", "LIA" );
  \endcode
  Here indices will use the ASR orientation, but voxels will actually be in memory in LIA orientation.

  \subsection volume_orient_io Volume orientation and IO

  \b Writing

  Soma-IO and AIMS support writing non-LPI Volumes. This means that writing a Volume after any Volume::flipToOrientation() will have in the same result on disk.

  Some formats like NIFTI support to write voxels in any orientation. This can be controlled via the Volume header "storage_to_memory" transformation. It has to be appropriately set before writing the Volume. Note that this transformation is in voxels (int coefficients) and gives the transformation between disk voxels layout and the indices axes.

  \b Reading

  Most, if not all, formats will assume a Volume allocated in LPI orientation for indices. However the strides management will usually allow to read volumes in arbitrary memory layout orientation. When the IO system allocates the volume for reading, it is possible to specify via an option the memory layout to be used. In the options dictionary, the property "orientation" can specify it:
  \code
  Reader<Volume<float> > reader( "/tmp/volume.nii" );
  Object options = Object::value( PropertySet() );
  options->setProperty( "orientation", "tASR" );
  reader.setOptions( options );
  VolumeRef<float> vol( reader.read() );
  \endcode

  The "orientation" property value is a string, which uses the letters for axes orientation ("LPI", "RAST" etc). Optionally the value "storage" will specify that we want to use the same memory layout as the disk storage layout.

  Note that this specifies the memory layout, but the axes indices will still be LPI oriented. If you need another orientation, just use Volume::flipToOrientation() afterwards, as it is a lightweight, non-copy operation.

  As usual the orientation option may be passed as an URI option in the filename (see \ref cartovolumes_io)

  \see \ref cartovolumes_io
*/

/*! \page cartovolumes_io Volumes IO in CartoData

  Soma-IO provides a new IO system: soma::Reader, soma::Writer etc.
  Their implementation for volumes support partial reading/writing features
  (depending on the format).

  These partial IO may be speficied in options, or in the file name URL, in
  the following manner:

  \b Reading:

  "filename.nii.gz?ox=30&sx=50&oy=40&sy=100" reads a view in the NIFTI file filename.nii.gz, with the origin (in AIMS coordinates) at (30, 40, 0, 0),
  with the view size (50, 100, ?, ?) (? being the whole volume size).

  \b Writing:

  "filename.ima?partial_writing=1&ox=30&oy=40" will write the 
  volume in memory as a part of a larger, existing file, filename.ima, at
  position (30, 40, 0, 0) in it.

  To save a part of the volume in memory, you have to make a view in it (as a
  secondary volume) and save this view.

  Such URL syntax is supported in the Soma-IO system implementation for 
  volumes, so will be used for every format. Some formats may or may not use
  these options, however.

  In AIMS, Soma-IO is used alternately, and in addition to the former AIMS IO
  system, which does not support such URL options. So the result will actually
  depend on whether the IO format is implemented in the Soma-IO or the AIMS
  layer.

  For formats implemented in the Soma-IO layer, such options will be available
  with every reader/writer or progrem: anatomist, AimsFileConvert, or the
  AIMS python API (aims.read() etc).

  \b Allocation:

  The IO system (both in AIMS and Soma-IO) can make use of soma::AllocatorContext hints to allocate data. See the details in Soma-IO \ref reader_writer section.
*/
}
