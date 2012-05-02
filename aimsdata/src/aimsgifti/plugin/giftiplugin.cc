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

#include <aims/plugin/giftiplugin.h>
#include <aims/io/giftimeshformat_d.h>
#include <aims/io/giftitextureformat_d.h>
#include <aims/io/giftifinderformat.h>
#include <aims/io/finder.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  bool initgifti()
  {
    new GiftiPlugin;
    return true;
  }

  bool giftiinit = initgifti();

}

GiftiPlugin::GiftiPlugin() : Plugin()
{
  vector<string> ext;
  ext.push_back( "gii" );

  GiftiMeshFormat<3, Void>   *df1 = new GiftiMeshFormat<3, Void>;
  FileFormatDictionary<AimsTimeSurface<3, Void> >::registerFormat( "GIFTI",
                                                                   df1, ext );

  GiftiTextureFormat<float> *df2 = new GiftiTextureFormat<float>;
  FileFormatDictionary<TimeTexture<float> >::registerFormat( "GIFTI",
                                                             df2, ext );

  GiftiTextureFormat<float> *df8 = new GiftiTextureFormat<double>;
  FileFormatDictionary<TimeTexture<double> >::registerFormat( "GIFTI",
                                                             df8, ext );

  GiftiTextureFormat<int16_t> *df3 = new GiftiTextureFormat<int16_t>;
  FileFormatDictionary<TimeTexture<int16_t> >::registerFormat( "GIFTI",
                                                               df3, ext );

  GiftiTextureFormat<int32_t> *df4 = new GiftiTextureFormat<int32_t>;
  FileFormatDictionary<TimeTexture<int32_t> >::registerFormat( "GIFTI",
                                                               df4, ext );

  GiftiTextureFormat<Point2df> *df5 = new GiftiTextureFormat<Point2df>;
  FileFormatDictionary<TimeTexture<Point2df> >::registerFormat( "GIFTI",
                                                               df5, ext );

  GiftiTextureFormat<uint32_t> *df6 = new GiftiTextureFormat<uint32_t>;
  FileFormatDictionary<TimeTexture<uint32_t> >::registerFormat( "GIFTI",
                                                                df6, ext );

  GiftiTextureFormat<Point2d> *df7 = new GiftiTextureFormat<Point2d>;
  FileFormatDictionary<TimeTexture<Point2d> >::registerFormat( "GIFTI",
                                                               df7, ext );

  Finder::registerFormat( "GIFTI", new FinderGiftiFormat, ext );
}


GiftiPlugin::~GiftiPlugin()
{
}


string GiftiPlugin::name() const
{
  return "GIFTI IO";
}


bool GiftiPlugin::noop()
{
  return true;
}
