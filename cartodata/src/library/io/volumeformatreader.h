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

#ifndef CARTODATA_IO_VOLUMEFORMATREADER_H
#define CARTODATA_IO_VOLUMEFORMATREADER_H
//--- soma io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/datasource/datasource.h>
#include <soma-io/image/imagereader.h>
#include <soma-io/reader/formatreader.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>                           // using none()
#include <cartobase/smart/rcptr.h>                                   // member
//--- system -----------------------------------------------------------------
#include <vector>
//----------------------------------------------------------------------------

namespace carto
{
  template<typename T> class Volume;
  template<typename T> class VolumeRef;
}

namespace soma
{
  class DataSourceInfo;
  
  //==========================================================================
  //   R E A D E R   O F   V O L U M E
  //==========================================================================
  /// FormatReader specialized for 4D Volume.
  /// 
  /// A such VolumeFormatReader needs to be linked to a specialized 
  /// ImageReader before being registered into a FormatDictionary.\n
  /// It understands the Volume and detects specific cases (borders, partial 
  /// reading, ...) before performing the reading through its linked 
  /// ImageReader.\n
  /// The parameters for partial reading or borders may be given through the 
  /// filename, at creation of the Reader.
  /// \see Reader FileUtil
  /// \note Parameters currently availables (GIS or OpenSlide) are :
  /// - (int) ox, oy, oz, ot : top left position of partial frame.
  /// - (int) sx, sy, sz, st : size of partial frame.
  /// - (int) bx, by, bz : size of borders.
  /// \note Is also possible to give any classical option as : 
  /// - (int) resolution_level : level to read (OpenSlide).
  /// - (bool) unallocated : if volume shouldn't be allocated.
  /// - (bool) partial_reading : if view is an allocated partial view to an
  ///   unallocated volume (GIS or OpenSlide).
  template<typename T>
  class VolumeFormatReader : public FormatReader<carto::Volume<T> >
  {
    public:
      virtual ~VolumeFormatReader();
      
      //======================================================================
      //   N E W   M E T H O D S
      //======================================================================
      /// FormatReader derived function
      /// This method understands a volume and calls read( buffer ) with
      /// appropriate arguments
      virtual void read( carto::Volume<T> & obj, 
                         carto::rc_ptr<DataSourceInfo> dsi, 
                         const AllocatorContext & context, 
                         carto::Object options = carto::none() );

      /// Linking to a ImageReader
      /// Allows us to declare only once the ImageReader
      void attach( carto::rc_ptr<ImageReader<T> > imr );

      /// \brief Factory mode: creates an object and reads it.
      /// The returned object belongs to the calling layer and may be deleted
      /// by the standard \c delete
      virtual Volume<T>* createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                                        const AllocatorContext & context,
                                        carto::Object options );

      /// Full reading procedure, for an already existing object
      virtual void setupAndRead( Volume<T> & obj,
                                 carto::rc_ptr<DataSourceInfo> dsi,
                                 const AllocatorContext & context,
                                 carto::Object options );
    protected:
      carto::rc_ptr<ImageReader<T> > _imr;
  };
  
  //==========================================================================
  //   R E A D E R   O F   P O I N T E R   T O   V O L U M E
  //==========================================================================
  /// FormatReader specialized for reference to 4D Volume.
  ///
  /// \see VolumeFormatReader
  template<typename T>
  class VolumeRefFormatReader : public FormatReader<carto::VolumeRef<T> >
  {
    public:
      virtual ~VolumeRefFormatReader();

      //======================================================================
      //   N E W   M E T H O D S
      //======================================================================
      /// FormatReader derived function
      /// This method understands a volume and calls read( buffer ) with
      /// appropriate arguments
      virtual void read( carto::VolumeRef<T> & obj, 
                         carto::rc_ptr<DataSourceInfo> dsi, 
                         const AllocatorContext & context, 
                         carto::Object options = carto::none() );

      /// Linking to a ImageReader
      /// Allows us to declare only once the ImageReader
      void attach( carto::rc_ptr<ImageReader<T> > imr );
      
      /// \brief Factory mode: creates an object and reads it.
      /// The returned object belongs to the calling layer and may be deleted
      /// by the standard \c delete
      virtual VolumeRef<T>* createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                                        const AllocatorContext & context,
                                        carto::Object options );

      /// Full reading procedure, for an already existing object
      virtual void setupAndRead( VolumeRef<T> & obj,
                                 carto::rc_ptr<DataSourceInfo> dsi,
                                 const AllocatorContext & context,
                                 carto::Object options );

    protected:
      carto::rc_ptr<ImageReader<T> > _imr;
  };

}
#endif