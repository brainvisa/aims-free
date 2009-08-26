/* Copyright (c) 2009 CEA
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

#include <aims/plugin/giftiplugin.h>
#include <aims/io/giftimeshformat_d.h>
#include <aims/io/giftifinderformat.h>
#include <aims/io/finder.h>
#include <aims/mesh/surface.h>

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
  ext.push_back( "gii.gz" );

  GiftiMeshFormat<3, Void>   *df1 = new GiftiMeshFormat<3, Void>;
  FileFormatDictionary<AimsTimeSurface<3, Void> >::registerFormat( "GIFTI",
                                                                   df1, ext );

#if 0

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  NiftiFormat<char>     *df0 = new NiftiFormat<char>;
  FileFormatDictionary<AimsData<char> >::registerFormat( "NIFTI1", df0, ext,
      "SPM" );
  VolumeFormat<char>    *vf0 = new VolumeFormat<char>( "NIFTI1" );
  FileFormatDictionary<Volume<char> >::registerFormat( "NIFTI1", vf0, ext,
      "SPM" );
#endif

  NiftiFormat<uint8_t>  *df2 = new NiftiFormat<uint8_t>;
  FileFormatDictionary<AimsData<uint8_t> >::registerFormat( "NIFTI1", df2, ext,
      "SPM" );
  VolumeFormat<uint8_t> *vf2 = new VolumeFormat<uint8_t>( "NIFTI1" );
  FileFormatDictionary<Volume<uint8_t> >::registerFormat( "NIFTI1", vf2, ext,
      "SPM" );

  NiftiFormat<int16_t>  *df3 = new NiftiFormat<int16_t>;
  FileFormatDictionary<AimsData<int16_t> >::registerFormat( "NIFTI1", df3, ext,
      "SPM" );
  VolumeFormat<int16_t> *vf3 = new VolumeFormat<int16_t>( "NIFTI1" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "NIFTI1", vf3, ext,
      "SPM" );

  NiftiFormat<uint16_t> *df4 = new NiftiFormat<uint16_t>;
  FileFormatDictionary<AimsData<uint16_t> >::registerFormat( "NIFTI1", df4,
      ext, "SPM" );
  VolumeFormat<uint16_t>        *vf4 = new VolumeFormat<uint16_t>( "NIFTI1" );
  FileFormatDictionary<Volume<uint16_t> >::registerFormat( "NIFTI1", vf4, ext,
      "SPM" );

  NiftiFormat<int32_t>  *df5 = new NiftiFormat<int32_t>;
  FileFormatDictionary<AimsData<int32_t> >::registerFormat( "NIFTI1", df5,
      ext , "SPM");
  VolumeFormat<int32_t> *vf5 = new VolumeFormat<int32_t>( "NIFTI1" );
  FileFormatDictionary<Volume<int32_t> >::registerFormat( "NIFTI1", vf5, ext,
      "SPM" );

  NiftiFormat<uint32_t> *df6 = new NiftiFormat<uint32_t>;
  FileFormatDictionary<AimsData<uint32_t> >::registerFormat( "NIFTI1", df6,
      ext, "SPM" );
  VolumeFormat<uint32_t>        *vf6 = new VolumeFormat<uint32_t>( "NIFTI1" );
  FileFormatDictionary<Volume<uint32_t> >::registerFormat( "NIFTI1", vf6, ext,
      "SPM" );

  NiftiFormat<float>    *df7 = new NiftiFormat<float>;
  FileFormatDictionary<AimsData<float> >::registerFormat( "NIFTI1", df7, ext,
      "SPM" );
  VolumeFormat<float>   *vf7 = new VolumeFormat<float>( "NIFTI1" );
  FileFormatDictionary<Volume<float> >::registerFormat( "NIFTI1", vf7, ext,
      "SPM" );

  NiftiFormat<double>   *df8 = new NiftiFormat<double>;
  FileFormatDictionary<AimsData<double> >::registerFormat( "NIFTI1", df8, ext,
      "SPM" );
  VolumeFormat<double>  *vf8 = new VolumeFormat<double>( "NIFTI1" );
  FileFormatDictionary<Volume<double> >::registerFormat( "NIFTI1", vf8, ext,
      "SPM" );

#endif

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
