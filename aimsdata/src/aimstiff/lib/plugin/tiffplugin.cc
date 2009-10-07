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

#include <aims/plugin/tiffplugin.h>
#include <aims/io/tiffformat_d.h>
#include <aims/io/tifffinderformat.h>
#include <aims/io/finder.h>
#include <aims/rgb/rgb.h>
#include <aims/io/baseFormats_cartovolume.h>
#ifdef __APPLE__
#include <aims/io/macosxbugs.h>
#endif

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  bool inittiff()
  {
    new TiffPlugin;
    return true;
  }

  bool tiffinit = inittiff();

}

TiffPlugin::TiffPlugin() : Plugin()
{
  vector<string>	ext;
  ext.push_back( "tif" ); ext.push_back( "Tif" );
  ext.push_back( "tiff" ); ext.push_back( "Tiff" );


  TiffFormat<uint8_t>  *tf8 = new TiffFormat<uint8_t>;
  FileFormatDictionary<AimsData<uint8_t> >::registerFormat( "TIFF", tf8,
                                                             ext );
  VolumeFormat<uint8_t> *vf8 = new VolumeFormat<uint8_t>( "TIFF" );
  FileFormatDictionary<Volume<uint8_t> >::registerFormat( "TIFF", vf8, ext );

  TiffFormat<int8_t>  *tfs8 = new TiffFormat<int8_t>;
  FileFormatDictionary<AimsData<int8_t> >::registerFormat( "TIFF", tfs8,
                                                           ext );
  VolumeFormat<int8_t> *vfs8 = new VolumeFormat<int8_t>( "TIFF" );
  FileFormatDictionary<Volume<int8_t> >::registerFormat( "TIFF", vfs8, ext );

  TiffFormat<uint16_t>	*df16 = new TiffFormat<uint16_t>;
  FileFormatDictionary<AimsData<uint16_t> >::registerFormat( "TIFF", df16, 
                                                             ext );
  VolumeFormat<uint16_t>	*vf4 = new VolumeFormat<uint16_t>( "TIFF" );
  FileFormatDictionary<Volume<uint16_t> >::registerFormat( "TIFF", vf4, ext );

  TiffFormat<int16_t>  *dfs16 = new TiffFormat<int16_t>;
  FileFormatDictionary<AimsData<int16_t> >::registerFormat( "TIFF", dfs16,
                                                             ext );
  VolumeFormat<int16_t>        *vfs4 = new VolumeFormat<int16_t>( "TIFF" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "TIFF", vfs4, ext );


  TiffFormat<uint32_t>	*df32 = new TiffFormat<uint32_t>;
  FileFormatDictionary<AimsData<uint32_t> >::registerFormat( "TIFF", df32, 
                                                             ext );
  VolumeFormat<uint32_t>	*vf5 = new VolumeFormat<uint32_t>( "TIFF" );
  FileFormatDictionary<Volume<uint32_t> >::registerFormat( "TIFF", vf5, ext );

  TiffFormat<int32_t>  *dfs32 = new TiffFormat<int32_t>;
  FileFormatDictionary<AimsData<int32_t> >::registerFormat( "TIFF", dfs32,
                                                             ext );
  VolumeFormat<int32_t>        *vfs5 = new VolumeFormat<int32_t>( "TIFF" );
  FileFormatDictionary<Volume<int32_t> >::registerFormat( "TIFF", vfs5, ext );

  TiffFormat<AimsRGB>	*df8 = new TiffFormat<AimsRGB>;
#ifdef AIMS_APPLE_GCC33BUG
  macosxbugs::fileFormatDictionary_dataRGB_registerFormat( "TIFF", df8, ext );
#else
  FileFormatDictionary<AimsData<AimsRGB> >::registerFormat( "TIFF", df8, ext );
#endif
  VolumeFormat<AimsRGB>	*vf13 = new VolumeFormat<AimsRGB>( "TIFF" );
  FileFormatDictionary<Volume<AimsRGB> >::registerFormat( "TIFF", vf13, ext );

  Finder::registerFormat( "TIFF", new FinderTiffFormat, ext );

  TiffFormat<AimsRGBA>	*df14 = new TiffFormat<AimsRGBA>;
  FileFormatDictionary<AimsData<AimsRGBA> >::registerFormat( "TIFF", df14, ext );
  VolumeFormat<AimsRGBA> *vf14 = new VolumeFormat<AimsRGBA>( "TIFF" );
  FileFormatDictionary<Volume<AimsRGBA> >::registerFormat( "TIFF", vf14, ext );

  TiffFormat<float>  *dffl = new TiffFormat<float>;
  FileFormatDictionary<AimsData<float> >::registerFormat( "TIFF", dffl,
                                                          ext );
  VolumeFormat<float>        *vffl = new VolumeFormat<float>( "TIFF" );
  FileFormatDictionary<Volume<float> >::registerFormat( "TIFF", vffl, ext );

  TiffFormat<double>  *dfdb = new TiffFormat<double>;
  FileFormatDictionary<AimsData<double> >::registerFormat( "TIFF", dfdb,
                                                           ext );
  VolumeFormat<double>        *vfdb = new VolumeFormat<double>( "TIFF" );
  FileFormatDictionary<Volume<double> >::registerFormat( "TIFF", vfdb, ext );

  TiffFormat<cfloat>  *dfcfl = new TiffFormat<cfloat>;
  FileFormatDictionary<AimsData<cfloat> >::registerFormat( "TIFF", dfcfl,
                                                          ext );
  VolumeFormat<cfloat>        *vfcfl = new VolumeFormat<cfloat>( "TIFF" );
  FileFormatDictionary<Volume<cfloat> >::registerFormat( "TIFF", vfcfl, ext );

  TiffFormat<cdouble>  *dfcdb = new TiffFormat<cdouble>;
  FileFormatDictionary<AimsData<cdouble> >::registerFormat( "TIFF", dfcdb,
                                                            ext );
  VolumeFormat<cdouble>        *vfcdb = new VolumeFormat<cdouble>( "TIFF" );
  FileFormatDictionary<Volume<cdouble> >::registerFormat( "TIFF", vfcdb, ext );

  Finder::registerFormat( "TIFF", new FinderTiffFormat, ext );
}


TiffPlugin::~TiffPlugin()
{
}


string TiffPlugin::name() const
{
  return "TIFF IO";
}


bool TiffPlugin::noop()
{
  return true;
}



