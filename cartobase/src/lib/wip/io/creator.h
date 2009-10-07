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

#ifndef CARTOBASE_IO_CREATOR_H
#define CARTOBASE_IO_CREATOR_H

#include <cartobase/object/object.h>

namespace carto
{
  class AllocatorContext;

  /** Object creation / resize or other setup factory

  Such factories are used by the IO system which has to create or setup new 
  objects according to information provided in the file header.

  Specific implementations for new object types may be specialized. It is 
  suggested that their ObjectCreator implementation is defined along with 
  the object definition itself, to avoid accidentally using the default 
  (non-specialized) creator somewhere (just like DataTypeCode).
  */
  template <typename T>
  class Creator
  {
  public:
    /** Create a new object according to the given header. 
        The default implementation just returns a new T.
    */
    static T* create( Object header, const AllocatorContext & context, 
                      Object options );
    /** Setup (modify) an existing object according to the given header
        (maybe resize it etc.).
        The default implementation does nothing.
    */
    static void setup( T & obj, Object header, 
                       const AllocatorContext & context, Object options );
  };


  template <typename T>
  inline
  T* Creator<T>::create( Object, const AllocatorContext &, Object )
  {
    return new T;
  }


  template <typename T>
  inline
  void Creator<T>::setup( T &, Object, const AllocatorContext &, Object )
  {
  }

}

#endif

