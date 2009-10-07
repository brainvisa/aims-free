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


#ifndef CARTOBASE_STREAM_ATTACHABLE_CUIFSTREAM_H
#define CARTOBASE_STREAM_ATTACHABLE_CUIFSTREAM_H

#include <cartobase/config/cartobase_config.h>
#include <cartobase/stream/cuifstream.h>

namespace carto
{

  /**	Cette classe n'es là que parce que Dimitri ne voulait pas que je 
	touche à sa belle classe cuifstream. Il aurait suffi de remplacer 
	dans cuifstream la vartiable "en dur" _base par un pointeur, mais 
	ça ne lui convenait pas.
  */
  class CARTOBASE_API attachable_cuifstream : public cuifstream
  {
  public:
    attachable_cuifstream();
    attachable_cuifstream( const char *name );
    attachable_cuifstream( std::istream & s );

    virtual ~attachable_cuifstream();

    ///	attach to an existing (and open) stream
    virtual void attach( std::istream & s, int line_num = 1 );
    virtual void close();

    virtual bool operator!() const;
    virtual bool is_open() const;

  protected:
    virtual void init_buffers();
    virtual void destroy_buffers();

  private:
    std::istream	*_attached;
  };

}

#endif

