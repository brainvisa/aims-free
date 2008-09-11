/* Copyright (c) 1995-2006 CEA
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

/*
 *  IO classes
 */

/* this source is only here to force instanciation of some of the most useful 
   Reader templates */

#include <aims/io/fileFormat_d.h>
#include <aims/io/baseFormats_volume_d.h>
#include <aims/rgb/rgb.h>
#include <aims/io/fileFormat_cartovolume_d.h>

using namespace aims;
using namespace carto;

namespace aims
{

template<> void
FileFormatDictionary<Volume<AimsRGB> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<AimsRGB> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<AimsRGB>	*f = new VolumeRefFormat<AimsRGB>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<AimsRGBA> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<AimsRGBA> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<AimsRGBA>	*f = new VolumeRefFormat<AimsRGBA>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<Point3df> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<Point3df> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<Point3df>	*f = new VolumeRefFormat<Point3df>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<Point3d> >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<Point3d> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<Point3d>	*f = new VolumeRefFormat<Point3d>;
  registerFormat( "ALLVOLUMES", f, ext );
}

template<> void 
FileFormatDictionary<Volume<AimsVector<float, 6> > >::registerBaseFormats()
{
}

template<> void 
FileFormatDictionary<VolumeRef<AimsVector<float, 6> > >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "" );
  VolumeRefFormat<AimsVector<float, 6> > 
    *f = new VolumeRefFormat<AimsVector<float, 6> >;
  registerFormat( "ALLVOLUMES", f, ext );
}


template<> void 
FileFormatDictionary<AimsData<AimsRGB> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<AimsRGB>	*gf = new GisFormat<AimsRGB>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<AimsRGB>	*vf = new VolumeFormat<AimsRGB>( "GIS" );
  FileFormatDictionary<Volume<AimsRGB> >::registerFormat( "GIS", vf, ext );

#ifdef AIMS_JPEG
  ext.clear();
  ext.push_back( "jpg" );
  JpegFormat<AimsRGB>	*jf = new JpegFormat<AimsRGB>;
  registerFormat( "JPEG", jf, ext );
  vf = new VolumeFormat<AimsRGB>( "JPEG" );
  FileFormatDictionary<Volume<AimsRGB> >::registerFormat( "GIS", vf, ext );
#endif

}

template<> void 
FileFormatDictionary<AimsData<AimsRGBA> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<AimsRGBA>	*gf = new GisFormat<AimsRGBA>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<AimsRGBA>	*vf = new VolumeFormat<AimsRGBA>( "GIS" );
  FileFormatDictionary<Volume<AimsRGBA> >::registerFormat( "GIS", vf, ext );
}

template<> void 
FileFormatDictionary<AimsData<Point3df> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<Point3df>	*gf = new GisFormat<Point3df>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<Point3df>	*vf = new VolumeFormat<Point3df>( "GIS" );
  FileFormatDictionary<Volume<Point3df> >::registerFormat( "GIS", vf, ext );
}

template<> void 
FileFormatDictionary<AimsData<Point3d> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<Point3d>	*gf = new GisFormat<Point3d>;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<Point3d>	*vf = new VolumeFormat<Point3d>( "GIS" );
  FileFormatDictionary<Volume<Point3d> >::registerFormat( "GIS", vf, ext );
}

template<> void 
FileFormatDictionary<AimsData<AimsVector<float,6> > >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "ima" );
  ext.push_back( "dim" );
  GisFormat<AimsVector<float,6> > *gf = new GisFormat<AimsVector<float,6> >;
  registerFormat( "GIS", gf, ext );
  VolumeFormat<AimsVector<float,6> >
    *vf = new VolumeFormat<AimsVector<float,6> >( "GIS" );
  FileFormatDictionary<Volume<AimsVector<float,6> > >::registerFormat( "GIS", 
                                                                       vf, 
                                                                       ext );
}

}

template class FileFormatDictionary<AimsData<AimsRGB> >;
template class FileFormatDictionary<AimsData<AimsRGBA> >;
template class FileFormatDictionary<AimsData<Point3df> >;
template class FileFormatDictionary<AimsData<Point3d> >;
template class FileFormatDictionary<AimsData<AimsVector<float,6> > >;

template class FileFormatDictionary<Volume<AimsRGB> >;
template class FileFormatDictionary<Volume<AimsRGBA> >;
template class FileFormatDictionary<Volume<Point3df> >;
template class FileFormatDictionary<Volume<Point3d> >;
template class FileFormatDictionary<Volume<AimsVector<float,6> > >;

template class FileFormatDictionary<VolumeRef<AimsRGB> >;
template class FileFormatDictionary<VolumeRef<AimsRGBA> >;
template class FileFormatDictionary<VolumeRef<Point3df> >;
template class FileFormatDictionary<VolumeRef<Point3d> >;
template class FileFormatDictionary<VolumeRef<AimsVector<float,6> > >;

template class FileFormat<AimsData<AimsRGB> >;
template class FileFormat<AimsData<AimsRGBA> >;
template class FileFormat<AimsData<Point3df> >;
template class FileFormat<AimsData<Point3d> >;
template class FileFormat<AimsData<AimsVector<float,6> > >;
template class FileFormat<Volume<AimsRGB> >;
template class FileFormat<Volume<AimsRGBA> >;
template class FileFormat<Volume<Point3df> >;
template class FileFormat<Volume<Point3d> >;
template class FileFormat<Volume<AimsVector<float,6> > >;

template class FileFormat<VolumeRef<AimsRGB> >;
template class FileFormat<VolumeRef<AimsRGBA> >;
template class FileFormat<VolumeRef<Point3df> >;
template class FileFormat<VolumeRef<Point3d> >;
template class FileFormat<VolumeRef<AimsVector<float,6> > >;

template class VolumeFormat<AimsRGB>;
template class VolumeFormat<AimsRGBA>;
template class VolumeFormat<Point3df>;
template class VolumeFormat<Point3d>;
template class VolumeFormat<AimsVector<float,6> >;

template class VolumeRefFormat<AimsRGB>;
template class VolumeRefFormat<AimsRGBA>;
template class VolumeRefFormat<Point3df>;
template class VolumeRefFormat<Point3d>;
template class VolumeRefFormat<AimsVector<float,6> >;

static bool _volvecdic()
{
  FileFormatDictionary<AimsData<AimsRGB> >::init();
  FileFormatDictionary<AimsData<AimsRGBA> >::init();
  FileFormatDictionary<AimsData<Point3df> >::init();
  FileFormatDictionary<AimsData<Point3d> >::init();
  FileFormatDictionary<AimsData<AimsVector<float,6> > >::init();

  FileFormatDictionary<Volume<AimsRGB> >::init();
  FileFormatDictionary<Volume<AimsRGBA> >::init();
  FileFormatDictionary<Volume<Point3df> >::init();
  FileFormatDictionary<Volume<Point3d> >::init();
  FileFormatDictionary<Volume<AimsVector<float,6> > >::init();

  FileFormatDictionary<VolumeRef<AimsRGB> >::init();
  FileFormatDictionary<VolumeRef<AimsRGBA> >::init();
  FileFormatDictionary<VolumeRef<Point3df> >::init();
  FileFormatDictionary<VolumeRef<Point3d> >::init();
  FileFormatDictionary<VolumeRef<AimsVector<float,6> > >::init();

  return true;
}

static bool volvecdic = _volvecdic();

