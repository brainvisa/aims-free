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

#ifndef AIMS_IO_MESHW_H
#define AIMS_IO_MESHW_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemW.h>
#include <aims/io/meshheader.h>
#include <cartobase/exception/file.h>
#include <aims/mesh/surface.h>
#include <aims/io/datatypecode.h>
#include <memory>


namespace aims
{

  /**	Mesh format writer for mesh objects.
	\see TriWriter MeshReader Writer
  */
  template<int D, class T=Void>
  class AIMSDATA_API MeshWriter
  {
  public:
    MeshWriter( const std::string & name, bool ascii = false, 
		ItemWriter<T> *ir = 0 ) 
      : _name( name ), _ascii( ascii ), _itemw( ir ) { }
    ~MeshWriter() { delete _itemw; }

    inline void write( const AimsTimeSurface<D,T> & thing );

    /// Return a name without .mesh extension
    inline std::string removeExtension(const std::string& name) const;
    void setItemWriter( ItemWriter<T>* ir )
    { delete _itemw; _itemw = ir; }

  private:
    std::string		_name;
    bool		_ascii;
    ItemWriter<T>	*_itemw;
  };


  template <int D, class T>
  inline 
  AIMSDATA_API MeshWriter<D,T> &
  operator << ( MeshWriter<D,T> & writer, const AimsTimeSurface<D,T> & thing )
  {
    writer.write( thing );
    return writer;
  }


  template <int D, class T> inline 
  std::string MeshWriter<D,T>::removeExtension( const std::string& name ) const
  {
    std::string res = name;
    std::string ext="";
    if( res.length() > 5 )
      ext = res.substr( int(res.length() - 5), 5 );
    if( ext == ".mesh" )
      res = res.substr( 0, res.length() - 5 );
    return res;
  }


  template <int D, class T> inline
  void MeshWriter<D,T>::write( const AimsTimeSurface<D,T> & thing )
  {
    std::string	fname = removeExtension( _name ) + ".mesh";
    std::string	mode = ( _ascii ? "ascii" : "binar" );
    std::ios::openmode	omd = std::ios::out;
    if( !_ascii )
      omd |= std::ios::binary;
    std::ofstream	os( fname.c_str(), omd );
    if( !os )
      throw carto::file_error( "Could not open file", fname );

    if( !_itemw )
      _itemw = new DefaultItemWriter<T>;
    std::auto_ptr< ItemWriter<T> > ir( _itemw->writer( mode, false ) );
    DefaultItemWriter<uint32_t> sr1;
    std::auto_ptr< ItemWriter<uint32_t> > sr( sr1.writer( mode, false ) );
    DefaultItemWriter<Point3df> pr1;
    std::auto_ptr< ItemWriter<Point3df> > pr( pr1.writer( mode, false ) );
    DefaultItemWriter<AimsVector<uint,D> > plr1;
    std::auto_ptr< ItemWriter<AimsVector<uint,D> > > plr ( plr1.writer( mode, false ) );

    //	write header

    os << ( _ascii ? "ascii\n" : "binar" );
    carto::DataTypeCode<T>	dt;
    std::string		code = dt.dataType();
    std::string		sep = ( _ascii ? "\n" : "" );
    
    if ( !_ascii )
      {
        uint32_t magicNumber = AIMS_MAGIC_NUMBER;
        os.write( (const char *) &magicNumber, sizeof(uint32_t) );
        
        size_t tmp = code.size();
        uint32_t typeSize = (uint32_t)tmp;
        sr->write( os, typeSize );
      }
    
    os << code << sep;
    
    uint32_t dim = (uint32_t)D;
    sr->write( os, dim );
    os << sep;
    
    uint32_t size = (uint32_t)thing.size();
    sr->write( os, size );
    os << sep;

    MeshHeader hdr( _name );
    hdr.copy( thing.header() );
    hdr.writeMinf( hdr.filename() + ".minf" );

    //	write main data

    uint32_t	nvertex, nnormal, time, ntexture, npolygon;
    typename AimsTimeSurface<D,T>::const_iterator	is, es = thing.end();

    for ( is=thing.begin(); is!=es; ++is )
      {
        time = (*is).first;
        const std::vector<Point3df>		& vert = (*is).second.vertex();
        const std::vector<Point3df>		& norm = (*is).second.normal();
        const std::vector<T>			& tex = (*is).second.texture();
        const std::vector<AimsVector<uint,D> >	& poly = (*is).second.polygon();
        
        nvertex = (uint32_t)vert.size();
        nnormal = (uint32_t)norm.size();
        ntexture = (uint32_t)tex.size();
        npolygon = (uint32_t)poly.size();
        
        // time
        sr->write( os, time );
        os << sep;
        
        // vertices
        sr->write( os, nvertex );
        os << sep;
        
        pr->write( os, &vert[0], nvertex );
        os << sep;
        
        // normals
        sr->write( os, nnormal );
        os << sep;
        pr->write( os, &norm[0], nnormal );
        os << sep;
        
        // textures
        sr->write( os, ntexture );
        os << sep;
        ir->write( os, &tex[0], ntexture );
        os << sep;
        
        // polygons
        sr->write( os, npolygon );
        os << sep;
        plr->write( os, &poly[0], npolygon );
      }
  }

}


#endif
