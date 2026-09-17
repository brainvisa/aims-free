// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/baseFormats_sparsematrix.h>
#include <aims/io/imasparseheader.h>

using namespace carto;
using namespace std;

namespace aims
{

  bool ImasFormat::read( const std::string & filename, SparseMatrix & obj,
                         const carto::AllocatorContext & /*context*/,
                         carto::Object /* options */ )
  {
    ImasHeader hdr( filename );
    if( !hdr.read() )
      return false;

    int ascii = false, bswap = false;
    hdr.getProperty( "ascii", ascii );
    hdr.getProperty( "byte_swapping", bswap );
    obj.read( filename, ascii ? "ascii" : "binar", bswap );

    obj.setHeader( Object::value( hdr.getValue() ) );
    return true;
  }


  bool ImasFormat::write( const std::string & filename,
                          const SparseMatrix & obj, carto::Object options )
  {
    bool ascii = false;
    try
    {
      if( !options.isNull() )
      {
        carto::Object aso = options->getProperty( "ascii" );
        if( !aso.isNull() )
          ascii = (bool) aso->getScalar();
      }
    }
    catch( ... )
    {
    }
    obj.write( filename, ascii ? "ascii" : "binar" );
    PythonHeader( PointerObject<PropertySet>( 
      obj.header()->value<PropertySet>(), false ) ).writeMinf( 
        filename + ".minf" );
    return true;
  }


  bool ImasSorDFormat::read( const std::string & filename, 
                             SparseOrDenseMatrix & obj,
                             const carto::AllocatorContext & /*context*/,
                             carto::Object /* options */ )
  {
    obj.read( filename );
    return true;
  }


  bool ImasSorDFormat::write( const std::string & filename,
                              const SparseOrDenseMatrix & obj, 
                              carto::Object options )
  {
    obj.write( filename, options );
    return true;
  }

}


