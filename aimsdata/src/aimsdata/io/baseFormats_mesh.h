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
 *  Data reader class
 */
#ifndef AIMS_IO_BASEFORMATS_MESH_H
#define AIMS_IO_BASEFORMATS_MESH_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/fileFormat.h>
#include <aims/mesh/surface.h>


namespace aims
{

  template<int D, class T>
  class MeshFormat : public FileFormat<AimsTimeSurface<D,T> >
  {
  public:
    virtual ~MeshFormat();

    virtual bool read( const std::string & filename, 
		       AimsTimeSurface<D,T> & obj, 
                       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, 
			const AimsTimeSurface<D,T> & vol, bool ascii = false );
  };

  class TriFormat : public FileFormat<AimsSurfaceTriangle>
  {
  public:
    virtual ~TriFormat();

    virtual bool read( const std::string & filename, 
		       AimsSurfaceTriangle & obj, 
                       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, 
			const AimsSurfaceTriangle & vol, bool ascii = false );
  };


  template<int D>
  class PovFormat : public FileFormat<AimsTimeSurface<D,Void> >
  {
  public:
    virtual ~PovFormat();

    virtual bool read( const std::string & filename, 
		       AimsTimeSurface<D,Void> & obj, 
                       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, 
			const AimsTimeSurface<D,Void> & vol, 
                        bool ascii = false );
  };


  class Vrml1Format : public FileFormat<AimsTimeSurface<3, Void> >
  {
  public:
    virtual ~Vrml1Format();

    virtual bool read( const std::string & filename,
                       AimsTimeSurface<3,Void>&obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );

    virtual bool write(const std::string & filename,
			const AimsTimeSurface<3, Void> &vol, bool ascii);
  };
}


#endif
