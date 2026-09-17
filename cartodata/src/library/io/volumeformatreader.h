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

      virtual FormatReader<carto::Volume<T> >* clone() const;

      virtual std::string formatID() const;

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

      virtual FormatReader<carto::VolumeRef<T> >* clone() const;
      virtual std::string formatID() const;

    protected:
      carto::rc_ptr<ImageReader<T> > _imr;
  };

}
#endif
