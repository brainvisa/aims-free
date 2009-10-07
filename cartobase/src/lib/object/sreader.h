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

#ifndef CARTOBASE_OBJECT_SREADER_H
#define CARTOBASE_OBJECT_SREADER_H

//=============================================================================
//	H E A D E R  F I L E S
//=============================================================================

#ifndef CARTOBASE_STREAMS_CUIFSTREAM_H
#include <cartobase/stream/cuifstream.h>
#endif
#ifndef CARTOBASE_OBJECT_SYNTAX_H
#include <cartobase/object/syntax.h>
#endif

namespace carto
{

  //===========================================================================
  //	C L A S S  D E C L A R A T I O N
  //===========================================================================

  /**	Read a SyntaxSet from a file
   */
  class CARTOBASE_API SyntaxReader
  {

  public:

    //---------------------------------------------------------------------
    /**	@name Constructors, destructor*/
    //---------------------------------------------------------------------
    //@{

    SyntaxReader(const std::string& filename);
    virtual ~SyntaxReader();

    //@}

    //---------------------------------------------------------------------
    /**	@name Read*/
    //---------------------------------------------------------------------
    //@{

    /**	read the referenced attributes from a stream
	@return syntactic attribute / associated rule pairs
    */
    void read(SyntaxSet& rules);

    //@}

  protected:

    //---------------------------------------------------------------------
    /**	@name Helpers*/
    //---------------------------------------------------------------------
    //@{

    /**	read characters not in s
	@param s continue while characters outside s
	@return accepted characters
    */
    std::string readUntil(const std::string& s);

    /**	skip characters in s
	@param s continue while characters in s
    */
    void skipWhile(const std::string& s);

    //@}

  private:

    //---------------------------------------------------------------------
    /**	@name Disable copy*/
    //---------------------------------------------------------------------
    //@{

    ///	Copy constructor
    SyntaxReader(const SyntaxReader&);

    ///	Assignment operator
    SyntaxReader& operator=(const SyntaxReader&);

    //@}

    //---------------------------------------------------------------------
    /**	@name Helpers*/
    //---------------------------------------------------------------------
    //@{

    /**	read a token from the input stream
	@return next token from the input stream
    */
    std::string nextToken();

    /**	
     */
    void read(Syntax& syntax);

    //@}

    //---------------------------------------------------------------------
    /**	@name Data*/
    //---------------------------------------------------------------------
    //@{

    ///
    std::string _filename;

    ///	filtering stream to read from
    cuifstream _stream;

    //@}
  };

}

//===========================================================================
//	F U N C T I O N
//===========================================================================

CARTOBASE_API carto::SyntaxReader& operator>>( carto::SyntaxReader& reader, 
					       carto::SyntaxSet& rules );

#endif
