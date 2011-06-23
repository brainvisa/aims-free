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

#ifndef CARTOBASE_STREAM_CUIFSTREAM_H
#define CARTOBASE_STREAM_CUIFSTREAM_H


//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <cartobase/config/cartobase_config.h>
#ifndef CARTOBASE_STREAM_FISTREAMBUF_H
#include <cartobase/stream/fistreambuf.h>
#endif
#ifndef CARTOBASE_STREAM_COUNTER_H
#include <cartobase/stream/counter.h>
#endif
#ifndef CARTOBASE_STREAM_UNCOMMENT_H
#include <cartobase/stream/uncomment.h>
#endif
#include <fstream>

namespace carto
{

  //===========================================================================
  //	C L A S S  D E C L A R A T I O N
  //===========================================================================

  /**	Filtering ifstream - counts lines and skips comments
   */
  class CARTOBASE_API cuifstream : public std::istream
  {

  public:
    /** open modes are not a standard type in C++, they changed from 
        int in g++ 2.9x to std::ios_base::openmode in gcc 3.x */
#if defined( __GNUC__ ) && ( __GNUC__-0 < 3 )
    typedef int openmode;
#else
    typedef  std::ios_base::openmode openmode;
#endif

    //---------------------------------------------------------------------
    /**	@name Constructors, destructor*/
    //---------------------------------------------------------------------
    //@{

    cuifstream();
    cuifstream(const char *name, openmode mode = in);

    virtual ~cuifstream();

    //@}

    //---------------------------------------------------------------------
    /**	@name Open, close*/
    //---------------------------------------------------------------------
    //@{

    virtual void open(const char *name, openmode mode = in);
    virtual void close();

    //@}

    //---------------------------------------------------------------------
    /**	@name Count*/
    //---------------------------------------------------------------------
    //@{

    /**	Get the current line number
	@return current line number
    */
    int line() const;

    //@}

    //---------------------------------------------------------------------
    /**	@name Stream state*/
    //---------------------------------------------------------------------
    //@{

    /**	Return the state of both associated streams
     */
    virtual bool operator!() const;

    /**	Is the ifstream open?
     */
    virtual bool is_open() const;
    void enableUncomment( bool );
    bool uncommentEnabled() const { return _enableuncomment; }
    virtual bool eof() const;

    //@}

  protected:

    //---------------------------------------------------------------------
    /**	@name Constructors, destructor*/
    //---------------------------------------------------------------------
    //@{

    virtual void init_buffers();
    virtual void destroy_buffers();

    //@}

    //---------------------------------------------------------------------
    /**	@name Data*/
    //---------------------------------------------------------------------
    //@{

    mutable std::ifstream _base;	// mutable because is_open is not
					// const on gcc-3
    fistreambuf<CounterExtractor>* _counter;
    fistreambuf<UncommentExtractor>* _uncomment;
    bool  _enableuncomment;

    //@}
  };


  //===========================================================================
  //	I N L I N E  M E T H O D S
  //===========================================================================

  inline
  int
  cuifstream::line() const
  {
    return _counter->extractor().line();
  }

}

#endif
