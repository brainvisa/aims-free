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

/*
 *  IO classes
 */

/* this source is only here to force instanciation of some of the most useful 
   Reader templates */

#include <aims/io/fileFormat_d.h>
#include <aims/io/baseFormats_mesh_d.h>


using namespace aims;

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
  Vrml1Format *fv = new Vrml1Format;
  registerFormat( "VRML1", fv, ext );
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
}

}


template class FileFormatDictionary<AimsTimeSurface<2, Void> >;
template class FileFormatDictionary<AimsSurfaceTriangle>;
template class FileFormatDictionary<AimsSurfaceFacet>;

// #ifdef __APPLE__ // also needed on linux + gcc 4.3...
template class FileFormat<AimsTimeSurface<2, Void> >;
template class FileFormat<AimsSurfaceTriangle>;
template class FileFormat<AimsSurfaceFacet>;
// #endif


static bool _meshdic()
{
  FileFormatDictionary<AimsSurfaceTriangle>::init();
  return true;
}

static bool meshdic = _meshdic();

