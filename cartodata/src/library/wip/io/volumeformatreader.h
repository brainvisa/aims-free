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

#ifndef CARTODATA_IO_VOLUMEFORMATREADER_H
#define CARTODATA_IO_VOLUMEFORMATREADER_H

#include <cartobase/io/formatreader.h>

namespace carto
{
  template <typename T> class Volume;

  template<typename T>
  class VolumeFormatReader : public FormatReader<Volume<T> >
  {
  public:
    virtual ~VolumeFormatReader();

    virtual rc_ptr<DataSource> getDataSource( Object header, 
                                              rc_ptr<DataSource> source, 
                                              Object options );
    virtual AllocatorContext 
    getAllocatorContext( Object header, rc_ptr<DataSource> decoder, 
                         const AllocatorContext & basecontext, 
                         Object options );
    virtual void read( Volume<T> & obj, Object header, 
                       const AllocatorContext & context, Object options );
  };


  template<typename T>
  class VolumeRefFormatReader : public FormatReader<rc_ptr<Volume<T> > >
  {
  public:
    virtual ~VolumeRefFormatReader();

    virtual rc_ptr<DataSource> getDataSource( Object header, 
                                              rc_ptr<DataSource> source, 
                                              Object options );
    virtual AllocatorContext 
    getAllocatorContext( Object header, rc_ptr<DataSource> decoder, 
                         const AllocatorContext & basecontext, 
                         Object options );
    virtual void read( rc_ptr<Volume<T> > & obj, Object header, 
                       const AllocatorContext & context, Object options );
  };

}

#endif

