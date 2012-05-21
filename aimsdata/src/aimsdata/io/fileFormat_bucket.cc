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

/* this source is only here to force instanciation of some of the most useful 
   Reader templates */

#include <aims/io/fileFormat_d.h>
#include <aims/io/baseFormats_bucket_d.h>
#include <aims/io/dtitensorItemR.h>
#include <aims/io/dtitensorItemW.h>
#include <aims/io/itemR_stl.h>
#include <aims/io/itemW_stl.h>


using namespace aims;
using namespace std;

namespace aims
{

template<> void 
FileFormatDictionary<AimsBucket<Void> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "bck" );
  BckFormat<Void>	*fm = new BckFormat<Void>;
  registerFormat( "BCK", fm, ext );
}

template<> void 
FileFormatDictionary<BucketMap<Void> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "bck" );
  BckMapFormat<Void>	*fm = new BckMapFormat<Void>;
  registerFormat( "BCK", fm, ext );
}

template<> void 
FileFormatDictionary<AimsBucket<short> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "bck" );
  BckFormat<short>	*fm = new BckFormat<short>;
  registerFormat( "BCK", fm, ext );
}

template<> void 
FileFormatDictionary<BucketMap<short> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "bck" );
  BckMapFormat<short>	*fm = new BckMapFormat<short>;
  registerFormat( "BCK", fm, ext );
}

template<> void
FileFormatDictionary<AimsBucket<uint16_t> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "bck" );
  BckFormat<uint16_t>      *fm = new BckFormat<uint16_t>;
  registerFormat( "BCK", fm, ext );
}

template<> void
FileFormatDictionary<BucketMap<uint16_t> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "bck" );
  BckMapFormat<uint16_t>   *fm = new BckMapFormat<uint16_t>;
  registerFormat( "BCK", fm, ext );
}

template<> void
FileFormatDictionary<AimsBucket<int32_t> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "bck" );
  BckFormat<int32_t>      *fm = new BckFormat<int32_t>;
  registerFormat( "BCK", fm, ext );
}

template<> void
FileFormatDictionary<BucketMap<int32_t> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "bck" );
  BckMapFormat<int32_t>   *fm = new BckMapFormat<int32_t>;
  registerFormat( "BCK", fm, ext );
}

template<> void
FileFormatDictionary<AimsBucket<uint32_t> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "bck" );
  BckFormat<uint32_t>      *fm = new BckFormat<uint32_t>;
  registerFormat( "BCK", fm, ext );
}

template<> void
FileFormatDictionary<BucketMap<uint32_t> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "bck" );
  BckMapFormat<uint32_t>   *fm = new BckMapFormat<uint32_t>;
  registerFormat( "BCK", fm, ext );
}

template<> void 
FileFormatDictionary<AimsBucket<float> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "bck" );
  BckFormat<float>	*fm = new BckFormat<float>;
  registerFormat( "BCK", fm, ext );
}

template<> void 
FileFormatDictionary<BucketMap<float> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "bck" );
  BckMapFormat<float>	*fm = new BckMapFormat<float>;
  registerFormat( "BCK", fm, ext );
}

template<> void 
FileFormatDictionary<AimsBucket<double> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "bck" );
  BckFormat<double>	*fm = new BckFormat<double>;
  registerFormat( "BCK", fm, ext );
}

template<> void 
FileFormatDictionary<BucketMap<double> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "bck" );
  BckMapFormat<double>	*fm = new BckMapFormat<double>;
  registerFormat( "BCK", fm, ext );
}

template<> void 
FileFormatDictionary<AimsBucket<DtiTensor> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "bck" );
  BckFormat<DtiTensor>	*fm = new BckFormat<DtiTensor>;
  registerFormat( "BCK", fm, ext );
}

template<> void 
FileFormatDictionary<BucketMap<DtiTensor> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "bck" );
  BckMapFormat<DtiTensor>	*fm = new BckMapFormat<DtiTensor>;
  registerFormat( "BCK", fm, ext );
}

  template<> void 
  FileFormatDictionary<AimsBucket<vector<int8_t> > >::registerBaseFormats()
  {
    std::vector<std::string>	ext;
    ext.push_back( "bck" );
    BckFormat<vector<int8_t> >	*fm = new BckFormat<vector<int8_t> >;
    registerFormat( "BCK", fm, ext );
  }

  template<> void 
  FileFormatDictionary<AimsBucket<vector<uint8_t> > >::registerBaseFormats()
  {
    std::vector<std::string>	ext;
    ext.push_back( "bck" );
    BckFormat<vector<uint8_t> >	*fm = new BckFormat<vector<uint8_t> >;
    registerFormat( "BCK", fm, ext );
  }

  template<> void 
  FileFormatDictionary<AimsBucket<vector<int16_t> > >::registerBaseFormats()
  {
    std::vector<std::string>	ext;
    ext.push_back( "bck" );
    BckFormat<vector<int16_t> >	*fm = new BckFormat<vector<int16_t> >;
    registerFormat( "BCK", fm, ext );
  }

  template<> void 
  FileFormatDictionary<AimsBucket<vector<uint16_t> > >::registerBaseFormats()
  {
    std::vector<std::string>	ext;
    ext.push_back( "bck" );
    BckFormat<vector<uint16_t> >	*fm = new BckFormat<vector<uint16_t> >;
    registerFormat( "BCK", fm, ext );
  }


  template<> void 
  FileFormatDictionary<AimsBucket<vector<int32_t> > >::registerBaseFormats()
  {
    std::vector<std::string>	ext;
    ext.push_back( "bck" );
    BckFormat<vector<int32_t> >	*fm = new BckFormat<vector<int32_t> >;
    registerFormat( "BCK", fm, ext );
  }

  template<> void 
  FileFormatDictionary<AimsBucket<vector<uint32_t> > >::registerBaseFormats()
  {
    std::vector<std::string>	ext;
    ext.push_back( "bck" );
    BckFormat<vector<uint32_t> >	*fm = new BckFormat<vector<uint32_t> >;
    registerFormat( "BCK", fm, ext );
  }

  template<> void 
  FileFormatDictionary<AimsBucket<vector<float> > >::registerBaseFormats()
  {
    std::vector<std::string>	ext;
    ext.push_back( "bck" );
    BckFormat<vector<float> >	*fm = new BckFormat<vector<float> >;
    registerFormat( "BCK", fm, ext );
  }

  template<> void 
  FileFormatDictionary<AimsBucket<vector<double> > >::registerBaseFormats()
  {
    std::vector<std::string>	ext;
    ext.push_back( "bck" );
    BckFormat<vector<double> >	*fm = new BckFormat<vector<double> >;
    registerFormat( "BCK", fm, ext );
  }

}

template class FileFormatDictionary<AimsBucket<Void> >;
template class FileFormatDictionary<AimsBucket<short> >;
template class FileFormatDictionary<AimsBucket<uint16_t> >;
template class FileFormatDictionary<AimsBucket<int32_t> >;
template class FileFormatDictionary<AimsBucket<uint32_t> >;
template class FileFormatDictionary<AimsBucket<float> >;
template class FileFormatDictionary<AimsBucket<double> >;

template class FileFormatDictionary<AimsBucket<DtiTensor> >;
template class FileFormatDictionary<BucketMap<Void> >;
template class FileFormatDictionary<BucketMap<short> >;
template class FileFormatDictionary<BucketMap<uint16_t> >;
template class FileFormatDictionary<BucketMap<int32_t> >;
template class FileFormatDictionary<BucketMap<uint32_t> >;
template class FileFormatDictionary<BucketMap<float> >;
template class FileFormatDictionary<BucketMap<double> >;
template class FileFormatDictionary<BucketMap<DtiTensor> >;

template class FileFormatDictionary<AimsBucket<vector<int8_t> > >;
template class FileFormatDictionary<AimsBucket<vector<uint8_t> > >;
template class FileFormatDictionary<AimsBucket<vector<int16_t> > >;
template class FileFormatDictionary<AimsBucket<vector<uint16_t> > >;
template class FileFormatDictionary<AimsBucket<vector<int32_t> > >;
template class FileFormatDictionary<AimsBucket<vector<uint32_t> > >;
template class FileFormatDictionary<AimsBucket<vector<float> > >;
template class FileFormatDictionary<AimsBucket<vector<double> > >;

static bool _buckdic()
{
  FileFormatDictionary<AimsBucket<Void> >::init();
  FileFormatDictionary<AimsBucket<short> >::init();
  FileFormatDictionary<AimsBucket<uint16_t> >::init();
  FileFormatDictionary<AimsBucket<int32_t> >::init();
  FileFormatDictionary<AimsBucket<uint32_t> >::init();
  FileFormatDictionary<AimsBucket<float> >::init();
  FileFormatDictionary<AimsBucket<DtiTensor> >::init();
  FileFormatDictionary<BucketMap<Void> >::init();
  FileFormatDictionary<BucketMap<short> >::init();
  FileFormatDictionary<BucketMap<uint16_t> >::init();
  FileFormatDictionary<BucketMap<int32_t> >::init();
  FileFormatDictionary<BucketMap<uint32_t> >::init();
  FileFormatDictionary<BucketMap<float> >::init();
  FileFormatDictionary<BucketMap<DtiTensor> >::init();
  FileFormatDictionary<AimsBucket<vector<float> > >::init();
  FileFormatDictionary<AimsBucket<vector<double> > >::init();
  return true;
}

static bool buckdic = _buckdic();

