/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL license version 2 under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#include <aims/plugin/qtformatsplugin.h>
#include <aims/io/qtformats_d.h>
#include <aims/io/qtfinderformats.h>
#include <aims/io/finder.h>
#include <aims/rgb/rgb.h>
#include <aims/def/path.h>
#include <cartobase/stream/fileutil.h>
#include <qapplication.h>
#include <qimage.h>
#include <aims/io/baseFormats_cartovolume.h>
#ifdef __APPLE__
#include <aims/io/macosxbugs.h>
#endif

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  /* This sets Qt plugins path for binary packages (actually needed only on 
     Mac/Fink Qt), according to an arbitrary path */
#ifdef __APPLE__
  bool initQtPlugins()
  {
    const Path		& p = Path::singleton();
    const string	& shared = p.globalShared();

    if( !shared.empty() )
      {
        char	sep = FileUtil::separator();
        string	base = FileUtil::dirname( shared ) + sep 
          + "lib" + sep + "qt3-plugins";

        QStringList	paths = QApplication::libraryPaths();
        if( paths.indexOf( base.c_str() ) == -1 )
          QApplication::addLibraryPath( base.c_str() );
      }
    return true;
  }
#endif

  bool initqtformats()
  {
#ifdef __APPLE__
    initQtPlugins();
#endif

    new QtFormatsPlugin;
    return true;
  }

  bool qtformatsinit __attribute__((unused)) = initqtformats();

}

QtFormatsPlugin::QtFormatsPlugin() : Plugin()
{
  set<QString>	fset;
  set<QString>::iterator	is, es = fset.end();

  QList<QByteArray>	iform = QImageReader::supportedImageFormats();
  QList<QByteArray>	oform = QImageWriter::supportedImageFormats();
  QList<QByteArray>::iterator	c, ce = iform.end();

  for( c=iform.begin(); c!=ce; ++c )
    fset.insert( QString( *c ).toUpper() );
  for( c=oform.begin(),ce=oform.end(); c!=ce; ++c )
    fset.insert( QString( *c ).toUpper() );

  QtFormats<int8_t>	*df1 = new QtFormats<int8_t>;
  QtFormats<uint8_t>	*df2 = new QtFormats<uint8_t>;
  QtFormats<int16_t>	*df3 = new QtFormats<int16_t>;
  QtFormats<uint16_t>	*df4 = new QtFormats<uint16_t>;
  QtFormats<int32_t>	*df5 = new QtFormats<int32_t>;
  QtFormats<uint32_t>	*df6 = new QtFormats<uint32_t>;
  QtFormats<AimsRGB>	*df8 = new QtFormats<AimsRGB>;
  QtFormats<AimsRGBA>	*df9 = new QtFormats<AimsRGBA>;
  vector<string>	exts;
  string		format = "QtFormats";

  // ### remove after everything has been moved to intN_t/uintN_t
  QtFormats<char>	*df10 = new QtFormats<char>;
  /* QtFormats<long>	*df11 = new QtFormats<long>;
  QtFormats<unsigned long>	*df12 = new QtFormats<unsigned long>; */

  for( is=fset.begin(); is!=es; ++is )
    {
      vector<string>	ext;
      const QString	& e = *is;
      string		fmt = e.toUpper().toStdString();

      if( fmt == "JPEG" )
        {
          fmt = "JPEG(Qt)";
          ext.push_back( "jpg" );
          ext.push_back( "jpeg" );
          exts.push_back( "jpg" );
        }
      else
        {
          if( fmt == "TIF" || fmt == "TIFF" )
            fmt = "TIFF(Qt)";
          ext.push_back( e.toLower().toStdString() );
          exts.push_back( e.toLower().toStdString() );
//           if( format == "QtFormats" )
//             format = fmt;
        }
      if( !FileFormatDictionary<carto::Volume<int8_t> >::fileFormat( fmt ) )
      {
#ifdef AIMS_APPLE_GCC33BUG
        macosxbugs::fileFormatDictionary_dataint8_registerFormat( fmt, df1,
          ext );
#else
        FileFormatDictionary<carto::Volume<int8_t> >::registerFormat( fmt, df1, ext );
#endif
        FileFormatDictionary<carto::VolumeRef<int8_t> >::registerFormat(
          fmt, new VolumeRefAimsFormat<int8_t>( fmt ), ext );
      }

      if( !FileFormatDictionary<carto::Volume<uint8_t> >::fileFormat( fmt ) )
      {
        FileFormatDictionary<carto::Volume<uint8_t> >::registerFormat(
          fmt, df2, ext );
        FileFormatDictionary<carto::VolumeRef<uint8_t> >::registerFormat(
          fmt, new VolumeRefAimsFormat<uint8_t>( fmt ), ext );
      }

      if( !FileFormatDictionary<carto::Volume<int16_t> >::fileFormat( fmt ) )
      {
        FileFormatDictionary<carto::Volume<int16_t> >::registerFormat(
          fmt, df3, ext );
        FileFormatDictionary<carto::VolumeRef<int16_t> >::registerFormat(
          fmt, new VolumeRefAimsFormat<int16_t>( fmt ), ext );
      }

      if( !FileFormatDictionary<carto::Volume<uint16_t> >::fileFormat( fmt ) )
      {
        FileFormatDictionary<carto::Volume<uint16_t> >::registerFormat(
          fmt, df4, ext );
        FileFormatDictionary<carto::VolumeRef<uint16_t> >::registerFormat(
          fmt, new VolumeRefAimsFormat<uint16_t>( fmt ), ext );
      }

      if( !FileFormatDictionary<carto::Volume<int32_t> >::fileFormat( fmt ) )
      {
        FileFormatDictionary<carto::Volume<int32_t> >::registerFormat(
          fmt, df5, ext );
        FileFormatDictionary<carto::VolumeRef<int32_t> >::registerFormat(
          fmt, new VolumeRefAimsFormat<int32_t>( fmt ), ext );
      }

      if( !FileFormatDictionary<carto::Volume<uint32_t> >::fileFormat( fmt ) )
      {
        FileFormatDictionary<carto::Volume<uint32_t> >::registerFormat(
          fmt, df6, ext );
        FileFormatDictionary<carto::VolumeRef<uint32_t> >::registerFormat(
          fmt, new VolumeRefAimsFormat<uint32_t>( fmt ), ext );
      }

      if( !FileFormatDictionary<carto::Volume<AimsRGB> >::fileFormat( fmt ) )
      {
#ifdef AIMS_APPLE_GCC33BUG
        macosxbugs::fileFormatDictionary_dataRGB_registerFormat( fmt, df8, ext );
#else
        FileFormatDictionary<carto::Volume<AimsRGB> >::registerFormat(
          fmt, df8, ext );
#endif
        FileFormatDictionary<carto::VolumeRef<AimsRGB> >::registerFormat(
          fmt, new VolumeRefAimsFormat<AimsRGB>( fmt ), ext );
      }

      if( !FileFormatDictionary<carto::Volume<AimsRGBA> >::fileFormat( fmt ) )
      {
        FileFormatDictionary<carto::Volume<AimsRGBA> >::registerFormat(
          fmt, df9, ext );
        FileFormatDictionary<carto::VolumeRef<AimsRGBA> >::registerFormat(
          fmt, new VolumeRefAimsFormat<AimsRGBA>( fmt ), ext );
      }

      if( !FileFormatDictionary<carto::Volume<int8_t> >::fileFormat( fmt ) )
      {
        // ### remove after everything has been moved to intN_t/uintN_t
        FileFormatDictionary<carto::Volume<char> >::registerFormat( fmt, df10, ext );
        FileFormatDictionary<carto::VolumeRef<char> >::registerFormat(
          fmt, new VolumeRefAimsFormat<char>( fmt ), ext );
      }
      Finder::registerFormat( fmt, new FinderQtFormats, ext );
    }
//   Finder::registerFormat( format, new FinderQtFormats, exts );
  QtFormatsHeader::qformatsMutex(); // force creating it now.
}


QtFormatsPlugin::~QtFormatsPlugin()
{
}


string QtFormatsPlugin::name() const
{
  return "Qt image formats IO";
}


bool QtFormatsPlugin::noop()
{
  return true;
}


