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
 *  Gis header class members
 */
#include <aims/io/gisheader.h>
#include <aims/def/general.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/io/datasourceinfo.h>
#include <cartobase/datasource/filedatasource.h>
#include <cartodata/io/gisformatchecker.h>
#include <cartodata/io/carto2aimsheadertranslator.h>
#include <fstream>
#include <vector>
#include <errno.h>

using namespace aims;
using namespace carto;
using namespace std;


set<string> GisHeader::extensions() const
{
  set<string>	exts;
  exts.insert( ".dim" );
  exts.insert( ".ima" );
  return exts;
}


//	helper for reading attributes
template<typename T> 
static void inHelper( GisHeader & object, const string& semantic, 
		      istream& is )
{
  if( is.eof() )
    return;

  T i = 0;
  is >> i;
  object.setProperty(semantic, i);
}


template<typename T> 
static void inVecHelper( GisHeader & object, const string& semantic, 
			 istream& is )
{
  vector<T> iv;
  while (1)
    {
      if( is.eof() )
	return;

      int c = is.get();
      while (c != '\n' && c != '\0' && c != EOF 
	     && (c == ' ' || c == '\t'))
	c = is.get();
      if (c == '\n' || c == '\0' || c == EOF)
	break;
      else
	is.unget();
      T i;
      is >> i;
      iv.push_back(i);
    }
  object.setProperty(semantic, iv);
}


void GisHeader::read()
{
  // cout << "GisHGeader::read - new style: " << _name << endl;
  GisFormatChecker	fc;
  DataSourceInfo	dsi;
  FileDataSource	ds( _name );
  Object		h = fc.check( ds, dsi );
  if( h.isNone() )
    dsi.launchException();

  // fill in specific fields
  h->getProperty( "sizeX", _dimX );
  h->getProperty( "sizeY", _dimY );
  h->getProperty( "sizeZ", _dimZ );
  h->getProperty( "sizeT", _dimT );
  try
    {
      _byteOrder =  
        h->getProperty( "byte_swapping" )->getScalar() ? 0x44434241 
        : AIMS_MAGIC_NUMBER;
    }
  catch( ... )
    {
      _byteOrder = AIMS_MAGIC_NUMBER;
    }
  try
    {
      _openMode = h->getProperty( "ascii" )->getScalar() ? "ascii" : "binar";
    }
  catch( ... )
    {
      _openMode = "binar";
    }
  
  Carto2AimsHeaderTranslator	t;
  t.translate( h );
  h->getProperty( "data_type", _type );
  copyProperties( h );
}



void GisHeader::write()
{
  string fileName = removeExtension(_name) + ".dim";
  ofstream os( fileName.c_str() );

  if( !os )
    io_error::launchErrnoExcept( fileName );

  os << _dimX << " " << _dimY << " " << _dimZ << " " << _dimT << endl;
  os << "-type " << _type << endl;
  os << "-dx " << _sizeX << " -dy " << _sizeY << " -dz " << _sizeZ
     << " -dt " << _sizeT << endl;
  os << "-bo ";
  uint magicNumber = AIMS_MAGIC_NUMBER;
  os.write( (char *) &magicNumber , sizeof(uint) );
  os << endl;
  os << "-om " << _openMode << endl;
  os.close();

  // write meta-info header
  setProperty( "file_type", string( "GIS" ) );
  vector<int>	dims;
  dims.push_back( _dimX );
  dims.push_back( _dimY );
  dims.push_back( _dimZ );
  dims.push_back( _dimT );
  setProperty( "volume_dimension", dims );
  vector<float>	vs;
  vs.push_back( _sizeX );
  vs.push_back( _sizeY );
  vs.push_back( _sizeZ );
  vs.push_back( _sizeT );
  setProperty( "voxel_size", vs );
  setProperty( "data_type", _type );
  setProperty( "object_type", string( "Volume" ) );

  writeMinf( removeExtension( _name ) + extension() + ".minf" );
}
