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

#include <aims/plugin/fdfplugin.h>
#include <aims/io/fdfformat_d.h>
#include <aims/io/fdffinderformat.h>
#include <aims/io/finder.h>
#include <aims/io/baseFormats_cartovolume.h>
#ifdef __APPLE__
#include <aims/io/macosxbugs.h>
#endif

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  bool initfdf()
  {
    new FdfPlugin;
    return true;
  }

  bool fdfinit = initfdf();

}

FdfPlugin::FdfPlugin() : Plugin()
{
  vector<string>	ext;
  ext.push_back( ".fdf" );  ext.push_back( ".Fdf" );

  FdfFormat<int8_t>	*df1 = new FdfFormat<int8_t>;
#ifdef AIMS_APPLE_GCC33BUG
  macosxbugs::fileFormatDictionary_dataint8_registerFormat( "FDF", df1, ext );
#else
  FileFormatDictionary<AimsData<int8_t> >::registerFormat( "FDF", df1, ext );
#endif
  VolumeFormat<int8_t>	*vf1 = new VolumeFormat<int8_t>( "FDF" );
  FileFormatDictionary<Volume<int8_t> >::registerFormat( "FDF", vf1, ext );

  FdfFormat<uint8_t>	*df2 = new FdfFormat<uint8_t>;
  FileFormatDictionary<AimsData<uint8_t> >::registerFormat( "FDF", df2,
                                                            ext );
  VolumeFormat<uint8_t>	*vf2 = new VolumeFormat<uint8_t>( "FDF" );
  FileFormatDictionary<Volume<uint8_t> >::registerFormat( "FDF", vf2, ext );

  FdfFormat<int16_t>	*df3 = new FdfFormat<int16_t>;
  FileFormatDictionary<AimsData<int16_t> >::registerFormat( "FDF", df3,
                                                            ext );
  VolumeFormat<int16_t>	*vf3 = new VolumeFormat<int16_t>( "FDF" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "FDF", vf3, ext );

  FdfFormat<uint16_t>	*df4 = new FdfFormat<uint16_t>;
  FileFormatDictionary<AimsData<uint16_t> >::registerFormat( "FDF", df4,
                                                             ext );
  VolumeFormat<uint16_t>	*vf4 = new VolumeFormat<uint16_t>( "FDF" );
  FileFormatDictionary<Volume<uint16_t> >::registerFormat( "FDF", vf4, ext );

  FdfFormat<int32_t>	*df5 = new FdfFormat<int32_t>;
  FileFormatDictionary<AimsData<int32_t> >::registerFormat( "FDF", df5,
                                                            ext );
  VolumeFormat<int32_t>	*vf5 = new VolumeFormat<int32_t>( "FDF" );
  FileFormatDictionary<Volume<int32_t> >::registerFormat( "FDF", vf5, ext );

  FdfFormat<uint32_t>	*df6 = new FdfFormat<uint32_t>;
  FileFormatDictionary<AimsData<uint32_t> >::registerFormat( "FDF", df6,
                                                             ext );
  VolumeFormat<uint32_t>	*vf6 = new VolumeFormat<uint32_t>( "FDF" );
  FileFormatDictionary<Volume<uint32_t> >::registerFormat( "FDF", vf6, ext );

  FdfFormat<float>	*df7 = new FdfFormat<float>;
  FileFormatDictionary<AimsData<float> >::registerFormat( "FDF", df7, ext );
  VolumeFormat<float>	*vf7 = new VolumeFormat<float>( "FDF" );
  FileFormatDictionary<Volume<float> >::registerFormat( "FDF", vf7, ext );

  FdfFormat<double>	*df8 = new FdfFormat<double>;
  FileFormatDictionary<AimsData<double> >::registerFormat( "FDF", df8, ext );
  VolumeFormat<double>	*vf8 = new VolumeFormat<double>( "FDF" );
  FileFormatDictionary<Volume<double> >::registerFormat( "FDF", vf8, ext );

  Finder::registerFormat( "FDF", new FinderFdfFormat, ext );
}


FdfPlugin::~FdfPlugin()
{
}


string FdfPlugin::name() const
{
  return string("FDF reading");
}


bool FdfPlugin::noop()
{
  return true;
}

