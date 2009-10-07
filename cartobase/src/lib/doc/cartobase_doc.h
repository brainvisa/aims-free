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

/*! \mainpage CartoBase documentation

  \anchor cartobase

  Cartograph - base package: support classes that don't deal with image 
  processing.


  \section cartobase_provides Provided libraries and commands:

  - cartobase library


  \section cartobase_requirements Requirements:

  The cartobase library now needs the following libraries installed on the 
  compiling system:
  - libsigc++ 2.0 or newer: http://sourceforge.net/projects/libsigc/
  - libxml2: http://xmlsoft.org/

  Optionally, the followng libraries may be used if available:
  - boost: http://boost.org for thread-safe reference counters


  \section cartobase_building Building:

  The use of our build system (build-config, configure, maker) is not mandatory
  but strongly encouraged, because it takes care of dependencies between 
  projects and libraries. Setting up the Makefiles by hand is quite a huge 
  work. For a little documentation on the build system, see our 
  web site: http://brainvisa.info/compilation.html


  \section cartobase_maincontents Main contents:

  - namespace: ::carto
  - generic objects: \link carto::Object Object\endlink, 
    \link carto::GenericObject GenericObject\endlink, 
    \link carto::ValueObject ValueObject\endlink, 
    \link carto::ReferenceObject ReferenceObject\endlink, and 
    standard interfaces: \link carto::Interface Interface\endlink \n
    See the \ref genericobject
  - exceptions: \link carto::io_error io_error\endlink, 
    \link carto::errno_error errno_error\endlink and families.
  - commandline options parsing system: \link carto::CartoApplication 
    CartoApplication\endlink, \link carto::OptionsParser OptionsParser\endlink
  - reference counters and other smart pointers: 
    \link carto::rc_ptr rc_ptr\endlink, std::auto_ptr, 
    \link carto::weak_ptr weak_ptr\endlink, 
    \link carto::scoped_ptr scoped_ptr\endlink
  - <b>\ref carto_io</b> (currently under development): 
    \link carto::DataSource DataSource\endlink, 
    \link carto::Reader Reader\endlink, \link carto::Writer Writer\endlink, 
    \link carto::DataSourceInfo DataSourceInfo\endlink
    \link carto::ReaderAlgorithm ReaderAlgorithm\endlink
  - other portable filename / file / directory utilities: 
    \link carto::FileUtil FileUtil\endlink, 
    \link carto::Directory Directory\endlink
  - <b>\ref allocators</b>
  - STL-like containers:
    - vectors using extended allocators: 
      \link carto::AllocatedVector AllocatedVector\endlink
    - \link carto::block block\endlink (buffers of fixed size)
  - plugins system: \link carto::Plugin Plugin\endlink, 
    \link carto::PluginManager PluginManager\endlink, 
    \link carto::PluginLoader PluginLoader\endlink
  - portable threads: \link carto::Thread Thread\endlink, 
    \link carto::Mutex Mutex\endlink, 
    \link carto::ThreadedLoop ThreadedLoop\endlink etc.
  - \link carto::UUID UUID\endlink: unique identifiers
  - libsigc++ observer patterns and protected includes (avoiding conflicts 
    with Qt)
  - future \link carto::Algorithm Algorithm\endlink class
  - type and conversion utilities:
    - type identifiers: \link carto::DataTypeCode DataTypeCode\endlink 
      template
    - \link carto::Converter Converter\endlink base template class
    - static inheritance test: ::SUPERSUBCLASS
    - backwards compatible std::numeric_limits in 
      \<cartobase/type/\ref limits.h\>
      and stringstreams: \<cartobase/stream/\ref sstream.h\>
    - string conversions: \link carto::toString toString\endlink, 
      \link carto::stringTo stringTo\endlink, 
      \link carto::split split\endlink, 
      \link carto::join join\endlink, 
      \link carto::stringLower stringLower\endlink, 
      \link carto::stringUpper stringUpper\endlink...
  - other general utility classes, functions and macros:
    - debug messages printing: ::dbgMsg, 
    - libraries information printing: \link carto::Info Info\endlink


  \section cartobase_programming Other programming rules and notes:

  - \ref porting

  - <b><a href="changelog.html">Change Log</a></b>

*/



/*! \page allocators Allocators system

  Allocators are handled by programmers via the AllocatorContext class. 
  This class holds information on how to allocate a (large) buffer. It is 
  prepared before the actual allocation, and at that time only a decision 
  will be made on how to allocate the buffer, depending on its memory size, 
  the available memory, the binary compatibility with a file to read...

  Allocation contexts holds the following information:

  - a DataSource: file, buffer, or none. This data source represents both the 
    file to read from and the reader system needed to get and convert data 
    to the internal memory format of the system. Alternately the data source 
    can be an already allocated memory buffer, or no source at all (which 
    will result in allocating an uninitialized buffer)
  - a status: allocated or unallocated
  - when determined, an actual LowLevelAllocator will be used. Currently 
    supported methods include:
    - Memory allocation
    - Direct, read-only memory mapping
    - Read/write memory mapping with copy of the whole DataSource
    - Direct read/write memory mapping (will modify the DataSource)
  - an access mode (read only, read/write, or internal modification): see 
    AllocatorStrategy::DataAccess

  As the allocator context has to check dynamically the amount of free RAM 
  and swap memory, and make a decision according to them each time it has to 
  allocate data, one allocation is rather slow. So this allocator system 
  should be reserved to large data blocks, and avoided to allocate many small 
  objects.

  If many small objects have to be allocated anyway, the allocation mode 
  may be forced by directly providing a LowLevelAllocator (generally the 
  MemoryAllocator::singleton()) to the allocator context.
*/



/*! \page genericobject Generic object API changes

There has been three generations of generic objects with three different 
interfaces. The last two have disapeared: they are not present anymore in
cartobase version 3.1, but still cited here for porting purpose.

- GenericObject API: this is the new API that should be used now.
  The generic object is separated into several interface (Interface) 
  API classes, each defining a set of virtual functions. The main interfaces 
  are directly inherited by GenericObject: DictionaryInterface, 
  StringInterface, ScalarInterface, ArrayInterface, SizeInterface, 
  DictionaryIteratorInterface, IterableInterface and, for compatibility, 
  GenObject.

  Some functions of DictionaryInterface replace older ones from the
  former GenObject and AttributedObject APIs:
  - \c getProperty() has several variants, either using a template (typed) 
    parameter, either using a generic Object
  - \c setProperty() with several variants (same as for getProperty() )
  - \c hasProperty()
  - \c removeProperty()
  - \c copyProperties()
  - \c clearProperties()

- old AttributedObject era and API:
  - \c getAttribute() is replaced by <tt>getProperty( string, T )</tt>
  - \c setAttribute() is replaced by <tt>setProperty( string, T )</tt>
  - \c hasAttribute() is replaced by \c hasProperty()
  - \c deleteAttribute() is replaced by \c removeProperty()
  - \c copyAttributes() is replaced by \c copyProperties()
  - \c deleteAttributes() is replaced by \c clearProperties()
  - \c getAttributes() has no equivalent in the new API but is not really 
    needed
  - \c check() has not been reimplemented but was used only in IO

  \link carto::AttributedObject AttributedObject\endlink is now a typedef to 
  SyntaxedObject<Dictionary>

- old GenObject era and API:
  - <tt>operator []</tt> is not provided anymore because of the
    ambiguities it could cause. Use \c getProperty() instead.
  - \c count() is now \c size() (SizeInterface)
  - \c getNumber() is now \c getScalar() (ScalarInterface)
  - \c keys() has not been reimplemented
  - Object had a template constructor which caused some ambiguities 
    for subclasses of GenericObject and GenObject, so it will be 
    removed. Object construction is now handled by two "factory" functions:
    Object::value( T ) and Object::reference( T )
  - other functions of GenObject are still found in various Interface 
    subclasses

  Once again, GenObject and AttributedObject don't exist anymore.

*/

}



/*! \page porting Keeping Portability

<b>A few tips for portable programming :</b>

It's a list of little things

- always use \c std:: in .h files before all classes of the standard namespace:
	- \c string
	- \c streams:
		- \c istream
		- \c ostream
		- \c ifstream
		- \c ofstream
		- \c cout
		- \c cerr
		- \c cin
		- \c streambuf
		- ...
	- \c iomanips:
		- \c ios
		- \c setw
		- \c setprec
		- ...
	- <b>STL</b> classes:
		- \c vector
		- \c set
		- \c map
		- \c multimap
		- ...


- explicitly use the namespace \c std in .cc files:

\code
using namespace std;
\endcode
#include <cstdlib>
  (put it after the \c #include lines)
  -> no need to use \c std:: everywhere in .cc files


- avoid writing <tt>"using something"</tt> in a .h (it's not a portability 
  issue but a potential cause of conflicts)


- when standard headers exist in both forms \c \<something.h\> and 
  \c \<something\>, always use the \c \<something\> version (without .h):

\code
#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>   // and NOT <algo.h> !
\endcode

  etc...
  Using *.h variants causes conflicts in modern compilers (gcc 3 and later), 
  and even with older ones (egcs and gcc 2.95) in some conditions 
  (\c vector\<bool\> for instance has diffent implementations in \c \<vector\> 
  and \c \<vector.h\> with distinct internal organization and objects sizes, 
  and mixing them is a source of ununderstandable crashes and bugs...)


- don't hesitate to include \c \<iostream\> as soon as \c cin or \c cout is 
  used in a source file, on some systems/compilers they are not always needed 
  but thay may not be defined when \c \<fstream\> is included and not 
  \c \<iostream\>


- do \b NOT use:

\code
#include <stl.h>
#include <algo.h> // -> use #include <algorithm>
\endcode


- do \b NOT use:

  \c MAXFLOAT etc

  but rather:

  \c FLT_MAX

  \c FLT_MAX is defined in \c \<float.h\>

  Or even better: use <tt>std::numeric_limits</tt>, which is part of standard 
  C++ since gcc 3.x, but also available in cartobase with older compilers:
\code
#include <cartobase/type/limits.h>
x = std::numeric_limits<float>::max();
\endcode

- do \b NOT use \c <values.h>

  Some systems don't provide it. Some (old) compilers do not provide 
  \c \<limits\> either. Instead, use the following:

  - \c \<cartobase/config/cartobase_config.h\> defines type limits macros on 
    integer types (\c INT32_MAX for instance) and includes \c \<limits.h\> 
    or equivalents in a portable way. It also defines \c isnan()

  - \c \<cartobase/type/limits.h\> includes in a portable way \c \<limits\> 
    (C++) or defines equivalents

- Always include \c \<cartobase/config/cartobase_config.h\> before using 
  file IO functions: cartobase config takes care of making use of 64 bit 
  file IO functions.

- do \b NOT re-use variable declarations from a loop to another one in the 
  same code block:

\code
for(int i=0; i<5; ++i)
  cout << "toto\n";
for(int i=0; i<10; ++i)
  cout << "tutu\n";
\endcode

but rather:

\code
int i;
for(i=0; i<5; ++i)
  cout << "toto\n";
for(i=0; i<10; ++i)
  cout << "tutu\n";
\endcode

  I know it should not cause any problem in principle, but even the Intel 
  compiler (which is not so bad in other situations) doesn't accept such 
  re-declarations. In any case, it's very simple to avoid, so please do it...


- do \b NOT use arrays allocated directly with a variable size:

\code
int	dim = getDim();
char	toto[ dim ];
\endcode

  I personnally liked such a convenient code, and it works well with gcc, 
  but other compilers don't allow it (it's simply not correct C). We have to 
  go back to more primitive forms (and far less convenient) such as:

\code
int	dim = getDim();
char	*toto = new char[ dim ];
// ... use the array ...
// be careful not to have a return or throw statement before delete[], 
// (otherwise there will be a memory leak...)
delete[] toto;
\endcode

  You can also use STL vectors, or a carto::block, instead in most situations:

\code
#include <vector>

int			dim = getDim();
std::vector<char>	toto( dim );
// utilisation
toto[12] = 5;
\endcode


- Avoid using a number of "dangerous" words which can be reserved keywords 
  in some specific compilers or systems, like for instance:

  - \c near, \c far (which are calling modes on Windows)
  - \c small, \c large


- non-portable functions, specific to Unix, etc...
  - all (or most) of \c <unistd.h> etc.
  - \c rint() is defined in \c \<cartobase/config/cartobase_config.h\>
  - \c max() is not defined in every compiler. But STL provides some templated 
    versions of it
  - directories manipulation fonctions: \c opendir(), \c closedir(), 
    \c readdir() - use carto::FileUtil and carto::Directory
  - \c mkdir( <i>name</i>, <i>access_rights</i> \c ) -> 
    \c mkdir( <i>name</i> \c ) 
    (on windows, no access rights, only 1 parameter). Use carto::Directory.
  - <i>named pipes</i> (which don't exist on older Windows)
  - \c isnan() is defined in \c \<cartobase/config/cartobase_config.h\>
  - <i>memory mapping</i>, the API is completely different on Windows:
    \c xxxFileMapping() functions: use cartobase allocators
  - ...


- some functions are present in different headers on different systems. When 
  we find such functions that may be useful, we wrap them in cartobase 
  header files.

*/
