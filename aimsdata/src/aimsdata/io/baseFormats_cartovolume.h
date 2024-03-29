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
 *  Data reader class
 */
#ifndef AIMS_IO_BASEFORMATS_CARTOVOLUME_H
#define AIMS_IO_BASEFORMATS_CARTOVOLUME_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/header.h>
#include <aims/io/fileFormat.h>
#include <cartodata/volume/volume.h>


namespace aims
{

  template<typename T>
  class VolumeFormat : public FileFormat<carto::Volume<T> >
  {
  public:
    VolumeFormat( const std::string & prefformat = std::string() );
    virtual ~VolumeFormat();

    virtual bool read( const std::string & filename, carto::Volume<T> & vol, 
		       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, 
                        const carto::Volume<T> & vol,
                        carto::Object options = carto::none() );
    virtual carto::Volume<T>* 
    read( const std::string &filename, 
          const carto::AllocatorContext & context, carto::Object options );

  private:
    std::string	_preferredFormat;
  };


  template<typename T>
  class VolumeRefFormat : public FileFormat<carto::VolumeRef<T> >
  {
  public:
    VolumeRefFormat( const std::string & prefformat = std::string() );
    virtual ~VolumeRefFormat();

    virtual bool read( const std::string & filename, 
                       carto::VolumeRef<T> & vol, 
		       const carto::AllocatorContext & context, 
                       carto::Object options );
    virtual bool write( const std::string & filename, 
                        const carto::VolumeRef<T> & vol, 
                        carto::Object options = carto::none() );
    virtual carto::VolumeRef<T>* 
    read( const std::string &filename, 
          const carto::AllocatorContext & context, carto::Object options );

  private:
    VolumeFormat<T>	_volformat;
  };


  template<typename T>
  class VolumeRefAimsFormat : public FileFormat<carto::VolumeRef<T> >
  {
  public:
    VolumeRefAimsFormat( const std::string & prefformat = std::string() );
    virtual ~VolumeRefAimsFormat();

    virtual bool read( const std::string & filename,
                       carto::VolumeRef<T> & vol,
		       const carto::AllocatorContext & context,
                       carto::Object options );
    virtual bool write( const std::string & filename,
                        const carto::VolumeRef<T> & vol,
                        carto::Object options = carto::none() );
    virtual carto::VolumeRef<T>*
    read( const std::string &filename,
          const carto::AllocatorContext & context, carto::Object options );

  private:
    std::string	_preferredFormat;
  };

}


#endif

