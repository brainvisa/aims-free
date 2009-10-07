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

#ifndef CARTOBASE_IO_FORMATREADER_H
#define CARTOBASE_IO_FORMATREADER_H

#include <cartobase/smart/rcptr.h>

namespace carto
{
  class DataSource;
  class AllocatorContext;
  class Object;

  /** Low-level object IO reader specialized for a specific format

  To invoque such a format-specific reader, information about the source 
  is supposed te be already read, this is to say: the object header has 
  already been read (using DataSourceInfo).

  This is a low-level format reader, which is not intended to be used alone: 
  Reader will use it to achieve appropriate decoding, allocation, and reading.

  The job of a reader can be decomposed into several steps:
  - (the header has already been read by a FormatChecker)
  - determination of the correct stream decoder for the data block (DataSource 
    decoder)
  - determination of the correct allocator, matching with the stream decoder 
    characterstics: this means creating an AllocatorContext bound to the 
    specific DataSource decoder
  - allocation or setup (reallocation, resizing) of the object to be read, 
    according to reading options (partial reading of an already allocated 
    object may not require a reallocation...). This is performed using a 
    Creator.
  - reading of the data, or of a part of it, which can be done on demand, 
    directly via the DataSource once it is setup and plugged

  As a summary, a FormatReader job is to create and bind alltogether an 
  object to read, an allocation context, and a decoder DataSource.

  \todo
  Needs to be added:
  - options description (mandatory / optional / unrecognized)
  - capabilities (sub-object reading, direct/sequential access...), given 
    by the decoder DataSource or by the FormatChecker ?
  - generic options to allocate a new object corresponding to the datasource
    contents (allocate a volume with adhoc dimensions, with given borders...), 
    done by Creator
  */
  template<typename T>
  class FormatReader
  {
  public:
    virtual ~FormatReader();

    /// Full reading procedure, for an already existing object
    virtual void setupAndRead( T & obj, Object header, rc_ptr<DataSource> ds, 
                               const AllocatorContext & context, 
                               Object options );
    /** \brief Factory mode: creates an object and reads it.
        The returned object belongs to the calling layer and may be deleted by 
        the standard \c delete
    */
    virtual T* createAndRead( Object header, rc_ptr<DataSource> ds, 
                             const AllocatorContext & context, 
                             Object options );

    /** create and return a decoder DataSource.
        The default implementation just returns the \c source DataSource with 
        no decoding (raw data reading)
    */
    virtual rc_ptr<DataSource> getDataSource( Object header, 
                                              rc_ptr<DataSource> source, 
                                              Object options );
    /** setup an allocation context bound to the decoder DataSource.
        The default implementation just binds the \c decoder with the 
        \c basecontext
     */
    virtual AllocatorContext 
    getAllocatorContext( Object header, rc_ptr<DataSource> decoder, 
                         const AllocatorContext & basecontext, 
                         Object options );
    /** create the object to be read, bind the allocation context.
        Either create() or setup() will be called, depending if we are 
        working on an existing object or have to allocate a new one.
        The default implementations just calls Creator<T>::create().
     */
    virtual T* create( Object header, const AllocatorContext & context, 
                       Object options );
    /** setup an existing object (for resizing or reallocation for instance).
        Either create() or setup() will be called, depending if we are 
        working on an existing object or have to allocate a new one.
        The default implementations just calls Creator<T>::create().
     */
    virtual void setup( T & obj, Object header, 
                        const AllocatorContext & context, Object options );
    /** read part or all of the object \c obj.
        This read() method should be called after everything is setup and bound
        (the decoder DataSource, allocation context, object allocation/setup).
        For objects supporting allocators, \c context should be the same as 
        the context bound to the object, and reading is just a matter of 
        calling the read methods of the DataSource bound to the context.
        \param obj object to read into
        \param header may be already bound to \c obj
        \param context may be already bound to \c obj
        \param options may specify a sub-object for partial reading
     */
    virtual void read( T & obj, Object header, 
                       const AllocatorContext & context, Object options ) = 0;
  };

}

#endif

