#ifndef CARTODATA_IO_VOLUMEFORMATWRITER_H
#define CARTODATA_IO_VOLUMEFORMATWRITER_H
//--- soma io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/datasource/datasource.h>
#include <soma-io/image/imagewriter.h>
#include <soma-io/writer/formatwriter.h>
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
  //   W R I T E R   O F   V O L U M E
  //==========================================================================
  /// FormatWriter specialized for 4D Volume.
  /// 
  /// A such VolumeFormatWriter needs to be linked to a specialized 
  /// ImageWriter before being registered into a FormatDictionary.\n
  /// It understands the Volume and detects specific cases (borders, partial 
  /// reading, ...) before performing the writing through its linked 
  /// ImageWriter.\n
  /// Options may be given through the filename, at creation of the Reader.
  /// \see Reader FileUtil
  /// \note Options currently available are : 
  /// - (bool) partial_writing : if view should be partially written into an
  ///          existing full volume.
  /// - (bool) byte_swapping : if data should be byte swapped (GIS).
  /// - (bool) ascii : if data should be written in ASCII (GIS).
  /// - (int) ox, oy, oz, ot : top left position of partial frame in the output 
  ///         file.
  template <typename T>
  class VolumeFormatWriter : public FormatWriter<carto::Volume<T> >
  {
    public:
      virtual ~VolumeFormatWriter();
      
      /// FormatWriter derived function
      /// It removes properties that must not be written.
      virtual bool filterProperties(carto::Object header,
                                    carto::Object options = carto::none());
      
      /// FormatWriter derived function
      /// This method understands a volume and calls appropriate ImageWriter
      /// methods
      virtual bool write( const carto::Volume<T> & obj, 
                          carto::rc_ptr<DataSourceInfo> dsi,
                          carto::Object options );
      
      /// Linking to a ImageWriter
      /// Allows us to declare only once the ImageWriter
      void attach( carto::rc_ptr<ImageWriter<T> > imw );
      
    protected:
      carto::rc_ptr<ImageWriter<T> > _imw;
  };
  
  //==========================================================================
  //   W R I T E R   O F   R E F E R E N C E   T O   V O L U M E
  //==========================================================================
  /// FormatWriter specialized for reference to 4D Volume.
  ///
  /// \see VolumeFormatWriter
  template<typename T>
  class VolumeRefFormatWriter : 
  public FormatWriter<carto::VolumeRef<T> >
  {
    public:
      virtual ~VolumeRefFormatWriter();

      /// FormatWriter derived function
      /// It removes properties that must not be written.
      virtual bool filterProperties(carto::Object header,
                                    carto::Object options = carto::none());
      
      /// FormatWriter derived function
      /// This method understands a volume and calls appropriate ImageWriter
      /// methods
      virtual bool write( const carto::VolumeRef<T> & obj, 
                          carto::rc_ptr<DataSourceInfo> dsi,
                          carto::Object options );
      
      /// Linking to a ImageWriter
      /// Allows us to declare only once the ImageWriter
      void attach( carto::rc_ptr<ImageWriter<T> > imw );
      
    protected:
      carto::rc_ptr<ImageWriter<T> > _imw;
  };

}
#endif
