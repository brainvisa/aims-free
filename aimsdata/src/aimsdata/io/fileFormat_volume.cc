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
FileFormatDictionary<Volume<long> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<long> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<long>	*f = new VolumeRefFormat<long>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<unsigned long> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<unsigned long> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<unsigned long> 
    *f = new VolumeRefFormat<unsigned long>;
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
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<int8_t>	*gf = new GisFormat<int8_t>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<int8_t>	*vf = new VolumeFormat<int8_t>( "GIS" );
  FileFormatDictionary<Volume<int8_t> >::registerFormat( "GIS", vf, ext );

  ext.clear();
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<int8_t>	*sf = new SpmFormat<int8_t>;
  registerFormat( "SPM", sf, ext );
  vf = new VolumeFormat<int8_t>( "SPM" );
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
}

template<> void FileFormatDictionary<AimsData<uint8_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<uint8_t>	*gf = new GisFormat<uint8_t>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<uint8_t>	*vf = new VolumeFormat<uint8_t>( "GIS" );
  FileFormatDictionary<Volume<uint8_t> >::registerFormat( "GIS", vf, ext );

  ext.clear();
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<uint8_t>	*sf = new SpmFormat<uint8_t>;
  registerFormat( "SPM", sf, ext );
  vf = new VolumeFormat<uint8_t>( "SPM" );
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
}


// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template<> void FileFormatDictionary<AimsData<char> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<char>	*gf = new GisFormat<char>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<char>	*vf = new VolumeFormat<char>( "GIS" );
  FileFormatDictionary<Volume<char> >::registerFormat( "GIS", vf, ext );

  ext.clear();
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<char>	*sf = new SpmFormat<char>;
  registerFormat( "SPM", sf, ext );
  vf = new VolumeFormat<char>( "SPM" );
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
}
#endif


//	specializations for short & float access VIDA & ECAT formats
template<> void FileFormatDictionary<AimsData<int16_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<int16_t>	*gf = new GisFormat<int16_t>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<int16_t>	*vvf = new VolumeFormat<int16_t>( "GIS" );
  FileFormatDictionary<Volume<int16_t> >::registerFormat( "GIS", vvf, ext );

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
}

template<> void 
FileFormatDictionary<AimsData<ushort> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<uint16_t>	*gf = new GisFormat<uint16_t>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<uint16_t>	*vvf = new VolumeFormat<uint16_t>( "GIS" );
  FileFormatDictionary<Volume<uint16_t> >::registerFormat( "GIS", vvf, ext );

  ext.clear();
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<uint16_t>	*sf = new SpmFormat<uint16_t>;
  registerFormat( "SPM", sf, ext );
  vvf = new VolumeFormat<uint16_t>( "SPM" );
  FileFormatDictionary<Volume<uint16_t> >::registerFormat( "SPM", vvf, ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<uint16_t>	*df = new DicomFormat<uint16_t>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<uint16_t>( "DICOM" );
  FileFormatDictionary<Volume<uint16_t> >::registerFormat( "DICOM", vvf, ext );
#endif
}

template<> void FileFormatDictionary<AimsData<int32_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<int32_t>	*gf = new GisFormat<int32_t>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<int32_t>	*vvf = new VolumeFormat<int32_t>( "GIS" );
  FileFormatDictionary<Volume<int32_t> >::registerFormat( "GIS", vvf, ext );

  ext.clear();
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<int32_t>	*sf = new SpmFormat<int32_t>;
  registerFormat( "SPM", sf, ext );
  vvf = new VolumeFormat<int32_t>( "SPM" );
  FileFormatDictionary<Volume<int32_t> >::registerFormat( "SPM", vvf, ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<int32_t>	*df = new DicomFormat<int32_t>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<int32_t>( "DICOM" );
  FileFormatDictionary<Volume<int32_t> >::registerFormat( "DICOM", vvf, ext );
#endif
}

template<> void 
FileFormatDictionary<AimsData<uint32_t> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<uint32_t>	*gf = new GisFormat<uint32_t>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<uint32_t>	*vvf = new VolumeFormat<uint32_t>( "GIS" );
  FileFormatDictionary<Volume<uint32_t> >::registerFormat( "GIS", vvf, ext );

  ext.clear();
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<uint32_t>	*sf = new SpmFormat<uint32_t>;
  registerFormat( "SPM", sf, ext );
  vvf = new VolumeFormat<uint32_t>( "SPM" );
  FileFormatDictionary<Volume<uint32_t> >::registerFormat( "SPM", vvf, ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<uint32_t>	*df = new DicomFormat<uint32_t>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<uint32_t>( "DICOM" );
  FileFormatDictionary<Volume<uint32_t> >::registerFormat( "DICOM", vvf, ext );
#endif
}

// ### remove after everything has been moved to intN_t/uintN_t
template<> void FileFormatDictionary<AimsData<long> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<long>	*gf = new GisFormat<long>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<long>	*vvf = new VolumeFormat<long>( "GIS" );
  FileFormatDictionary<Volume<long> >::registerFormat( "GIS", vvf, ext );

  ext.clear();
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<long>	*sf = new SpmFormat<long>;
  registerFormat( "SPM", sf, ext );
  vvf = new VolumeFormat<long>( "SPM" );
  FileFormatDictionary<Volume<long> >::registerFormat( "SPM", vvf, ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<long>	*df = new DicomFormat<long>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<long>( "DICOM" );
  FileFormatDictionary<Volume<long> >::registerFormat( "DIcOM", vvf, ext );
#endif
}

// ### remove after everything has been moved to intN_t/uintN_t
template<> 
void FileFormatDictionary<AimsData<unsigned long> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<unsigned long>	*gf = new GisFormat<unsigned long>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<unsigned long>	*vvf
    = new VolumeFormat<unsigned long>( "GIS" );
  FileFormatDictionary<Volume<unsigned long> >::registerFormat( "GIS", vvf, 
                                                                ext );

  ext.clear();
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<unsigned long>	*sf = new SpmFormat<unsigned long>;
  registerFormat( "SPM", sf, ext );
  vvf = new VolumeFormat<unsigned long>( "SPM" );
  FileFormatDictionary<Volume<unsigned long> >::registerFormat( "SPM", vvf, 
                                                                ext );

#ifdef DICOM_LIB
  ext.clear();
  ext.push_back( "" );
  DicomFormat<unsigned long>	*df = new DicomFormat<unsigned long>;
  registerFormat( "DICOM", df, ext );
  vvf = new VolumeFormat<unsigned long>( "DICOM" );
  FileFormatDictionary<Volume<unsigned long> >::registerFormat( "DICOM", vvf, 
                                                                ext );
#endif
}

template<> void FileFormatDictionary<AimsData<float> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<float>	*gf = new GisFormat<float>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<float>	*vvf = new VolumeFormat<float>( "GIS" );
  FileFormatDictionary<Volume<float> >::registerFormat( "GIS", vvf, ext );

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
}

template<> void 
FileFormatDictionary<AimsData<double> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<double>	*gf = new GisFormat<double>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<double>	*vvf = new VolumeFormat<double>( "GIS" );
  FileFormatDictionary<Volume<double> >::registerFormat( "GIS", vvf, ext );

  ext.clear();
  ext.push_back( "img" );
  ext.push_back( "hdr" );
  SpmFormat<double>	*sf = new SpmFormat<double>;
  registerFormat( "SPM", sf, ext );
  vvf = new VolumeFormat<double>( "SPM" );
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
}

template<> void 
FileFormatDictionary<AimsData<cfloat> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<cfloat>	*gf = new GisFormat<cfloat>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<cfloat>	*vvf = new VolumeFormat<cfloat>( "GIS" );
  FileFormatDictionary<Volume<cfloat> >::registerFormat( "GIS", vvf, ext );
}

template<> void 
FileFormatDictionary<AimsData<cdouble> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<cdouble>	*gf = new GisFormat<cdouble>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<cdouble>	*vvf = new VolumeFormat<cdouble>( "GIS" );
  FileFormatDictionary<Volume<cdouble> >::registerFormat( "GIS", vvf, ext );
}

}

template class FileFormatDictionary<AimsData<int8_t> >;
template class FileFormatDictionary<AimsData<uint8_t> >;
template class FileFormatDictionary<AimsData<int16_t> >;
template class FileFormatDictionary<AimsData<uint16_t> >;
template class FileFormatDictionary<AimsData<int32_t> >;
template class FileFormatDictionary<AimsData<uint32_t> >;
template class FileFormatDictionary<AimsData<float> >;
template class FileFormatDictionary<AimsData<double> >;
template class FileFormatDictionary<AimsData<cfloat> >;
template class FileFormatDictionary<AimsData<cdouble> >;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class FileFormatDictionary<AimsData<char> >;
#endif
template class FileFormatDictionary<AimsData<long> >;
template class FileFormatDictionary<AimsData<unsigned long> >;


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
template class FileFormatDictionary<Volume<long> >;
template class FileFormatDictionary<Volume<unsigned long> >;

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
template class FileFormatDictionary<VolumeRef<long> >;
template class FileFormatDictionary<VolumeRef<unsigned long> >;

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
template class FileFormat<AimsData<float> >;
template class FileFormat<AimsData<double> >;
template class FileFormat<AimsData<cfloat> >;
template class FileFormat<AimsData<cdouble> >;

// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class FileFormat<AimsData<char> >;
#endif
template class FileFormat<AimsData<long> >;
template class FileFormat<AimsData<unsigned long> >;

template class FileFormat<Volume<int8_t> >;
template class FileFormat<Volume<uint8_t> >;
template class FileFormat<Volume<int16_t> >;
template class FileFormat<Volume<uint16_t> >;
template class FileFormat<Volume<int32_t> >;
template class FileFormat<Volume<uint32_t> >;
template class FileFormat<Volume<float> >;
template class FileFormat<Volume<double> >;
template class FileFormat<Volume<cfloat> >;
template class FileFormat<Volume<cdouble> >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class FileFormat<Volume<char> >;
#endif
template class FileFormat<Volume<long> >;
template class FileFormat<Volume<unsigned long> >;

template class VolumeFormat<int8_t>;
template class VolumeFormat<uint8_t>;
template class VolumeFormat<int16_t>;
template class VolumeFormat<uint16_t>;
template class VolumeFormat<int32_t>;
template class VolumeFormat<uint32_t>;
template class VolumeFormat<float>;
template class VolumeFormat<double>;
template class VolumeFormat<cfloat>;
template class VolumeFormat<cdouble>;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class VolumeFormat<char>;
#endif
template class VolumeFormat<long>;
template class VolumeFormat<unsigned long>;

template class FileFormat<VolumeRef<int8_t> >;
template class FileFormat<VolumeRef<uint8_t> >;
template class FileFormat<VolumeRef<int16_t> >;
template class FileFormat<VolumeRef<uint16_t> >;
template class FileFormat<VolumeRef<int32_t> >;
template class FileFormat<VolumeRef<uint32_t> >;
template class FileFormat<VolumeRef<float> >;
template class FileFormat<VolumeRef<double> >;
template class FileFormat<VolumeRef<cfloat> >;
template class FileFormat<VolumeRef<cdouble> >;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class FileFormat<VolumeRef<char> >;
#endif
template class FileFormat<VolumeRef<long> >;
template class FileFormat<VolumeRef<unsigned long> >;

template class VolumeRefFormat<int8_t>;
template class VolumeRefFormat<uint8_t>;
template class VolumeRefFormat<int16_t>;
template class VolumeRefFormat<uint16_t>;
template class VolumeRefFormat<int32_t>;
template class VolumeRefFormat<uint32_t>;
template class VolumeRefFormat<float>;
template class VolumeRefFormat<double>;
template class VolumeRefFormat<cfloat>;
template class VolumeRefFormat<cdouble>;
// ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
template class VolumeRefFormat<char>;
#endif
template class VolumeRefFormat<long>;
template class VolumeRefFormat<unsigned long>;


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
    FileFormatDictionary<AimsData<float> >::init();
    FileFormatDictionary<AimsData<double> >::init();
    FileFormatDictionary<AimsData<cfloat> >::init();
    FileFormatDictionary<AimsData<cdouble> >::init();

   // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
    FileFormatDictionary<AimsData<char> >::init();
#endif
    FileFormatDictionary<AimsData<long> >::init();
    FileFormatDictionary<AimsData<unsigned long> >::init();

    FileFormatDictionary<Volume<int8_t> >::init();
    FileFormatDictionary<Volume<uint8_t> >::init();
    FileFormatDictionary<Volume<int16_t> >::init();
    FileFormatDictionary<Volume<uint16_t> >::init();
    FileFormatDictionary<Volume<int32_t> >::init();
    FileFormatDictionary<Volume<uint32_t> >::init();
    FileFormatDictionary<Volume<float> >::init();
    FileFormatDictionary<Volume<double> >::init();
    FileFormatDictionary<Volume<cfloat> >::init();
    FileFormatDictionary<Volume<cdouble> >::init();

   // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
    FileFormatDictionary<Volume<char> >::init();
#endif
    FileFormatDictionary<Volume<long> >::init();
    FileFormatDictionary<Volume<unsigned long> >::init();

    FileFormatDictionary<VolumeRef<int8_t> >::init();
    FileFormatDictionary<VolumeRef<uint8_t> >::init();
    FileFormatDictionary<VolumeRef<int16_t> >::init();
    FileFormatDictionary<VolumeRef<uint16_t> >::init();
    FileFormatDictionary<VolumeRef<int32_t> >::init();
    FileFormatDictionary<VolumeRef<uint32_t> >::init();
    FileFormatDictionary<VolumeRef<float> >::init();
    FileFormatDictionary<VolumeRef<double> >::init();
    FileFormatDictionary<VolumeRef<cfloat> >::init();
    FileFormatDictionary<VolumeRef<cdouble> >::init();

   // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
    FileFormatDictionary<VolumeRef<char> >::init();
#endif
    FileFormatDictionary<VolumeRef<long> >::init();
    FileFormatDictionary<VolumeRef<unsigned long> >::init();

    return true;
  }

  static bool voldic = _voldic();
}
