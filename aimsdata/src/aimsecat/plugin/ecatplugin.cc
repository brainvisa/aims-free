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

// we don't want to issue a warning
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/plugin/ecatplugin.h>
#include <aims/io/ecatformat_d.h>
#include <aims/io/ecatfinderformat.h>
#include <aims/io/finder.h>
#include <aims/io/baseFormats_cartovolume.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  bool initecat()
  {
    new EcatPlugin;
    return true;
  }

  bool ecatinit __attribute__((unused)) = initecat();

}

EcatPlugin::EcatPlugin() : Plugin()
{
  vector<string>	ext;
  ext.push_back( "v" );
  ext.push_back( "i" );
  ext.push_back( "p" );
  EcatFormat<int16_t>	*df3 = new EcatFormat<int16_t>;
  FileFormatDictionary<AimsData<int16_t> >::registerFormat( "ECAT", df3, ext );
  VolumeFormat<int16_t>	*vf3 = new VolumeFormat<int16_t>( "ECAT" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "ECAT", vf3, ext );

  EcatFormat<float>	*df7 = new EcatFormat<float>;
  FileFormatDictionary<AimsData<float> >::registerFormat( "ECAT", df7, ext );
  VolumeFormat<float>	*vf7 = new VolumeFormat<float>( "ECAT" );
  FileFormatDictionary<Volume<float> >::registerFormat( "ECAT", vf7, ext );

  Finder::registerFormat( "ECAT", new FinderEcatFormat, ext );
}


EcatPlugin::~EcatPlugin()
{
}


string EcatPlugin::name() const
{
  return "ECAT IO";
}


bool EcatPlugin::noop()
{
  return true;
}
