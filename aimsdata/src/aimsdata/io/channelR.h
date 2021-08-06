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
#ifndef AIMSDATA_IO_CHANNELR_H
#define AIMSDATA_IO_CHANNELR_H

#include <aims/io/process.h>
#include <aims/utility/converter_rgb.h>
#include <aims/utility/converter_hsv.h>
#include <aims/utility/converter_volume.h>
#include <aims/utility/channel.h>
#include <string>


namespace {

    template<class OUTP>
    class InternalConverter : public aims::Process
    {
    public:
      InternalConverter( uint8_t c, int b, int frm, OUTP & d )
        : Process(), channel(c), border( b ), frame( frm ), data( d ) {}
      virtual ~InternalConverter() {}

      uint8_t           channel;
      int               border;
      int               frame;
      OUTP            & data;
    };

}

namespace aims
{
  /// Reads a AimsData or carto::VolumeRef, with conversion if needed
  template<class OUTP>
  class ChannelReader
  {
  public:
    ChannelReader( const std::string & f );
    virtual ~ChannelReader();

    virtual void read( OUTP & data, 
                       uint8_t channel = 4,
                       int border = 0, 
                       const std::string* format = 0, 
                       int frame = -1 );
    void setAllowedInputDataTypes(
                       const std::map<std::string, std::set<std::string> > & );

  private:
    std::string _filename;
    std::map<std::string, std::set<std::string> > _allowedTypes;
  };

}

#endif
