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

/*
 *  IO classes
 */


/* this source is here to force instanciation of some of the most useful 
   Reader templates and specialize their default builtin formats */

#include <aims/io/fileFormat_d.h>
#include <aims/io/baseFormats_volume_d.h>
#include <aims/io/fileFormat_cartovolume_d.h>

using namespace aims;
using namespace carto;

namespace aims
{

template<> void
FileFormatDictionary<Volume<int8_t> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<int8_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<int8_t>	*f = new VolumeRefFormat<int8_t>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<uint8_t> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<uint8_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<uint8_t>	*f = new VolumeRefFormat<uint8_t>;
  registerFormat( "ALLVOLUMES", f, ext );
}

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template<> void 
FileFormatDictionary<Volume<char> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<char> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<char>	*f = new VolumeRefFormat<char>;
  registerFormat( "ALLVOLUMES", f, ext );
}
#endif

template<> void 
FileFormatDictionary<Volume<int16_t> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<int16_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<int16_t>	*f = new VolumeRefFormat<int16_t>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<uint16_t> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<uint16_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<uint16_t>	*f = new VolumeRefFormat<uint16_t>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<int32_t> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<int32_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<int32_t>	*f = new VolumeRefFormat<int32_t>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<uint32_t> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<uint32_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<uint32_t>	*f = new VolumeRefFormat<uint32_t>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<int64_t> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<int64_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<int64_t>	*f = new VolumeRefFormat<int64_t>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<uint64_t> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<uint64_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<uint64_t> 
    *f = new VolumeRefFormat<uint64_t>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<float> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<float> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<float>	*f = new VolumeRefFormat<float>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<double> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<double> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<double>	*f = new VolumeRefFormat<double>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<cfloat> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<cfloat> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<cfloat>	*f = new VolumeRefFormat<cfloat>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<cdouble> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<cdouble> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<cdouble>	*f = new VolumeRefFormat<cdouble>;
  registerFormat( "ALLVOLUMES", f, ext );
}


template<> void FileFormatDictionary<AimsData<int8_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<int8_t>	*sf = new SpmFormat<int8_t>;
  registerFormat( "SPM", sf, ext );
  VolumeFormat<int8_t> *vf = new VolumeFormat<int8_t>( "SPM" );
  FileFormatDictionary<Volume<int8_t> >::registerFormat( "SPM", vf, ext );

  ext.clear();
  ext.push_back( "" );
  GenesisFormat<int8_t>	*gnf = new GenesisFormat<int8_t>;
  registerFormat( "GENESIS", gnf, ext );
  vf = new VolumeFormat<int8_t>( "GENESIS" );
  FileFormatDictionary<Volume<int8_t> >::registerFormat( "GENESIS", vf, ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<int8_t>	*df = new DicomFormat<int8_t>;
  registerFormat( "DICOM", df, ext );
  vf = new VolumeFormat<int8_t>( "DICOM" );
  FileFormatDictionary<Volume<int8_t> >::registerFormat( "DICOM", vf, ext );
#endif

  ext.clear();
  ext.push_back( "" );
  SomaIOAimsDataFormat<int8_t>  *df = new SomaIOAimsDataFormat<int8_t>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  vf = new VolumeFormat<int8_t>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<int8_t> >::registerFormat( "SOMAIO_VOLUMES", 
                                                          vf, ext );
}

template<> void FileFormatDictionary<AimsData<uint8_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<uint8_t>	*sf = new SpmFormat<uint8_t>;
  registerFormat( "SPM", sf, ext );
  VolumeFormat<uint8_t> *vf = new VolumeFormat<uint8_t>( "SPM" );
  FileFormatDictionary<Volume<uint8_t> >::registerFormat( "SPM", vf, ext );

#ifdef AIMS_JPEG
  ext.clear();
  ext.push_back( "jpg" );
  JpegFormat<uint8_t>	*jf = new JpegFormat<uint8_t>;
  registerFormat( "JPEG", jf, ext );
  vf = new VolumeFormat<uint8_t>( "JPEG" );
  FileFormatDictionary<Volume<uint8_t> >::registerFormat( "JPEG", vf, ext );
#endif

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<uint8_t>	*df = new DicomFormat<uint8_t>;
  registerFormat( "DICOM", df, ext );
  vf = new VolumeFormat<uint8_t>( "DICOM" );
  FileFormatDictionary<Volume<uint8_t> >::registerFormat( "DICOM", vf, ext );
#endif

  ext.clear();
  ext.push_back( "" );
  SomaIOAimsDataFormat<uint8_t>  *df = new SomaIOAimsDataFormat<uint8_t>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  vf = new VolumeFormat<uint8_t>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<uint8_t> >::registerFormat( "SOMAIO_VOLUMES", 
                                                          vf, ext );
}


// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template<> void FileFormatDictionary<AimsData<char> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<char>	*sf = new SpmFormat<char>;
  registerFormat( "SPM", sf, ext );
  VolumeFormat<char> *vf = new VolumeFormat<char>( "SPM" );
  FileFormatDictionary<Volume<char> >::registerFormat( "SPM", vf, ext );

  ext.clear();
  ext.push_back( "" );
  GenesisFormat<char>	*gnf = new GenesisFormat<char>;
  registerFormat( "GENESIS", gnf, ext );
  vf = new VolumeFormat<char>( "GENESIS" );
  FileFormatDictionary<Volume<char> >::registerFormat( "GENESIS", vf, ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<char>	*df = new DicomFormat<char>;
  registerFormat( "DICOM", df, ext );
  vf = new VolumeFormat<char>( "DICOM" );
  FileFormatDictionary<Volume<char> >::registerFormat( "DICOM", vf, ext );
#endif

  ext.clear();
  ext.push_back( "" );
  SomaIOAimsDataFormat<char>  *df = new SomaIOAimsDataFormat<char>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  vf = new VolumeFormat<char>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<char> >::registerFormat( "SOMAIO_VOLUMES", 
                                                       vf, ext );
}
#endif


//	specializations for short & float access VIDA & ECAT formats
template<> void FileFormatDictionary<AimsData<int16_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  VolumeFormat<int16_t> *vvf;

#ifdef VIDA_LIB
  ext.clear();
  ext.push_back( "vimg" );
  ext.push_back( "vhdr" );
  ext.push_back( "vinfo" );
  VidaFormat<int16_t>	*vf = new VidaFormat<int16_t>;
  registerFormat( "VIDA", vf, ext );
  vvf = new VolumeFormat<int16_t>;
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "VIDA", vvf, ext );
#endif

  ext.clear();
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<int16_t>	*sf = new SpmFormat<int16_t>;
  registerFormat( "SPM", sf, ext );
  vvf = new VolumeFormat<int16_t>( "SPM" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "SPM", vvf, ext );

#ifdef ECAT_LIB
  ext.clear();
  ext.push_back( "v" );
  ext.push_back( "i" );
  EcatFormat<int16_t>	*ef = new EcatFormat<int16_t>;
  registerFormat( "ECAT", ef, ext );
  vvf = new VolumeFormat<int16_t>( "ECAT" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "ECAT", vvf, ext );
#endif

  ext.clear();
  ext.push_back( "" );
  GenesisFormat<int16_t>	*gnf = new GenesisFormat<int16_t>;
  registerFormat( "GENESIS", gnf, ext );
  vvf = new VolumeFormat<int16_t>( "GENESIS" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "GENESIS", vvf, 
                                                          ext );
#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<int16_t>	*df = new DicomFormat<int16_t>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<int16_t>( "DICOM" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "DICOM", vvf, ext );
#endif

  ext.clear();
  ext.push_back( "" );
  SomaIOAimsDataFormat<int16_t>  *df = new SomaIOAimsDataFormat<int16_t>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  vvf = new VolumeFormat<int16_t>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "SOMAIO_VOLUMES", 
                                                          vvf, ext );
}

template<> void 
FileFormatDictionary<AimsData<uint16_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<uint16_t>	*sf = new SpmFormat<uint16_t>;
  registerFormat( "SPM", sf, ext );
  VolumeFormat<uint16_t> *vvf = new VolumeFormat<uint16_t>( "SPM" );
  FileFormatDictionary<Volume<uint16_t> >::registerFormat( "SPM", vvf, ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<uint16_t>	*df = new DicomFormat<uint16_t>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<uint16_t>( "DICOM" );
  FileFormatDictionary<Volume<uint16_t> >::registerFormat( "DICOM", vvf, ext );
#endif

  ext.clear();
  ext.push_back( "" );
  SomaIOAimsDataFormat<uint16_t>  *df = new SomaIOAimsDataFormat<uint16_t>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  vvf = new VolumeFormat<uint16_t>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<uint16_t> >::registerFormat( "SOMAIO_VOLUMES", 
                                                           vvf, ext );
}

template<> void FileFormatDictionary<AimsData<int32_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<int32_t>	*sf = new SpmFormat<int32_t>;
  registerFormat( "SPM", sf, ext );
  VolumeFormat<int32_t> *vvf = new VolumeFormat<int32_t>( "SPM" );
  FileFormatDictionary<Volume<int32_t> >::registerFormat( "SPM", vvf, ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<int32_t>	*df = new DicomFormat<int32_t>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<int32_t>( "DICOM" );
  FileFormatDictionary<Volume<int32_t> >::registerFormat( "DICOM", vvf, ext );
#endif

  ext.clear();
  ext.push_back( "" );
  SomaIOAimsDataFormat<int32_t>  *df = new SomaIOAimsDataFormat<int32_t>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  vvf = new VolumeFormat<int32_t>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<int32_t> >::registerFormat( "SOMAIO_VOLUMES", 
                                                          vvf, ext );
}

template<> void 
FileFormatDictionary<AimsData<uint32_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<uint32_t>	*sf = new SpmFormat<uint32_t>;
  registerFormat( "SPM", sf, ext );
  VolumeFormat<uint32_t> *vvf = new VolumeFormat<uint32_t>( "SPM" );
  FileFormatDictionary<Volume<uint32_t> >::registerFormat( "SPM", vvf, ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<uint32_t>	*df = new DicomFormat<uint32_t>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<uint32_t>( "DICOM" );
  FileFormatDictionary<Volume<uint32_t> >::registerFormat( "DICOM", vvf, ext );
#endif

  ext.clear();
  ext.push_back( "" );
  SomaIOAimsDataFormat<uint32_t>  *df = new SomaIOAimsDataFormat<uint32_t>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  vvf = new VolumeFormat<uint32_t>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<uint32_t> >::registerFormat( "SOMAIO_VOLUMES", 
                                                           vvf, ext );
}

template<> void FileFormatDictionary<AimsData<int64_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<int64_t>	*sf = new SpmFormat<int64_t>;
  registerFormat( "SPM", sf, ext );
  VolumeFormat<int64_t> *vvf = new VolumeFormat<int64_t>( "SPM" );
  FileFormatDictionary<Volume<int64_t> >::registerFormat( "SPM", vvf, ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<int64_t>	*df = new DicomFormat<int64_t>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<int64_t>( "DICOM" );
  FileFormatDictionary<Volume<int64_t> >::registerFormat( "DIcOM", vvf, ext );
#endif

  ext.clear();
  ext.push_back( "" );
  SomaIOAimsDataFormat<int64_t>  *df = new SomaIOAimsDataFormat<int64_t>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  vvf = new VolumeFormat<int64_t>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<int64_t> >::registerFormat( "SOMAIO_VOLUMES", 
                                                          vvf, ext );
}

template<> 
void FileFormatDictionary<AimsData<uint64_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<uint64_t>	*sf = new SpmFormat<uint64_t>;
  registerFormat( "SPM", sf, ext );
  VolumeFormat<uint64_t> *vvf = new VolumeFormat<uint64_t>( "SPM" );
  FileFormatDictionary<Volume<uint64_t> >::registerFormat( "SPM", vvf, 
                                                                ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<uint64_t>	*df = new DicomFormat<uint64_t>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<uint64_t>( "DICOM" );
  FileFormatDictionary<Volume<uint64_t> >::registerFormat( "DICOM", vvf, 
                                                                ext );
#endif

  ext.clear();
  ext.push_back( "" );
  SomaIOAimsDataFormat<uint64_t>  *df = new SomaIOAimsDataFormat<uint64_t>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  vvf = new VolumeFormat<uint64_t>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<uint64_t> >::registerFormat( "SOMAIO_VOLUMES", 
                                                           vvf, ext );
}

template<> void FileFormatDictionary<AimsData<float> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  VolumeFormat<float> *vvf;

#ifdef VIDA_LIB
  ext.clear();
  ext.push_back( "vimg" );
  ext.push_back( "vhdr" );
  ext.push_back( "vinfo" );
  VidaFormat<float>	*vf = new VidaFormat<float>;
  registerFormat( "VIDA", vf, ext );
  vvf = new VolumeFormat<float>( "VIDA" );
  FileFormatDictionary<Volume<float> >::registerFormat( "VIDA", vvf, ext );
#endif

  ext.clear();
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<float>	*sf = new SpmFormat<float>;
  registerFormat( "SPM", sf, ext );
  vvf = new VolumeFormat<float>( "SPM" );
  FileFormatDictionary<Volume<float> >::registerFormat( "SPM", vvf, ext );

#ifdef ECAT_LIB
  ext.clear();
  ext.push_back( "v" );
  ext.push_back( "i" );
  EcatFormat<float>	*ef = new EcatFormat<float>;
  registerFormat( "ECAT", ef, ext );
  vvf = new VolumeFormat<float>( "ECAT" );
  FileFormatDictionary<Volume<float> >::registerFormat( "ECAT", vvf, ext );
#endif

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<float>	*df = new DicomFormat<float>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<float>( "DICOM" );
  FileFormatDictionary<Volume<float> >::registerFormat( "DICOM", vvf, ext );
#endif

  ext.clear();
  ext.push_back( "" );
  SomaIOAimsDataFormat<float>  *df = new SomaIOAimsDataFormat<float>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  vvf = new VolumeFormat<float>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<float> >::registerFormat( "SOMAIO_VOLUMES", 
                                                        vvf, ext );
}

template<> void 
FileFormatDictionary<AimsData<double> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<double>	*sf = new SpmFormat<double>;
  registerFormat( "SPM", sf, ext );
  VolumeFormat<double> *vvf = new VolumeFormat<double>( "SPM" );
  FileFormatDictionary<Volume<double> >::registerFormat( "SPM", vvf, ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<double>	*df = new DicomFormat<double>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<double>( "DICOM" );
  FileFormatDictionary<Volume<double> >::registerFormat( "DICOM", vvf, ext );
#endif

  ext.clear();
  ext.push_back( "imas" );
  ImasVolFormat<double>     *svf = new ImasVolFormat<double>;
  registerFormat( "IMASPARSE", svf, ext );
  vvf = new VolumeFormat<double>( "IMASPARSE" );
  FileFormatDictionary<Volume<double> >::registerFormat( "IMASPARSE", vvf, 
                                                         ext );

  ext.clear();
  ext.push_back( "" );
  SomaIOAimsDataFormat<double>  *df = new SomaIOAimsDataFormat<double>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  vvf = new VolumeFormat<double>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<double> >::registerFormat( "SOMAIO_VOLUMES", 
                                                         vvf, ext );
}

template<> void 
FileFormatDictionary<AimsData<cfloat> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  SomaIOAimsDataFormat<cfloat>  *df = new SomaIOAimsDataFormat<cfloat>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  VolumeFormat<cfloat> *vvf = new VolumeFormat<cfloat>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<cfloat> >::registerFormat( "SOMAIO_VOLUMES", 
                                                         vvf, ext );
}

template<> void 
FileFormatDictionary<AimsData<cdouble> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  SomaIOAimsDataFormat<cdouble>  *df = new SomaIOAimsDataFormat<cdouble>;
  registerFormat( "SOMAIO_VOLUMES", df, ext );
  VolumeFormat<cdouble> *vvf = new VolumeFormat<cdouble>( "SOMAIO_VOLUMES" );
  FileFormatDictionary<Volume<cdouble> >::registerFormat( "SOMAIO_VOLUMES", 
                                                          vvf, ext );
}


template class FileFormatDictionary<AimsData<int8_t> >;
template class FileFormatDictionary<AimsData<uint8_t> >;
template class FileFormatDictionary<AimsData<int16_t> >;
template class FileFormatDictionary<AimsData<uint16_t> >;
template class FileFormatDictionary<AimsData<int32_t> >;
template class FileFormatDictionary<AimsData<uint32_t> >;
template class FileFormatDictionary<AimsData<int64_t> >;
template class FileFormatDictionary<AimsData<uint64_t> >;
template class FileFormatDictionary<AimsData<float> >;
template class FileFormatDictionary<AimsData<double> >;
template class FileFormatDictionary<AimsData<cfloat> >;
template class FileFormatDictionary<AimsData<cdouble> >;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class FileFormatDictionary<AimsData<char> >;
#endif


template class FileFormatDictionary<Volume<int8_t> >;
template class FileFormatDictionary<Volume<uint8_t> >;
template class FileFormatDictionary<Volume<int16_t> >;
template class FileFormatDictionary<Volume<uint16_t> >;
template class FileFormatDictionary<Volume<int32_t> >;
template class FileFormatDictionary<Volume<uint32_t> >;
template class FileFormatDictionary<Volume<float> >;
template class FileFormatDictionary<Volume<double> >;
template class FileFormatDictionary<Volume<cfloat> >;
template class FileFormatDictionary<Volume<cdouble> >;
template class FileFormatDictionary<Volume<int64_t> >;
template class FileFormatDictionary<Volume<uint64_t> >;

template class FileFormatDictionary<VolumeRef<int8_t> >;
template class FileFormatDictionary<VolumeRef<uint8_t> >;
template class FileFormatDictionary<VolumeRef<int16_t> >;
template class FileFormatDictionary<VolumeRef<uint16_t> >;
template class FileFormatDictionary<VolumeRef<int32_t> >;
template class FileFormatDictionary<VolumeRef<uint32_t> >;
template class FileFormatDictionary<VolumeRef<float> >;
template class FileFormatDictionary<VolumeRef<double> >;
template class FileFormatDictionary<VolumeRef<cfloat> >;
template class FileFormatDictionary<VolumeRef<cdouble> >;
template class FileFormatDictionary<VolumeRef<int64_t> >;
template class FileFormatDictionary<VolumeRef<uint64_t> >;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class FileFormatDictionary<Volume<char> >;
template class FileFormatDictionary<VolumeRef<char> >;
#endif


template class FileFormat<AimsData<int8_t> >;
template class FileFormat<AimsData<uint8_t> >;
template class FileFormat<AimsData<int16_t> >;
template class FileFormat<AimsData<uint16_t> >;
template class FileFormat<AimsData<int32_t> >;
template class FileFormat<AimsData<uint32_t> >;
template class FileFormat<AimsData<int64_t> >;
template class FileFormat<AimsData<uint64_t> >;
template class FileFormat<AimsData<float> >;
template class FileFormat<AimsData<double> >;
template class FileFormat<AimsData<cfloat> >;
template class FileFormat<AimsData<cdouble> >;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class FileFormat<AimsData<char> >;
#endif

template class FileFormat<Volume<int8_t> >;
template class FileFormat<Volume<uint8_t> >;
template class FileFormat<Volume<int16_t> >;
template class FileFormat<Volume<uint16_t> >;
template class FileFormat<Volume<int32_t> >;
template class FileFormat<Volume<uint32_t> >;
template class FileFormat<Volume<int64_t> >;
template class FileFormat<Volume<uint64_t> >;
template class FileFormat<Volume<float> >;
template class FileFormat<Volume<double> >;
template class FileFormat<Volume<cfloat> >;
template class FileFormat<Volume<cdouble> >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class FileFormat<Volume<char> >;
#endif

template class VolumeFormat<int8_t>;
template class VolumeFormat<uint8_t>;
template class VolumeFormat<int16_t>;
template class VolumeFormat<uint16_t>;
template class VolumeFormat<int32_t>;
template class VolumeFormat<uint32_t>;
template class VolumeFormat<int64_t>;
template class VolumeFormat<uint64_t>;
template class VolumeFormat<float>;
template class VolumeFormat<double>;
template class VolumeFormat<cfloat>;
template class VolumeFormat<cdouble>;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class VolumeFormat<char>;
#endif

template class FileFormat<VolumeRef<int8_t> >;
template class FileFormat<VolumeRef<uint8_t> >;
template class FileFormat<VolumeRef<int16_t> >;
template class FileFormat<VolumeRef<uint16_t> >;
template class FileFormat<VolumeRef<int32_t> >;
template class FileFormat<VolumeRef<uint32_t> >;
template class FileFormat<VolumeRef<int64_t> >;
template class FileFormat<VolumeRef<uint64_t> >;
template class FileFormat<VolumeRef<float> >;
template class FileFormat<VolumeRef<double> >;
template class FileFormat<VolumeRef<cfloat> >;
template class FileFormat<VolumeRef<cdouble> >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class FileFormat<VolumeRef<char> >;
#endif

template class VolumeRefFormat<int8_t>;
template class VolumeRefFormat<uint8_t>;
template class VolumeRefFormat<int16_t>;
template class VolumeRefFormat<uint16_t>;
template class VolumeRefFormat<int32_t>;
template class VolumeRefFormat<uint32_t>;
template class VolumeRefFormat<int64_t>;
template class VolumeRefFormat<uint64_t>;
template class VolumeRefFormat<float>;
template class VolumeRefFormat<double>;
template class VolumeRefFormat<cfloat>;
template class VolumeRefFormat<cdouble>;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class VolumeRefFormat<char>;
#endif

} // namespace aims


#if __GNUC__ < 3

// Why aren't those compiled in the standard library??
#include <std/complext.cc>
template ostream & operator << (ostream &, const complex<double> &);
template istream & operator >> (istream &, complex<float> &);
#endif

namespace
{
  bool _voldic()
  {
    FileFormatDictionary<AimsData<int8_t> >::init();
    FileFormatDictionary<AimsData<uint8_t> >::init();
    FileFormatDictionary<AimsData<int16_t> >::init();
    FileFormatDictionary<AimsData<uint16_t> >::init();
    FileFormatDictionary<AimsData<int32_t> >::init();
    FileFormatDictionary<AimsData<uint32_t> >::init();
    FileFormatDictionary<AimsData<int64_t> >::init();
    FileFormatDictionary<AimsData<uint64_t> >::init();
    FileFormatDictionary<AimsData<float> >::init();
    FileFormatDictionary<AimsData<double> >::init();
    FileFormatDictionary<AimsData<cfloat> >::init();
    FileFormatDictionary<AimsData<cdouble> >::init();

   // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
    FileFormatDictionary<AimsData<char> >::init();
#endif

    FileFormatDictionary<Volume<int8_t> >::init();
    FileFormatDictionary<Volume<uint8_t> >::init();
    FileFormatDictionary<Volume<int16_t> >::init();
    FileFormatDictionary<Volume<uint16_t> >::init();
    FileFormatDictionary<Volume<int32_t> >::init();
    FileFormatDictionary<Volume<uint32_t> >::init();
    FileFormatDictionary<Volume<int64_t> >::init();
    FileFormatDictionary<Volume<uint64_t> >::init();
    FileFormatDictionary<Volume<float> >::init();
    FileFormatDictionary<Volume<double> >::init();
    FileFormatDictionary<Volume<cfloat> >::init();
    FileFormatDictionary<Volume<cdouble> >::init();

   // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
    FileFormatDictionary<Volume<char> >::init();
#endif

    FileFormatDictionary<VolumeRef<int8_t> >::init();
    FileFormatDictionary<VolumeRef<uint8_t> >::init();
    FileFormatDictionary<VolumeRef<int16_t> >::init();
    FileFormatDictionary<VolumeRef<uint16_t> >::init();
    FileFormatDictionary<VolumeRef<int32_t> >::init();
    FileFormatDictionary<VolumeRef<uint32_t> >::init();
    FileFormatDictionary<VolumeRef<int64_t> >::init();
    FileFormatDictionary<VolumeRef<uint64_t> >::init();
    FileFormatDictionary<VolumeRef<float> >::init();
    FileFormatDictionary<VolumeRef<double> >::init();
    FileFormatDictionary<VolumeRef<cfloat> >::init();
    FileFormatDictionary<VolumeRef<cdouble> >::init();

   // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
    FileFormatDictionary<VolumeRef<char> >::init();
#endif

    return true;
  }

  static bool voldic __attribute__((unused)) = _voldic();
}
