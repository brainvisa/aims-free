/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */


#include <aims/data/data_g.h>
#include <cartodata/volume/volume.h>

#ifdef AIMSDATA_NEW_AIMSDATA
  // compilation of some Volume classes on Aims types
  #include <cartodata/volume/volume_d.h>
  namespace carto {
    template class carto::Volume< Point3df >;
    template class carto::Volume< AimsData< Point3df> >;
  }
#endif

#include <soma-io/io/reader_d.h>
#include <soma-io/io/writer_d.h>
#include <soma-io/io/formatdictionary_d.h>
#include <aims/io/reader_d.h>
#include <aims/io/writer_d.h>
#include <aims/io/fileFormat_d.h>
#include <aims/io/baseFormats_volume_d.h>
#include <aims/io/fileFormat_cartovolume_d.h>

namespace aims
{

  template<> void FileFormatDictionary<AimsData<AimsData<Point3df> > >::registerBaseFormats()
  {
    std::vector<std::string>      ext;
    ext.push_back( "ima" );
    ext.push_back( "dim" );
    GisFormat<AimsData<Point3df> >     *gf = new GisFormat<AimsData<Point3df> >;
    registerFormat( "GIS", gf, ext );
#ifdef AIMSDATA_NEW_AIMSDATA
    VolumeFormat<AimsData<Point3df> >  *vf = new VolumeFormat<AimsData<Point3df> >( "GIS" );
    FileFormatDictionary<carto::Volume<AimsData<Point3df> > >::registerFormat( "GIS", vf, ext );
#endif
  }

  template<> void FileFormatDictionary<carto::Volume<AimsData<Point3df> > >::registerBaseFormats()
  {
  }

  template<> void FileFormatDictionary<AimsData<Point3df> >::registerBaseFormats()
  {
    std::vector<std::string>      ext;
    ext.push_back( "ima" );
    ext.push_back( "dim" );
    GisFormat<Point3df>     *gf = new GisFormat<Point3df>;
    registerFormat( "GIS", gf, ext );
#ifdef AIMSDATA_NEW_AIMSDATA
    VolumeFormat<Point3df>  *vf = new VolumeFormat<Point3df>( "GIS" );
    FileFormatDictionary<carto::Volume<Point3df> >::registerFormat( "GIS", vf, ext );
#endif
  }

  template<> void FileFormatDictionary<carto::Volume<Point3df> >::registerBaseFormats()
  {
  }

  template class FileFormatDictionary< AimsData< AimsData< Point3df > > >;
  template class FileFormatDictionary< AimsData< Point3df > >;
}

AIMS_INSTANTIATE_READER( AimsData< AimsData< Point3df > > );
AIMS_INSTANTIATE_WRITER( AimsData< AimsData< Point3df > > );
AIMS_INSTANTIATE_READER( AimsData< Point3df > );
AIMS_INSTANTIATE_WRITER( AimsData< Point3df > );
