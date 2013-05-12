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


