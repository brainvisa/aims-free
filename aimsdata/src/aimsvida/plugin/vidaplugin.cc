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

#include <aims/plugin/vidaplugin.h>
#include <aims/io/vidaformat_d.h>
#include <aims/io/vidafinderformat.h>
#include <aims/io/finder.h>
#include <aims/rgb/rgb.h>
#include <aims/io/baseFormats_cartovolume.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  bool initvida()
  {
    new VidaPlugin;
    return true;
  }

  bool vidainit = initvida();

}

VidaPlugin::VidaPlugin() : Plugin()
{
  vector<string>	ext;
  ext.push_back( "vimg" );
  ext.push_back( "vhdr" );
  ext.push_back( "vinfo" );
  VidaFormat<int16_t>	*df3 = new VidaFormat<int16_t>;
  FileFormatDictionary<AimsData<int16_t> >::registerFormat( "VIDA", df3, ext );
  VolumeFormat<int16_t>	*vf3 = new VolumeFormat<int16_t>( "VIDA" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "VIDA", vf3, ext );

  VidaFormat<float>	*df7 = new VidaFormat<float>;
  FileFormatDictionary<AimsData<float> >::registerFormat( "VIDA", df7, ext );
  VolumeFormat<float>	*vf7 = new VolumeFormat<float>( "VIDA" );
  FileFormatDictionary<Volume<float> >::registerFormat( "VIDA", vf7, ext );

  Finder::registerFormat( "VIDA", new FinderVidaFormat, ext );
}


VidaPlugin::~VidaPlugin()
{
}


string VidaPlugin::name() const
{
  return "VIDA IO";
}


bool VidaPlugin::noop()
{
  return true;
}
