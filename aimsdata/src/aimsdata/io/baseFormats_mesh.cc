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

#include <aims/io/baseFormats_mesh.h>
#include <cartobase/exception/file.h>
#include <aims/io/triR.h>
#include <aims/io/triW.h>

namespace aims
{
  //	TRI

  TriFormat::~TriFormat()
  {
  }


  bool TriFormat::read( const std::string & filename, 
			AimsSurfaceTriangle & obj, 
                        const carto::AllocatorContext & /*context*/, 
                        carto::Object options )
  {
    int	frame = -1;
    options->getProperty( "frame", frame );
    TriReader	r( filename );
    r.read( obj, frame );
    return( true );
  }

  bool TriFormat::write( const std::string & filename, 
			 const AimsSurfaceTriangle & obj, carto::Object )
  {
    TriWriter	r( filename );
    r.write( obj );
    return( true );
  }


  // VRML 1

  Vrml1Format::~Vrml1Format()
  {
  }


  bool Vrml1Format::read( const std::string & /*filename*/,
                          AimsTimeSurface<3,Void>& /*obj*/,
                          const carto::AllocatorContext & /*context*/,
                          carto::Object /*options*/ )
  { 
    return false;
  }

  bool Vrml1Format::write(const std::string & filename,
                          const AimsTimeSurface<3, Void> &vol, carto::Object )
  {
    std::ofstream os( filename.c_str());
    if( !os )
      throw carto::file_error( filename );


    os << "#VRML V1.0 ascii\n\n";
    os << "#Generated by AIMS Vrml1 output\n";
    os << "#cut & pasted from Blender export script\n";

    os << "Separator {\n\tDEF Brain\n\tSeparator {\n\t\tCoordinate3 {\n"
      "\t\t\tpoint [\n";

    const std::vector<Point3df> &p = vol.vertex();
    std::vector<Point3df>::const_iterator i;
    for (i = p.begin(); i != p.end(); i++)
      os << "\t\t\t\t" << i->item(0) << " " << i->item(1) << 
        " " << i->item(2) << "," << std::endl;

    os << "\t\t\t]\n\t\t}\n\n\t\tIndexedFaceSet {\n\t\tcoordIndex [\n";
    const std::vector< AimsVector<uint,3> >&k = vol.polygon();
    std::vector< AimsVector<uint,3> >::const_iterator ii;
    for (ii = k.begin(); ii != k.end(); ii++)
      os << "\t\t\t\t" << ii->item(0) << ", " << ii->item(1) << 
        ", " << ii->item(2) << ", -1," << std::endl;

    os << "\t\t\t]\n\t\t}\n\t}\n}\n";
    return true;
  }
}
