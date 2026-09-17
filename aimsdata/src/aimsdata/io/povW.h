#ifndef AIMS_IO_POVW_H
#define AIMS_IO_POVW_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/defaultItemW.h>
#include <aims/io/meshheader.h>
#include <aims/mesh/surface.h>
#include <aims/io/datatypecode.h>
#include <cartobase/exception/file.h>


namespace aims
{

  /**	POV (http://www.povray.org)  format writer for mesh objects.
	\see TriWriter MeshReader Writer
  */
  template<int D, class T=Void>
  class AIMSDATA_API PovWriter
  {
  public:
    PovWriter( const std::string & name ) 
      : _name( name ) { }
    ~PovWriter() {}

    inline void write( const AimsTimeSurface<D,T> & thing );

    /// Return a name without .pov extension
    inline std::string removeExtension(const std::string& name) const;

  private:
    std::string _name;
  };


  template <int D, class T>
  inline 
  AIMSDATA_API PovWriter<D,T> &
  operator << ( PovWriter<D,T> & writer, const AimsTimeSurface<D,T> & thing )
  {
    writer.write( thing );
    return( writer );
  }


  template <int D, class T> 
  std::string PovWriter<D,T>::removeExtension( const std::string& name ) const
  {
    std::string res = name;
    std::string ext="";
    if( res.length() > 4 )
      ext = res.substr( int(res.length() - 4), 4 );
    if( ext == ".pov" )
      res = res.substr( 0, res.length() - 4 );
    return res;
  }


  template <int D, class T>
  void PovWriter<D,T>::write( const AimsTimeSurface<D,T> & thing )
  {
    std::string	fname = removeExtension( _name ) + ".pov";
    std::ofstream	os( fname.c_str() );
    if( !os )
      throw carto::file_error( fname );

    //	write data
    os << "mesh2 {" << std::endl;

    typename AimsTimeSurface<D,T>::const_iterator is = thing.begin();

    // Only fisrt time step is written
    if ( is != thing.end() ) {
      const std::vector<Point3df> &vert = (*is).second.vertex();
      const std::vector<Point3df> &norm = (*is).second.normal();
      const std::vector<AimsVector<uint,D> > &poly 
	= (*is).second.polygon();


      // vertices
      os << "  vertex_vectors {" << std::endl 
         << vert.size() << "," << std::endl << "    ";
      for( std::vector<Point3df>::const_iterator iv = vert.begin();
           iv != vert.end(); ++iv ) {
        os << "<" << (*iv)[0] << "," << (*iv)[1] << "," << (*iv)[2] << ">,";
      }
      os << "  }" << std::endl;
      
      // normals
      if ( ! norm.empty() ) {
        os << "  normal_vectors {" << std::endl 
           << norm.size() << "," << std::endl << "    ";
        for( std::vector<Point3df>::const_iterator in = norm.begin();
             in != norm.end(); ++in ) {
          os << "<" << (*in)[0] << "," << (*in)[1] << "," << (*in)[2] << ">,";
        }
        os << "  }" << std::endl;
      }
      
      // polygons
      os << "  face_indices {" << std::endl 
         << poly.size() << "," << std::endl << "    ";
      for( typename std::vector<AimsVector<uint,D> >::const_iterator ip = 
             poly.begin(); ip != poly.end(); ++ip ) {
        os << "<" << (*ip)[0] << "," << (*ip)[1] << "," << (*ip)[2] << ">,";
      }
      os << "  }" << std::endl;
      
      os << "  pigment { Gray80 }\n}" << std::endl;
    }
  }
}


#endif
