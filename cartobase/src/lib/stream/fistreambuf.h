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

#ifndef CARTOBASE_STREAM_FISTREAMBUF_H
#define CARTOBASE_STREAM_FISTREAMBUF_H


//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#include <cartobase/config/cartobase_config.h>
#if defined(CARTO_USE_PRE_ISO_HEADERS)
#include <streambuf.h>
#else
#include <streambuf>
#endif

namespace carto
{

  //===========================================================================
  //	C L A S S  D E C L A R A T I O N
  //===========================================================================

  /**	Filtering input stream
   */
  template <class Extractor>
  class CARTOBASE_API fistreambuf : public std::streambuf
  {

  public:

    //---------------------------------------------------------------------
    /**	@name Constructors, destructor*/
    //---------------------------------------------------------------------
    //@{

    /**	Initialize with another streambuf as the source
	@param source source streambuf
	@param extractor filter
	@param d delete the source streambuf on exit
    */
    fistreambuf(std::streambuf* source, const Extractor& extractor,
		bool d = false);

    /**	Initialize with another streambuf as the source
	@param source source streambuf
	@param d delete the source streambuf on exit
    */
    fistreambuf(std::streambuf* source, bool d = false);

    /**	Call sync(), delete the source streambuf if asked
	to do so in the constructor
    */
    virtual ~fistreambuf();

    //@}

    //---------------------------------------------------------------------
    /**	@name Filtering*/
    //---------------------------------------------------------------------
    //@{

    /**	Actual filter
	@return the actual filter
    */
    Extractor& extractor();

    //@}

  protected:

    //---------------------------------------------------------------------
    /**	@name Get area*/
    //---------------------------------------------------------------------
    //@{

    /**	Get the first character of the pending sequence.
	If the pending sequence is null then the function returns
	EOF to indicate failure.
	@return the first character of the pending sequence
    */
    virtual int underflow();

    //@}

    //---------------------------------------------------------------------
    /**	@name Buffer management and positioning*/
    //---------------------------------------------------------------------
    //@{

    virtual int sync();

    /**	pass on to the source
	@param p 
	@param len 
	@return this
    */
    virtual std::streambuf* setbuf(char* p , int len);
    virtual pos_type seekoff( off_type, std::ios_base::seekdir, 
                              std::ios_base::openmode = std::ios_base::in 
                              | std::ios_base::out );

    //@}

  private:

    //---------------------------------------------------------------------
    /**	@name Data*/
    //---------------------------------------------------------------------
    //@{

    ///	Source streambuf to read data from
    std::streambuf* _source;

    ///	Actual filter
    Extractor _extractor;

    char _buffer;

    ///	Delete _source on exit?
    bool _delete;

    //@}
  };


  //===========================================================================
  //	I N L I N E  M E T H O D S
  //===========================================================================

  template <class Extractor>
  inline
  Extractor&
  fistreambuf<Extractor>::extractor()
  {
    return _extractor;
  }

}

#endif
