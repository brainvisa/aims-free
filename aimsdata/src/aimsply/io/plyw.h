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

#ifndef AIMS_IO_PLYW_H
#define AIMS_IO_PLYW_H

#include <aims/io/plyheader.h>
#include <aims/io/datatypecode.h>
#include <aims/data/pheader.h>
#include <aims/mesh/surface.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/sstream.h>
#include <cartobase/exception/file.h>
#include <aims/rply/rply.h>


namespace aims
{

  template<long D, typename T>
  class PlyWriter
  {
  public:
    PlyWriter( const std::string& name, bool ascii = false )
      : _name( name ), _ascii( ascii )
    {}
    ~PlyWriter() {}

    void write( const AimsTimeSurface<D,T> & thing );
    /// Return a name without .ply extension
    std::string removeExtension( const std::string& name ) const;

  private:
    std::string		_name;
    bool		_ascii;
  };


  template <long D, typename T>
  inline PlyWriter<D,T> & 
  operator << ( PlyWriter<D,T> & writer, const AimsTimeSurface<D,T> & thing )
  {
    writer.write( thing );
    return writer;
  }


  template <long D, typename T>
  inline
  void PlyWriter<D,T>::write( const AimsTimeSurface<D,T>& thing )
  {
    PlyHeader	hdr( _name );
    const PythonHeader 
      *ph = dynamic_cast<const PythonHeader *>( &thing.header() );
    if( ph )
      hdr.copy( *ph );

    std::string			dir = carto::FileUtil::dirname( _name );

    hdr.setProperty( "file_type", std::string( "PLY" ) );
    hdr.setProperty( "object_type", 
                     carto::DataTypeCode<AimsTimeSurface<D,T> >::objectType()
                     );
    hdr.setProperty( "data_type", carto::DataTypeCode<T>::name() );
    if( !dir.empty() )
      dir += carto::FileUtil::separator();

    std::string	fname = carto::FileUtil::removeExtension( _name ) 
      + hdr.extension();
    e_ply_storage_mode	mode = PLY_LITTLE_ENDIAN;
    if( _ascii )
      mode = PLY_ASCII;
    p_ply	ply = ply_create( fname.c_str(), mode, 0 );
    if( !ply )
      carto::io_error::launchErrnoExcept( fname );

    if( thing.size() > 1 )
      std::cout << "PlyWriter warning: only the first timestep of the " 
                << "surface will be writen" << std::endl;

    const std::vector<Point3df>	& vert = thing.vertex();
    const std::vector<Point3df>	& norm = thing.normal();
    const std::vector<AimsVector<uint, D> >	& poly = thing.polygon();
    unsigned			i, j, n = vert.size(), np = poly.size();
    bool			normal = (n <= norm.size() );

   if( !ply_add_element( ply, "vertex", n ) )
     throw carto::invalid_format_error( "PLY format internal error", fname );
    if( !ply_add_scalar_property( ply, "x", PLY_FLOAT ) 
        || !ply_add_scalar_property( ply, "y", PLY_FLOAT ) 
        || !ply_add_scalar_property( ply, "z", PLY_FLOAT ) )
      throw carto::invalid_format_error( "PLY format internal error", fname );
    if( normal )
      if( !ply_add_scalar_property( ply, "nx", PLY_FLOAT ) 
          || !ply_add_scalar_property( ply, "ny", PLY_FLOAT ) 
          || !ply_add_scalar_property( ply, "nz", PLY_FLOAT ) )
        throw carto::invalid_format_error( "PLY format internal error", 
                                           fname );
    if( !ply_add_element( ply, "face", np ) )
      throw carto::invalid_format_error( "PLY format internal error", fname );
    if( !ply_add_list_property( ply, "vertex_indices", PLY_UCHAR, PLY_INT ) )
      throw carto::invalid_format_error( "PLY format internal error", fname );

    if( !ply_write_header( ply ) )
      throw carto::invalid_format_error( "PLY format write error", fname );

    for( i=0; i<n; ++i )
      {
        if( !ply_write( ply, vert[i][0] ) 
            || !ply_write( ply, vert[i][1] ) 
            || !ply_write( ply, vert[i][2] ) 
            )
          carto::io_error::launchErrnoExcept( fname );
        if( normal )
          if( !ply_write( ply, norm[i][0] ) 
              || !ply_write( ply, norm[i][1] ) 
              || !ply_write( ply, norm[i][2] ) 
              )
            carto::io_error::launchErrnoExcept( fname );
      }
    for( i=0; i<np; ++i )
      {
        if( !ply_write( ply, D ) )
          carto::io_error::launchErrnoExcept( fname );
        for( j=0; j<D; ++j )
          if( !ply_write( ply, poly[i][j] ) )
            carto::io_error::launchErrnoExcept( fname );
      }

    if( !ply_close(ply) )
      carto::io_error::launchErrnoExcept( fname );

    hdr.writeMinf( fname + ".minf" );
  }

}

#endif

