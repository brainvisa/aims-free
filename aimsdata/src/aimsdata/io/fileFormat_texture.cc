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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  IO classes
 */

/* this source is only here to force instanciation of some of the most useful 
   Reader templates */

#include <aims/io/fileFormat_d.h>
#include <aims/io/baseFormats_texture_d.h>
#include <aims/io_soma/fscurvformatchecker.h>
#include <aims/io_soma/fsannotformatchecker.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>


using namespace aims;
using namespace std;
using namespace soma;

namespace aims
{

template<> void 
FileFormatDictionary<TimeTexture<float> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<float>	*fm = new TexFormat<float>;
  registerFormat( "TEX", fm, ext );
}

template<> void
FileFormatDictionary<TimeTexture<double> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "tex" );
  TexFormat<double>      *fm = new TexFormat<double>;
  registerFormat( "TEX", fm, ext );
}

template<> void 
FileFormatDictionary<TimeTexture<Point2df> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<Point2df>	*fm = new TexFormat<Point2df>;
  registerFormat( "TEX", fm, ext );
}

template<> void
FileFormatDictionary<TimeTexture<int8_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<int8_t>	*fm = new TexFormat<int8_t>;
  registerFormat( "TEX", fm, ext );
}

template<> void
FileFormatDictionary<TimeTexture<uint8_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<uint8_t>	*fm = new TexFormat<uint8_t>;
  registerFormat( "TEX", fm, ext );
}

template<> void
FileFormatDictionary<TimeTexture<int16_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<int16_t>	*fm = new TexFormat<int16_t>;
  registerFormat( "TEX", fm, ext );
}

template<> void
FileFormatDictionary<TimeTexture<uint16_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<uint16_t>	*fm = new TexFormat<uint16_t>;
  registerFormat( "TEX", fm, ext );
}

template<> void
FileFormatDictionary<TimeTexture<int32_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<int32_t>	*fm = new TexFormat<int32_t>;
  registerFormat( "TEX", fm, ext );
}

template<> void
FileFormatDictionary<TimeTexture<uint32_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<uint32_t>	*fm = new TexFormat<uint32_t>;
  registerFormat( "TEX", fm, ext );
}

template<> void
FileFormatDictionary<TimeTexture<int64_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<int64_t>	*fm = new TexFormat<int64_t>;
  registerFormat( "TEX", fm, ext );
}

template<> void
FileFormatDictionary<TimeTexture<uint64_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<uint64_t>	*fm = new TexFormat<uint64_t>;
  registerFormat( "TEX", fm, ext );
}

// template<> void
// FileFormatDictionary<TimeTexture<long> >::registerBaseFormats()
// {
//   std::vector<std::string>	ext;
//   ext.push_back( "tex" );
//   TexFormat<long>	*fm = new TexFormat<long>;
//   registerFormat( "TEX", fm, ext );
// }
//
// template<> void
// FileFormatDictionary<TimeTexture<unsigned long> >::registerBaseFormats()
// {
//   std::vector<std::string>	ext;
//   ext.push_back( "tex" );
//   TexFormat<unsigned long>	*fm = new TexFormat<unsigned long>;
//   registerFormat( "TEX", fm, ext );
// }


template<> void 
FileFormatDictionary<TimeTexture<Point2d> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<Point2d>	*fm = new TexFormat<Point2d>;
  registerFormat( "TEX", fm, ext );
}

template class FileFormatDictionary<TimeTexture<float> >;
template class FileFormatDictionary<TimeTexture<double> >;
template class FileFormatDictionary<TimeTexture<Point2df> >;
template class FileFormatDictionary<TimeTexture<int8_t> >;
template class FileFormatDictionary<TimeTexture<uint8_t> >;
template class FileFormatDictionary<TimeTexture<int16_t> >;
template class FileFormatDictionary<TimeTexture<uint16_t> >;
template class FileFormatDictionary<TimeTexture<int32_t> >;
template class FileFormatDictionary<TimeTexture<uint32_t> >;
template class FileFormatDictionary<TimeTexture<int64_t> >;
template class FileFormatDictionary<TimeTexture<uint64_t> >;
template class FileFormatDictionary<TimeTexture<Point2d> >;

template class FileFormat<TimeTexture<float> >;
template class FileFormat<TimeTexture<double> >;
template class FileFormat<TimeTexture<Point2df> >;
template class FileFormat<TimeTexture<int8_t> >;
template class FileFormat<TimeTexture<uint8_t> >;
template class FileFormat<TimeTexture<int16_t> >;
template class FileFormat<TimeTexture<uint16_t> >;
template class FileFormat<TimeTexture<int32_t> >;
template class FileFormat<TimeTexture<uint32_t> >;
template class FileFormat<TimeTexture<int64_t> >;
template class FileFormat<TimeTexture<uint64_t> >;
template class FileFormat<TimeTexture<Point2d> >;

} // namespace aims


static bool _texdic()
{
  FileFormatDictionary<TimeTexture<float> >::init();
  FileFormatDictionary<TimeTexture<double> >::init();
  FileFormatDictionary<TimeTexture<int16_t> >::init();
  FileFormatDictionary<TimeTexture<uint32_t> >::init();
  FileFormatDictionary<TimeTexture<int32_t> >::init();
  FileFormatDictionary<TimeTexture<Point2d> >::init();
  FileFormatDictionary<TimeTexture<Point2df> >::init();

  // register soma-io checker for the FSCURV format
  {
    vector<string>  exts(1);
    exts[0] = "curv";
    DataSourceInfoLoader::registerFormat( "FSCURV",
                                          new FsCurvFormatChecker, exts );
  }

  // register soma-io checker for the FSANNOT format
  {
    vector<string>  exts(1);
    exts[0] = "annot";
    DataSourceInfoLoader::registerFormat( "FSANNOT",
                                          new FsAnnotFormatChecker, exts );
  }

  return true;
}

static bool texdic __attribute__((unused)) = _texdic();


#include <soma-io/io/formatdictionary_d.h>
#include <soma-io/reader/formatreader_d.h>
#include <soma-io/io/reader_d.h>
#include <soma-io/io/writer_d.h>

namespace soma
{
  template class FormatReader<TimeTexture<float> >;
  template class FormatDictionary<TimeTexture<float> >;
  template class Reader<TimeTexture<float> >;
  template class Writer<TimeTexture<float> >;

  template class FormatReader<TimeTexture<int16_t> >;
  template class FormatDictionary<TimeTexture<int16_t> >;
  template class Reader<TimeTexture<int16_t> >;
  template class Writer<TimeTexture<int16_t> >;
}

