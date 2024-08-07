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
#include <aims/io/baseFormats_mesh_d.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <aims/io_soma/fssurfformatchecker.h>


using namespace aims;
using namespace soma;
using namespace std;

namespace aims
{

template<> void
FileFormatDictionary<AimsSurfaceTriangle>::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "mesh" );
  MeshFormat<3,Void>	*fm = new MeshFormat<3,Void>;
  registerFormat( "MESH", fm, ext );

  ext.clear();
  ext.push_back( "tri" );
  TriFormat	*ft = new TriFormat;
  registerFormat( "TRI", ft, ext );
  ext.clear();

  ext.push_back( "pov" );
  PovFormat<3> *fp = new PovFormat<3>;
  registerFormat( "POV", fp, ext );
  ext.clear();

  ext.push_back( "vrml" );
  ext.push_back( "wrl" );
  Vrml1Format *fv = new Vrml1Format;
  registerFormat( "VRML1", fv, ext );
  ext.clear();

  ext.push_back( "obj" );
  MniObjFormat *fo = new MniObjFormat;
  registerFormat( "MNI_OBJ", fo, ext );
  ext.clear();

  ext.push_back( "obj" );
  ext.push_back( "OBJ" );
  WavefrontMeshFormat<3, Void> *wo = new WavefrontMeshFormat<3, Void>;
  registerFormat( "WAVEFRONT", wo, ext );
  ext.clear();
}


template<> void
FileFormatDictionary<AimsSurfaceFacet>::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "mesh" );
  MeshFormat<4,Void>	*fm = new MeshFormat<4,Void>;
  registerFormat( "MESH", fm, ext );

  ext.push_back( "pov" );
  PovFormat<4> *fp = new PovFormat<4>;
  registerFormat( "POV", fp, ext );

  ext.push_back( "obj" );
  WavefrontMeshFormat<4, Void> *wo = new WavefrontMeshFormat<4, Void>;
  registerFormat( "WAVEFRONT", wo, ext );
  ext.clear();
}


template<> void
FileFormatDictionary<AimsTimeSurface<2,Void> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "mesh" );
  MeshFormat<2,Void>	*fm = new MeshFormat<2,Void>;
  registerFormat( "MESH", fm, ext );

  ext.push_back( "pov" );
  PovFormat<2> *fp = new PovFormat<2>;
  registerFormat( "POV", fp, ext );

  ext.push_back( "obj" );
  WavefrontMeshFormat<2, Void> *wo = new WavefrontMeshFormat<2, Void>;
  registerFormat( "WAVEFRONT", wo, ext );
  ext.clear();
}


template<> void
FileFormatDictionary<AimsTimeSurface<3, float> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "mesh" );
  MeshFormat<3, float>    *fm = new MeshFormat<3, float>;
  registerFormat( "MESH", fm, ext );

  ext.push_back( "obj" );
  WavefrontMeshFormat<3, float> *wo = new WavefrontMeshFormat<3, float>;
  registerFormat( "WAVEFRONT", wo, ext );
  ext.clear();
}


template<> void
FileFormatDictionary<AimsTimeSurface<2, float> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "mesh" );
  MeshFormat<2, float>    *fm = new MeshFormat<2, float>;
  registerFormat( "MESH", fm, ext );

  ext.push_back( "obj" );
  WavefrontMeshFormat<2, float> *wo = new WavefrontMeshFormat<2, float>;
  registerFormat( "WAVEFRONT", wo, ext );
  ext.clear();
}


template<> void
FileFormatDictionary<AimsTimeSurface<4, float> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "mesh" );
  MeshFormat<4, float>    *fm = new MeshFormat<4, float>;
  registerFormat( "MESH", fm, ext );

  ext.push_back( "obj" );
  WavefrontMeshFormat<4, float> *wo = new WavefrontMeshFormat<4, float>;
  registerFormat( "WAVEFRONT", wo, ext );
  ext.clear();
}


template<> void
FileFormatDictionary<AimsTimeSurface<3, Point2df> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "mesh" );
  MeshFormat<3, Point2df>    *fm = new MeshFormat<3, Point2df>;
  registerFormat( "MESH", fm, ext );

  ext.push_back( "obj" );
  WavefrontMeshFormat<3, Point2df> *wo = new WavefrontMeshFormat<3, Point2df>;
  registerFormat( "WAVEFRONT", wo, ext );
  ext.clear();
}


template<> void
FileFormatDictionary<AimsTimeSurface<2, Point2df> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "mesh" );
  MeshFormat<2, Point2df>    *fm = new MeshFormat<2, Point2df>;
  registerFormat( "MESH", fm, ext );

  ext.push_back( "obj" );
  WavefrontMeshFormat<2, Point2df> *wo = new WavefrontMeshFormat<2, Point2df>;
  registerFormat( "WAVEFRONT", wo, ext );
  ext.clear();
}


template<> void
FileFormatDictionary<AimsTimeSurface<4, Point2df> >::registerBaseFormats()
{
  std::vector<std::string>      ext;
  ext.push_back( "mesh" );
  MeshFormat<4, Point2df>    *fm = new MeshFormat<4, Point2df>;
  registerFormat( "MESH", fm, ext );

  ext.push_back( "obj" );
  WavefrontMeshFormat<4, Point2df> *wo = new WavefrontMeshFormat<4, Point2df>;
  registerFormat( "WAVEFRONT", wo, ext );
  ext.clear();
}


template class FileFormatDictionary<AimsTimeSurface<2, Void> >;
template class FileFormatDictionary<AimsSurfaceTriangle>;
template class FileFormatDictionary<AimsSurfaceFacet>;
template class FileFormatDictionary<AimsTimeSurface<2, float> >;
template class FileFormatDictionary<AimsTimeSurface<3, float> >;
template class FileFormatDictionary<AimsTimeSurface<4, float> >;
template class FileFormatDictionary<AimsTimeSurface<2, Point2df> >;
template class FileFormatDictionary<AimsTimeSurface<3, Point2df> >;
template class FileFormatDictionary<AimsTimeSurface<4, Point2df> >;

// #ifdef __APPLE__ // also needed on linux + gcc 4.3...
template class FileFormat<AimsTimeSurface<2, Void> >;
template class FileFormat<AimsSurfaceTriangle>;
template class FileFormat<AimsSurfaceFacet>;
template class FileFormat<AimsTimeSurface<2, float> >;
template class FileFormat<AimsTimeSurface<3, float> >;
template class FileFormat<AimsTimeSurface<4, float> >;
template class FileFormat<AimsTimeSurface<2, Point2df> >;
template class FileFormat<AimsTimeSurface<3, Point2df> >;
template class FileFormat<AimsTimeSurface<4, Point2df> >;
// #endif

} // namespace aims


static bool _meshdic()
{
  FileFormatDictionary<AimsSurfaceTriangle>::init();
  FileFormatDictionary<AimsSurfaceFacet>::init();
  FileFormatDictionary<AimsTimeSurface<2, Void> >::init();
  FileFormatDictionary<AimsTimeSurface<2, float> >::init();
  FileFormatDictionary<AimsTimeSurface<3, float> >::init();
  FileFormatDictionary<AimsTimeSurface<4, float> >::init();
  FileFormatDictionary<AimsTimeSurface<2, Point2df> >::init();
  FileFormatDictionary<AimsTimeSurface<3, Point2df> >::init();
  FileFormatDictionary<AimsTimeSurface<4, Point2df> >::init();

  // register soma-io checker for the FSSURF format
  {
    vector<string>  exts(2);
    exts[0] = "white";
    exts[1] = "pial";
    DataSourceInfoLoader::registerFormat( "FSSURF",
                                          new FsSurfFormatChecker, exts );
  }

  return true;
}

static bool meshdic __attribute__((unused)) = _meshdic();


#include <soma-io/io/formatdictionary_d.h>
#include <soma-io/reader/formatreader_d.h>
#include <soma-io/io/reader_d.h>
#include <soma-io/io/writer_d.h>

namespace soma
{
  template class FormatReader<AimsSurfaceTriangle>;
  template class FormatDictionary<AimsSurfaceTriangle>;
  template class Reader<AimsSurfaceTriangle>;
  template class Writer<AimsSurfaceTriangle>;

//   template class FormatReader<AimsTimeSurface<4, Void> >;
//   template class FormatDictionary<AimsTimeSurface<4, Void> >;
//   template class Reader<AimsTimeSurface<4, Void> >;
//   template class Writer<AimsTimeSurface<4, Void> >;
}

