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
 *  Surface reader class
 */
#ifndef AIMS_IO_MESHR_H
#define AIMS_IO_MESHR_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemR.h>
#include <aims/io/meshheader.h>
#include <cartobase/exception/ioexcept.h>


namespace aims
{

  /**	Mesh format readers for mesh objects.
	This reader can be used directly, or via the Reader / MeshFormat 
	interface.
	End users should always use Reader which hides the format-specific 
	operations, MeshReader is only a part of the mechanism

	\see TriReader
  */
  template<int D, class T=Void>
  class MeshReader
  {
  public:
    MeshReader( const std::string & name, ItemReader<T> *ir = 0 ) 
      : _name( name ), _itemr( ir ) { }
    ~MeshReader() { delete _itemr; }

    inline void read( AimsTimeSurface<D,T>& thing, int frame = -1 );

    /// Return a name without .mesh extension
    inline std::string removeExtension(const std::string& name);
    void setItemReader( ItemReader<T>* ir )
      { delete _itemr; _itemr = ir; }

  private:
    std::string		_name;
    ItemReader<T>	*_itemr;
  };


  template <int D, class T>
  inline 
  AIMSDATA_API MeshReader<D,T> &
  operator >> ( MeshReader<D,T> & reader, AimsTimeSurface<D,T> & thing )
  {
    reader.read( thing );
    return reader;
  }


  template <int D, class T> inline 
  std::string MeshReader<D,T>::removeExtension( const std::string& name )
  {
    std::string res = name;
    std::string ext="";
    if ( res.length() > 5 )
      ext = res.substr( int(res.length() - 5), 5 );
    if ( ext == ".mesh" )
      res = res.substr( 0, res.length() - 5 );
    return res;
  }


  template <int D, class T> inline
  void 
  MeshReader<D,T>::read( AimsTimeSurface<D,T> & thing, int frame )
  {
    if ( frame >= 0 )
      std::cerr << "Warning : .mesh single frame reading not implemented yet " 
		<< "-\nreading whole mesh\n";

    MeshHeader	hdr( _name );
    uint32_t	buf;
    if ( !hdr.read( &buf ) )
      carto::io_error::launchErrnoExcept( hdr.filename() );
    std::ifstream::off_type offset = buf; // ## does not support large files!

    std::ios::openmode	omd = std::ios::in;
    if ( hdr.openMode() == "binar" )
      omd |= std::ios::binary;
    std::ifstream	is( hdr.filename().c_str(), omd );
    is.unsetf( std::ios::skipws );
    if ( !is )
      carto::io_error::launchErrnoExcept( hdr.filename() );
    is.seekg( offset );
    if ( !is )
      carto::io_error::launchErrnoExcept( hdr.filename() );

    if ( hdr.openMode() == "ascii" )
      is.setf( std::ios::skipws );
    thing.erase();
    if ( !_itemr )
      _itemr = new DefaultItemReader<T>;
    ItemReader<T>		*ir 
      = _itemr->reader( hdr.openMode(), hdr.byteSwapping() );
    DefaultItemReader<uint32_t>	sr1;
    ItemReader<uint32_t>	*sr = sr1.reader( hdr.openMode(), 
                                          hdr.byteSwapping() );
    DefaultItemReader<Point3df>	pr1;
    ItemReader<Point3df>	*pr = pr1.reader( hdr.openMode(), 
						  hdr.byteSwapping() );
    DefaultItemReader<AimsVector<uint,D> >	plr1;
    ItemReader<AimsVector<uint,D> >		*plr 
      = plr1.reader( hdr.openMode(), hdr.byteSwapping() );

    int size = 0;
    hdr.getProperty( "nb_t_pos", size );
    bool	broken = false;
    typename std::vector<AimsVector<uint,D> >::iterator	ip;

    thing.setHeader( hdr );

    uint32_t time, nvertex, nnormal, ntexture, npolygon;
    for ( int t=0; t<size; ++t )
      {
	// time
	sr->read( is, time );

	// vertices
	sr->read( is, nvertex );

	AimsSurface<D,T>	& surf = thing[ time ];

	std::vector<Point3df>	& vert = surf.vertex();
	vert.insert( vert.end(), nvertex, Point3df() );
	pr->read( is, &vert[0], nvertex );

	// normals
	sr->read( is, nnormal );
	std::vector<Point3df>	& norm = surf.normal();
	norm.insert( norm.end(), nnormal, Point3df() );
	pr->read( is, &norm[0], nnormal );

	// textures
	sr->read( is, ntexture );
	std::vector<T>	& tex = surf.texture();
	tex.insert( tex.end(), ntexture, T() );
	ir->read( is, &tex[0], ntexture );

	// polygons
	sr->read( is, npolygon );
	std::vector<AimsVector<uint,D> >  & poly = surf.polygon();
	poly.insert( poly.end(), npolygon, AimsVector<uint,D>() );
	plr->read( is, &poly[0], npolygon );

        // verify polygons are all in the vertices range
        for ( ip=poly.begin(); ip!=poly.end(); ++ip )
          {
            AimsVector<uint,D>	& pol = *ip;
            for ( int j=0; j<D; ++j )
              if ( pol[j] >= nvertex )
                {
                  if ( !broken )
                    {
                      broken = true;
                      std::cerr << "Broken mesh: polygon pointing to a " 
                                << "vertex out of range" << std::endl;
                    }
                  poly.erase( ip );
                  --ip;
                  break;
                }
          }

	if ( norm.size() != vert.size() )
	  surf.updateNormals();
      }

    delete plr;
    delete pr;
    delete sr;
    delete ir;
  }

}


#endif
