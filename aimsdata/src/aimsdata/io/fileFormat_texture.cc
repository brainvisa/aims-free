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
#include <aims/io/baseFormats_texture_d.h>


using namespace aims;
using namespace std;

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
FileFormatDictionary<TimeTexture<Point2df> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<Point2df>	*fm = new TexFormat<Point2df>;
  registerFormat( "TEX", fm, ext );
}

template<> void 
FileFormatDictionary<TimeTexture<short> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<short>	*fm = new TexFormat<short>;
  registerFormat( "TEX", fm, ext );
}

template<> void 
FileFormatDictionary<TimeTexture<unsigned> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<unsigned>	*fm = new TexFormat<unsigned>;
  registerFormat( "TEX", fm, ext );
}


template<> void 
FileFormatDictionary<TimeTexture<int> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<int>	*fm = new TexFormat<int>;
  registerFormat( "TEX", fm, ext );
}


template<> void 
FileFormatDictionary<TimeTexture<Point2d> >::registerBaseFormats()
{
  std::vector<std::string>	ext;
  ext.push_back( "tex" );
  TexFormat<Point2d>	*fm = new TexFormat<Point2d>;
  registerFormat( "TEX", fm, ext );
}

}

template class FileFormatDictionary<TimeTexture<float> >;
template class FileFormatDictionary<TimeTexture<Point2df> >;
template class FileFormatDictionary<TimeTexture<short> >;
template class FileFormatDictionary<TimeTexture<unsigned> >;
template class FileFormatDictionary<TimeTexture<int> >;
template class FileFormatDictionary<TimeTexture<Point2d> >;

#ifdef __APPLE__
template class FileFormat<TimeTexture<float> >;
template class FileFormat<TimeTexture<Point2df> >;
template class FileFormat<TimeTexture<short> >;
template class FileFormat<TimeTexture<unsigned> >;
template class FileFormat<TimeTexture<int> >;
template class FileFormat<TimeTexture<Point2d> >;
#endif

static bool _texdic()
{
  FileFormatDictionary<TimeTexture<float> >::init();
  FileFormatDictionary<TimeTexture<short> >::init();
  FileFormatDictionary<TimeTexture<unsigned> >::init();
  FileFormatDictionary<TimeTexture<int> >::init();
  FileFormatDictionary<TimeTexture<Point2d> >::init();
  FileFormatDictionary<TimeTexture<Point2df> >::init();
  return true;
}

static bool texdic = _texdic();

