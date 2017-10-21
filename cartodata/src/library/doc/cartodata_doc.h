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

  Use <a href="http://www.cmake.org/">CMake</a>, a cross-platform and open-source build system. More information on <a href="http://brainvisa.info/brainvisa-cmake/compile_existing.html">The BrainVISA-cmake build system</a>. See also <a href="http://brainvisa.info/web/infrastructure.html#sources">this page on BrainVISA website</a>.

  \section cartodata_maincontents Main contents

  - <b>\ref cartovolumes</b>
  - \ref cartovolumes_io : IO types and formats implementations on volumes

  <!--- <b><a href="changelog.html">Change Log</a>--></b>

*/


/*! \page cartovolumes Volumes concept philosophy

  Volumes rely on the allocator system of somaio: see \ref allocators.

  First, a Volume is built using any allocator method. This means a DataSource 
  and an AllocatorContext are provided to it. It can remain unallocated (no 
  data buffer) if it is not used directly, but its data values should not be 
  accessed until it is allocated.

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
*/
}
