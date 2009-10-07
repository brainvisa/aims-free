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

/*! \page carto_io IO system

\section datasource DataSource

In Cartograph, many objects loading and saving may be done not only on files, 
but in memory, or across a network, or across special transforming streams 
(compressed streams for instance).

DataSource is a file/stream abstraction layer, which may be compared to the 
C++ std::iostream classes, but more general and extensible. Actually DataSource
closely resembles Qt's QIODevice classes.

Specialized DataSource subclasses can be built to wrap different types of 
input/output styles: files (given by a file name), memory buffers, low-level 
file descriptors (including network sockets), C++ streams...

The idea here is that cartograph IO classes may provide a specialized 
DataSource which will decode a particular format and that will be attached to 
the object being read. This will allow lazy (partial) reading/writing.


\section datasourceinfo DataSourceInfo

Files and other data sources have to be analyzed to determine what kind of 
object/format is stored in them. This is the job of DataSourceInfo.

The main method of DataSourceInfo is check(): it tries to identify the 
data source and read a header from it.

DataSourceInfo is a modular and open system: different formats can be 
registered into it via FormatChecker subclasses. This system is designed to 
be used by external plugins.


\section reader_writer Reader and Writer

Reader and Writer are the main IO classes. They are template classes, this 
means that the object type to read (or write) should already be known before 
using a Reader / Writer.

Reader transparenlty handles (and hides) the file format selection. It uses 
a modular registration system for individual file formats, which may be used 
by external plugins: see FormatDictionary and IOObjectTypesDictionary.

Options can be passed to Reader and Writer when performing IO. Some are 
general (understood by the central Reader/Writer classes), but some may 
be specific to specialized formats. Such options may deal with ascii/binary 
saving modes, or with partial (sub-volume) reading/writing.

\todo Querying which options may apply to a given DataSource (ie: is partial 
reading implemented or not?)
\todo List of standard options, options checking...


\section readeralgo Upper-level: ReaderAlgo

A commandline has generally to deal with multiple possible data types, 
depending on what is contained in input files. ReaderAlgorithm provides 
a mechanism for switching to different algorithm variants depending on the 
type of what is read in a file (or any DataSource). It uses a type / callback 
registration mechanism.


\section partial_io Partial reading and writing

Partial IO is used via special Reader/Writer options: the \c "partial_reading" 
property of the Reader options should be set to \c true. In this case the 
object to read in is already allocated (no reallocation will be done in the 
reader), and only the allocated part is read.

*/

}

