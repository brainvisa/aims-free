/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
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
        if( paths.find( base.c_str() ) == paths.end() )
          QApplication::addLibraryPath( base.c_str() );
      }
    return true;
  }

  bool initqtformats()
  {
#ifdef __APPLE__
    initQtPlugins();
#endif

    new QtFormatsPlugin;
    return true;
  }

  bool qtformatsinit = initqtformats();

}

QtFormatsPlugin::QtFormatsPlugin() : Plugin()
{
  set<QString>	fset;
  set<QString>::iterator	is, es = fset.end();

#if QT_VERSION >= 0x040000
  QList<QByteArray>	iform = QImageReader::supportedImageFormats();
  QList<QByteArray>	oform = QImageWriter::supportedImageFormats();
  QList<QByteArray>::iterator	c, ce = iform.end();

  for( c=iform.begin(); c!=ce; ++c )
    fset.insert( QString( *c ).upper().utf8().data() );
  for( c=oform.begin(),ce=oform.end(); c!=ce; ++c )
    fset.insert( QString( *c ).upper().utf8().data() );

#else
  QStrList	formats = QImageIO::outputFormats();
  QStrList	iform = QImageIO::inputFormats();
  char		*c;

  for( c=iform.first(); c; c=iform.next() )
    fset.insert( c );
  for( c=formats.first(); c; c=formats.next() )
    fset.insert( c );
#endif

  QtFormats<int8_t>	*df1 = new QtFormats<int8_t>;
  QtFormats<uint8_t>	*df2 = new QtFormats<uint8_t>;
  /* QtFormats<int16_t>	*df3 = new QtFormats<int16_t>;
  QtFormats<uint16_t>	*df4 = new QtFormats<uint16_t>;
  QtFormats<int32_t>	*df5 = new QtFormats<int32_t>;
  QtFormats<uint32_t>	*df6 = new QtFormats<uint32_t>; */
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
      string		fmt = e.upper().utf8().data();

      if( fmt == "JPEG" )
        {
          fmt = "JPEG(Qt)";
          ext.push_back( "jpg" );
          exts.push_back( "jpg" );
        }
      else
        {
          ext.push_back( e.lower().utf8().data() );
          exts.push_back( e.lower().utf8().data() );
          if( format == "QtFormats" )
            format = fmt;
        }
#ifdef AIMS_APPLE_GCC33BUG
      macosxbugs::fileFormatDictionary_dataint8_registerFormat( fmt, df1,
          ext );
#else
      FileFormatDictionary<AimsData<int8_t> >::registerFormat( fmt, df1, ext );
#endif
  VolumeFormat<int8_t>	*vf1 = new VolumeFormat<int8_t>( fmt );
  FileFormatDictionary<Volume<int8_t> >::registerFormat( fmt, vf1, ext );

      FileFormatDictionary<AimsData<uint8_t> >::registerFormat( fmt, df2, 
                                                                ext );
  VolumeFormat<uint8_t>	*vf2 = new VolumeFormat<uint8_t>( fmt );
  FileFormatDictionary<Volume<uint8_t> >::registerFormat( fmt, vf2, ext );

#ifdef AIMS_APPLE_GCC33BUG
      macosxbugs::fileFormatDictionary_dataRGB_registerFormat( fmt, df8, ext );
#else
      FileFormatDictionary<AimsData<AimsRGB> >::registerFormat( fmt, df8, 
                                                                ext );
#endif
      VolumeFormat<AimsRGB>	*vf13 = new VolumeFormat<AimsRGB>( fmt );
      FileFormatDictionary<Volume<AimsRGB> >::registerFormat( fmt, vf13, 
                                                              ext );

      FileFormatDictionary<AimsData<AimsRGBA> >::registerFormat( fmt, df9, 
                                                                 ext );
      VolumeFormat<AimsRGBA>	*vf14 = new VolumeFormat<AimsRGBA>( fmt );
      FileFormatDictionary<Volume<AimsRGBA> >::registerFormat( fmt, vf14, 
                                                               ext );

      // ### remove after everything has been moved to intN_t/uintN_t
      FileFormatDictionary<AimsData<char> >::registerFormat( fmt, df10, ext );
      VolumeFormat<char>	*vf10 = new VolumeFormat<char>( fmt );
      FileFormatDictionary<Volume<char> >::registerFormat( fmt, vf10, ext );
    }
  Finder::registerFormat( format, new FinderQtFormats, exts );
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


