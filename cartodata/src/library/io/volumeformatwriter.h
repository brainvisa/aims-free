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
      virtual bool filterProperties(carto::Object header);
      
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
      virtual bool filterProperties(carto::Object header);
      
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
