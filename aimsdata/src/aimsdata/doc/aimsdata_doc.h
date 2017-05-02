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


/*! \mainpage AIMS documentation

  \anchor aimsdata

  AIMS is constantly under changes. Several things are moving, or are being
  rewritten in a cleaner and more modern way into \ref cartobase or 
  \ref cartodata, our newer library for medical imaging structures. The IO
  system is in the process of being reimplemented in \ref somaio.

  A very incomplete <a href="../dev_doc/index.html">developer documentation can
  also be read here.</a>

  Some bits of <a href="../user_doc/index.html">user documentation can
  be found here.</a>

  \section aims_meaning What does AIMS mean ?

  Well, nobody really knows...
  Analyse d'IMages et de Signaux, perhaps ?


  \section aimsdata_provides Provided libraries and commands:

  - libraries: 
    - aimsdata library: structures and IO
    - aimsgui library: Qt widgets related to aimsdata structures
    - several IO plugins libraries: aimsdicom, aimsecat, aimsminc, 
      aimsqtformats, aimstiff, aimsvida
  - commandlines:
    - file information and conversion: \c AimsFileInfo, \c AimsFileConvert, 
      \c AimsGraphConvert, \c AimsSetMinf, \c AimsAttributedViewer
    - cut / cat / merge and other simple operations: \c AimsTCat, 
      \c AimsSubVolume, \c AimsOverVolume, \c AimsFlip, \c AimsMerge2RGB, 
      \c AimsSplitRGB, \c AimsGraphMerge, \c AimsMergeLabel
    - simple, basic processing: \c AimsThreshold, \c AimsAverage, 
      \c AimsMassCenter, \c AimsMeshArea
    - more specific conversions: \c AimsTex2Graph, \c AimsGraphComplete
    - coordinates transformations: \c AimsComposeTransformation, 
      \c AimsInvertTransformation, \c AimsGraphExtractTransformation
    - labels selection (ROI): \c AimsLabelSelector, \c AimsSelectLabel
    - other mesh operation: \c AimsMeshGenerate, \c AimsMeshCut
    - ...


  \section aimsdata_requirements Requirements

  AimsData now relies on \ref cartodata "CartoData" and on 
  \ref graph "Graph", and so needs their requirements.

  Additionally, some shared data may be needed by some commands (see the 
  \c shared project).

  To run correctly, at least one environment variables has to be set:
  - \c BRAINVISA_SHARE has to point to the global share directory
  - for backwards compatibility, if \c $BRAINVISA_SHARE is not set, then
    \c $SHFJ_SHARED_PATH is used instead. But it is deprecated, please do not
    use it any longer
  - normally \c BRAINVISA_SHARE is enough. But if AIMS-specific shared
    data are not located in the standard
    \c $BRAINVISA_SHARE/aims-\<aims_version\>
    or \c $BRAINVISA_SHARE/aims directory, then an additional \c AIMS_PATH
    variable may to point to AIMS shared directory.
  - In aims 3.2, if no variable is set, the share directory is searched from
    the \c $PATH environment variable.


  \section aimsdata_building Building:

  Use <a href="http://www.cmake.org/">CMake</a>, a cross-platform and open-source build system. More information on <a href="https://bioproj.extra.cea.fr/redmine/projects/brainvisa-devel/wiki/How_to_compile_BrainVISA_projects">this wiki page on Bioproj</a>, our Redmine based projects platform. To know more about Bioproj, see <a href="http://brainvisa.info/repository.html">this page on BrainVISA website</a>.


  \section aimsdata_maincontents Main contents:

  - namespace: aims
  - Data structures for:
    - volumes: AimsData (soon obsolete: see carto::Volume in cartodata)
    - meshes: AimsTimeSurfrace
    - "buckets" (voxels lists): AimsBucket and 
      \link aims::BucketMap BucketMap\endlink
    - textures: TimeTexture
    - fixed size vectors: AimsVector
    - transformations: Motion
  - \ref input_output, currently in the process of being rewritten in 
    soma-io: see \ref soma_io
  - commandline parsing: \link aims::AimsApplication AimsApplication\endlink
  - miscelaneous utilities: \c utility package and 
    \link carto::Converter Converter\endlink
    specializations for volumes, buckets, textures, RGB
  - IO plugins for several IO formats that use external libraries
  - common math algorithms: integration, LU decomposition, ...
  - aimsgui: Qt widgets for graphical interfaces related to AIMS objects
  - ...


  \section aimsdata_programming Other programming rules and notes:

  - <b>\ref programming</b>

  - <b><a href="../html/en/changelog.html">Change Log</a></b>

  - <b>To do list:</b> <a href="https://bioproj.extra.cea.fr/redmine/projects/brainvisa-aims/issues">issues in Bioproj</a>.

*/


/*! \page programming Programming principles

- to be written...

- \ref input_output

- \ref porting

- \ref api_changes

*/


/*! \page api_changes changes in classes and API

\ref genericobject (cartobase)

\ref aimsvolumes

New volumes: \ref cartovolumes (cartodata)

\section AimsData template types and IO

Most builtin C numeric types have variable sizes on different architectures 
or systems: for instance \c long is 32 bits on 32 bit systems, but 64 bits 
on 64 bit systems. It may even be dependent on some compilation flags.

There are some fixed size types defined in compilers: \c in32_t, \c uint16_t, 
etc. We now insist on using these types for AimsData:

\code
AimsData<int16_t>		data;
Reader<AimsData<int16_t> >	reader( "filename.ima" );
reader.read( data );
\endcode

Especially, <b>don't use \c long types anymore for any IO</b>.

*/




/*! \page aimsvolumes Volumes

AimsData will soon be deprecated. Don't use it anymore. 
carto::Volume will replace it. It is defined in a new library: cartodata.

In the meantime, AimsData will only be a wrapper to carto::Volume. This 
means carto::Volume is actually used behind the scene. AimsData wrapper is 
still provided for compatibility (and because it would have been too much work 
to eradicate it from our own codes), but should progressively disapear from 
all codes.

carto::Volume API will be accessible from a AimsData object so programs 
working with AimsData can switch to carto::Volume whenever they want.

In the other way, making an AimsData from a carto::Volume is trivial: just 
give it to AimsData constructor.

This way, compatibility is provided in both ways: old code will continue to 
work as is using AimsData, and new code using carto::Volume will be able to 
use old processing routines written for AimsData.

The native AimsData class (the old one, without a wrapper) has been replaced
by the wrapper (\<aims/data/cartodatavolume.h\>) in Aims 3.0.

In the future, other base classes (meshes, buckets, texture...) may move to a 
cleaner and more modern version into cartodata library.

<b>Things that must change soon (now!)</b>

A complete 100% compatibility is difficult to achieve (not to say impossible), 
so there will be a small number of behaviour change in AimsData. These affect 
mainly IO and copy operations:

- AimsData::setHeader() will now perform a copy of all attributes of the 
  given header, not just grab the pointer. This is needed by the underlying 
  carto::Volume header (which is builtin, not a changeable pointer). To keep 
  most compatibility with the former behaviour, giving a Header* to setHeader()
  will still transfer ownership of the header to the target AimsData. But 
  as the input header will not be kept at all, it will just be deleted 
  by setHeader(). This has two consequences:
  - a header is no longer usable after it has been passed to setHeader, 
    whereas it used to still live until the AimsData was deleted. So keep in 
    mind that you must perform all operations on the header before passing 
    it to the volume.
  - working on a header after it is passed to setHeader() will be a double 
    mistake: you will work on a deleted structure, and changes will not 
    affect at all the AimsData owning it.

- copying an AimsData will not copy its header anymore: the data block was 
  and will still be ref-counted, but now the data block is not separable 
  from its header anymore: they are tied in a single carto::Volume structure, 
  so the header will be shared just like the data. Changes done to the header 
  of a copied volume will also affect the original volume (unless a clone() 
  is performed instead of a regular copy).

*/




/*! \page input_output AIMS input / output system

There are 3 different layers of IO in Aims for virtually all objects:

- low-level readers / writers for each specific <b>file format</b>
- mid-level IO encapsulate \e all formats supported by Aims <b>for a given 
  object / data type</b> (\e ie read an AimsData<short> whatever the disk file 
  format): aims::Reader and aims::Writer template classes
- the highest level readers allow to switch to different pieces of code dealing
  with each object and data type (ie read an AimsData<short> or a 
  BucketMap<Void>): the aims::Process class

Normal programs perform IO at mid or high level, <i>never</i> at low-level: 
the lower layer is reserved to the specific usage of the mid-layer Reader and 
Writer classes for their internal purpose

\ref io_mid

\ref io_high

\ref io_adding

\section io_mid Mid-level IO: What to do to read a volume of short data ?

  \code
#include <cstdlib>
  #include <aims/io/reader.h>
  #include <aims/data/data.h>
  #include <stdexcept>

  using namespace aims;
  using namespace std;

  AimsData<short>			data;
  try
  {
    Reader<AimsData<short> >	r( "filename" );
    r.read( data );
  }
  catch( exception & e )	// the file couldn't be read
  {
    cerr << e.what() << endl;
  }
  \endcode

You may use an aims::Finder to read the file header and help you determine 
what's in the file:

  \code
  #include <aims/io/finder.h>

  Finder	f;
  if( f.check( "filename" ) )
  {
    if( f.objectType() == "Volume" )
    {
      if( f.dataType() == "S16" )
      {
        // ... (call previous function with Reader<AimsData<short> > )
      }
      else
        ...
    }
    else
      ...
  }
  \endcode

\section io_high High-level IO: My program has to read a file but doesn't know what's in it

Use a aims::Process class, you will avoid using a Finder by yourself and 
testing all possible types by hand: Process does it for you.

  \code
  #include <aims/io/process.h>
  #include <aims/io/reader.h>
  #include <aims/data/data.h>
  #include <stdexcept>

  using namespace aims;
  using namespace std;

  void myAimsData_short_function( Process &, const string &filename, Finder & )
  {
    // here I'm sure the file filename contains an AimsData<short>
    AimsData<short>			data;
    Reader<AimsData<short> >	r( filename );
    r.read( data );
    // ...
  }

  void myBucket_void_function( Process &, const string &filename, Finder & )
  {
    // here I'm sure the file filename contains a bucket<Void>
    // (either AimsBucket<Void> or BucketMap<Void>, both use the same files)
    BucketMap<Void>			data;
    Reader<BucketMap<Void> >	r( filename );
    r.read( data );
    // ...
  }

  int main( int argc, char** argv )
  {
    Process	p;
    string	filename = argv[1];

    p.registerProcessType( "Volume", "S16", &myAimsData_short_function );
    p.registerProcessType( "Bucket", "VOID", &myBucket_void_function );

    if( p.execute( filename ) )
      return EXIT_SUCCESS;
    return EXIT_FAILURE;
  }
  \endcode

  It's possible to use template functions as callbacks: see aims::Process 
  documentation for details

\section io_adding Adding new IO formats

It's the most complex part since there are several operations to perform. Here 
you have to know a little more of the mechanisms of Aims IO systems:

- A header system is responsible for reading the beginning of the file (or the 
  header file): this is the aims::Finder part, which purpose is to determine 
  what's in the input file: object type (volume, bucket, mesh etc), data type 
  (short: "S16", float: "FLOAT"...) and other information which may depend 
  on the objet/data type and/or the format.
- A reader system (aims::Reader) which actually reads the file and fills an 
  object with it. To do so the object must be already existing, ie you must 
  know its type.

- \ref finder
- \ref reader
- \ref io_add_operations

\subsection finder Finder
A aims::Finder object checks a file header. It uses a list of header reader 
classes and tries them one after another until a header reader  succeeds in 
reading the file header. There is one header reader for each file format: 
aims::FinderFormat classes which may in turn use a lower level Header class. 
This Header object may contain dynamic attributes (see aims::PythonHeader) and
is accessible via the Finder after it has been read

\subsection reader Reader / Writer
aims::Reader is a template for each object/data type. Its role is to read a 
data file into an object in a format-independant way. To do so, Reader has 
a list of specific format readers which can be tried one after another until 
one of them "understands" the file and is able to read it. These 
readers/writers are subclasses of aims::FileFormat and are generally template 
classes (because there must be one for each data type). These FileFormat 
objects are stored in a aims::FileFormatDictionary object and shared wy both 
Reader and Writer classes. There is one FileFormatDictionary for each 
object/data type (it's a template like Reader and Writer)

\subsection io_add_operations Operations to add an IO format
In the most general case these operations are:
- Finder (header) part:
  - write a header class with read (and write) function to read the file 
    header (one class per format, ex.: aims::JpegHeader)
  - wrap it in a aims::FinderFormat inherited class (ex.: 
    aims::FinderJpegFormat)
  - register it in the Finder class to allow Finder to recognize the new 
    file format: use the static aims::Finder::registerFormat() function
- Reader (body) part:
  - define the specific format reader and writer classes (see aims::JpegReader
    for an example) (usually template classes for all data types)
  - wrap them in a (a set of) aims::FileFormat inherited class(es) (ex.: 
    aims::JpegFormat)
  - compile the aims::FileFormatDictionary template on your type(s) and 
    eventually specialize the registerBaseFormats() function to register 
    default formats (you must <tt>#include <aims/io/fileFormat_d.h></tt>)
  - compile the Reader and Writer classes for your new objects/data types 
    (you must <tt>#include <aims/io/reader_d.h></tt> and 
    <tt>#include <aims/io/writer_d.h></tt>)

But:
- if you're adding a new format for already supported objects (ie your own 
  volume format to be read as AimsData), aims::Reader, aims::Writer and 
  aims::FileFormatDictionary classes are already compiled on the needed types:
  - write a header class with read the file header
  - wrap it in a aims::FinderFormat inherited class
  - register the FinderFormat in the Finder class
  - define the specific format reader and writer classes
  - wrap them in a (a set of) aims::FileFormat inherited class(es)
  - register them in already defined (and compiled) aims::FileFormatDictionary 
    instances: don't specialize the registerBaseFormats() function (because 
    it's already compiled in the library), but use the static registerFormat() 
    function
- if you want to compile and use existing IO formats for new data types:
  - (nothing to do for the Finder part)
  - specialize the aims::FinderFormatDictionary::registerBaseFormats() 
    functions and compile them
  - compile (instantiate) the specific aims::Reader and aims::Writer on your 
    type(s)

todo: code examples...
*/

